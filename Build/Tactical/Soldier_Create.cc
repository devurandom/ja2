#include "Soldier_Create.h"
#include "Overhead.h"
#include "Soldier_Profile.h"
#include "Animation_Control.h"
#include "Animation_Data.h"
#include "Sound_Control.h"
#include "Overhead_Types.h"
#include "Faces.h"
#include "Isometric_Utils.h"
#include "Items.h"
#include "Weapons.h"
#include "OppList.h"
#include "Random.h"
#include "Assignments.h"
#include "Soldier_Init_List.h"
#include "EditorMercs.h"
#include "Smell.h"
#include "Squads.h"
#include "Interface_Panels.h"
#include "StrategicMap.h"
#include "Inventory_Choosing.h"
#include "Queen_Command.h"
#include "Soldier_Add.h"
#include "Quests.h"
#include "Sys_Globals.h"
#include "Scheduling.h"
#include "Rotting_Corpses.h"
#include "Vehicles.h"
#include "Handle_UI.h"
#include "Text.h"
#include "Campaign.h"
#include "GameSettings.h"
#include "PreBattle_Interface.h"
#include "Auto_Resolve.h"
#include "Morale.h"
#include "AI.h"
#include "Strategic_Mines.h"
#include "MapScreen.h"
#include <math.h>
#include "JAScreens.h"
#include "SoundMan.h"
#include "MemMan.h"
#include "Debug.h"
#include "ScreenIDs.h"


// THESE 3 DIFFICULTY FACTORS MUST ALWAYS ADD UP TO 100% EXACTLY!!!
#define DIFF_FACTOR_PLAYER_PROGRESS			50
#define DIFF_FACTOR_PALACE_DISTANCE			30
#define DIFF_FACTOR_GAME_DIFFICULTY			20

// additional difficulty modifiers
#define DIFF_MODIFIER_SOME_PROGRESS			+5
#define DIFF_MODIFIER_NO_INCOME					-5
#define DIFF_MODIFIER_DRASSEN_MILITIA		+10


#define PALACE_SECTOR_X 3
#define PALACE_SECTOR_Y 16

#define MAX_PALACE_DISTANCE		20


BOOLEAN gfProfiledEnemyAdded = FALSE;

UINT32 guiCurrentUniqueSoldierId = 1;

// CJC note: trust me, it's easiest just to put this here; this is the only
// place it should need to be used
UINT8 gubItemDroppableFlag[NUM_INV_SLOTS] =
{
	0x01,
	0x02,
	0x04,
	0,
	0,
	0x08,
	0,
	0x10,
	0x20,
	0x40,
	0x80,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};


void RandomizeNewSoldierStats( SOLDIERCREATE_STRUCT *pCreateStruct )
{
	pCreateStruct->bLifeMax  							= (UINT8)Random(50)+50;
	pCreateStruct->bLife	  							= pCreateStruct->bLifeMax;
	pCreateStruct->bAgility								= (UINT8)Random(50)+50;
	pCreateStruct->bDexterity							= (UINT8)Random(50)+50;
	pCreateStruct->bExpLevel							= 1 + (UINT8)Random(4);

	// Randomize skills (for now)
	pCreateStruct->bMarksmanship					= (UINT8)Random(50)+50;
	pCreateStruct->bMedical								= (UINT8)Random(50)+50;
	pCreateStruct->bMechanical						= (UINT8)Random(50)+50;
	pCreateStruct->bExplosive							= (UINT8)Random(50)+50;
	pCreateStruct->bLeadership						= (UINT8)Random(50)+50;
	pCreateStruct->bStrength							= (UINT8)Random(50)+50;
	pCreateStruct->bWisdom								= (UINT8)Random(50)+50;
	pCreateStruct->bAttitude							= (INT8) Random( MAXATTITUDES );
	pCreateStruct->bOrders								= FARPATROL;
	pCreateStruct->bMorale								= 50;
	pCreateStruct->bAIMorale							= MORALE_FEARLESS;
}


static void CopyProfileItems(SOLDIERTYPE* pSoldier, const SOLDIERCREATE_STRUCT* pCreateStruct);
static void InitSoldierStruct(SOLDIERTYPE* pSoldier);
static void TacticalCopySoldierFromCreateStruct(SOLDIERTYPE* pSoldier, SOLDIERCREATE_STRUCT const* pCreateStruct);
static void TacticalCopySoldierFromProfile(SOLDIERTYPE* pSoldier, SOLDIERCREATE_STRUCT const* pCreateStruct);


SOLDIERTYPE* TacticalCreateSoldier(SOLDIERCREATE_STRUCT const* const pCreateStruct)
try
{
	/* Kris: Huge no no! See the header file for description of static detailed
	 * placements. If this expression ever evaluates to true, then it will expose
	 * serious problems. Simply returning won't help. */
	Assert(!pCreateStruct->fStatic);

	ProfileID const profile = pCreateStruct->ubProfile;
	INT8      const team_id = pCreateStruct->bTeam;

	// Given team, get an ID for this guy!
	SOLDIERTYPE* s;
	SoldierID    id;
	if (guiCurrentScreen == AUTORESOLVE_SCREEN)
	{
		// We are creating a dynamically allocated soldier for autoresolve.
		s  = MALLOC(SOLDIERTYPE);
		id = 255;
	}
	else
	{
		TacticalTeamType const& team = gTacticalStatus.Team[team_id];
		id = team.bFirstID;
		// ATE: If we are a vehicle, and a player, start at a different slot (2 - max)
		if (team_id == gbPlayerNum)
		{
			switch (profile != NO_PROFILE ? GetProfile(profile)->ubBodyType : pCreateStruct->bBodyType)
			{
				case ELDORADO:
				case HUMVEE:
				case ICECREAMTRUCK:
				case JEEP:
					id = team.bLastID - 1;
					break;
			}
		}

		UINT8 const last_id = team.bLastID;
		for (;;)
		{
			s = GetMan(id);
			if (!s->bActive) break;
			if (++id > last_id) return 0;
		}
	}

	// Some values initialized here but could be changed before going to the common one
	InitSoldierStruct(s);
	s->ubID                   = id;
	s->uiUniqueSoldierIdValue = guiCurrentUniqueSoldierId++;

	if (profile != NO_PROFILE)
	{
		TacticalCopySoldierFromProfile(s, pCreateStruct);
	}
	else
	{
		TacticalCopySoldierFromCreateStruct(s, pCreateStruct);
	}

	if (team_id == OUR_TEAM || team_id == PLAYER_PLAN)
	{
		s->uiStatusFlags |= SOLDIER_PC;
		s->bVisible       = 1;
	}
	else
	{
		s->uiStatusFlags |= SOLDIER_ENEMY;
	}

	s->bTeam = team_id;

	// if WE_SEE_WHAT_MILITIA_SEES
	if (team_id == MILITIA_TEAM) s->bVisible = 1;

	if (profile != NO_PROFILE)
	{
		CopyProfileItems(s, pCreateStruct);
		if (team_id == OUR_TEAM) InitSoldierFace(s);
	}

	s->bActionPoints        = CalcActionPoints(s);
	s->bInitialActionPoints = s->bActionPoints;
	s->bSide                = gTacticalStatus.Team[team_id].bSide;
	s->sSectorX             = pCreateStruct->sSectorX;
	s->sSectorY             = pCreateStruct->sSectorY;
	s->bSectorZ             = pCreateStruct->bSectorZ;
	s->ubInsertionDirection = pCreateStruct->bDirection;
	s->bDesiredDirection    = pCreateStruct->bDirection;
	s->bDominantDir         = pCreateStruct->bDirection;
	s->bDirection           = pCreateStruct->bDirection;
	s->sInsertionGridNo     = pCreateStruct->sInsertionGridNo;
	s->bOldLife             = s->bLifeMax;

	if (team_id == CIV_TEAM)
	{
		s->bNeutral =
			profile            == WARDEN        ? FALSE :
			s->ubCivilianGroup == NON_CIV_GROUP ? TRUE  :
			gTacticalStatus.fCivGroupHostile[s->ubCivilianGroup] != CIV_GROUP_HOSTILE;

		// Weaken stats based on the bodytype of the civilian.
		if (profile == NO_PROFILE)
		{
			switch (s->ubBodyType)
			{
				case REGMALE:
				case BIGMALE:
				case STOCKYMALE:
				case REGFEMALE:
					// No adjustments necessary for these "healthy" bodytypes.
					break;
				case FATCIV:
					//fat, so slower
					s->bAgility = 30 + Random(26); //30 - 55
					break;
				case MANCIV:
					s->bLife = s->bLifeMax = 35 + Random(26); //35 - 60
					break;
				case MINICIV:
				case DRESSCIV:
					s->bLife = s->bLifeMax = 30 + Random(16); //30 - 45
					break;
				case HATKIDCIV:
				case KIDCIV:
					s->bLife = s->bLifeMax = 20 + Random(16); //20 - 35
					break;
				case CRIPPLECIV:
					s->bLife = s->bLifeMax = 20 + Random(26); //20 - 45
					s->bAgility = 30 + Random(16); // 30 - 45
					break;
			}
		}
	}
	else if (team_id == CREATURE_TEAM)
	{
		// bloodcats are neutral to start out
		if (s->ubBodyType == BLOODCAT)
		{
			s->bNeutral = TRUE;
		} // otherwise (creatures) false
	}

	/* If not given a profile num, set a randomized default battle sound set and
	 * then adjust it according to body type */
	if (profile == NO_PROFILE)
	{
		s->ubBattleSoundID = Random(3);
	}

	// ATE: TEMP: No enemy women mercs (unless elite)!
	if (profile           == NO_PROFILE &&
			team_id           == ENEMY_TEAM &&
			s->ubBodyType     == REGFEMALE  &&
			s->ubSoldierClass != SOLDIER_CLASS_ELITE)
	{
		s->ubBodyType = REGMALE + Random(3);
	}

	// ATE: Set some values for variation in anims
	if (s->ubBodyType == BIGMALE)
	{
		s->uiAnimSubFlags |= SUB_ANIM_BIGGUYTHREATENSTANCE;
	}

	/* For inventory, look for any face class items that may be located in the big
	 * pockets and if found, move that item to a face slot and clear the pocket!
	 */
	if (team_id != OUR_TEAM)
	{
		bool second_face_item = false;
		for (INT32 i = BIGPOCK1POS; i <= BIGPOCK4POS; ++i)
		{
			OBJECTTYPE& o = s->inv[i];
			if (!(Item[o.usItem].usItemClass & IC_FACE)) continue;

			if (!second_face_item)
			{ /* Don't check for compatibility, automatically assume there are no head
				 * positions filled. */
				second_face_item = true;
				s->inv[HEAD1POS] = o;
				memset(&o, 0, sizeof(o));
			}
			else if (CompatibleFaceItem(s->inv[HEAD1POS].usItem, o.usItem))
			{
				s->inv[HEAD2POS] = o;
				memset(&o, 0, sizeof(o));
				break;
			}
		}

		/* If an army guy has camouflage, roll to determine whether they start
		 * camouflaged */
		if (guiCurrentScreen != AUTORESOLVE_SCREEN &&
				team_id == ENEMY_TEAM                  &&
				FindObj(s, CAMOUFLAGEKIT) != NO_SLOT   &&
				Random(5) < SoldierDifficultyLevel(s))
		{ // Start camouflaged
			s->bCamo = 100;
		}
	}

	/* Set some flags, actions based on what body type we are.
	 * NOTE:  Be very careful what you do in this section!
	 * It is possible to override editor settings, especially orders and attitude.
	 * In those cases, you can check for !gfEditMode (not in editor). */
	switch (s->ubBodyType)
	{
		case HATKIDCIV:
		case KIDCIV:
			s->ubBattleSoundID = Random(2);
			break;

		case REGFEMALE:
		case MINICIV:
		case DRESSCIV:
			s->ubBattleSoundID = 7 + Random(2);
			s->bNormalSmell    = NORMAL_HUMAN_SMELL_STRENGTH;
			break;

		case BLOODCAT:
			AssignCreatureInventory(s);
			s->bNormalSmell   = NORMAL_HUMAN_SMELL_STRENGTH;
			s->uiStatusFlags |= SOLDIER_ANIMAL;
			break;

		case ADULTFEMALEMONSTER:
		case AM_MONSTER:
		case YAF_MONSTER:
		case YAM_MONSTER:
		case LARVAE_MONSTER:
		case INFANT_MONSTER:
		case QUEENMONSTER:
			AssignCreatureInventory(s);
			s->ubCaller = NOBODY;
			if (!gfEditMode)
			{
				s->bOrders   = FARPATROL;
				s->bAttitude = AGGRESSIVE;
			}
			s->uiStatusFlags |= SOLDIER_MONSTER;
			s->bMonsterSmell  = NORMAL_CREATURE_SMELL_STRENGTH;
			break;

		case COW:
			s->uiStatusFlags |= SOLDIER_ANIMAL;
			s->bNormalSmell   = COW_SMELL_STRENGTH;
			break;

		case CROW:
			s->uiStatusFlags |= SOLDIER_ANIMAL;
			break;

		case ROBOTNOWEAPON:
			s->uiStatusFlags |= SOLDIER_ROBOT;
			break;

		case HUMVEE:
		case ELDORADO:
		case ICECREAMTRUCK:
		case JEEP:
		case TANK_NW:
		case TANK_NE:
		{
			s->uiStatusFlags |= SOLDIER_VEHICLE;

			UINT8 ubVehicleID = 0;
			switch (s->ubBodyType)
			{
				case HUMVEE:
					ubVehicleID = HUMMER;
					s->bNeutral = TRUE;
					break;

				case ELDORADO:
					ubVehicleID = ELDORADO_CAR;
					s->bNeutral = TRUE;
					break;

				case ICECREAMTRUCK:
					ubVehicleID = ICE_CREAM_TRUCK;
					s->bNeutral = TRUE;
					break;

				case JEEP:
					ubVehicleID = JEEP_CAR;
					break;

				case TANK_NW:
				case TANK_NE:
					ubVehicleID = TANK_CAR;
					break;
			}

			s->bVehicleID =
				pCreateStruct->fUseGivenVehicle ? pCreateStruct->bUseGivenVehicleID :
				(INT8)AddVehicleToList(s->sSectorX, s->sSectorY, s->bSectorZ, ubVehicleID);
			SetVehicleValuesIntoSoldierType(s);
			break;
		}

		default:
			s->bNormalSmell = NORMAL_HUMAN_SMELL_STRENGTH;
			break;
	}

	if (guiCurrentScreen == AUTORESOLVE_SCREEN)
	{
		UINT8 const sector_id = GetAutoResolveSectorID();
		s->sSectorX = SECTORX(sector_id);
		s->sSectorY = SECTORY(sector_id);
		s->bSectorZ = 0;
		return s;
	}

	Assert(s->usAnimState == STANDING);

	// We are set to create the merc, stuff after here can fail
	CreateSoldierCommon(s);

	if (pCreateStruct->fOnRoof && FlatRoofAboveGridNo(pCreateStruct->sInsertionGridNo))
	{
		SetSoldierHeight(s, 58.0);
	}

	AddManToTeam(team_id);
	return s;
}
catch (...) { return 0; }


