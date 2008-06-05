#include "Buffer.h"
#include "Font_Control.h"
#include "Handle_Items.h"
#include "Structure.h"
#include "TileDef.h"
#include "Types.h"
#include "SaveLoadMap.h"
#include "Overhead.h"
#include "FileMan.h"
#include "Tactical_Save.h"
#include "Debug.h"
#include "WorldMan.h"
#include "StrategicMap.h"
#include "Campaign_Types.h"
#include "Render_Fun.h"
#include "FOV.h"
#include "WorldDef.h"
#include "Exit_Grids.h"
#include "Message.h"
#include "GameSettings.h"
#include "Smell.h"
#include "MemMan.h"


#define			NUM_REVEALED_BYTES			3200

extern BOOLEAN gfLoadingExitGrids;

BOOLEAN			gfApplyChangesToTempFile = FALSE;

//  There are 3200 bytes, and each bit represents the revelaed status.
//	3200 bytes * 8 bits = 25600 map elements
UINT8				*gpRevealedMap;


void	ApplyMapChangesToMapTempFile( BOOLEAN fAddToMap )
{
	gfApplyChangesToTempFile = fAddToMap;
}


static BOOLEAN SaveModifiedMapStructToMapTempFile(MODIFY_MAP* pMap, INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ)
try
{
	CHAR8		zMapName[ 128 ];

	GetMapTempFileName( SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS, zMapName, sSectorX, sSectorY, bSectorZ );

	AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS));
	if (!hFile) return FALSE;

	FileWrite(hFile, pMap, sizeof(MODIFY_MAP));

	SetSectorFlag( sSectorX, sSectorY, bSectorZ, SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS );
	return TRUE;
}
catch (...) { return FALSE; }


static void AddBloodOrSmellFromMapTempFileToMap(MODIFY_MAP* pMap);
static void AddObjectFromMapTempFileToMap(UINT32 uiMapIndex, UINT16 usIndex);
static void DamageStructsFromMapTempFile(MODIFY_MAP* pMap);
static BOOLEAN ModifyWindowStatus(UINT32 uiMapIndex);
static void RemoveSavedStructFromMap(UINT32 uiMapIndex, UINT16 usIndex);
static void SetOpenableStructStatusFromMapTempFile(UINT32 uiMapIndex, BOOLEAN fOpened);


