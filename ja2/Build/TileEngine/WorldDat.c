#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "WorldDat.h"
#include "WorldDef.h"
#include "TileDef.h"
#include "Sys_Globals.h"
#include "Tile_Surface.h"
#include "FileMan.h"


// THIS FILE CONTAINS DEFINITIONS FOR TILESET FILES

TILESET	 gTilesets[ NUM_TILESETS ];


static void SetTilesetFourTerrainValues(void);
static void SetTilesetThreeTerrainValues(void);
static void SetTilesetTwoTerrainValues(void);


void InitEngineTilesets( )
{
	UINT8			ubNumSets;
	UINT32		cnt, cnt2, uiNumFiles;
//	FILE					*hfile;
	HWFILE			hfile;
	CHAR8			zName[32];

	//OPEN FILE
//	hfile = fopen( "BINARYDATA/JA2SET.DAT", "rb" );
	hfile = FileOpen("BINARYDATA/JA2SET.DAT", FILE_ACCESS_READ);
	if ( !hfile  )
	{
		SET_ERROR( "Cannot open tileset data file" );
		return;
	}


	// READ # TILESETS and compare
//	fread( &ubNumSets, sizeof( ubNumSets ), 1, hfile );
	FileRead(hfile, &ubNumSets, sizeof(ubNumSets));
	// CHECK
	if ( ubNumSets != NUM_TILESETS )
	{
		// Report error
		SET_ERROR( "Number of tilesets in code does not match data file" );
		return;
	}

	// READ #files
//	fread( &uiNumFiles, sizeof( uiNumFiles ), 1, hfile );
	FileRead(hfile, &uiNumFiles, sizeof(uiNumFiles));

	// COMPARE
	if ( uiNumFiles != NUMBEROFTILETYPES )
	{
		// Report error
		SET_ERROR( "Number of tilesets slots in code does not match data file" );
		return;
	}


	// Loop through each tileset, load name then files
	for ( cnt = 0; cnt < NUM_TILESETS; cnt++ )
	{
		//Read name
//		fread( &zName, sizeof( zName ), 1, hfile );
		FileRead( hfile, &zName, sizeof(zName));

		// Read ambience value
//		fread( &(gTilesets[ cnt ].ubAmbientID), sizeof( UINT8), 1, hfile );
		FileRead(hfile, &gTilesets[cnt].ubAmbientID, sizeof(UINT8));

		// Set into tileset
		swprintf(gTilesets[cnt].zName, lengthof(gTilesets[cnt].zName), L"%s", zName);

		// Loop for files
		for ( cnt2 = 0; cnt2 < uiNumFiles; cnt2++ )
		{
			// Read file name
//			fread( &zName, sizeof( zName ), 1, hfile );
			FileRead(hfile, &zName, sizeof(zName));

			// Set into database
			strcpy( gTilesets[ cnt ].TileSurfaceFilenames[ cnt2 ], zName );

		}

	}

//	fclose( hfile );
	FileClose( hfile );


	// SET CALLBACK FUNTIONS!!!!!!!!!!!!!
	gTilesets[CAVES_1      ].MovementCostFnc = SetTilesetTwoTerrainValues;
	gTilesets[AIRSTRIP     ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[DEAD_AIRSTRIP].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_14      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_18      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_19      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_26      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_27      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_28      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TEMP_29      ].MovementCostFnc = SetTilesetThreeTerrainValues;
	gTilesets[TROPICAL_1   ].MovementCostFnc = SetTilesetFourTerrainValues;
	gTilesets[TEMP_20      ].MovementCostFnc = SetTilesetFourTerrainValues;
}


void SetTilesetOneTerrainValues( )
{
		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;

		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}


static void SetTilesetTwoTerrainValues(void)
{
		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_GROUND;

}


static void SetTilesetThreeTerrainValues(void)
{
		// DIFFERENCE FROM #1 IS THAT ROADS ARE PAVED

		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = PAVED_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = PAVED_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}


static void SetTilesetFourTerrainValues(void)
{
		// DIFFERENCE FROM #1 IS THAT FLOOR2 IS NOT FLAT_FLOOR BUT FLAT_GROUND

		// FIRST TEXUTRES
		gTileSurfaceArray[ FIRSTTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ SECONDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FOURTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ FIFTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SIXTHTEXTURE ]->ubTerrainID = LOW_GRASS;
		gTileSurfaceArray[ SEVENTHTEXTURE ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ REGWATERTEXTURE ]->ubTerrainID = LOW_WATER;
		gTileSurfaceArray[ DEEPWATERTEXTURE ]->ubTerrainID = DEEP_WATER;

		// NOW ROADS
		gTileSurfaceArray[ FIRSTROAD ]->ubTerrainID = DIRT_ROAD;
		gTileSurfaceArray[ ROADPIECES ]->ubTerrainID = DIRT_ROAD;


		// NOW FLOORS
		gTileSurfaceArray[ FIRSTFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ SECONDFLOOR ]->ubTerrainID = FLAT_GROUND;
		gTileSurfaceArray[ THIRDFLOOR ]->ubTerrainID = FLAT_FLOOR;
		gTileSurfaceArray[ FOURTHFLOOR ]->ubTerrainID = FLAT_FLOOR;

		// NOW ANY TERRAIN MODIFYING DEBRIS

}