SOLDIERTYPE* TacticalCreateSoldierFromExisting(const SOLDIERTYPE* const existing)
{
	Assert(guiCurrentScreen != AUTORESOLVE_SCREEN);
	Assert(existing->face  == NULL);
	Assert(existing->light == NULL);

	SOLDIERTYPE* const s = GetMan(existing->ubID);
	*s = *existing;

	if (s->ubProfile != NO_PROFILE &&
			s->bTeam     == OUR_TEAM   &&
			!(s->uiStatusFlags & SOLDIER_VEHICLE))
	{
		InitSoldierFace(s);
	}

	if (s->ubBodyType == HUMVEE || s->ubBodyType == ICECREAMTRUCK)
	{
		s->bNeutral = TRUE;
	}

	CreateSoldierCommon(s);
	return s;
}


static void TacticalCopySoldierFromProfile(SOLDIERTYPE* const pSoldier, SOLDIERCREATE_STRUCT const* const pCreateStruct)
{
	UINT8						ubProfileIndex;
	MERCPROFILESTRUCT * pProfile;

	ubProfileIndex = pCreateStruct->ubProfile;
	pProfile = &(gMercProfiles[ubProfileIndex]);

	SET_PALETTEREP_ID ( pSoldier->HeadPal,		pProfile->HAIR );
	SET_PALETTEREP_ID ( pSoldier->VestPal,		pProfile->VEST );
	SET_PALETTEREP_ID ( pSoldier->SkinPal,		pProfile->SKIN );
	SET_PALETTEREP_ID ( pSoldier->PantsPal,   pProfile->PANTS );

	// Set profile index!
	pSoldier->ubProfile									= ubProfileIndex;
	pSoldier->ubScheduleID							= pCreateStruct->ubScheduleID;
	pSoldier->bHasKeys									= pCreateStruct->fHasKeys;

	wcscpy( pSoldier->name, pProfile->zNickname );

	pSoldier->bLife 										= pProfile->bLife;
	pSoldier->bLifeMax									= pProfile->bLifeMax;
	pSoldier->bAgility									= pProfile->bAgility;
	pSoldier->bLeadership								= pProfile->bLeadership;
	pSoldier->bDexterity								= pProfile->bDexterity;
	pSoldier->bStrength									= pProfile->bStrength;
	pSoldier->bWisdom										= pProfile->bWisdom;
	pSoldier->bExpLevel									= pProfile->bExpLevel;
	pSoldier->bMarksmanship							= pProfile->bMarksmanship;
	pSoldier->bMedical									= pProfile->bMedical;
	pSoldier->bMechanical								= pProfile->bMechanical;
	pSoldier->bExplosive								= pProfile->bExplosive;

	pSoldier->uiAnimSubFlags						= pProfile->uiBodyTypeSubFlags;
	pSoldier->ubBodyType								= pProfile->ubBodyType;
	pSoldier->ubCivilianGroup						= pProfile->ubCivilianGroup;
	//OK set initial duty
//  pSoldier->bAssignment=ON_DUTY;

	pSoldier->ubSkillTrait1 = pProfile->bSkillTrait;
	pSoldier->ubSkillTrait2 = pProfile->bSkillTrait2;

	pSoldier->bOrders								= pCreateStruct->bOrders;
	pSoldier->bAttitude							= pCreateStruct->bAttitude;
	pSoldier->bDirection						= pCreateStruct->bDirection;
	pSoldier->bPatrolCnt						= pCreateStruct->bPatrolCnt;
	memcpy( pSoldier->usPatrolGrid, pCreateStruct->sPatrolGrid, sizeof( INT16 ) * MAXPATROLGRIDS );

	if ( HAS_SKILL_TRAIT( pSoldier, CAMOUFLAGED ) )
	{
		// set camouflaged to 100 automatically
		pSoldier->bCamo = 100;
	}
}


enum { PINKSKIN, TANSKIN, DARKSKIN, BLACKSKIN, NUMSKINS };
enum {
	WHITEHEAD, BLACKHEAD, //black skin (only this line )
	BROWNHEAD,						//dark skin (this line plus all above)
	BLONDEHEAD, REDHEAD,   //pink/tan skin (this line plus all above )
	NUMHEADS
};


static INT32 ChooseHairColor(SOLDIERTYPE* pSoldier, INT32 skin)
{
	INT32 iRandom;
	INT32 hair = 0;
	iRandom = Random( 100 );
	switch( skin )
	{
		case PINKSKIN:
		case TANSKIN:
			if( iRandom < 12 )
			{
				hair = REDHEAD;
			}
			else if( iRandom < 34 )
			{
				hair = BLONDEHEAD;
			}
			else if( iRandom < 60 )
			{
				hair = BROWNHEAD;
			}
			else if( iRandom < 92 )
			{
				hair = BLACKHEAD;
			}
			else
			{
				hair = WHITEHEAD;
				if( pSoldier->ubBodyType == REGFEMALE ||
					  pSoldier->ubBodyType == MINICIV ||
						pSoldier->ubBodyType == DRESSCIV ||
						pSoldier->ubBodyType == HATKIDCIV ||
						pSoldier->ubBodyType == KIDCIV )
				{
					hair = Random( NUMHEADS - 1 ) + 1;
				}
			}
			hair = Random( NUMHEADS );
			break;
		case DARKSKIN:
			if( iRandom < 35 )
			{
				hair = BROWNHEAD;
			}
			else if( iRandom < 84 )
			{
				hair = BLACKHEAD;
			}
			else
			{
				hair = WHITEHEAD;
				if( pSoldier->ubBodyType == REGFEMALE ||
					  pSoldier->ubBodyType == MINICIV ||
						pSoldier->ubBodyType == DRESSCIV ||
						pSoldier->ubBodyType == HATKIDCIV ||
						pSoldier->ubBodyType == KIDCIV )
				{
					hair = Random( 2 ) + 1;
				}
			}
			break;
		case BLACKSKIN:
			if( iRandom < 84 )
			{
				hair = BLACKHEAD;
			}
			else
			{
				hair = WHITEHEAD;
				if( pSoldier->ubBodyType == REGFEMALE ||
					  pSoldier->ubBodyType == MINICIV ||
						pSoldier->ubBodyType == DRESSCIV ||
						pSoldier->ubBodyType == HATKIDCIV ||
						pSoldier->ubBodyType == KIDCIV )
				{
					hair = BLACKHEAD;
				}
			}
			break;
		default:
			AssertMsg( 0, "Skin type not accounted for." );
			break;
	}
	if( pSoldier->ubBodyType == CRIPPLECIV )
	{
		if( Chance( 50 ) )
		{
			hair = WHITEHEAD;
		}
	}
	return hair;
}