BOOLEAN LoadAllMapChangesFromMapTempFileAndApplyThem( )
try
{
	CHAR8		zMapName[ 128 ];
	UINT32	uiNumberOfElementsSavedBackToFile = 0;	// added becuase if no files get saved back to disk, the flag needs to be erased
	UINT32	cnt;
	MODIFY_MAP *pMap;

	GetMapTempFileName( SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS, zMapName, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );


	//Check to see if the file exists
	if( !FileExists( zMapName ) )
	{
		//If the file doesnt exists, its no problem.
		return( TRUE );
	}

	UINT32                  uiNumberOfElements;
	SGP::Buffer<MODIFY_MAP> pTempArrayOfMaps;
	{
		AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_READ));
		if (!hFile) return FALSE;

		//Get the size of the file
		uiNumberOfElements = FileGetSize(hFile) / sizeof(MODIFY_MAP);

		//Read the map temp file into a buffer
		pTempArrayOfMaps.Allocate(uiNumberOfElements);
		FileRead(hFile, pTempArrayOfMaps, sizeof(*pTempArrayOfMaps) * uiNumberOfElements);
	}

	//Delete the file
	FileDelete( zMapName );

	for( cnt=0; cnt< uiNumberOfElements; cnt++ )
	{
		pMap = &pTempArrayOfMaps[ cnt ];

		//Switch on the type that should either be added or removed from the map
		switch( pMap->ubType )
		{
			//If we are adding to the map
			case SLM_LAND:
				break;
			case SLM_OBJECT:
			{
				UINT16 usIndex = GetTileIndexFromTypeSubIndex(pMap->usImageType, pMap->usSubImageIndex);
				AddObjectFromMapTempFileToMap( pMap->usGridNo, usIndex );

				// Save this struct back to the temp file
				SaveModifiedMapStructToMapTempFile( pMap, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

				//Since the element is being saved back to the temp file, increment the #
				uiNumberOfElementsSavedBackToFile++;
				break;
			}

			case SLM_STRUCT:
			{
				UINT16 usIndex = GetTileIndexFromTypeSubIndex(pMap->usImageType, pMap->usSubImageIndex);
				AddStructToTail(pMap->usGridNo, usIndex);

				// Save this struct back to the temp file
				SaveModifiedMapStructToMapTempFile( pMap, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

				//Since the element is being saved back to the temp file, increment the #
				uiNumberOfElementsSavedBackToFile++;
				break;
			}

			case SLM_SHADOW:
				break;
			case SLM_MERC:
				break;
			case SLM_ROOF:
				break;
			case SLM_ONROOF:
				break;
			case SLM_TOPMOST:
				break;


			//Remove objects out of the world
			case SLM_REMOVE_LAND:
				break;
			case SLM_REMOVE_OBJECT:
				break;
			case SLM_REMOVE_STRUCT:

				// ATE: OK, dor doors, the usIndex can be varied, opened, closed, etc
				// we MUSTR delete ANY door type on this gridno
				// Since we can only have one door per gridno, we're safe to do so.....
				if ( pMap->usImageType >= FIRSTDOOR && pMap->usImageType <= FOURTHDOOR )
				{
					// Remove ANY door...
					RemoveAllStructsOfTypeRange( pMap->usGridNo, FIRSTDOOR, FOURTHDOOR );
				}
				else
				{
					UINT16 usIndex = GetTileIndexFromTypeSubIndex(pMap->usImageType, pMap->usSubImageIndex);
					RemoveSavedStructFromMap( pMap->usGridNo, usIndex );
				}

				// Save this struct back to the temp file
				SaveModifiedMapStructToMapTempFile( pMap, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

				//Since the element is being saved back to the temp file, increment the #
				uiNumberOfElementsSavedBackToFile++;
				break;
			case SLM_REMOVE_SHADOW:
				break;
			case SLM_REMOVE_MERC:
				break;
			case SLM_REMOVE_ROOF:
				break;
			case SLM_REMOVE_ONROOF:
				break;
			case SLM_REMOVE_TOPMOST:
				break;


			case SLM_BLOOD_SMELL:
				AddBloodOrSmellFromMapTempFileToMap( pMap );
				break;

			case SLM_DAMAGED_STRUCT:
				DamageStructsFromMapTempFile( pMap );
				break;

			case SLM_EXIT_GRIDS:
				{
					EXITGRID ExitGrid;
					gfLoadingExitGrids = TRUE;
					ExitGrid.usGridNo = pMap->usSubImageIndex;
					ExitGrid.ubGotoSectorX = (UINT8) pMap->usImageType;
					ExitGrid.ubGotoSectorY = (UINT8) ( pMap->usImageType >> 8 ) ;
					ExitGrid.ubGotoSectorZ = pMap->ubExtra;

					AddExitGridToWorld( pMap->usGridNo, &ExitGrid );
					gfLoadingExitGrids = FALSE;

					// Save this struct back to the temp file
					SaveModifiedMapStructToMapTempFile( pMap, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

					//Since the element is being saved back to the temp file, increment the #
					uiNumberOfElementsSavedBackToFile++;
				}
				break;

			case SLM_OPENABLE_STRUCT:
				SetOpenableStructStatusFromMapTempFile( pMap->usGridNo, (BOOLEAN)pMap->usImageType );
				break;

			case SLM_WINDOW_HIT:
				if ( ModifyWindowStatus( pMap->usGridNo ) )
				{
					// Save this struct back to the temp file
					SaveModifiedMapStructToMapTempFile( pMap, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

					//Since the element is being saved back to the temp file, increment the #
					uiNumberOfElementsSavedBackToFile++;
				}
				break;

			default:
				AssertMsg( 0, "ERROR!  Map Type not in switch when loading map changes from temp file");
				break;
		}

	}

	//if no elements are saved back to the file, remove the flag indicating that there is a temp file
	if( uiNumberOfElementsSavedBackToFile == 0 )
	{
		ReSetSectorFlag( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS );
	}

	return( TRUE );
}
catch (...) { return FALSE; }


void AddStructToMapTempFile( UINT32 uiMapIndex, UINT16 usIndex )
{
	MODIFY_MAP Map;

	if( !gfApplyChangesToTempFile )
		return;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
//	Map.usIndex		= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;

	Map.ubType		= SLM_STRUCT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
}


void AddObjectToMapTempFile( UINT32 uiMapIndex, UINT16 usIndex )
{
	MODIFY_MAP Map;

	if( !gfApplyChangesToTempFile )
		return;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
//	Map.usIndex		= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;

	Map.ubType		= SLM_OBJECT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
}


static void AddObjectFromMapTempFileToMap(UINT32 uiMapIndex, UINT16 usIndex)
{
	AddObjectToHead( uiMapIndex, usIndex );
}

void AddRemoveObjectToMapTempFile( UINT32 uiMapIndex, UINT16 usIndex )
{
	MODIFY_MAP Map;

	if( !gfApplyChangesToTempFile )
		return;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
//	Map.usIndex		= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;

	Map.ubType		= SLM_REMOVE_OBJECT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
}


void RemoveStructFromMapTempFile( UINT32 uiMapIndex, UINT16 usIndex )
{
	MODIFY_MAP Map;

	if( !gfApplyChangesToTempFile )
		return;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo	= (UINT16)uiMapIndex;
//	Map.usIndex			= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;

	Map.ubType			= SLM_REMOVE_STRUCT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

}


static void RemoveSavedStructFromMap(UINT32 uiMapIndex, UINT16 usIndex)
{
	RemoveStruct( uiMapIndex, usIndex );
}


static void AddOpenableStructStatusToMapTempFile(UINT32 uiMapIndex, BOOLEAN fOpened);
static void SetSectorsRevealedBit(UINT16 usMapIndex);


void SaveBloodSmellAndRevealedStatesFromMapToTempFile()
{
	MODIFY_MAP Map;
	UINT16	cnt;
	STRUCTURE * pStructure;

	gpRevealedMap = MALLOCNZ(UINT8, NUM_REVEALED_BYTES);

	//Loop though all the map elements
	for ( cnt = 0; cnt < WORLD_MAX; cnt++ )
	{
		//if there is either blood or a smell on the tile, save it
		if( gpWorldLevelData[cnt].ubBloodInfo || gpWorldLevelData[cnt].ubSmellInfo )
		{
			memset( &Map, 0, sizeof( MODIFY_MAP ) );


			// Save the BloodInfo in the bottom byte and the smell info in the upper byte
			Map.usGridNo	= cnt;
//			Map.usIndex			= gpWorldLevelData[cnt].ubBloodInfo | ( gpWorldLevelData[cnt].ubSmellInfo << 8 );
			Map.usImageType = gpWorldLevelData[cnt].ubBloodInfo;
			Map.usSubImageIndex = gpWorldLevelData[cnt].ubSmellInfo;


			Map.ubType			= SLM_BLOOD_SMELL;

			//Save the change to the map file
			SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		}


		//if the element has been revealed
		if( gpWorldLevelData[cnt].uiFlags & MAPELEMENT_REVEALED )
		{
			SetSectorsRevealedBit( cnt );
		}

		//if there is a structure that is damaged
		if( gpWorldLevelData[cnt].uiFlags & MAPELEMENT_STRUCTURE_DAMAGED )
		{
			STRUCTURE * pCurrent;

			pCurrent =  gpWorldLevelData[cnt].pStructureHead;

			pCurrent = FindStructure( cnt, STRUCTURE_BASE_TILE );

			//loop through all the structures and add all that are damaged
			while( pCurrent )
			{
				//if the structure has been damaged
				if( pCurrent->ubHitPoints < pCurrent->pDBStructureRef->pDBStructure->ubHitPoints )
				{
					UINT8	ubBitToSet = 0x80;
					UINT8	ubLevel=0;

					if( pCurrent->sCubeOffset != 0 )
						ubLevel |= ubBitToSet;

					memset( &Map, 0, sizeof( MODIFY_MAP ) );

					// Save the Damaged value
					Map.usGridNo	= cnt;
//					Map.usIndex			= StructureFlagToType( pCurrent->fFlags ) | ( pCurrent->ubHitPoints << 8 );
					Map.usImageType = StructureFlagToType( pCurrent->fFlags );
					Map.usSubImageIndex = pCurrent->ubHitPoints;


					Map.ubType			= SLM_DAMAGED_STRUCT;
					Map.ubExtra			= pCurrent->ubWallOrientation | ubLevel;

					//Save the change to the map file
					SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
				}

				pCurrent = FindNextStructure( pCurrent, STRUCTURE_BASE_TILE );
			}
		}

		pStructure = FindStructure( cnt, STRUCTURE_OPENABLE );

		//if this structure
		if( pStructure )
		{
			// if the current structure has an openable structure in it, and it is NOT a door
			if( !( pStructure->fFlags & STRUCTURE_ANYDOOR ) )
			{
				BOOLEAN			fStatusOnTheMap;

				fStatusOnTheMap = ( ( pStructure->fFlags & STRUCTURE_OPEN ) != 0 );

				AddOpenableStructStatusToMapTempFile( cnt, fStatusOnTheMap );
			}
		}
	}
}


// The BloodInfo is saved in the bottom byte and the smell info in the upper byte
static void AddBloodOrSmellFromMapTempFileToMap(MODIFY_MAP* pMap)
{
	gpWorldLevelData[ pMap->usGridNo ].ubBloodInfo = (UINT8)pMap->usImageType;

	//if the blood and gore option IS set, add blood
	if( gGameSettings.fOptions[ TOPTION_BLOOD_N_GORE ] )
	{
		// Update graphics for both levels...
		gpWorldLevelData[ pMap->usGridNo ].uiFlags |= MAPELEMENT_REEVALUATEBLOOD;
		UpdateBloodGraphics( pMap->usGridNo, 0 );
		gpWorldLevelData[ pMap->usGridNo ].uiFlags |= MAPELEMENT_REEVALUATEBLOOD;
		UpdateBloodGraphics( pMap->usGridNo, 1 );
	}

	gpWorldLevelData[ pMap->usGridNo ].ubSmellInfo = (UINT8)pMap->usSubImageIndex;
}



BOOLEAN SaveRevealedStatusArrayToRevealedTempFile( INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ )
try
{
	CHAR8		zMapName[ 128 ];

	Assert( gpRevealedMap != NULL );

	GetMapTempFileName( SF_REVEALED_STATUS_TEMP_FILE_EXISTS, zMapName, sSectorX, sSectorY, bSectorZ );

	AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));
	if (!hFile) return FALSE;

	//Write the revealed array to the Revealed temp file
	FileWrite(hFile, gpRevealedMap, NUM_REVEALED_BYTES);

	SetSectorFlag( sSectorX, sSectorY, bSectorZ, SF_REVEALED_STATUS_TEMP_FILE_EXISTS );

	MemFree( gpRevealedMap );
	gpRevealedMap = NULL;

	return( TRUE );
}
catch (...) { return FALSE; }


static void SetMapRevealedStatus(void);


BOOLEAN LoadRevealedStatusArrayFromRevealedTempFile()
try
{
	CHAR8		zMapName[ 128 ];

	GetMapTempFileName( SF_REVEALED_STATUS_TEMP_FILE_EXISTS, zMapName, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

	//Check to see if the file exists
	if( !FileExists( zMapName ) )
	{
		//If the file doesnt exists, its no problem.
		return( TRUE );
	}

	{
		AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_READ));
		if (!hFile) return FALSE;

		Assert( gpRevealedMap == NULL );
		gpRevealedMap = MALLOCNZ(UINT8, NUM_REVEALED_BYTES);

		// Load the Reveal map array structure
		FileRead(hFile, gpRevealedMap, NUM_REVEALED_BYTES);
	}

	//Loop through and set the bits in the map that are revealed
	SetMapRevealedStatus();

	MemFree( gpRevealedMap );
	gpRevealedMap = NULL;



	return( TRUE );
}
catch (...) { return FALSE; }


static void SetSectorsRevealedBit(UINT16 usMapIndex)
{
	UINT16	usByteNumber;
	UINT8		ubBitNumber;

	usByteNumber = usMapIndex / 8;
	ubBitNumber  = usMapIndex % 8;

	gpRevealedMap[ usByteNumber ] |= 1 << ubBitNumber;
}


static void SetMapRevealedStatus(void)
{
	UINT16	usByteCnt;
	UINT8		ubBitCnt;
	UINT16	usMapIndex;

	AssertMsg(gpRevealedMap != NULL, "gpRevealedMap is NULL.  DF 1");

	ClearSlantRoofs( );

	//Loop through all bytes in the array
	for( usByteCnt=0; usByteCnt< 3200; usByteCnt++)
	{
		//loop through all the bits in the byte
		for( ubBitCnt=0; ubBitCnt<8; ubBitCnt++)
		{
			usMapIndex = ( usByteCnt * 8 ) + ubBitCnt;

			if( gpRevealedMap[ usByteCnt ] & ( 1 << ubBitCnt ) )
			{
				gpWorldLevelData[ usMapIndex ].uiFlags |= MAPELEMENT_REVEALED;
				SetGridNoRevealedFlag( usMapIndex );
			}
			else
			{
				gpWorldLevelData[ usMapIndex ].uiFlags &= (~MAPELEMENT_REVEALED );
			}
		}
	}

	ExamineSlantRoofFOVSlots( );

}


static void DamageStructsFromMapTempFile(MODIFY_MAP* pMap)
{
	STRUCTURE *pCurrent=NULL;
	INT8			bLevel;
	UINT8			ubWallOrientation;
	UINT8			ubBitToSet = 0x80;
	UINT8			ubType=0;


	//Find the base structure
	pCurrent = FindStructure( (INT16)pMap->usGridNo, STRUCTURE_BASE_TILE );

	if( pCurrent == NULL )
		return;

	bLevel = pMap->ubExtra & ubBitToSet;
	ubWallOrientation = pMap->ubExtra & ~ubBitToSet;
	ubType = (UINT8) pMap->usImageType;


	//Check to see if the desired strucure node is in this tile
	pCurrent = FindStructureBySavedInfo( pMap->usGridNo, ubType, ubWallOrientation, bLevel );

	if( pCurrent != NULL )
	{
		//Assign the hitpoints
		pCurrent->ubHitPoints = (UINT8)( pMap->usSubImageIndex );

		gpWorldLevelData[ pCurrent->sGridNo ].uiFlags |= MAPELEMENT_STRUCTURE_DAMAGED;
	}
}


void AddStructToUnLoadedMapTempFile( UINT32 uiMapIndex, UINT16 usIndex, INT16 sSectorX, INT16 sSectorY, UINT8 ubSectorZ  )
{
	MODIFY_MAP Map;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
//	Map.usIndex		= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;


	Map.ubType		= SLM_STRUCT;

	SaveModifiedMapStructToMapTempFile( &Map, sSectorX, sSectorY, ubSectorZ );
}


void RemoveStructFromUnLoadedMapTempFile( UINT32 uiMapIndex, UINT16 usIndex, INT16 sSectorX, INT16 sSectorY, UINT8 ubSectorZ  )
{
	MODIFY_MAP Map;

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo	= (UINT16)uiMapIndex;
//	Map.usIndex			= usIndex;
	Map.usImageType = (UINT16)uiType;
	Map.usSubImageIndex = usSubIndex;

	Map.ubType			= SLM_REMOVE_STRUCT;

	SaveModifiedMapStructToMapTempFile( &Map, sSectorX, sSectorY, ubSectorZ );
}


void AddExitGridToMapTempFile( UINT16 usGridNo, EXITGRID *pExitGrid, INT16 sSectorX, INT16 sSectorY, UINT8 ubSectorZ )
{
	MODIFY_MAP Map;

	if( !gfApplyChangesToTempFile )
	{
		ScreenMsg( FONT_MCOLOR_WHITE, MSG_BETAVERSION, L"Called AddExitGridToMapTempFile() without calling ApplyMapChangesToMapTempFile()" );
		return;
	}

	if( gTacticalStatus.uiFlags & LOADING_SAVED_GAME )
		return;

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = usGridNo;
//	Map.usIndex		= pExitGrid->ubGotoSectorX;

	Map.usImageType = pExitGrid->ubGotoSectorX | ( pExitGrid->ubGotoSectorY << 8 );
	Map.usSubImageIndex = pExitGrid->usGridNo;

	Map.ubExtra		= pExitGrid->ubGotoSectorZ;
	Map.ubType		= SLM_EXIT_GRIDS;

	SaveModifiedMapStructToMapTempFile( &Map, sSectorX, sSectorY, ubSectorZ );
}

BOOLEAN RemoveGraphicFromTempFile( UINT32 uiMapIndex, UINT16 usIndex, INT16 sSectorX, INT16 sSectorY, UINT8 ubSectorZ )
try
{
	CHAR8		zMapName[ 128 ];
	MODIFY_MAP *pMap;
	BOOLEAN	fRetVal=FALSE;
	UINT32	cnt;

	GetMapTempFileName( SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS, zMapName, sSectorX, sSectorY, ubSectorZ );

	UINT32                  uiNumberOfElements;
	SGP::Buffer<MODIFY_MAP> pTempArrayOfMaps;
	{
		AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_READ));
		if (!hFile) return FALSE;

		//Get the number of elements in the file
		uiNumberOfElements = FileGetSize(hFile) / sizeof(MODIFY_MAP);

		//Read the map temp file into a buffer
		pTempArrayOfMaps.Allocate(uiNumberOfElements);
		FileRead(hFile, pTempArrayOfMaps, sizeof(*pTempArrayOfMaps) * uiNumberOfElements);
	}

	//Delete the file
	FileDelete( zMapName );

	//Get the image type and subindex
	const UINT32 uiType     = GetTileType(usIndex);
	const UINT16 usSubIndex = GetSubIndexFromTileIndex(usIndex);

	for( cnt=0; cnt< uiNumberOfElements; cnt++ )
	{
		pMap = &pTempArrayOfMaps[ cnt ];

		//if this is the peice we are looking for
		if( pMap->usGridNo == uiMapIndex && pMap->usImageType == uiType && pMap->usSubImageIndex == usSubIndex )
		{
			//Do nothin
			fRetVal = TRUE;
		}
		else
		{
			//save the struct back to the temp file
			SaveModifiedMapStructToMapTempFile( pMap, sSectorX, sSectorY, ubSectorZ );
		}
	}

	return( fRetVal );
}
catch (...) { return FALSE; }


