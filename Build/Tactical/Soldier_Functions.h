#ifndef _SOLDIER_FUNCTIONS_H
#define _SOLDIER_FUNCTIONS_H

#include "Soldier_Control.h"


void ContinueMercMovement( SOLDIERTYPE *pSoldier );

BOOLEAN IsValidStance(const SOLDIERTYPE* pSoldier, INT8 bNewStance);
void SelectMoveAnimationFromStance( SOLDIERTYPE *pSoldier );
BOOLEAN IsValidMovementMode(const SOLDIERTYPE* pSoldier, INT16 usMovementMode);
void SoldierCollapse( SOLDIERTYPE *pSoldier );

BOOLEAN ReevaluateEnemyStance( SOLDIERTYPE *pSoldier, UINT16 usAnimState );

void HandlePlacingRoofMarker( SOLDIERTYPE *pSoldier, INT16 sGridNo, BOOLEAN fSet, BOOLEAN fForce );

void PickPickupAnimation( SOLDIERTYPE *pSoldier, INT32 iItemIndex, INT16 sGridNo, INT8 bZLevel );

void MercStealFromMerc( SOLDIERTYPE *pSoldier, SOLDIERTYPE *pTarget );

void HandleCrowShadowVisibility( SOLDIERTYPE *pSoldier );


#endif
