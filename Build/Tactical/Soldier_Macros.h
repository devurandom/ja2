#ifndef SOLDIER_MACROS_H
#define SOLDIER_MACROS_H

// MACROS FOR EASIER SOLDIER CONTROL
#include "TeamTurns.h"
#include "Soldier_Profile.h"
#include "Assignments.h"
#include "Animation_Data.h"

// MACROS
#define RPC_RECRUITED( p )	( ( p->ubProfile == NO_PROFILE ) ? FALSE : ( gMercProfiles[ p->ubProfile ].ubMiscFlags & PROFILE_MISC_FLAG_RECRUITED ) )

#define AM_AN_EPC( p )	( ( p->ubProfile == NO_PROFILE ) ? FALSE : ( gMercProfiles[ p->ubProfile ].ubMiscFlags & PROFILE_MISC_FLAG_EPCACTIVE ) )

#define AM_A_ROBOT( p )	( ( p->ubProfile == NO_PROFILE ) ? FALSE : ( gMercProfiles[ p->ubProfile ].ubBodyType == ROBOTNOWEAPON ) )


#define OK_ENEMY_MERC( p ) ( !p->bNeutral && (p->bSide != gbPlayerNum ) && p->bLife >= OKLIFE )

// Checks if our guy can be controllable .... checks bInSector, team, on duty, etc...
static inline BOOLEAN OkControllableMerc(const SOLDIERTYPE* const s)
{
	Assert(s->bActive);
	return
		s->bLife >= OKLIFE &&
		s->bInSector &&
		s->bTeam == gbPlayerNum &&
		s->bAssignment < ON_DUTY;
}
#define OK_CONTROLLABLE_MERC(s) ((s)->bActive && OkControllableMerc((s)))

// Checkf if our guy can be selected and is not in a position where our team has an interupt and he does not have one...
#define OK_INTERRUPT_MERC(p) (!INTERRUPT_QUEUED || !p->bMoved)

#define CREATURE_OR_BLOODCAT( p ) ( (p->uiStatusFlags & SOLDIER_MONSTER) || p->ubBodyType == BLOODCAT )

#define TANK( p ) (p->ubBodyType == TANK_NE || p->ubBodyType == TANK_NW )

#define OK_ENTERABLE_VEHICLE( p )	( ( p->uiStatusFlags & SOLDIER_VEHICLE ) && !TANK( p ) && p->bLife >= OKLIFE  )

#endif