static void AddOpenableStructStatusToMapTempFile(UINT32 uiMapIndex, BOOLEAN fOpened)
{
	MODIFY_MAP Map;

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
	Map.usImageType = fOpened;

	Map.ubType = SLM_OPENABLE_STRUCT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
}

void AddWindowHitToMapTempFile( UINT32 uiMapIndex )
{
	MODIFY_MAP Map;

	memset( &Map, 0, sizeof( MODIFY_MAP ) );

	Map.usGridNo = (UINT16)uiMapIndex;
	Map.ubType = SLM_WINDOW_HIT;

	SaveModifiedMapStructToMapTempFile( &Map, gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
}


static BOOLEAN ModifyWindowStatus(UINT32 uiMapIndex)
{
	STRUCTURE *		pStructure;

	pStructure = FindStructure( (INT16) uiMapIndex, STRUCTURE_WALLNWINDOW );
	if (pStructure)
	{
		SwapStructureForPartner( (INT16) uiMapIndex, pStructure );
		return( TRUE );
	}
	// else forget it, window could be destroyed
	return( FALSE );
}


static void SetOpenableStructStatusFromMapTempFile(UINT32 uiMapIndex, BOOLEAN fOpened)
{
	STRUCTURE * pStructure;
	STRUCTURE * pBase;
	BOOLEAN			fStatusOnTheMap;
  INT16     sBaseGridNo = (INT16)uiMapIndex;

	pStructure = FindStructure( (UINT16)uiMapIndex, STRUCTURE_OPENABLE );

	if( pStructure == NULL )
	{
//		ScreenMsg( FONT_MCOLOR_WHITE, MSG_BETAVERSION, L"SetOpenableStructStatusFromMapTempFile( %d, %d ) failed to find the openable struct.  DF 1.", uiMapIndex, fOpened );
		return;
	}

	fStatusOnTheMap = ( ( pStructure->fFlags & STRUCTURE_OPEN ) != 0 );

	if( fStatusOnTheMap != fOpened )
	{
		// Adjust the item's gridno to the base of struct.....
		pBase = FindBaseStructure( pStructure );

		// Get LEVELNODE for struct and remove!
    if ( pBase )
    {
		  sBaseGridNo = pBase->sGridNo;
    }

		if(SwapStructureForPartnerWithoutTriggeringSwitches( (UINT16)uiMapIndex, pStructure ) == NULL )
		{
			//an error occured
		}


		// Adjust visiblity of any item pools here....
    // ATE: Nasty bug here - use base gridno for structure for items!
    // since items always drop to base gridno in AddItemToPool
		ITEM_POOL* pItemPool = GetItemPool(sBaseGridNo, 0);
		if (pItemPool != NULL)
		{
			if ( fOpened )
			{
				 // We are open, make un-hidden if so....
				 SetItemPoolVisibilityOn( pItemPool, ANY_VISIBILITY_VALUE, FALSE );
			}
			else
			{
				 // Make sure items are hidden...
				 SetItemPoolVisibilityHidden( pItemPool );
			}

		}

	}
}


BOOLEAN ChangeStatusOfOpenableStructInUnloadedSector(const UINT16 usSectorX, const UINT16 usSectorY, const INT8 bSectorZ, const UINT16 usGridNo, const BOOLEAN fChangeToOpen)
try
{
	char map_name[128];
	GetMapTempFileName(SF_MAP_MODIFICATIONS_TEMP_FILE_EXISTS, map_name, usSectorX, usSectorY, bSectorZ);

	// If the file doesn't exists, it's no problem.
	if (!FileExists(map_name)) return TRUE;

	UINT32                  uiNumberOfElements;
	SGP::Buffer<MODIFY_MAP> mm;
	{
		// Read the map temp file into a buffer
		AutoSGPFile src(FileOpen(map_name, FILE_ACCESS_READ));
		if (src == 0) return FALSE;

		uiNumberOfElements = FileGetSize(src) / sizeof(MODIFY_MAP);

		mm.Allocate(uiNumberOfElements);
		FileRead(src, mm, sizeof(*mm) * uiNumberOfElements);
	}

	for (UINT32 i = 0; i < uiNumberOfElements; ++i)
	{
		MODIFY_MAP* const m = &mm[i];
		if (m->ubType != SLM_OPENABLE_STRUCT || m->usGridNo != usGridNo) continue;
		// This element is of the same type and on the same gridno

		// Change to the desired settings
		m->usImageType = fChangeToOpen;
		break;
	}

	AutoSGPFile dst(FileOpen(map_name, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));
	if (dst == 0) return FALSE;

	FileWrite(dst, mm, sizeof(*mm) * uiNumberOfElements);
	return TRUE;
}
catch (...) { return FALSE; }