static void GeneratePaletteForSoldier(SOLDIERTYPE* pSoldier, UINT8 ubSoldierClass)
{
	INT32 skin, hair;
	BOOLEAN fMercClothingScheme;
	hair = -1;

	//choose random skin tone which will limit the choice of hair colors.
	skin = (INT8)Random( NUMSKINS );
	switch( skin )
	{
		case PINKSKIN:
			SET_PALETTEREP_ID( pSoldier->SkinPal,  "PINKSKIN" );
			break;
		case TANSKIN:
			SET_PALETTEREP_ID( pSoldier->SkinPal,  "TANSKIN" );
			break;
		case DARKSKIN:
			SET_PALETTEREP_ID( pSoldier->SkinPal,  "DARKSKIN" );
			break;
		case BLACKSKIN:
			SET_PALETTEREP_ID( pSoldier->SkinPal,  "BLACKSKIN" );
			break;
		default:
			AssertMsg( 0, "Skin type not accounted for." );
			break;
	}

	//Choose hair color which uses the skin color to limit choices
	hair = ChooseHairColor( pSoldier, skin );
	switch( hair )
	{
		case BROWNHEAD: SET_PALETTEREP_ID( pSoldier->HeadPal, "BROWNHEAD" ); break;
		case BLACKHEAD: SET_PALETTEREP_ID( pSoldier->HeadPal, "BLACKHEAD" ); break;
		case WHITEHEAD: SET_PALETTEREP_ID( pSoldier->HeadPal, "WHITEHEAD" ); break;
		case BLONDEHEAD:SET_PALETTEREP_ID( pSoldier->HeadPal, "BLONDHEAD" ); break;
		case REDHEAD:   SET_PALETTEREP_ID( pSoldier->HeadPal, "REDHEAD"   ); break;
		default:  AssertMsg( 0, "Hair type not accounted for.");						 break;
	}

	// OK, After skin, hair we could have a forced color scheme.. use here if so
	switch( ubSoldierClass )
	{
		case SOLDIER_CLASS_ADMINISTRATOR:
			SET_PALETTEREP_ID( pSoldier->VestPal, "YELLOWVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "GREENPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_ELITE:
			SET_PALETTEREP_ID( pSoldier->VestPal, "BLACKSHIRT"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "BLACKPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_ARMY:
			SET_PALETTEREP_ID( pSoldier->VestPal, "REDVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "GREENPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_GREEN_MILITIA:
			SET_PALETTEREP_ID( pSoldier->VestPal, "GREENVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "BEIGEPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_REG_MILITIA:
			SET_PALETTEREP_ID( pSoldier->VestPal, "JEANVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "BEIGEPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_ELITE_MILITIA:
			SET_PALETTEREP_ID( pSoldier->VestPal, "BLUEVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "BEIGEPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
		case SOLDIER_CLASS_MINER:
			SET_PALETTEREP_ID( pSoldier->VestPal, "greyVEST"  );
			SET_PALETTEREP_ID( pSoldier->PantsPal, "BEIGEPANTS"   );
			pSoldier->ubSoldierClass = ubSoldierClass;
			return;
	}

	//there are 2 clothing schemes, 1 for mercs and 1 for civilians.  The
	//merc clothing scheme is much larger and general and is an exclusive superset
	//of the civilian clothing scheme which means the civilians will choose the
	//merc clothing scheme often ( actually 60% of the time ).
	if( !pSoldier->PantsPal[0] || !pSoldier->VestPal[0] )
	{
		fMercClothingScheme = TRUE;
		if( pSoldier->bTeam == CIV_TEAM && Random( 100 ) < 40 )
		{ //40% chance of using cheezy civilian colors
			fMercClothingScheme = FALSE;
		}
		if( !fMercClothingScheme ) //CHEEZY CIVILIAN COLORS
		{
			if( Random( 100 ) < 30 )
			{ //30% chance that the civilian will choose a gaudy yellow shirt with pants.
				SET_PALETTEREP_ID( pSoldier->VestPal, "GYELLOWSHIRT" );
				switch( Random( 3 ) )
				{
					case 0:	SET_PALETTEREP_ID( pSoldier->PantsPal, "TANPANTS"   ); break;
					case 1: SET_PALETTEREP_ID( pSoldier->PantsPal, "BEIGEPANTS" ); break;
					case 2: SET_PALETTEREP_ID( pSoldier->PantsPal, "GREENPANTS" ); break;
				}
			}
			else
			{ //70% chance that the civilian will choose jeans with a shirt.
				SET_PALETTEREP_ID( pSoldier->PantsPal, "JEANPANTS" );
				switch( Random( 7 ) )
				{
					case 0:	SET_PALETTEREP_ID( pSoldier->VestPal, "WHITEVEST"   ); break;
					case 1: SET_PALETTEREP_ID( pSoldier->VestPal, "BLACKSHIRT"  ); break;
					case 2: SET_PALETTEREP_ID( pSoldier->VestPal, "PURPLESHIRT" ); break;
					case 3: SET_PALETTEREP_ID( pSoldier->VestPal, "BLUEVEST"    ); break;
					case 4: SET_PALETTEREP_ID( pSoldier->VestPal, "BROWNVEST"   ); break;
					case 5: SET_PALETTEREP_ID( pSoldier->VestPal, "JEANVEST"    ); break;
					case 6: SET_PALETTEREP_ID( pSoldier->VestPal, "REDVEST"     ); break;
				}
			}
			return;
		}
		//MERC COLORS
		switch( Random( 3 ) )
		{
			case 0:
				SET_PALETTEREP_ID( pSoldier->PantsPal, "GREENPANTS" );
				switch( Random( 4 ) )
				{
					case 0: SET_PALETTEREP_ID( pSoldier->VestPal, "YELLOWVEST" ); break;
					case 1: SET_PALETTEREP_ID( pSoldier->VestPal, "WHITEVEST"  ); break;
					case 2: SET_PALETTEREP_ID( pSoldier->VestPal, "BROWNVEST"  ); break;
					case 3: SET_PALETTEREP_ID( pSoldier->VestPal, "GREENVEST"  ); break;
				}
				break;
			case 1:
				SET_PALETTEREP_ID( pSoldier->PantsPal, "TANPANTS" );
				switch( Random( 8 ) )
				{
					case 0: SET_PALETTEREP_ID( pSoldier->VestPal, "YELLOWVEST" ); break;
					case 1: SET_PALETTEREP_ID( pSoldier->VestPal, "WHITEVEST"  ); break;
					case 2: SET_PALETTEREP_ID( pSoldier->VestPal, "BLACKSHIRT" ); break;
					case 3: SET_PALETTEREP_ID( pSoldier->VestPal, "BLUEVEST"   ); break;
					case 4: SET_PALETTEREP_ID( pSoldier->VestPal, "BROWNVEST"  ); break;
					case 5: SET_PALETTEREP_ID( pSoldier->VestPal, "GREENVEST"  ); break;
					case 6: SET_PALETTEREP_ID( pSoldier->VestPal, "JEANVEST"   ); break;
					case 7: SET_PALETTEREP_ID( pSoldier->VestPal, "REDVEST"    ); break;
				}
				break;
			case 2:
				SET_PALETTEREP_ID( pSoldier->PantsPal, "BLUEPANTS" );
				switch( Random( 4 ) )
				{
					case 0: SET_PALETTEREP_ID( pSoldier->VestPal, "YELLOWVEST" ); break;
					case 1: SET_PALETTEREP_ID( pSoldier->VestPal, "WHITEVEST"  ); break;
					case 2: SET_PALETTEREP_ID( pSoldier->VestPal, "REDVEST"    ); break;
					case 3: SET_PALETTEREP_ID( pSoldier->VestPal, "BLACKSHIRT" ); break;
				}
				break;
		}
	}
}


static void TacticalCopySoldierFromCreateStruct(SOLDIERTYPE* const pSoldier, SOLDIERCREATE_STRUCT const* const pCreateStruct)
{
	pSoldier->ubProfile							= NO_PROFILE;

	// Randomize attributes
	pSoldier->bLife	  							= pCreateStruct->bLife;
	pSoldier->bLifeMax  						= pCreateStruct->bLifeMax;
	pSoldier->bAgility							= pCreateStruct->bAgility;
	pSoldier->bDexterity						= pCreateStruct->bDexterity;
	pSoldier->bExpLevel							= pCreateStruct->bExpLevel;

	pSoldier->bMarksmanship					= pCreateStruct->bMarksmanship;
	pSoldier->bMedical							= pCreateStruct->bMedical;
	pSoldier->bMechanical						= pCreateStruct->bMechanical;
	pSoldier->bExplosive						= pCreateStruct->bExplosive;
	pSoldier->bLeadership						= pCreateStruct->bLeadership;
	pSoldier->bStrength							= pCreateStruct->bStrength;
	pSoldier->bWisdom								= pCreateStruct->bWisdom;

	pSoldier->bAttitude							= pCreateStruct->bAttitude;
	pSoldier->bOrders								= pCreateStruct->bOrders;
	pSoldier->bMorale								= pCreateStruct->bMorale;
	pSoldier->bAIMorale							= pCreateStruct->bAIMorale;
	pSoldier->ubBodyType						= pCreateStruct->bBodyType;
	pSoldier->ubCivilianGroup				= pCreateStruct->ubCivilianGroup;

	pSoldier->ubScheduleID					= pCreateStruct->ubScheduleID;
	pSoldier->bHasKeys							= pCreateStruct->fHasKeys;
	pSoldier->ubSoldierClass				= pCreateStruct->ubSoldierClass;

	if( pCreateStruct->fVisible )
	{
		strcpy(pSoldier->HeadPal,  pCreateStruct->HeadPal);
		strcpy(pSoldier->PantsPal, pCreateStruct->PantsPal);
		strcpy(pSoldier->VestPal,  pCreateStruct->VestPal);
		strcpy(pSoldier->SkinPal,  pCreateStruct->SkinPal);
	}

	//KM:  March 25, 1999
	//Assign nightops traits to enemies/militia
	if( pSoldier->ubSoldierClass == SOLDIER_CLASS_ELITE || pSoldier->ubSoldierClass == SOLDIER_CLASS_ELITE_MILITIA )
	{
		INT32 iChance;
		UINT8	ubProgress;

		ubProgress = HighestPlayerProgressPercentage();

		if ( ubProgress < 60 )
		{
			// ramp chance from 40 to 80% over the course of 60% progress
			// 60 * 2/3 = 40, and 40+40 = 80
			iChance = 40 + (ubProgress * 2) / 3;
		}
		else
		{
			iChance = 80;
		}

		if ( Chance( iChance ) )
		{
			pSoldier->ubSkillTrait1 = NIGHTOPS;
			if ( ubProgress >= 40 && Chance( 30 ) )
			{
				pSoldier->ubSkillTrait2 = NIGHTOPS;
			}
		}
	}
	else if( pSoldier->ubSoldierClass == SOLDIER_CLASS_ARMY || pSoldier->ubSoldierClass == SOLDIER_CLASS_REG_MILITIA )
	{
		INT32 iChance;
		UINT8	ubProgress;

		ubProgress = HighestPlayerProgressPercentage();

		if ( ubProgress < 60 )
		{
			// ramp chance from 0 to 40% over the course of 60% progress
			// 60 * 2/3 = 40
			iChance = (ubProgress * 2) / 3;
		}
		else
		{
			iChance = 40;
		}

		if ( Chance( iChance ) )
		{
			pSoldier->ubSkillTrait1 = NIGHTOPS;
			if ( ubProgress >= 50 && Chance( 20 ) )
			{
				pSoldier->ubSkillTrait2 = NIGHTOPS;
			}
		}
	}

	//KM:  November 10, 1997
	//Adding patrol points
	//CAUTION:  CONVERTING SIGNED TO UNSIGNED though the values should never be negative.
	pSoldier->bPatrolCnt						= pCreateStruct->bPatrolCnt;
	memcpy( pSoldier->usPatrolGrid, pCreateStruct->sPatrolGrid, sizeof( INT16 ) * MAXPATROLGRIDS );

	//Kris:  November 10, 1997
	//Expanded the default names based on team.
	const wchar_t* Name;
	switch( pCreateStruct->bTeam )
	{
		case ENEMY_TEAM:    Name = TacticalStr[ENEMY_TEAM_MERC_NAME];   break;
		case MILITIA_TEAM:  Name = TacticalStr[MILITIA_TEAM_MERC_NAME]; break;

		case CIV_TEAM:
			if( pSoldier->ubSoldierClass == SOLDIER_CLASS_MINER )
			{
				Name = TacticalStr[CIV_TEAM_MINER_NAME];
			}
			else if (pSoldier->ubBodyType == CROW)
			{
				Name = TacticalStr[CROW_HIT_LOCATION_STR];
			}
			else
			{
				Name = TacticalStr[CIV_TEAM_MERC_NAME];
			}
			break;

		case CREATURE_TEAM:
			if( pSoldier->ubBodyType == BLOODCAT )
			{
				Name = gzLateLocalizedString[36];
			}
			else
			{
				Name = TacticalStr[CREATURE_TEAM_MERC_NAME];
			}
			break;

		default: goto no_name; // XXX fishy
	}
	wcslcpy(pSoldier->name, Name, lengthof(pSoldier->name));
no_name:

	//Generate colors for soldier based on the body type.
	GeneratePaletteForSoldier( pSoldier, pCreateStruct->ubSoldierClass );

	// Copy item info over
	memcpy( pSoldier->inv, pCreateStruct->Inv, sizeof( OBJECTTYPE ) * NUM_INV_SLOTS );
}


static void InitSoldierStruct(SOLDIERTYPE* pSoldier)
{
	// Memset values
	memset( pSoldier, 0, sizeof( SOLDIERTYPE ) );

	// Set default values
	pSoldier->bVisible							= -1;
	pSoldier->face = NULL;

	// Set morale default
	pSoldier->bMorale								= DEFAULT_MORALE;

	pSoldier->attacker                  = NULL;
	pSoldier->previous_attacker         = NULL;
	pSoldier->next_to_previous_attacker = NULL;

	pSoldier->light = NULL;

	// Set update time to new speed
	pSoldier->ubDesiredHeight				= NO_DESIRED_HEIGHT;
	pSoldier->bViewRange						= NORMAL_VIEW_RANGE;
	pSoldier->bInSector							= FALSE;
	pSoldier->sGridNo								= NO_MAP_POS;
	pSoldier->muzzle_flash          = NULL;
	pSoldier->usPendingAnimation		= NO_PENDING_ANIMATION;
	pSoldier->ubPendingStanceChange	= NO_PENDING_STANCE;
	pSoldier->ubPendingDirection		= NO_PENDING_DIRECTION;
	pSoldier->ubPendingAction				= NO_PENDING_ACTION;
	pSoldier->bLastRenderVisibleValue	= -1;
	pSoldier->bBreath								= 99;
	pSoldier->bBreathMax						= 100;
	pSoldier->bActive									= TRUE;
	pSoldier->fShowLocator						= FALSE;
	pSoldier->sLastTarget							= NOWHERE;
	pSoldier->sAbsoluteFinalDestination = NOWHERE;
	pSoldier->sZLevelOverride					= -1;
	pSoldier->service_partner 				= NULL;
	pSoldier->ubAttackingHand					= HANDPOS;
	pSoldier->usAnimState							= STANDING;
	pSoldier->bInterruptDuelPts				= NO_INTERRUPT;
	pSoldier->bMoved									= FALSE;
	pSoldier->robot_remote_holder  		= NULL;
	pSoldier->sNoiseGridno						= NOWHERE;
	pSoldier->ubPrevSectorID					= 255;
	pSoldier->bNextPatrolPnt					= 1;
	pSoldier->bCurrentCivQuote				= -1;
	pSoldier->bCurrentCivQuoteDelta		= 0;
	pSoldier->uiBattleSoundID					= NO_SAMPLE;
	pSoldier->xrayed_by 							= NULL;
	pSoldier->uiXRayActivatedTime			= 0;
	pSoldier->bBulletsLeft						= 0;
	pSoldier->bVehicleUnderRepairID		= -1;
}


void InternalTacticalRemoveSoldier(SOLDIERTYPE* const s, BOOLEAN const fRemoveVehicle)
{
	if (GetSelectedMan() == s) SetSelectedMan(0);
	if (gUIFullTarget    == s) gUIFullTarget   = 0;
	if (gpSMCurrentMerc  == s) gpSMCurrentMerc = 0;

	if (!s->bActive) return;

	if (s->ubScheduleID) DeleteSchedule(s->ubScheduleID);

	if (s->uiStatusFlags & SOLDIER_VEHICLE && fRemoveVehicle)
	{
		RemoveVehicleFromList(GetVehicle(s->bVehicleID));
	}

	if (s->ubBodyType == CROW) HandleCrowLeave(s);

	if (guiCurrentScreen != AUTORESOLVE_SCREEN)
	{
		RemoveCharacterFromSquads(s);
		RemovePlayerFromTeamSlot(s);
		RemoveSoldierFromGridNo(s);

		// Delete shadow of crow....
		if (s->pAniTile)
		{
			DeleteAniTile(s->pAniTile);
			s->pAniTile = 0;
		}

		if (!(s->uiStatusFlags & SOLDIER_OFF_MAP))
		{
			// Decrement men in sector number!
			RemoveManFromTeam(s->bTeam);
		} // people specified off-map have already been removed from their team count

		DeleteSoldier(s);
	}
	else
	{
		if (gfPersistantPBI) DeleteSoldier(s);
		MemFree(s);
	}
}


void TacticalRemoveSoldier(SOLDIERTYPE* const s)
{
	InternalTacticalRemoveSoldier(s, TRUE);
}


static UINT8 GetLocationModifier(UINT8 ubSoldierClass);


// returns a soldier difficulty modifier from 0 to 100 based on player's progress, distance from the Palace, mining income, and
// playing difficulty level.  Used for generating soldier stats, equipment, and AI skill level.
INT8 CalcDifficultyModifier( UINT8 ubSoldierClass )
{
	INT8 bDiffModifier = 0;
	UINT8 ubProgress;
	UINT8 ubProgressModifier;


	if( gfEditMode )
	{
		// return an average rating for editor purposes
		return( 50 );
	}


	// THESE 3 DIFFICULTY FACTORS MUST ALWAYS ADD UP TO 100% EXACTLY!!!
	Assert( ( DIFF_FACTOR_PLAYER_PROGRESS	+ DIFF_FACTOR_PALACE_DISTANCE	+ DIFF_FACTOR_GAME_DIFFICULTY ) == 100 );


	// adjust for game difficulty level
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			// very strong militia, very weak enemies/cratures/bloodcats
			if ( SOLDIER_CLASS_MILITIA( ubSoldierClass ) )
			{
				// +20
				bDiffModifier += DIFF_FACTOR_GAME_DIFFICULTY;
			}
			break;

		case DIF_LEVEL_MEDIUM:
			// equally strong militia, enemies, creatures, bloodcats (+10)
			bDiffModifier += ( DIFF_FACTOR_GAME_DIFFICULTY / 2 );
			break;

		case DIF_LEVEL_HARD:
			// equally stronger militia/enemies/creatures/bloodcats (+20)
			bDiffModifier += DIFF_FACTOR_GAME_DIFFICULTY;
			break;
	}


	// the progress returned ranges from 0 to 100
	ubProgress = HighestPlayerProgressPercentage();

	// bump it a bit once we've accomplished something (killed some enemies or taken an important sector)
	if ( ubProgress > 0 )
	{
		// +5
		bDiffModifier += DIFF_MODIFIER_SOME_PROGRESS;
	}

	// drop it down a bit if we still don't have any mine income
	if ( PredictIncomeFromPlayerMines() == 0 )
	{
		// -5
		bDiffModifier += DIFF_MODIFIER_NO_INCOME;
	}


	// adjust for progress level (0 to +50)
	ubProgressModifier = ( ubProgress * DIFF_FACTOR_PLAYER_PROGRESS ) / 100;
	bDiffModifier += ubProgressModifier;


	// adjust for map location
	bDiffModifier += GetLocationModifier( ubSoldierClass );


	// should be no way to go over 100, although it's possible to go below 0 when just starting on easy
	// Assert( bDiffModifier <= 100 );

	// limit the range of the combined factors to between 0 and 100
	bDiffModifier = __max(   0, bDiffModifier );
	bDiffModifier = __min( 100, bDiffModifier );

	// DON'T change this function without carefully considering the impact on GenerateRandomEquipment(),
	// CreateDetailedPlacementGivenBasicPlacementInfo(), and SoldierDifficultyLevel().

	return(bDiffModifier);
}


static void ReduceHighExpLevels(INT8* pbExpLevel);


//When the editor modifies the soldier's relative attribute level,
//this function is called to update that information.
//Used to generate a detailed placement from a basic placement.  This assumes that the detailed placement
//doesn't exist, meaning there are no static attributes.  This is called when you wish to convert a basic
//placement into a detailed placement just before creating a soldier.
void CreateDetailedPlacementGivenBasicPlacementInfo( SOLDIERCREATE_STRUCT *pp, BASIC_SOLDIERCREATE_STRUCT *bp )
{
	INT8 bBaseAttribute;
	UINT8 ubSoldierClass;
	UINT8 ubDiffFactor;
	INT8 bExpLevelModifier;
	INT8 bStatsModifier;
	UINT8 ubStatsLevel;


	if( !pp || !bp )
		return;
	pp->fStatic								= FALSE;
	pp->ubProfile							= NO_PROFILE;
	pp->sInsertionGridNo			= bp->usStartingGridNo;
	pp->fCopyProfileItemsOver = FALSE;
	pp->bTeam									= bp->bTeam;
	pp->ubSoldierClass				= bp->ubSoldierClass;
	pp->ubCivilianGroup				= bp->ubCivilianGroup;
	pp->ubScheduleID					= 0;
	pp->sSectorX							= gWorldSectorX;
	pp->sSectorY							= gWorldSectorY;
	pp->bSectorZ							= gbWorldSectorZ;
	pp->fHasKeys							= bp->fHasKeys;

	//Choose a body type randomly if none specified.
	if( bp->bBodyType < 0 )
	{
		switch ( bp->bTeam )
		{
			case OUR_TEAM:
			case ENEMY_TEAM:
			case MILITIA_TEAM:
				switch( Random( 4 ) )
				{
					case 0:	pp->bBodyType = REGMALE;		break;
					case 1:	pp->bBodyType = BIGMALE;		break;
					case 2:	pp->bBodyType = STOCKYMALE;	break;
					case 3:	pp->bBodyType = REGFEMALE;	break;
				}
				break;
			case CIV_TEAM:
				if( pp->ubSoldierClass == SOLDIER_CLASS_MINER )
				{
					switch( Random( 3 ) )
					{ //only strong and fit men can be miners.
						case 0:	pp->bBodyType = REGMALE;	break;
						case 1:	pp->bBodyType = BIGMALE;	break;
						case 2: pp->bBodyType = MANCIV;		break;
					}
				}
				else
				{
					INT32 iRandom;
					iRandom = Random( 100 );
					if( iRandom < 8 )
					{ //8% chance FATCIV
						pp->bBodyType = FATCIV;
					}
					else if( iRandom < 38 )
					{ //30% chance MANCIV
						pp->bBodyType = MANCIV;
					}
					else if( iRandom < 57 )
					{ //19% chance MINICIV
						pp->bBodyType = MINICIV;
					}
					else if( iRandom < 76 )
					{ //19% chance DRESSCIV
						pp->bBodyType = DRESSCIV;
					}
					else if( iRandom < 88 )
					{ //12% chance HATKIDCIV
						pp->bBodyType = HATKIDCIV;
					}
					else
					{ //12% chance KIDCIV
						pp->bBodyType = KIDCIV;
					}
				}
				break;
		}
	}
	else
	{
		pp->bBodyType = bp->bBodyType;
	}

	//Pass over mandatory information specified from the basic placement
	pp->bOrders = bp->bOrders;
	pp->bAttitude = bp->bAttitude;
	pp->bDirection = bp->bDirection;


	// determine this soldier's soldier class
	if( bp->bTeam == CREATURE_TEAM )
	{
		ubSoldierClass = SOLDIER_CLASS_CREATURE;
	}
	else
	{
		ubSoldierClass = bp->ubSoldierClass;
	}

	ubDiffFactor = CalcDifficultyModifier( ubSoldierClass );


	// experience level is modified by game difficulty, player's progress, & proximity to Queen's Palace, etc.
	// This formula gives the following results:
	//	DIFFICULTY FACTOR			EXP. LEVEL  MODIFIER		LEVEL OF AVG REGULAR TROOP
	//			   0 to 19									-2													2
	//			  20 to 39									-1													3
	//			  41 to 59									-0													4
	//				60 to 79									+1													5
	//				80 to 99									+2													6
	//				  100											+3													7		(can happen in P3 Meduna itself on HARD only!)
	bExpLevelModifier = ( ubDiffFactor / 20 ) - 2;

	// if in the upper half of this difficulty rating (10-19, 30-39, 50-59, 70-79, and 90-99)
	if ( ( ubDiffFactor % 20 ) >= 10 )
	{
		// increase stats only by one level's worth
		bStatsModifier = +1;
	}
	else
	{
		// normal stats for this level
		bStatsModifier = 0;
	}

	// Adjust level and statistics for Linda's prespecified relative attribute level
	switch ( bp->bRelativeAttributeLevel )
	{
		// NOTE: bStatsModifier already includes the bExpLevelModifier since it's based on the level itself!
		case 0:	bExpLevelModifier += -1; bStatsModifier += -1; break;
		case 1:	bExpLevelModifier += -1; bStatsModifier +=  0; break;
		case 2:	bExpLevelModifier +=  0; bStatsModifier +=  0; break;
		case 3:	bExpLevelModifier += +1; bStatsModifier +=  0; break;
		case 4:	bExpLevelModifier += +1; bStatsModifier += +1; break;

		default:
			AssertMsg( FALSE, String( "Invalid bRelativeAttributeLevel = %d", bp->bRelativeAttributeLevel ) );
			break;
	}


	//Set the experience level based on the soldier class and exp level modifier or relative attribute level
	switch( ubSoldierClass )
	{
		case SOLDIER_CLASS_ADMINISTRATOR:
			pp->bExpLevel = (INT8) 2 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_ARMY:
			pp->bExpLevel = (INT8) 4 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_ELITE:
			pp->bExpLevel = (INT8) 6 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_GREEN_MILITIA:
			pp->bExpLevel = (INT8) 2 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_REG_MILITIA:
			pp->bExpLevel = (INT8) 4 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_ELITE_MILITIA:
			pp->bExpLevel = (INT8) 6 + bExpLevelModifier;
			break;
		case SOLDIER_CLASS_MINER:
			pp->bExpLevel = bp->bRelativeAttributeLevel;					 //avg 2 (1-4)
			break;

		case SOLDIER_CLASS_CREATURE:
			switch( bp->bBodyType )
			{
				case LARVAE_MONSTER:
					pp->bExpLevel = 1 + bExpLevelModifier;
					break;
				case INFANT_MONSTER:
					pp->bExpLevel = 2 + bExpLevelModifier;
					break;
				case YAF_MONSTER:
				case YAM_MONSTER:
					pp->bExpLevel = (INT8)(3 + Random( 2 ) + bExpLevelModifier ); //3-4
					break;
				case ADULTFEMALEMONSTER:
				case AM_MONSTER:
					pp->bExpLevel = (INT8)(5 + Random( 2 ) + bExpLevelModifier ); //5-6
					break;
				case QUEENMONSTER:
					pp->bExpLevel = 7 + bExpLevelModifier;
					break;

				case BLOODCAT:
					pp->bExpLevel = 5 + bExpLevelModifier;
					if( SECTOR( gWorldSectorX, gWorldSectorY ) == SEC_I16 )
					{
						pp->bExpLevel += gGameOptions.ubDifficultyLevel;
					}
					break;
			}
			break;

		default:
			pp->bExpLevel = bp->bRelativeAttributeLevel;					 //avg 2 (1-4)
			ubSoldierClass = SOLDIER_CLASS_NONE;
			break;
	}


	pp->bExpLevel = MAX( 1, pp->bExpLevel ); //minimum exp. level of 1
	pp->bExpLevel = MIN( 9, pp->bExpLevel ); //maximum exp. level of 9

	ubStatsLevel = pp->bExpLevel + bStatsModifier;
	#if 0 /* unsigned < 0 ? */
	ubStatsLevel = MAX( 0, ubStatsLevel );	//minimum stats level of 0
	#else
	ubStatsLevel = ubStatsLevel;	//minimum stats level of 0
	#endif
	ubStatsLevel = MIN( 9, ubStatsLevel );	//maximum stats level of 9

	//Set the minimum base attribute
	bBaseAttribute = 49 + ( 4 * ubStatsLevel );

	//Roll soldier's statistics and skills
	//Stat range is currently 49-99, bell-curved around a range of 16 values dependent on the stats level
	pp->bLifeMax			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bLife	  			= pp->bLifeMax;
	pp->bAgility			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bDexterity		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));

	pp->bMarksmanship	= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bMedical			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bMechanical		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bExplosive		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bLeadership		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bStrength			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bWisdom				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	pp->bMorale				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));

	// CJC: now calculate the REAL experience level if in the really upper end
	ReduceHighExpLevels( &( pp->bExpLevel ) );

	pp->fVisible			= 0;

	pp->fOnRoof				= bp->fOnRoof;

	pp->ubSoldierClass = ubSoldierClass;

	//Transfer over the patrol points.
	pp->bPatrolCnt		= bp->bPatrolCnt;
	memcpy( pp->sPatrolGrid, bp->sPatrolGrid, sizeof( INT16 ) * MAXPATROLGRIDS );

	//If it is a detailed placement, don't do this yet, as detailed placements may have their
	//own equipment.
	if( !bp->fDetailedPlacement && ubSoldierClass != SOLDIER_CLASS_NONE && ubSoldierClass != SOLDIER_CLASS_CREATURE && ubSoldierClass != SOLDIER_CLASS_MINER )
		GenerateRandomEquipment( pp, ubSoldierClass, bp->bRelativeEquipmentLevel);
}


//Used exclusively by the editor when the user wishes to change a basic placement into a detailed placement.
//Because the intention is to make some of the attributes static, all of the information that can be generated
//are defaulted to -1.  When an attribute is made to be static, that value in replaced by the new static value.
//This information is NOT compatible with TacticalCreateSoldier.  Before doing so, you must first convert the
//static detailed placement to a regular detailed placement.
void CreateStaticDetailedPlacementGivenBasicPlacementInfo( SOLDIERCREATE_STRUCT *spp, BASIC_SOLDIERCREATE_STRUCT *bp )
{
	INT32 i;
	if( !spp || !bp )
		return;
	memset( spp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	spp->fStatic								= TRUE;
	spp->ubProfile							= NO_PROFILE;
	spp->sInsertionGridNo				= bp->usStartingGridNo;
	spp->fCopyProfileItemsOver	= FALSE;
	spp->bTeam									= bp->bTeam;
	spp->ubSoldierClass					= bp->ubSoldierClass;
	spp->ubCivilianGroup				= bp->ubCivilianGroup;
	spp->ubScheduleID						= 0;
	spp->sSectorX							= gWorldSectorX;
	spp->sSectorY							= gWorldSectorY;
	spp->bSectorZ							= gbWorldSectorZ;
	spp->fHasKeys							= bp->fHasKeys;

	//Pass over mandatory information specified from the basic placement
	spp->bOrders = bp->bOrders;
	spp->bAttitude = bp->bAttitude;
	spp->bDirection = bp->bDirection;

	//Only creatures have mandatory body types specified.
	if( spp->bTeam == CREATURE_TEAM )
		spp->bBodyType = bp->bBodyType;
	else
		spp->bBodyType = -1;

	//Set up all possible static values.
	//By setting them all to -1, that signifies that the attribute isn't static.
	//The static placement needs to be later *regenerated* in order to create a valid soldier.
	spp->bExpLevel			= -1;
	spp->bLifeMax				= -1;
	spp->bLife	  			= -1;
	spp->bAgility				= -1;
	spp->bDexterity			= -1;
	spp->bMarksmanship	= -1;
	spp->bMedical				= -1;
	spp->bMechanical		= -1;
	spp->bExplosive			= -1;
	spp->bLeadership		= -1;
	spp->bStrength			= -1;
	spp->bWisdom				= -1;
	spp->bMorale				= -1;

	spp->fVisible				=  0;

	spp->fOnRoof				= bp->fOnRoof;

	//Transfer over the patrol points.
	spp->bPatrolCnt			= bp->bPatrolCnt;
	memcpy( spp->sPatrolGrid, bp->sPatrolGrid, sizeof( INT16 ) * MAXPATROLGRIDS );

	//Starts with nothing
	for( i = 0; i < NUM_INV_SLOTS; i++ )
	{
		memset( &(spp->Inv[ i ]), 0, sizeof( OBJECTTYPE ) );
		spp->Inv[ i ].usItem = NOTHING;
		spp->Inv[ i ].fFlags |= OBJECT_UNDROPPABLE;
	}
}

//When you are ready to generate a soldier with a static detailed placement slot, this function will generate
//the proper detailed placement slot given the static detailed placement and it's accompanying basic placement.
//For the purposes of merc editing, the static detailed placement is preserved.
void CreateDetailedPlacementGivenStaticDetailedPlacementAndBasicPlacementInfo(
		   SOLDIERCREATE_STRUCT *pp, SOLDIERCREATE_STRUCT *spp, BASIC_SOLDIERCREATE_STRUCT *bp )
{
	INT32 i;

	memset( pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	pp->fOnRoof = spp->fOnRoof = bp->fOnRoof;
	pp->fStatic = FALSE;
	pp->ubSoldierClass = bp->ubSoldierClass;
	//Generate the new placement
	pp->ubProfile = spp->ubProfile;
	if( pp->ubProfile != NO_PROFILE )
	{
		// Copy over team
		pp->bTeam = bp->bTeam;

		pp->bDirection						= bp->bDirection;
		pp->sInsertionGridNo			= bp->usStartingGridNo;

		//ATE: Copy over sector coordinates from profile to create struct
		pp->sSectorX							= gMercProfiles[ pp->ubProfile ].sSectorX;
		pp->sSectorY							= gMercProfiles[ pp->ubProfile ].sSectorY;
		pp->bSectorZ							= gMercProfiles[ pp->ubProfile ].bSectorZ;

		pp->ubScheduleID					= spp->ubScheduleID;

		pp->bOrders								= bp->bOrders;
		pp->bAttitude							= bp->bAttitude;
		pp->bDirection						= bp->bDirection;
		pp->bPatrolCnt						= bp->bPatrolCnt;
		memcpy( pp->sPatrolGrid, bp->sPatrolGrid, sizeof( INT16 ) * MAXPATROLGRIDS );
		pp->fHasKeys							= bp->fHasKeys;
		pp->ubCivilianGroup				= bp->ubCivilianGroup;

		return; //done
	}
	CreateDetailedPlacementGivenBasicPlacementInfo( pp, bp );
	pp->ubScheduleID			= spp->ubScheduleID;
	//Replace any of the new placement's attributes with static attributes.
	if( spp->bExpLevel		!= -1 )			pp->bExpLevel			=	spp->bExpLevel;
	if( spp->bLife				!= -1 )			pp->bLife					= spp->bLife;
	if( spp->bLifeMax			!= -1 )			pp->bLifeMax			= spp->bLifeMax;
	if( spp->bMarksmanship!= -1 )			pp->bMarksmanship	= spp->bMarksmanship;
	if( spp->bStrength		!= -1 )			pp->bStrength			= spp->bStrength;
	if( spp->bAgility			!= -1 )			pp->bAgility			= spp->bAgility;
	if( spp->bDexterity		!= -1 )			pp->bDexterity		= spp->bDexterity;
	if( spp->bWisdom			!= -1 )			pp->bWisdom				= spp->bWisdom;
	if( spp->bLeadership	!= -1 )			pp->bLeadership		= spp->bLeadership;
	if( spp->bExplosive		!= -1 )			pp->bExplosive		= spp->bExplosive;
	if( spp->bMedical			!= -1 )			pp->bMedical			= spp->bMedical;
	if( spp->bMechanical	!= -1 )			pp->bMechanical		= spp->bMechanical;
	if( spp->bMorale			!= -1 )			pp->bMorale				= spp->bMorale;

	pp->fVisible = spp->fVisible;
	if( spp->fVisible )
	{
		strcpy(pp->HeadPal,  spp->HeadPal);
		strcpy(pp->PantsPal, spp->PantsPal);
		strcpy(pp->VestPal,  spp->VestPal);
		strcpy(pp->SkinPal,  spp->SkinPal);
	}

	//This isn't perfect, however, it blindly brings over the items from the static
	//detailed placement.  Due to the order of things, other items would
	for( i = 0; i < NUM_INV_SLOTS; i++ )
	{
		//copy over static items and empty slots that are droppable (signifies a forced empty slot)
		if( spp->Inv[ i ].fFlags & OBJECT_NO_OVERWRITE )
		{
			pp->Inv[i] = spp->Inv[i];
			//memcpy( pp->Inv, spp->Inv, sizeof( OBJECTTYPE ) * NUM_INV_SLOTS );
			//return;
		}
	}
	if ( !gGameOptions.fGunNut )
	{
		ReplaceExtendedGuns( pp, bp->ubSoldierClass );
	}
	if( bp->ubSoldierClass != SOLDIER_CLASS_NONE && bp->ubSoldierClass != SOLDIER_CLASS_CREATURE && bp->ubSoldierClass != SOLDIER_CLASS_MINER )
	{
		GenerateRandomEquipment( pp, bp->ubSoldierClass, bp->bRelativeEquipmentLevel);
	}
}


static void UpdateStaticDetailedPlacementWithProfileInformation(SOLDIERCREATE_STRUCT* spp, UINT8 ubProfile);


//Used to update a existing soldier's attributes with the new static detailed placement info.  This is used
//by the editor upon exiting the editor into the game, to update the existing soldiers with new information.
//This gives flexibility of testing mercs.  Upon entering the editor again, this call will reset all the
//mercs to their original states.
void UpdateSoldierWithStaticDetailedInformation( SOLDIERTYPE *s, SOLDIERCREATE_STRUCT *spp )
{
	//First, check to see if the soldier has a profile.  If so, then it'll extract the information
	//and update the soldier with the profile information instead.  This has complete override
	//authority.
	if( spp->ubProfile != NO_PROFILE )
	{
		TacticalCopySoldierFromProfile( s, spp );
		UpdateStaticDetailedPlacementWithProfileInformation( spp, spp->ubProfile );
		SetSoldierAnimationSurface( s, s->usAnimState );
		CreateSoldierPalettes( s );
		return;
	}

	switch( spp->ubSoldierClass )
	{ //If the soldier is an administrator, then
		case SOLDIER_CLASS_ADMINISTRATOR:
		case SOLDIER_CLASS_ARMY:
		case SOLDIER_CLASS_ELITE:
			GeneratePaletteForSoldier( s, spp->ubSoldierClass );
			break;
	}

	if( spp->bExpLevel != -1 )
	{ //We have a static experience level, so generate all of the soldier's attributes.
		INT8 bBaseAttribute;
		s->bExpLevel = spp->bExpLevel;
		//Set the minimum base attribute
		bBaseAttribute = 49 + ( 4 * s->bExpLevel );

		//Roll enemy's combat statistics, taking bExpLevel into account.
		//Stat range is currently 40-99, slightly bell-curved around the bExpLevel
		s->bLifeMax				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bLife	  			= s->bLifeMax;
		s->bAgility				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bDexterity			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bMarksmanship	= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bMedical				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bMechanical		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bExplosive			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bLeadership		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bStrength			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bWisdom				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
		s->bMorale				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	}
	//Replace any soldier attributes with any static values in the detailed placement.
	if( spp->bLife				!= -1 )			s->bLife					= spp->bLife;
	if( spp->bLifeMax			!= -1 )			s->bLifeMax				= spp->bLifeMax;
	if( spp->bMarksmanship!= -1 )			s->bMarksmanship	= spp->bMarksmanship;
	if( spp->bStrength		!= -1 )			s->bStrength			= spp->bStrength;
	if( spp->bAgility			!= -1 )			s->bAgility				= spp->bAgility;
	if( spp->bDexterity		!= -1 )			s->bDexterity			= spp->bDexterity;
	if( spp->bWisdom			!= -1 )			s->bWisdom				= spp->bWisdom;
	if( spp->bLeadership	!= -1 )			s->bLeadership		= spp->bLeadership;
	if( spp->bExplosive		!= -1 )			s->bExplosive			= spp->bExplosive;
	if( spp->bMedical			!= -1 )			s->bMedical				= spp->bMedical;
	if( spp->bMechanical	!= -1 )			s->bMechanical		= spp->bMechanical;
	if( spp->bMorale			!= -1 )			s->bMorale				= spp->bMorale;

	//life can't exceed the life max.
	if( s->bLife > s->bLifeMax )
		s->bLife = s->bLifeMax;

	s->ubScheduleID		=	spp->ubScheduleID;

	//Copy over the current inventory list.
	memcpy( s->inv, spp->Inv, sizeof( OBJECTTYPE ) * NUM_INV_SLOTS );
}


//In the case of setting a profile ID in order to extract a soldier from the profile array, we
//also want to copy that information to the static detailed placement, for editor viewing purposes.
static void UpdateStaticDetailedPlacementWithProfileInformation(SOLDIERCREATE_STRUCT* spp, UINT8 ubProfile)
{
	UINT32					cnt;
	MERCPROFILESTRUCT * pProfile;

	spp->ubProfile = ubProfile;

	pProfile = &(gMercProfiles[ ubProfile ]);

	SET_PALETTEREP_ID ( spp->HeadPal,		pProfile->HAIR );
	SET_PALETTEREP_ID ( spp->VestPal,		pProfile->VEST );
	SET_PALETTEREP_ID ( spp->SkinPal,		pProfile->SKIN );
	SET_PALETTEREP_ID ( spp->PantsPal,  pProfile->PANTS );

	wcscpy( spp->name, pProfile->zNickname );

	spp->bLife										= pProfile->bLife;
	spp->bLifeMax									= pProfile->bLifeMax;
	spp->bAgility									= pProfile->bAgility;
	spp->bLeadership							= pProfile->bLeadership;
	spp->bDexterity								= pProfile->bDexterity;
	spp->bStrength								= pProfile->bStrength;
	spp->bWisdom									= pProfile->bWisdom;
	spp->bExpLevel								= pProfile->bExpLevel;
	spp->bMarksmanship						= pProfile->bMarksmanship;
	spp->bMedical									= pProfile->bMedical;
	spp->bMechanical							= pProfile->bMechanical;
	spp->bExplosive								= pProfile->bExplosive;

	spp->bBodyType								= pProfile->ubBodyType;

	// Copy over inv if we want to
	for ( cnt = 0; cnt < NUM_INV_SLOTS; cnt++ )
	{
		CreateItems( pProfile->inv[ cnt ], pProfile->bInvStatus[ cnt ], pProfile->bInvNumber[ cnt ], &(spp->Inv[ cnt ]) );
	}
}

//When the editor modifies the soldier's relative attribute level,
//this function is called to update that information.
void ModifySoldierAttributesWithNewRelativeLevel( SOLDIERTYPE *s, INT8 bRelativeAttributeLevel )
{
	INT8 bBaseAttribute;
	//Set the experience level based on the relative attribute level
	// NOTE OF WARNING: THIS CURRENTLY IGNORES THE ENEMY CLASS (ADMIN/REG/ELITE) FOR CALCULATING LEVEL & ATTRIBUTES

	// Rel level 0: Lvl 1, 1: Lvl 2-3, 2: Lvl 4-5, 3: Lvl 6-7, 4: Lvl 8-9
	s->bExpLevel = (INT8)(2 * bRelativeAttributeLevel + Random(2));

	s->bExpLevel = MAX( 1, s->bExpLevel ); //minimum level of 1
	s->bExpLevel = MIN( 9, s->bExpLevel ); //maximum level of 9

	//Set the minimum base attribute
	bBaseAttribute = 49 + ( 4 * s->bExpLevel );

	//Roll enemy's combat statistics, taking bExpLevel into account.
	//Stat range is currently 40-99, slightly bell-curved around the bExpLevel
	s->bLifeMax				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bLife	  			= s->bLifeMax;
	s->bAgility				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bDexterity			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bMarksmanship	= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bMedical				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bMechanical		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bExplosive			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bLeadership		= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bStrength			= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bWisdom				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
	s->bMorale				= (INT8)(bBaseAttribute + Random( 9 ) + Random( 8 ));
}


void ForceSoldierProfileID( SOLDIERTYPE *pSoldier, UINT8 ubProfileID )
{
	SOLDIERCREATE_STRUCT CreateStruct;

	memset( &CreateStruct, 0, sizeof( CreateStruct ) );
	CreateStruct.ubProfile = ubProfileID;


	TacticalCopySoldierFromProfile( pSoldier, &CreateStruct );

	// Delete face and re-create
	DeleteSoldierFace( pSoldier );

	// Init face
	InitSoldierFace(pSoldier);

	// Update animation, palettes
	SetSoldierAnimationSurface( pSoldier, pSoldier->usAnimState );

	// Re-Create palettes
	CreateSoldierPalettes( pSoldier );

}

#define CENTRAL_GRIDNO 13202
#define CENTRAL_RADIUS 30


static SOLDIERTYPE* ReserveTacticalSoldierForAutoresolve(UINT8 ubSoldierClass)
try
{
	//This code looks for a soldier of specified type that currently exists in tactical and
	//returns the pointer to that soldier.  This is used when copying the exact status of
	//all remaining enemy troops (or creatures) to finish the battle in autoresolve.  To
	//signify that the troop has already been reserved, we simply set their gridno to NOWHERE.
	const INT8 team = (ubSoldierClass == SOLDIER_CLASS_CREATURE ? CREATURE_TEAM : ENEMY_TEAM);
	FOR_ALL_IN_TEAM(s, team)
	{
		if (s->bInSector && s->bLife != 0 && s->sGridNo != NOWHERE)
		{
			if (s->ubSoldierClass == ubSoldierClass)
			{
				//reserve this soldier
				s->sGridNo = NOWHERE;

				//Allocate and copy the soldier
				SOLDIERTYPE* const pSoldier = MALLOC(SOLDIERTYPE);
				*pSoldier = *s;

				//Assign a bogus ID, then return it
				pSoldier->ubID = 255;
				return pSoldier;
			}
		}
	}
	return NULL;
}
catch (...) { return 0; }


//USED BY STRATEGIC AI and AUTORESOLVE
SOLDIERTYPE* TacticalCreateAdministrator()
{
	BASIC_SOLDIERCREATE_STRUCT bp;
	SOLDIERCREATE_STRUCT pp;

	if( guiCurrentScreen == AUTORESOLVE_SCREEN && !gfPersistantPBI )
	{
		return ReserveTacticalSoldierForAutoresolve( SOLDIER_CLASS_ADMINISTRATOR );
	}

	memset( &bp, 0, sizeof( BASIC_SOLDIERCREATE_STRUCT ) );
	memset( &pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	RandomizeRelativeLevel( &( bp.bRelativeAttributeLevel ), SOLDIER_CLASS_ADMINISTRATOR );
	RandomizeRelativeLevel( &( bp.bRelativeEquipmentLevel ), SOLDIER_CLASS_ADMINISTRATOR );
	bp.bTeam = ENEMY_TEAM;
	bp.bOrders = SEEKENEMY;
	bp.bAttitude = (INT8) Random( MAXATTITUDES );
	bp.bBodyType = -1;
	bp.ubSoldierClass = SOLDIER_CLASS_ADMINISTRATOR;
	CreateDetailedPlacementGivenBasicPlacementInfo( &pp, &bp );
	SOLDIERTYPE* const pSoldier = TacticalCreateSoldier(&pp);
	if ( pSoldier )
	{
		// send soldier to centre of map, roughly
		pSoldier->sNoiseGridno = (INT16) (CENTRAL_GRIDNO + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) * WORLD_COLS);
		pSoldier->ubNoiseVolume = MAX_MISC_NOISE_DURATION;
	}
	return( pSoldier );
}

//USED BY STRATEGIC AI and AUTORESOLVE
SOLDIERTYPE* TacticalCreateArmyTroop()
{
	BASIC_SOLDIERCREATE_STRUCT bp;
	SOLDIERCREATE_STRUCT pp;

	if( guiCurrentScreen == AUTORESOLVE_SCREEN && !gfPersistantPBI )
	{
		return ReserveTacticalSoldierForAutoresolve( SOLDIER_CLASS_ARMY );
	}

	memset( &bp, 0, sizeof( BASIC_SOLDIERCREATE_STRUCT ) );
	memset( &pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	RandomizeRelativeLevel( &( bp.bRelativeAttributeLevel ), SOLDIER_CLASS_ARMY );
	RandomizeRelativeLevel( &( bp.bRelativeEquipmentLevel ), SOLDIER_CLASS_ARMY );
	bp.bTeam = ENEMY_TEAM;
	bp.bOrders	= SEEKENEMY;
	bp.bAttitude = (INT8) Random( MAXATTITUDES );
	bp.bBodyType = -1;
	bp.ubSoldierClass = SOLDIER_CLASS_ARMY;
	CreateDetailedPlacementGivenBasicPlacementInfo( &pp, &bp );
	SOLDIERTYPE* const pSoldier = TacticalCreateSoldier(&pp);
	if ( pSoldier )
	{
		// send soldier to centre of map, roughly
		pSoldier->sNoiseGridno = (INT16) (CENTRAL_GRIDNO + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) * WORLD_COLS);
		pSoldier->ubNoiseVolume = MAX_MISC_NOISE_DURATION;
	}
	return( pSoldier );
}

//USED BY STRATEGIC AI and AUTORESOLVE
SOLDIERTYPE* TacticalCreateEliteEnemy()
{
	BASIC_SOLDIERCREATE_STRUCT bp;
	SOLDIERCREATE_STRUCT pp;

	if( guiCurrentScreen == AUTORESOLVE_SCREEN && !gfPersistantPBI )
	{
		return ReserveTacticalSoldierForAutoresolve( SOLDIER_CLASS_ELITE );
	}

	memset( &bp, 0, sizeof( BASIC_SOLDIERCREATE_STRUCT ) );
	memset( &pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );

	RandomizeRelativeLevel( &( bp.bRelativeAttributeLevel ), SOLDIER_CLASS_ELITE );
	RandomizeRelativeLevel( &( bp.bRelativeEquipmentLevel ), SOLDIER_CLASS_ELITE );
	bp.bTeam = ENEMY_TEAM;
	bp.bOrders	= SEEKENEMY;
	bp.bAttitude = (INT8) Random( MAXATTITUDES );
	bp.bBodyType = -1;
	bp.ubSoldierClass = SOLDIER_CLASS_ELITE;
	CreateDetailedPlacementGivenBasicPlacementInfo( &pp, &bp );

	//SPECIAL!  Certain events in the game can cause profiled NPCs to become enemies.  The two cases are
	//adding Mike and Iggy.  We will only add one NPC in any given combat and the conditions for setting
	//the associated facts are done elsewhere.  There is also another place where NPCs can get added, which
	//is in AddPlacementToWorld() used for inserting defensive enemies.
	//NOTE:  We don't want to add Mike or Iggy if this is being called from autoresolve!
	OkayToUpgradeEliteToSpecialProfiledEnemy( &pp );

	SOLDIERTYPE* const pSoldier = TacticalCreateSoldier(&pp);
	if ( pSoldier )
	{
		// send soldier to centre of map, roughly
		pSoldier->sNoiseGridno = (INT16)(CENTRAL_GRIDNO + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) + ( Random( CENTRAL_RADIUS * 2 + 1 ) - CENTRAL_RADIUS ) * WORLD_COLS);
		pSoldier->ubNoiseVolume = MAX_MISC_NOISE_DURATION;
	}
	return( pSoldier );
}

SOLDIERTYPE* TacticalCreateMilitia( UINT8 ubMilitiaClass )
{
	BASIC_SOLDIERCREATE_STRUCT bp;
	SOLDIERCREATE_STRUCT pp;

	memset( &bp, 0, sizeof( BASIC_SOLDIERCREATE_STRUCT ) );
	memset( &pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	RandomizeRelativeLevel( &( bp.bRelativeAttributeLevel ), ubMilitiaClass );
	RandomizeRelativeLevel( &( bp.bRelativeEquipmentLevel ), ubMilitiaClass );
	bp.bTeam = MILITIA_TEAM;
	bp.ubSoldierClass = ubMilitiaClass;
	bp.bOrders = STATIONARY;
	bp.bAttitude = (INT8) Random( MAXATTITUDES );
	//bp.bAttitude = AGGRESSIVE;
	bp.bBodyType = -1;
	CreateDetailedPlacementGivenBasicPlacementInfo( &pp, &bp );
	return TacticalCreateSoldier(&pp);
}

SOLDIERTYPE* TacticalCreateCreature( INT8 bCreatureBodyType )
{
	BASIC_SOLDIERCREATE_STRUCT bp;
	SOLDIERCREATE_STRUCT pp;

	if( guiCurrentScreen == AUTORESOLVE_SCREEN && !gfPersistantPBI )
	{
		return ReserveTacticalSoldierForAutoresolve( SOLDIER_CLASS_CREATURE );
	}

	memset( &bp, 0, sizeof( BASIC_SOLDIERCREATE_STRUCT ) );
	memset( &pp, 0, sizeof( SOLDIERCREATE_STRUCT ) );
	RandomizeRelativeLevel( &( bp.bRelativeAttributeLevel ), SOLDIER_CLASS_CREATURE );
	RandomizeRelativeLevel( &( bp.bRelativeEquipmentLevel ), SOLDIER_CLASS_CREATURE );
	bp.bTeam = CREATURE_TEAM;
	bp.ubSoldierClass = SOLDIER_CLASS_CREATURE;
	bp.bOrders	= SEEKENEMY;
	bp.bAttitude = AGGRESSIVE;
	bp.bBodyType = bCreatureBodyType;
	CreateDetailedPlacementGivenBasicPlacementInfo( &pp, &bp );
	return TacticalCreateSoldier(&pp);
}


void RandomizeRelativeLevel( INT8 *pbRelLevel, UINT8 ubSoldierClass )
{
	UINT8 ubLocationModifier;
	INT8 bRollModifier;
	INT8 bRoll, bAdjustedRoll;


	// We now adjust the relative level by location on the map, so enemies in NE corner will be generally very crappy (lots
	// of bad and poor, with avg about best), while enemies in the SW will have lots of great and good, with avg about as
	// lousy as it gets.  Militia are generally unmodified by distance, and never get bad or great at all.

	// this returns 0 to DIFF_FACTOR_PALACE_DISTANCE (0 to +30)
	ubLocationModifier = GetLocationModifier( ubSoldierClass );

	// convert to 0 to 10 (divide by 3), the subtract 5 to get a range of -5 to +5
	bRollModifier = ( INT8 ) ( ubLocationModifier / ( DIFF_FACTOR_PALACE_DISTANCE / 10 ) ) - 5;

	// roll a number from 0 to 9
	bRoll = ( INT8 ) Random( 10 );

	// adjust by the modifier (giving -5 to +14)
	bAdjustedRoll = bRoll + bRollModifier;

	if ( SOLDIER_CLASS_MILITIA( ubSoldierClass ) )
	{
		// Militia never get to roll bad/great results at all (to avoid great equipment drops from them if killed)
		bAdjustedRoll = __max( 1, bAdjustedRoll );
		bAdjustedRoll = __min( 8, bAdjustedRoll );
		if( IsAutoResolveActive() )
		{ //Artificially strengthen militia strength for sake of gameplay
			bAdjustedRoll++;
		}
	}
	else
	{
		// max-min this to a range of 0-9
		bAdjustedRoll = __max( 0, bAdjustedRoll );
		bAdjustedRoll = __min( 9, bAdjustedRoll );
		if( IsAutoResolveActive() )
		{ //Artificially weaken enemy/creature strength for sake of gameplay
			if( bAdjustedRoll > 0 )
			{
				bAdjustedRoll--;
			}
		}
	}

	switch ( bAdjustedRoll )
	{
		case 0:
			// bad
			*pbRelLevel = 0;
			break;
		case 1:
		case 2:
			// poor
			*pbRelLevel = 1;
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			// average
			*pbRelLevel = 2;
			break;
		case 7:
		case 8:
			// good
			*pbRelLevel = 3;
			break;
		case 9:
			// great
			*pbRelLevel = 4;
			break;

		default:
			Assert( FALSE );
			*pbRelLevel = 2;
			break;
	}

/*
	if( IsAutoResolveActive() )
	{ //Artificially strengthen militia strength for sake of gameplay
		if ( SOLDIER_CLASS_MILITIA( ubSoldierClass ) )
		{
			*pbRelLevel = 4;
		}
		else
		{
			*pbRelLevel = 0;
		}
	}
*/
}


//This function shouldn't be called outside of tactical
void QuickCreateProfileMerc( INT8 bTeam, UINT8 ubProfileID )
{
	const GridNo pos = GetMouseMapPos();
	if (pos == NOWHERE) return;

	INT16 sSectorX;
	INT16 sSectorY;
	GetCurrentWorldSector(&sSectorX, &sSectorY);

	SOLDIERCREATE_STRUCT MercCreateStruct;
	memset(&MercCreateStruct, 0, sizeof(MercCreateStruct));
	MercCreateStruct.bTeam            = bTeam;
	MercCreateStruct.ubProfile        = ubProfileID;
	MercCreateStruct.sSectorX         = sSectorX;
	MercCreateStruct.sSectorY         = sSectorY;
	MercCreateStruct.bSectorZ         = gbWorldSectorZ;
	MercCreateStruct.sInsertionGridNo = pos;

	RandomizeNewSoldierStats(&MercCreateStruct);

	SOLDIERTYPE* const s = TacticalCreateSoldier(&MercCreateStruct);
	if (s != NULL)
	{
		AddSoldierToSector(s);

		// So we can see them!
		AllTeamsLookForAll(NO_INTERRUPTS);
	}
}


static BOOLEAN TryToAttach(SOLDIERTYPE* const s, OBJECTTYPE* const o)
{
	if (!(Item[o->usItem].fFlags & ITEM_ATTACHMENT)) return FALSE;

	// try to find the appropriate item to attach to!
	for (UINT32 i = 0; i < NUM_INV_SLOTS; ++i)
	{
		OBJECTTYPE* const tgt_o = &s->inv[i];
		if (tgt_o->usItem != NOTHING && ValidAttachment(o->usItem, tgt_o->usItem))
		{
			AttachObject(NULL, tgt_o, o);
			return TRUE;
		}
	}
	return FALSE;
}


static void CopyProfileItems(SOLDIERTYPE* const s, const SOLDIERCREATE_STRUCT* const pCreateStruct)
{
	MERCPROFILESTRUCT* const p = GetProfile(pCreateStruct->ubProfile);

	// Copy over inv if we want to
	if (s->bTeam != OUR_TEAM)
	{
		for (UINT32 i = 0; i < NUM_INV_SLOTS; ++i)
		{
			const UINT16      item = p->inv[i];
			OBJECTTYPE* const slot = &s->inv[i];
			if (item != NOTHING)
			{
				const UINT8 count = p->bInvNumber[i];
				if (Item[item].usItemClass == IC_KEY)
				{
					/* since keys depend on 2 values, they pretty much have to be
					 * hardcoded.  if a case isn't handled here it's better to not give
					 * any key than to provide one which doesn't work and would confuse
					 * everything. */
					if (KEY_1 <= item && item <= KEY_32)
					{
						switch (pCreateStruct->ubProfile)
						{
							case BREWSTER: CreateKeyObject(slot, count, 19); break;
							case SKIPPER:  CreateKeyObject(slot, count, 11); break;
							case DOREEN:   CreateKeyObject(slot, count, 32); break;
							default:       memset(slot, 0, sizeof(*slot));   break;
						}
					}
					else
					{
						memset(slot, 0, sizeof(*slot));
					}
				}
				else
				{
					CreateItems(item, p->bInvStatus[i], count, slot);
				}
				if (item == ROCKET_RIFLE || item == AUTO_ROCKET_RIFLE)
				{
					slot->ubImprintID = s->ubProfile;
				}
				if (p->ubInvUndroppable & gubItemDroppableFlag[i])
				{
					slot->fFlags |= OBJECT_UNDROPPABLE;
				}
			}
			else
			{
				memset(slot, 0, sizeof(*slot));
			}
		}

		for (UINT32 money_left = p->uiMoney; money_left > 0;)
		{
			const INT8 slot_id = FindEmptySlotWithin(s, BIGPOCK1POS, SMALLPOCK8POS);
			if (slot_id == NO_SLOT) break;
			OBJECTTYPE* const slot = &s->inv[slot_id];

			const UINT32 slot_limit  = MoneySlotLimit(slot_id);
			const UINT32 slot_amount = MIN(money_left, slot_limit);
			CreateMoney(slot_amount, slot);
			money_left -= slot_amount;
		}
	}
	else if (pCreateStruct->fCopyProfileItemsOver)
	{
		/* do some special coding to put stuff in the profile in better-looking
		 * spots */
		memset(s->inv, 0, sizeof(s->inv));
		for (UINT32 i = 0; i < NUM_INV_SLOTS; ++i)
		{
			if (p->inv[i] == NOTHING) continue;

			OBJECTTYPE o;
			CreateItems(p->inv[i], p->bInvStatus[i], p->bInvNumber[i], &o);
			if (!TryToAttach(s, &o)) AutoPlaceObject(s, &o, FALSE);
		}
		p->usOptionalGearCost = 0;
	}
}


//SPECIAL!  Certain events in the game can cause profiled NPCs to become enemies.  The two cases are
//adding Mike and Iggy.  We will only add one NPC in any given combat and the conditions for setting
//the associated facts are done elsewhere.  The function will set the profile for the SOLDIERCREATE_STRUCT
//and the rest will be handled automatically so long the ubProfile field doesn't get changed.
//NOTE:  We don't want to add Mike or Iggy if this is being called from autoresolve!
void OkayToUpgradeEliteToSpecialProfiledEnemy( SOLDIERCREATE_STRUCT *pp )
{
	if( !gfProfiledEnemyAdded && gubEnemyEncounterCode != ENEMY_ENCOUNTER_CODE && gubEnemyEncounterCode != ENEMY_INVASION_CODE )
	{
		if( gubFact[ FACT_MIKE_AVAILABLE_TO_ARMY ] == 1 && !pp->fOnRoof )
		{
			gubFact[ FACT_MIKE_AVAILABLE_TO_ARMY ] = 2; //so it fails all subsequent checks
			pp->ubProfile = MIKE;
			gfProfiledEnemyAdded = TRUE;
		}
		else if( gubFact[ FACT_IGGY_AVAILABLE_TO_ARMY ] == 1 && !pp->fOnRoof )
		{
			gubFact[ FACT_IGGY_AVAILABLE_TO_ARMY ] = 2; //so it fails all subsequent checks
			pp->ubProfile = IGGY;
			gfProfiledEnemyAdded = TRUE;
		}
	}
}


void TrashAllSoldiers( )
{
	FOR_ALL_NON_PLANNING_SOLDIERS(s) TacticalRemoveSoldier(s);
}


static UINT8 GetLocationModifier(UINT8 ubSoldierClass)
{
	UINT8 ubLocationModifier;
	UINT8 ubPalaceDistance;
	INT16 sSectorX, sSectorY, sSectorZ;
	INT8 bTownId;
	BOOLEAN fSuccess;


	// where is all this taking place?
	fSuccess = GetCurrentBattleSectorXYZ( &sSectorX, &sSectorY, &sSectorZ );
	Assert( fSuccess );

	// ignore sSectorZ - treat any underground enemies as if they were on the surface!
	bTownId = GetTownIdForSector( sSectorX, sSectorY );


	switch ( bTownId )
	{
		case ORTA:
		case TIXA:
			// enemy troops in these special places are stronger than geography would indicate
			ubPalaceDistance = 4;
			break;

		case ALMA:
			// enemy troops in these special places are stronger than geography would indicate
			ubPalaceDistance = 10;
			break;

		default:
			// how far is this sector from the palace ?
			// the distance returned is in sectors, and the possible range is about 0-20
			ubPalaceDistance = GetPythDistanceFromPalace( sSectorX, sSectorY );
			if ( ubPalaceDistance > MAX_PALACE_DISTANCE )
			{
				ubPalaceDistance = MAX_PALACE_DISTANCE;
			}
	}

	// adjust for distance from Queen's palace (P3) (0 to +30)
	ubLocationModifier = ( ( MAX_PALACE_DISTANCE - ubPalaceDistance ) * DIFF_FACTOR_PALACE_DISTANCE ) / MAX_PALACE_DISTANCE;

	return( ubLocationModifier );
}



// grab the distance from the palace
UINT8 GetPythDistanceFromPalace( INT16 sSectorX, INT16 sSectorY )
{
	UINT8 ubDistance = 0;
	INT16 sRows = 0, sCols = 0;
	float fValue = 0.0;

	// grab number of rows and cols
	sRows = (INT16)(abs((sSectorX) - ( PALACE_SECTOR_X )));
	sCols = (INT16)(abs((sSectorY) - ( PALACE_SECTOR_Y )));


	// apply Pythagoras's theorem for right-handed triangle:
	// dist^2 = rows^2 + cols^2, so use the square root to get the distance
	fValue = ( float )sqrt(( float )(sRows * sRows) + ( float )(sCols * sCols));

	if(  fmod( fValue, 1.0 ) >= 0.50 )
	{
		ubDistance = (UINT8)( 1 + fValue );
	}
	else
	{
		ubDistance = ( UINT8 )fValue;
	}

	return( ubDistance );
}


static void ReduceHighExpLevels(INT8* pbExpLevel)
{
	UINT8 ubRoll;
	// important: must reset these to 0 by default for logic to work!
	UINT8 ubChanceLvl8 = 0;
	UINT8 ubChanceLvl7 = 0;
	UINT8 ubChanceLvl6 = 0;
	UINT8 ubChanceLvl5 = 0;


	// this function reduces the experience levels of very high level enemies to something that player can compete with
	// for interrupts.  It doesn't affect attributes and skills, those are rolled prior to this reduction!


	// adjust for game difficulty level
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			// max level: 6
			switch ( *pbExpLevel )
			{
				case 6:
					ubChanceLvl6 = 25;
					ubChanceLvl5 = 100;
					break;
				case 7:
					ubChanceLvl6 = 50;
					ubChanceLvl5 = 100;
					break;
				case 8:
					ubChanceLvl6 = 75;
					ubChanceLvl5 = 100;
					break;
				case 9:
					ubChanceLvl6 = 100;
					break;
			}
			break;

		case DIF_LEVEL_MEDIUM:
			// max level: 7
			switch ( *pbExpLevel )
			{
				case 7:
					ubChanceLvl7 = 25;
					ubChanceLvl6 = 100;
					break;
				case 8:
					ubChanceLvl7 = 50;
					ubChanceLvl6 = 100;
					break;
				case 9:
					ubChanceLvl7 = 75;
					ubChanceLvl6 = 100;
					break;
			}
			break;

		case DIF_LEVEL_HARD:
			// max level: 8
			switch ( *pbExpLevel )
			{
				case 8:
					ubChanceLvl8 = 25;
					ubChanceLvl7 = 100;
					break;
				case 9:
					ubChanceLvl8 = 50;
					ubChanceLvl7 = 100;
					break;
			}
			break;
	}


	ubRoll = ( UINT8 ) Random( 100 );

	if			( ubRoll < ubChanceLvl8 )
		*pbExpLevel	= 8;
	else if ( ubRoll < ubChanceLvl7 )
		*pbExpLevel	= 7;
	else if ( ubRoll < ubChanceLvl6 )
		*pbExpLevel	= 6;
	else if ( ubRoll < ubChanceLvl5 )
		*pbExpLevel	= 5;
	// else leave it alone

}
