#include "Debug.h"
#include "LoadSaveData.h"
#include "LoadSaveObjectType.h"
#include "LoadSaveSoldierType.h"
#include "Overhead.h"


void ExtractSoldierType(const BYTE* Src, SOLDIERTYPE* Soldier)
{
	const BYTE* S = Src;

	EXTR_U8(S, Soldier->ubID)
	EXTR_SKIP(S, 1)
	EXTR_U8(S, Soldier->ubBodyType)
	EXTR_I8(S, Soldier->bActionPoints)
	EXTR_I8(S, Soldier->bInitialActionPoints)
	EXTR_SKIP(S, 3)
	EXTR_U32(S, Soldier->uiStatusFlags)
	for (size_t i = 0; i < lengthof(Soldier->inv); i++)
	{
		S = ExtractObject(S, &Soldier->inv[i]);
	}
	EXTR_PTR(S, Soldier->pTempObject)
	EXTR_PTR(S, Soldier->pKeyRing)
	EXTR_I8(S, Soldier->bOldLife)
	EXTR_U8(S, Soldier->bInSector)
	EXTR_I8(S, Soldier->bFlashPortraitFrame)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sFractLife)
	EXTR_I8(S, Soldier->bBleeding)
	EXTR_I8(S, Soldier->bBreath)
	EXTR_I8(S, Soldier->bBreathMax)
	EXTR_I8(S, Soldier->bStealthMode)
	EXTR_I16(S, Soldier->sBreathRed)
	EXTR_BOOL(S, Soldier->fDelayedMovement)
	EXTR_BOOL(S, Soldier->fReloading)
	EXTR_U8(S, Soldier->ubWaitActionToDo)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->ubInsertionDirection)
	EXTR_SKIP(S, 1)
	EXTR_SOLDIER(S, Soldier->opponent)
	EXTR_I8(S, Soldier->bLastRenderVisibleValue)
	EXTR_SKIP(S, 1)
	EXTR_U8(S, Soldier->ubAttackingHand)
	EXTR_I8(S, Soldier->bScientific)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sWeightCarriedAtTurnStart)
#ifdef _WIN32 // XXX HACK000A
	EXTR_WSTR16(S, Soldier->name, lengthof(Soldier->name))
#else
	EXTR_SKIP(S, 2)
	EXTR_WSTR32(S, Soldier->name, lengthof(Soldier->name))
#endif
	EXTR_I8(S, Soldier->bVisible)
	EXTR_I8(S, Soldier->bActive)
	EXTR_I8(S, Soldier->bTeam)
	EXTR_U8(S, Soldier->ubGroupID)
	EXTR_BOOL(S, Soldier->fBetweenSectors)
	EXTR_U8(S, Soldier->ubMovementNoiseHeard)
#ifndef _WIN32 // XXX HACK000A
	EXTR_SKIP(S, 2)
#endif
	EXTR_FLOAT(S, Soldier->dXPos)
	EXTR_FLOAT(S, Soldier->dYPos)
	EXTR_SKIP(S, 8)
	EXTR_I16(S, Soldier->sInitialGridNo)
	EXTR_I16(S, Soldier->sGridNo)
	EXTR_I8(S, Soldier->bDirection)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sHeightAdjustment)
	EXTR_I16(S, Soldier->sDesiredHeight)
	EXTR_I16(S, Soldier->sTempNewGridNo)
	EXTR_SKIP(S, 2)
	EXTR_I8(S, Soldier->bOverTerrainType)
	EXTR_I8(S, Soldier->bOldOverTerrainType)
	EXTR_I8(S, Soldier->bCollapsed)
	EXTR_I8(S, Soldier->bBreathCollapsed)
	EXTR_U8(S, Soldier->ubDesiredHeight)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usPendingAnimation)
	EXTR_U8(S, Soldier->ubPendingStanceChange)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usAnimState)
	EXTR_BOOL(S, Soldier->fNoAPToFinishMove)
	EXTR_BOOL(S, Soldier->fPausedMove)
	EXTR_BOOL(S, Soldier->fUIdeadMerc)
	EXTR_SKIP(S, 1)
	EXTR_BOOL(S, Soldier->fUICloseMerc)
	EXTR_SKIP(S, 5)
	EXTR_I32(S, Soldier->UpdateCounter)
	EXTR_I32(S, Soldier->DamageCounter)
	EXTR_I32(S, Soldier->ReloadCounter)
	EXTR_SKIP(S, 4)
	EXTR_I32(S, Soldier->AICounter)
	EXTR_I32(S, Soldier->FadeCounter)
	EXTR_U8(S, Soldier->ubSkillTrait1)
	EXTR_U8(S, Soldier->ubSkillTrait2)
	EXTR_SKIP(S, 6)
	EXTR_I8(S, Soldier->bDexterity)
	EXTR_I8(S, Soldier->bWisdom)
	EXTR_I16(S, Soldier->sReloadDelay)
	EXTR_SOLDIER(S, Soldier->attacker)
	EXTR_SOLDIER(S, Soldier->previous_attacker)
	EXTR_BOOL(S, Soldier->fTurnInProgress)
	EXTR_BOOL(S, Soldier->fIntendedTarget)
	EXTR_BOOL(S, Soldier->fPauseAllAnimation)
	EXTR_I8(S, Soldier->bExpLevel)
	EXTR_I16(S, Soldier->sInsertionGridNo)
	EXTR_BOOL(S, Soldier->fContinueMoveAfterStanceChange)
	EXTR_SKIP(S, 3)
	EXTR_PTR(S, Soldier->AnimCache.usCachedSurfaces)
	EXTR_PTR(S, Soldier->AnimCache.sCacheHits)
	EXTR_U8(S, Soldier->AnimCache.ubCacheSize)
	EXTR_SKIP(S, 3)
	EXTR_I8(S, Soldier->bLife)
	EXTR_U8(S, Soldier->bSide)
	EXTR_U8(S, Soldier->bViewRange)
	EXTR_I8(S, Soldier->bNewOppCnt)
	EXTR_SKIP(S, 2)
	EXTR_U16(S, Soldier->usAniCode)
	EXTR_U16(S, Soldier->usAniFrame)
	EXTR_I16(S, Soldier->sAniDelay)
	EXTR_I8(S, Soldier->bAgility)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sDelayedMovementCauseGridNo)
	EXTR_I16(S, Soldier->sReservedMovementGridNo)
	EXTR_I8(S, Soldier->bStrength)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sTargetGridNo)
	EXTR_I8(S, Soldier->bTargetLevel)
	EXTR_I8(S, Soldier->bTargetCubeLevel)
	EXTR_I16(S, Soldier->sLastTarget)
	EXTR_I8(S, Soldier->bTilesMoved)
	EXTR_I8(S, Soldier->bLeadership)
	EXTR_FLOAT(S, Soldier->dNextBleed)
	EXTR_BOOL(S, Soldier->fWarnedAboutBleeding)
	EXTR_BOOL(S, Soldier->fDyingComment)
	EXTR_U8(S, Soldier->ubTilesMovedPerRTBreathUpdate)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usLastMovementAnimPerRTBreathUpdate)
	EXTR_BOOL(S, Soldier->fTurningToShoot)
	EXTR_SKIP(S, 1)
	EXTR_BOOL(S, Soldier->fTurningUntilDone)
	EXTR_BOOL(S, Soldier->fGettingHit)
	EXTR_BOOL(S, Soldier->fInNonintAnim)
	EXTR_BOOL(S, Soldier->fFlashLocator)
	EXTR_I16(S, Soldier->sLocatorFrame)
	EXTR_BOOL(S, Soldier->fShowLocator)
	EXTR_BOOL(S, Soldier->fFlashPortrait)
	EXTR_I8(S, Soldier->bMechanical)
	EXTR_I8(S, Soldier->bLifeMax)
	EXTR_SKIP(S, 6)
	EXTR_STR(S, Soldier->HeadPal, lengthof(Soldier->HeadPal))
	EXTR_STR(S, Soldier->PantsPal, lengthof(Soldier->PantsPal))
	EXTR_STR(S, Soldier->VestPal, lengthof(Soldier->VestPal))
	EXTR_STR(S, Soldier->SkinPal, lengthof(Soldier->SkinPal))
	EXTR_STR(S, Soldier->MiscPal, lengthof(Soldier->MiscPal))
	EXTR_SKIP(S, 14)
	EXTR_PTR(S, Soldier->p8BPPPalette)
	EXTR_SKIP(S, 4)
	EXTR_PTRA(S, Soldier->pShades)
	EXTR_PTRA(S, Soldier->pGlowShades)
	EXTR_SKIP(S, 4)
	EXTR_I8(S, Soldier->bMedical)
	EXTR_BOOL(S, Soldier->fBeginFade)
	EXTR_U8(S, Soldier->ubFadeLevel)
	EXTR_U8(S, Soldier->ubServiceCount)
	EXTR_SOLDIER(S, Soldier->service_partner)
	EXTR_I8(S, Soldier->bMarksmanship)
	EXTR_I8(S, Soldier->bExplosive)
	EXTR_SKIP(S, 1)
	EXTR_PTR(S, Soldier->pThrowParams)
	EXTR_BOOL(S, Soldier->fTurningFromPronePosition)
	EXTR_I8(S, Soldier->bReverse)
	EXTR_SKIP(S, 2)
	EXTR_PTR(S, Soldier->pLevelNode)
	EXTR_SKIP(S, 8)
	EXTR_I8(S, Soldier->bDesiredDirection)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sDestXPos)
	EXTR_I16(S, Soldier->sDestYPos)
	EXTR_SKIP(S, 2)
	EXTR_I16(S, Soldier->sDestination)
	EXTR_I16(S, Soldier->sFinalDestination)
	EXTR_I8(S, Soldier->bLevel)
	EXTR_SKIP(S, 3)
	EXTR_U16A(S, Soldier->usPathingData, lengthof(Soldier->usPathingData))
	EXTR_U16(S, Soldier->usPathDataSize)
	EXTR_U16(S, Soldier->usPathIndex)
	EXTR_I16(S, Soldier->sBlackList)
	EXTR_I8(S, Soldier->bAimTime)
	EXTR_I8(S, Soldier->bShownAimTime)
	EXTR_I8(S, Soldier->bPathStored)
	EXTR_I8(S, Soldier->bHasKeys)
	EXTR_SKIP(S, 18)
	EXTR_U8(S, Soldier->ubStrategicInsertionCode)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usStrategicInsertionData)
	EXTR_SKIP(S, 8)
	EXTR_I8(S, Soldier->bMuzFlashCount)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sX)
	EXTR_I16(S, Soldier->sY)
	EXTR_U16(S, Soldier->usOldAniState)
	EXTR_I16(S, Soldier->sOldAniCode)
	EXTR_I8(S, Soldier->bBulletsLeft)
	EXTR_U8(S, Soldier->ubSuppressionPoints)
	EXTR_U32(S, Soldier->uiTimeOfLastRandomAction)
	EXTR_SKIP(S, 2)
	EXTR_I8A(S, Soldier->bOppList, lengthof(Soldier->bOppList))
	EXTR_I8(S, Soldier->bLastAction)
	EXTR_I8(S, Soldier->bAction)
	EXTR_U16(S, Soldier->usActionData)
	EXTR_I8(S, Soldier->bNextAction)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usNextActionData)
	EXTR_I8(S, Soldier->bActionInProgress)
	EXTR_I8(S, Soldier->bAlertStatus)
	EXTR_I8(S, Soldier->bOppCnt)
	EXTR_I8(S, Soldier->bNeutral)
	EXTR_I8(S, Soldier->bNewSituation)
	EXTR_I8(S, Soldier->bNextTargetLevel)
	EXTR_I8(S, Soldier->bOrders)
	EXTR_I8(S, Soldier->bAttitude)
	EXTR_I8(S, Soldier->bUnderFire)
	EXTR_I8(S, Soldier->bShock)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->bBypassToGreen)
	EXTR_U8(S, Soldier->ubLastMercToRadio)
	EXTR_I8(S, Soldier->bDominantDir)
	EXTR_I8(S, Soldier->bPatrolCnt)
	EXTR_I8(S, Soldier->bNextPatrolPnt)
	EXTR_I16A(S, Soldier->usPatrolGrid, lengthof(Soldier->usPatrolGrid))
	EXTR_I16(S, Soldier->sNoiseGridno)
	EXTR_U8(S, Soldier->ubNoiseVolume)
	EXTR_I8(S, Soldier->bLastAttackHit)
	EXTR_U8(S, Soldier->ubXRayedBy)
	EXTR_SKIP(S, 1)
	EXTR_FLOAT(S, Soldier->dHeightAdjustment)
	EXTR_I8(S, Soldier->bMorale)
	EXTR_I8(S, Soldier->bTeamMoraleMod)
	EXTR_I8(S, Soldier->bTacticalMoraleMod)
	EXTR_I8(S, Soldier->bStrategicMoraleMod)
	EXTR_I8(S, Soldier->bAIMorale)
	EXTR_U8(S, Soldier->ubPendingAction)
	EXTR_U8(S, Soldier->ubPendingActionAnimCount)
	EXTR_SKIP(S, 1)
	EXTR_U32(S, Soldier->uiPendingActionData1)
	EXTR_I16(S, Soldier->sPendingActionData2)
	EXTR_I8(S, Soldier->bPendingActionData3)
	EXTR_I8(S, Soldier->ubDoorHandleCode)
	EXTR_U32(S, Soldier->uiPendingActionData4)
	EXTR_I8(S, Soldier->bInterruptDuelPts)
	EXTR_I8(S, Soldier->bPassedLastInterrupt)
	EXTR_I8(S, Soldier->bIntStartAPs)
	EXTR_I8(S, Soldier->bMoved)
	EXTR_I8(S, Soldier->bHunting)
	EXTR_SKIP(S, 1)
	EXTR_U8(S, Soldier->ubCaller)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sCallerGridNo)
	EXTR_U8(S, Soldier->bCallPriority)
	EXTR_I8(S, Soldier->bCallActedUpon)
	EXTR_I8(S, Soldier->bFrenzied)
	EXTR_I8(S, Soldier->bNormalSmell)
	EXTR_I8(S, Soldier->bMonsterSmell)
	EXTR_I8(S, Soldier->bMobility)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->fAIFlags)
	EXTR_BOOL(S, Soldier->fDontChargeReadyAPs)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usAnimSurface)
	EXTR_U16(S, Soldier->sZLevel)
	EXTR_BOOL(S, Soldier->fPrevInWater)
	EXTR_BOOL(S, Soldier->fGoBackToAimAfterHit)
	EXTR_I16(S, Soldier->sWalkToAttackGridNo)
	EXTR_I16(S, Soldier->sWalkToAttackWalkToCost)
	EXTR_SKIP(S, 7)
	EXTR_BOOL(S, Soldier->fForceShade)
	EXTR_SKIP(S, 2)
	EXTR_PTR(S, Soldier->pForcedShade)
	EXTR_I8(S, Soldier->bDisplayDamageCount)
	EXTR_I8(S, Soldier->fDisplayDamage)
	EXTR_I16(S, Soldier->sDamage)
	EXTR_I16(S, Soldier->sDamageX)
	EXTR_I16(S, Soldier->sDamageY)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->bDoBurst)
	EXTR_I16(S, Soldier->usUIMovementMode)
	EXTR_SKIP(S, 1)
	EXTR_BOOL(S, Soldier->fUIMovementFast)
	EXTR_SKIP(S, 2)
	EXTR_I32(S, Soldier->BlinkSelCounter)
	EXTR_I32(S, Soldier->PortraitFlashCounter)
	EXTR_BOOL(S, Soldier->fDeadSoundPlayed)
	EXTR_U8(S, Soldier->ubProfile)
	EXTR_U8(S, Soldier->ubQuoteRecord)
	EXTR_U8(S, Soldier->ubQuoteActionID)
	EXTR_U8(S, Soldier->ubBattleSoundID)
	EXTR_BOOL(S, Soldier->fClosePanel)
	EXTR_BOOL(S, Soldier->fClosePanelToDie)
	EXTR_U8(S, Soldier->ubClosePanelFrame)
	EXTR_BOOL(S, Soldier->fDeadPanel)
	EXTR_U8(S, Soldier->ubDeadPanelFrame)
	EXTR_SKIP(S, 2)
	EXTR_I16(S, Soldier->sPanelFaceX)
	EXTR_I16(S, Soldier->sPanelFaceY)
	EXTR_I8(S, Soldier->bNumHitsThisTurn)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usQuoteSaidFlags)
	EXTR_I8(S, Soldier->fCloseCall)
	EXTR_I8(S, Soldier->bLastSkillCheck)
	EXTR_I8(S, Soldier->ubSkillCheckAttempts)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->bStartFallDir)
	EXTR_I8(S, Soldier->fTryingToFall)
	EXTR_U8(S, Soldier->ubPendingDirection)
	EXTR_SKIP(S, 1)
	EXTR_U32(S, Soldier->uiAnimSubFlags)
	EXTR_U8(S, Soldier->bAimShotLocation)
	EXTR_U8(S, Soldier->ubHitLocation)
	EXTR_SKIP(S, 2)
	EXTR_PTRA(S, Soldier->pEffectShades)
	EXTR_U8(S, Soldier->ubPlannedUIAPCost)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sPlannedTargetX)
	EXTR_I16(S, Soldier->sPlannedTargetY)
	EXTR_I16A(S, Soldier->sSpreadLocations, lengthof(Soldier->sSpreadLocations))
	EXTR_BOOL(S, Soldier->fDoSpread)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sStartGridNo)
	EXTR_I16(S, Soldier->sEndGridNo)
	EXTR_I16(S, Soldier->sForcastGridno)
	EXTR_I16(S, Soldier->sZLevelOverride)
	EXTR_I8(S, Soldier->bMovedPriorToInterrupt)
	EXTR_SKIP(S, 3)
	EXTR_I32(S, Soldier->iEndofContractTime)
	EXTR_I32(S, Soldier->iStartContractTime)
	EXTR_I32(S, Soldier->iTotalContractLength)
	EXTR_I32(S, Soldier->iNextActionSpecialData)
	EXTR_U8(S, Soldier->ubWhatKindOfMercAmI)
	EXTR_I8(S, Soldier->bAssignment)
	EXTR_I8(S, Soldier->bOldAssignment)
	EXTR_BOOL(S, Soldier->fForcedToStayAwake)
	EXTR_I8(S, Soldier->bTrainStat)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sSectorX)
	EXTR_I16(S, Soldier->sSectorY)
	EXTR_I8(S, Soldier->bSectorZ)
	EXTR_SKIP(S, 1)
	EXTR_I32(S, Soldier->iVehicleId)
	EXTR_PTR(S, Soldier->pMercPath)
	EXTR_U8(S, Soldier->fHitByGasFlags)
	EXTR_SKIP(S, 1)
	EXTR_U16(S, Soldier->usMedicalDeposit)
	EXTR_U16(S, Soldier->usLifeInsurance)
	EXTR_SKIP(S, 26)
	EXTR_I32(S, Soldier->iStartOfInsuranceContract)
	EXTR_U32(S, Soldier->uiLastAssignmentChangeMin)
	EXTR_I32(S, Soldier->iTotalLengthOfInsuranceContract)
	EXTR_U8(S, Soldier->ubSoldierClass)
	EXTR_U8(S, Soldier->ubAPsLostToSuppression)
	EXTR_BOOL(S, Soldier->fChangingStanceDueToSuppression)
	EXTR_SOLDIER(S, Soldier->suppressor)
	EXTR_SKIP(S, 4)
	EXTR_U8(S, Soldier->ubCivilianGroup)
	EXTR_SKIP(S, 3)
	EXTR_U32(S, Soldier->uiChangeLevelTime)
	EXTR_U32(S, Soldier->uiChangeHealthTime)
	EXTR_U32(S, Soldier->uiChangeStrengthTime)
	EXTR_U32(S, Soldier->uiChangeDexterityTime)
	EXTR_U32(S, Soldier->uiChangeAgilityTime)
	EXTR_U32(S, Soldier->uiChangeWisdomTime)
	EXTR_U32(S, Soldier->uiChangeLeadershipTime)
	EXTR_U32(S, Soldier->uiChangeMarksmanshipTime)
	EXTR_U32(S, Soldier->uiChangeExplosivesTime)
	EXTR_U32(S, Soldier->uiChangeMedicalTime)
	EXTR_U32(S, Soldier->uiChangeMechanicalTime)
	EXTR_U32(S, Soldier->uiUniqueSoldierIdValue)
	EXTR_I8(S, Soldier->bBeingAttackedCount)
	EXTR_I8A(S, Soldier->bNewItemCount, lengthof(Soldier->bNewItemCount))
	EXTR_I8A(S, Soldier->bNewItemCycleCount, lengthof(Soldier->bNewItemCycleCount))
	EXTR_BOOL(S, Soldier->fCheckForNewlyAddedItems)
	EXTR_I8(S, Soldier->bEndDoorOpenCode)
	EXTR_U8(S, Soldier->ubScheduleID)
	EXTR_I16(S, Soldier->sEndDoorOpenCodeData)
	EXTR_I32(S, Soldier->NextTileCounter)
	EXTR_BOOL(S, Soldier->fBlockedByAnotherMerc)
	EXTR_I8(S, Soldier->bBlockedByAnotherMercDirection)
	EXTR_U16(S, Soldier->usAttackingWeapon)
	EXTR_I8(S, Soldier->bWeaponMode)
	EXTR_SOLDIER(S, Soldier->target)
	EXTR_I8(S, Soldier->bAIScheduleProgress)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sOffWorldGridNo)
	EXTR_SKIP(S, 2)
	EXTR_PTR(S, Soldier->pAniTile)
	EXTR_I8(S, Soldier->bCamo)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sAbsoluteFinalDestination)
	EXTR_U8(S, Soldier->ubHiResDirection)
	EXTR_SKIP(S, 1)
	EXTR_U8(S, Soldier->ubLastFootPrintSound)
	EXTR_I8(S, Soldier->bVehicleID)
	EXTR_I8(S, Soldier->fPastXDest)
	EXTR_I8(S, Soldier->fPastYDest)
	EXTR_I8(S, Soldier->bMovementDirection)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sOldGridNo)
	EXTR_U16(S, Soldier->usDontUpdateNewGridNoOnMoveAnimChange)
	EXTR_I16(S, Soldier->sBoundingBoxWidth)
	EXTR_I16(S, Soldier->sBoundingBoxHeight)
	EXTR_I16(S, Soldier->sBoundingBoxOffsetX)
	EXTR_I16(S, Soldier->sBoundingBoxOffsetY)
	EXTR_U32(S, Soldier->uiTimeSameBattleSndDone)
	EXTR_I8(S, Soldier->bOldBattleSnd)
	EXTR_SKIP(S, 1)
	EXTR_BOOL(S, Soldier->fContractPriceHasIncreased)
	EXTR_SKIP(S, 1)
	EXTR_I32(S, Soldier->iBurstSoundID)
	EXTR_BOOL(S, Soldier->fFixingSAMSite)
	EXTR_BOOL(S, Soldier->fFixingRobot)
	EXTR_I8(S, Soldier->bSlotItemTakenFrom)
	EXTR_BOOL(S, Soldier->fSignedAnotherContract)
	EXTR_SOLDIER(S, Soldier->auto_bandaging_medic)
	EXTR_BOOL(S, Soldier->fDontChargeTurningAPs)
	EXTR_U8(S, Soldier->ubRobotRemoteHolderID)
	EXTR_SKIP(S, 1)
	EXTR_U32(S, Soldier->uiTimeOfLastContractUpdate)
	EXTR_I8(S, Soldier->bTypeOfLastContract)
	EXTR_I8(S, Soldier->bTurnsCollapsed)
	EXTR_I8(S, Soldier->bSleepDrugCounter)
	EXTR_U8(S, Soldier->ubMilitiaKills)
	EXTR_I8A(S, Soldier->bFutureDrugEffect, lengthof(Soldier->bFutureDrugEffect))
	EXTR_I8A(S, Soldier->bDrugEffectRate, lengthof(Soldier->bDrugEffectRate))
	EXTR_I8A(S, Soldier->bDrugEffect, lengthof(Soldier->bDrugEffect))
	EXTR_I8A(S, Soldier->bDrugSideEffectRate, lengthof(Soldier->bDrugSideEffectRate))
	EXTR_I8A(S, Soldier->bDrugSideEffect, lengthof(Soldier->bDrugSideEffect))
	EXTR_I8A(S, Soldier->bTimesDrugUsedSinceSleep, lengthof(Soldier->bTimesDrugUsedSinceSleep))
	EXTR_I8(S, Soldier->bBlindedCounter)
	EXTR_BOOL(S, Soldier->fMercCollapsedFlag)
	EXTR_BOOL(S, Soldier->fDoneAssignmentAndNothingToDoFlag)
	EXTR_BOOL(S, Soldier->fMercAsleep)
	EXTR_BOOL(S, Soldier->fDontChargeAPsForStanceChange)
	EXTR_SKIP(S, 2)
	EXTR_U8(S, Soldier->ubTurnsUntilCanSayHeardNoise)
	EXTR_U16(S, Soldier->usQuoteSaidExtFlags)
	EXTR_U16(S, Soldier->sContPathLocation)
	EXTR_I8(S, Soldier->bGoodContPath)
	EXTR_SKIP(S, 1)
	EXTR_I8(S, Soldier->bNoiseLevel)
	EXTR_I8(S, Soldier->bRegenerationCounter)
	EXTR_I8(S, Soldier->bRegenBoostersUsedToday)
	EXTR_I8(S, Soldier->bNumPelletsHitBy)
	EXTR_I16(S, Soldier->sSkillCheckGridNo)
	EXTR_U8(S, Soldier->ubLastEnemyCycledID)
	EXTR_U8(S, Soldier->ubPrevSectorID)
	EXTR_U8(S, Soldier->ubNumTilesMovesSinceLastForget)
	EXTR_I8(S, Soldier->bTurningIncrement)
	EXTR_U32(S, Soldier->uiBattleSoundID)
	EXTR_BOOL(S, Soldier->fSoldierWasMoving)
	EXTR_BOOL(S, Soldier->fSayAmmoQuotePending)
	EXTR_U16(S, Soldier->usValueGoneUp)
	EXTR_U8(S, Soldier->ubNumLocateCycles)
	EXTR_U8(S, Soldier->ubDelayedMovementFlags)
	EXTR_BOOL(S, Soldier->fMuzzleFlash)
	EXTR_SOLDIER(S, Soldier->CTGTTarget)
	EXTR_I32(S, Soldier->PanelAnimateCounter)
	EXTR_U32(S, Soldier->uiMercChecksum)
	EXTR_I8(S, Soldier->bCurrentCivQuote)
	EXTR_I8(S, Soldier->bCurrentCivQuoteDelta)
	EXTR_U8(S, Soldier->ubMiscSoldierFlags)
	EXTR_U8(S, Soldier->ubReasonCantFinishMove)
	EXTR_I16(S, Soldier->sLocationOfFadeStart)
	EXTR_U8(S, Soldier->bUseExitGridForReentryDirection)
	EXTR_SKIP(S, 1)
	EXTR_U32(S, Soldier->uiTimeSinceLastSpoke)
	EXTR_U8(S, Soldier->ubContractRenewalQuoteCode)
	EXTR_SKIP(S, 1)
	EXTR_I16(S, Soldier->sPreTraversalGridNo)
	EXTR_U32(S, Soldier->uiXRayActivatedTime)
	EXTR_I8(S, Soldier->bTurningFromUI)
	EXTR_I8(S, Soldier->bPendingActionData5)
	EXTR_I8(S, Soldier->bDelayedStrategicMoraleMod)
	EXTR_U8(S, Soldier->ubDoorOpeningNoise)
	EXTR_SKIP(S, 4)
	EXTR_U8(S, Soldier->ubLeaveHistoryCode)
	EXTR_BOOL(S, Soldier->fDontUnsetLastTargetFromTurn)
	EXTR_I8(S, Soldier->bOverrideMoveSpeed)
	EXTR_BOOL(S, Soldier->fUseMoverrideMoveSpeed)
	EXTR_U32(S, Soldier->uiTimeSoldierWillArrive)
	EXTR_SKIP(S, 1)
	EXTR_BOOL(S, Soldier->fUseLandingZoneForArrival)
	EXTR_BOOL(S, Soldier->fFallClockwise)
	EXTR_I8(S, Soldier->bVehicleUnderRepairID)
	EXTR_I32(S, Soldier->iTimeCanSignElsewhere)
	EXTR_I8(S, Soldier->bHospitalPriceModifier)
	EXTR_SKIP(S, 3)
	EXTR_U32(S, Soldier->uiStartTimeOfInsuranceContract)
	EXTR_BOOL(S, Soldier->fRTInNonintAnim)
	EXTR_BOOL(S, Soldier->fDoingExternalDeath)
	EXTR_I8(S, Soldier->bCorpseQuoteTolerance)
	EXTR_SKIP(S, 1)
	EXTR_I32(S, Soldier->iPositionSndID)
	EXTR_I32(S, Soldier->iTuringSoundID)
	EXTR_U8(S, Soldier->ubLastDamageReason)
	EXTR_BOOL(S, Soldier->fComplainedThatTired)
	EXTR_I16A(S, Soldier->sLastTwoLocations, lengthof(Soldier->sLastTwoLocations))
	EXTR_SKIP(S, 2)
	EXTR_I32(S, Soldier->uiTimeSinceLastBleedGrunt)
	EXTR_SOLDIER(S, Soldier->next_to_previous_attacker)
	EXTR_SKIP(S, 39)

	Soldier->face         = NULL;
	Soldier->light        = NULL;
	Soldier->muzzle_flash = NULL;

#ifdef _WIN32 // XXX HACK000A
	Assert(S == Src + 2328);
#else
	Assert(S == Src + 2352);
#endif
}


void InjectSoldierType(BYTE* Dst, const SOLDIERTYPE* Soldier)
{
	BYTE* D = Dst;

	INJ_U8(D, Soldier->ubID)
	INJ_SKIP(D, 1)
	INJ_U8(D, Soldier->ubBodyType)
	INJ_I8(D, Soldier->bActionPoints)
	INJ_I8(D, Soldier->bInitialActionPoints)
	INJ_SKIP(D, 3)
	INJ_U32(D, Soldier->uiStatusFlags)
	for (size_t i = 0; i < lengthof(Soldier->inv); i++)
	{
		D = InjectObject(D, &Soldier->inv[i]);
	}
	INJ_PTR(D, Soldier->pTempObject)
	INJ_PTR(D, Soldier->pKeyRing)
	INJ_I8(D, Soldier->bOldLife)
	INJ_U8(D, Soldier->bInSector)
	INJ_I8(D, Soldier->bFlashPortraitFrame)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sFractLife)
	INJ_I8(D, Soldier->bBleeding)
	INJ_I8(D, Soldier->bBreath)
	INJ_I8(D, Soldier->bBreathMax)
	INJ_I8(D, Soldier->bStealthMode)
	INJ_I16(D, Soldier->sBreathRed)
	INJ_BOOL(D, Soldier->fDelayedMovement)
	INJ_BOOL(D, Soldier->fReloading)
	INJ_U8(D, Soldier->ubWaitActionToDo)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->ubInsertionDirection)
	INJ_SKIP(D, 1)
	INJ_SOLDIER(D, Soldier->opponent)
	INJ_I8(D, Soldier->bLastRenderVisibleValue)
	INJ_SKIP(D, 1)
	INJ_U8(D, Soldier->ubAttackingHand)
	INJ_I8(D, Soldier->bScientific)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sWeightCarriedAtTurnStart)
#ifdef _WIN32 // XXX HACK000A
	INJ_WSTR16(D, Soldier->name, lengthof(Soldier->name))
#else
	INJ_SKIP(D, 2)
	INJ_WSTR32(D, Soldier->name, lengthof(Soldier->name))
#endif
	INJ_I8(D, Soldier->bVisible)
	INJ_I8(D, Soldier->bActive)
	INJ_I8(D, Soldier->bTeam)
	INJ_U8(D, Soldier->ubGroupID)
	INJ_BOOL(D, Soldier->fBetweenSectors)
	INJ_U8(D, Soldier->ubMovementNoiseHeard)
#ifndef _WIN32 // XXX HACK000A
	INJ_SKIP(D, 2)
#endif
	INJ_FLOAT(D, Soldier->dXPos)
	INJ_FLOAT(D, Soldier->dYPos)
	INJ_SKIP(D, 8)
	INJ_I16(D, Soldier->sInitialGridNo)
	INJ_I16(D, Soldier->sGridNo)
	INJ_I8(D, Soldier->bDirection)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sHeightAdjustment)
	INJ_I16(D, Soldier->sDesiredHeight)
	INJ_I16(D, Soldier->sTempNewGridNo)
	INJ_SKIP(D, 2)
	INJ_I8(D, Soldier->bOverTerrainType)
	INJ_I8(D, Soldier->bOldOverTerrainType)
	INJ_I8(D, Soldier->bCollapsed)
	INJ_I8(D, Soldier->bBreathCollapsed)
	INJ_U8(D, Soldier->ubDesiredHeight)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usPendingAnimation)
	INJ_U8(D, Soldier->ubPendingStanceChange)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usAnimState)
	INJ_BOOL(D, Soldier->fNoAPToFinishMove)
	INJ_BOOL(D, Soldier->fPausedMove)
	INJ_BOOL(D, Soldier->fUIdeadMerc)
	INJ_SKIP(D, 1)
	INJ_BOOL(D, Soldier->fUICloseMerc)
	INJ_SKIP(D, 5)
	INJ_I32(D, Soldier->UpdateCounter)
	INJ_I32(D, Soldier->DamageCounter)
	INJ_I32(D, Soldier->ReloadCounter)
	INJ_SKIP(D, 4)
	INJ_I32(D, Soldier->AICounter)
	INJ_I32(D, Soldier->FadeCounter)
	INJ_U8(D, Soldier->ubSkillTrait1)
	INJ_U8(D, Soldier->ubSkillTrait2)
	INJ_SKIP(D, 6)
	INJ_I8(D, Soldier->bDexterity)
	INJ_I8(D, Soldier->bWisdom)
	INJ_I16(D, Soldier->sReloadDelay)
	INJ_SOLDIER(D, Soldier->attacker)
	INJ_SOLDIER(D, Soldier->previous_attacker)
	INJ_BOOL(D, Soldier->fTurnInProgress)
	INJ_BOOL(D, Soldier->fIntendedTarget)
	INJ_BOOL(D, Soldier->fPauseAllAnimation)
	INJ_I8(D, Soldier->bExpLevel)
	INJ_I16(D, Soldier->sInsertionGridNo)
	INJ_BOOL(D, Soldier->fContinueMoveAfterStanceChange)
	INJ_SKIP(D, 3)
	INJ_PTR(D, Soldier->AnimCache.usCachedSurfaces)
	INJ_PTR(D, Soldier->AnimCache.sCacheHits)
	INJ_U8(D, Soldier->AnimCache.ubCacheSize)
	INJ_SKIP(D, 3)
	INJ_I8(D, Soldier->bLife)
	INJ_U8(D, Soldier->bSide)
	INJ_U8(D, Soldier->bViewRange)
	INJ_I8(D, Soldier->bNewOppCnt)
	INJ_SKIP(D, 2)
	INJ_U16(D, Soldier->usAniCode)
	INJ_U16(D, Soldier->usAniFrame)
	INJ_I16(D, Soldier->sAniDelay)
	INJ_I8(D, Soldier->bAgility)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sDelayedMovementCauseGridNo)
	INJ_I16(D, Soldier->sReservedMovementGridNo)
	INJ_I8(D, Soldier->bStrength)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sTargetGridNo)
	INJ_I8(D, Soldier->bTargetLevel)
	INJ_I8(D, Soldier->bTargetCubeLevel)
	INJ_I16(D, Soldier->sLastTarget)
	INJ_I8(D, Soldier->bTilesMoved)
	INJ_I8(D, Soldier->bLeadership)
	INJ_FLOAT(D, Soldier->dNextBleed)
	INJ_BOOL(D, Soldier->fWarnedAboutBleeding)
	INJ_BOOL(D, Soldier->fDyingComment)
	INJ_U8(D, Soldier->ubTilesMovedPerRTBreathUpdate)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usLastMovementAnimPerRTBreathUpdate)
	INJ_BOOL(D, Soldier->fTurningToShoot)
	INJ_SKIP(D, 1)
	INJ_BOOL(D, Soldier->fTurningUntilDone)
	INJ_BOOL(D, Soldier->fGettingHit)
	INJ_BOOL(D, Soldier->fInNonintAnim)
	INJ_BOOL(D, Soldier->fFlashLocator)
	INJ_I16(D, Soldier->sLocatorFrame)
	INJ_BOOL(D, Soldier->fShowLocator)
	INJ_BOOL(D, Soldier->fFlashPortrait)
	INJ_I8(D, Soldier->bMechanical)
	INJ_I8(D, Soldier->bLifeMax)
	INJ_SKIP(D, 6)
	INJ_STR(D, Soldier->HeadPal, lengthof(Soldier->HeadPal))
	INJ_STR(D, Soldier->PantsPal, lengthof(Soldier->PantsPal))
	INJ_STR(D, Soldier->VestPal, lengthof(Soldier->VestPal))
	INJ_STR(D, Soldier->SkinPal, lengthof(Soldier->SkinPal))
	INJ_STR(D, Soldier->MiscPal, lengthof(Soldier->MiscPal))
	INJ_SKIP(D, 14)
	INJ_PTR(D, Soldier->p8BPPPalette)
	INJ_SKIP(D, 4)
	INJ_PTRA(D, Soldier->pShades)
	INJ_PTRA(D, Soldier->pGlowShades)
	INJ_SKIP(D, 4)
	INJ_I8(D, Soldier->bMedical)
	INJ_BOOL(D, Soldier->fBeginFade)
	INJ_U8(D, Soldier->ubFadeLevel)
	INJ_U8(D, Soldier->ubServiceCount)
	INJ_SOLDIER(D, Soldier->service_partner)
	INJ_I8(D, Soldier->bMarksmanship)
	INJ_I8(D, Soldier->bExplosive)
	INJ_SKIP(D, 1)
	INJ_PTR(D, Soldier->pThrowParams)
	INJ_BOOL(D, Soldier->fTurningFromPronePosition)
	INJ_I8(D, Soldier->bReverse)
	INJ_SKIP(D, 2)
	INJ_PTR(D, Soldier->pLevelNode)
	INJ_SKIP(D, 8)
	INJ_I8(D, Soldier->bDesiredDirection)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sDestXPos)
	INJ_I16(D, Soldier->sDestYPos)
	INJ_SKIP(D, 2)
	INJ_I16(D, Soldier->sDestination)
	INJ_I16(D, Soldier->sFinalDestination)
	INJ_I8(D, Soldier->bLevel)
	INJ_SKIP(D, 3)
	INJ_U16A(D, Soldier->usPathingData, lengthof(Soldier->usPathingData))
	INJ_U16(D, Soldier->usPathDataSize)
	INJ_U16(D, Soldier->usPathIndex)
	INJ_I16(D, Soldier->sBlackList)
	INJ_I8(D, Soldier->bAimTime)
	INJ_I8(D, Soldier->bShownAimTime)
	INJ_I8(D, Soldier->bPathStored)
	INJ_I8(D, Soldier->bHasKeys)
	INJ_SKIP(D, 18)
	INJ_U8(D, Soldier->ubStrategicInsertionCode)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usStrategicInsertionData)
	INJ_SKIP(D, 8)
	INJ_I8(D, Soldier->bMuzFlashCount)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sX)
	INJ_I16(D, Soldier->sY)
	INJ_U16(D, Soldier->usOldAniState)
	INJ_I16(D, Soldier->sOldAniCode)
	INJ_I8(D, Soldier->bBulletsLeft)
	INJ_U8(D, Soldier->ubSuppressionPoints)
	INJ_U32(D, Soldier->uiTimeOfLastRandomAction)
	INJ_SKIP(D, 2)
	INJ_I8A(D, Soldier->bOppList, lengthof(Soldier->bOppList))
	INJ_I8(D, Soldier->bLastAction)
	INJ_I8(D, Soldier->bAction)
	INJ_U16(D, Soldier->usActionData)
	INJ_I8(D, Soldier->bNextAction)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usNextActionData)
	INJ_I8(D, Soldier->bActionInProgress)
	INJ_I8(D, Soldier->bAlertStatus)
	INJ_I8(D, Soldier->bOppCnt)
	INJ_I8(D, Soldier->bNeutral)
	INJ_I8(D, Soldier->bNewSituation)
	INJ_I8(D, Soldier->bNextTargetLevel)
	INJ_I8(D, Soldier->bOrders)
	INJ_I8(D, Soldier->bAttitude)
	INJ_I8(D, Soldier->bUnderFire)
	INJ_I8(D, Soldier->bShock)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->bBypassToGreen)
	INJ_U8(D, Soldier->ubLastMercToRadio)
	INJ_I8(D, Soldier->bDominantDir)
	INJ_I8(D, Soldier->bPatrolCnt)
	INJ_I8(D, Soldier->bNextPatrolPnt)
	INJ_I16A(D, Soldier->usPatrolGrid, lengthof(Soldier->usPatrolGrid))
	INJ_I16(D, Soldier->sNoiseGridno)
	INJ_U8(D, Soldier->ubNoiseVolume)
	INJ_I8(D, Soldier->bLastAttackHit)
	INJ_U8(D, Soldier->ubXRayedBy)
	INJ_SKIP(D, 1)
	INJ_FLOAT(D, Soldier->dHeightAdjustment)
	INJ_I8(D, Soldier->bMorale)
	INJ_I8(D, Soldier->bTeamMoraleMod)
	INJ_I8(D, Soldier->bTacticalMoraleMod)
	INJ_I8(D, Soldier->bStrategicMoraleMod)
	INJ_I8(D, Soldier->bAIMorale)
	INJ_U8(D, Soldier->ubPendingAction)
	INJ_U8(D, Soldier->ubPendingActionAnimCount)
	INJ_SKIP(D, 1)
	INJ_U32(D, Soldier->uiPendingActionData1)
	INJ_I16(D, Soldier->sPendingActionData2)
	INJ_I8(D, Soldier->bPendingActionData3)
	INJ_I8(D, Soldier->ubDoorHandleCode)
	INJ_U32(D, Soldier->uiPendingActionData4)
	INJ_I8(D, Soldier->bInterruptDuelPts)
	INJ_I8(D, Soldier->bPassedLastInterrupt)
	INJ_I8(D, Soldier->bIntStartAPs)
	INJ_I8(D, Soldier->bMoved)
	INJ_I8(D, Soldier->bHunting)
	INJ_SKIP(D, 1)
	INJ_U8(D, Soldier->ubCaller)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sCallerGridNo)
	INJ_U8(D, Soldier->bCallPriority)
	INJ_I8(D, Soldier->bCallActedUpon)
	INJ_I8(D, Soldier->bFrenzied)
	INJ_I8(D, Soldier->bNormalSmell)
	INJ_I8(D, Soldier->bMonsterSmell)
	INJ_I8(D, Soldier->bMobility)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->fAIFlags)
	INJ_BOOL(D, Soldier->fDontChargeReadyAPs)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usAnimSurface)
	INJ_U16(D, Soldier->sZLevel)
	INJ_BOOL(D, Soldier->fPrevInWater)
	INJ_BOOL(D, Soldier->fGoBackToAimAfterHit)
	INJ_I16(D, Soldier->sWalkToAttackGridNo)
	INJ_I16(D, Soldier->sWalkToAttackWalkToCost)
	INJ_SKIP(D, 7)
	INJ_BOOL(D, Soldier->fForceShade)
	INJ_SKIP(D, 2)
	INJ_PTR(D, Soldier->pForcedShade)
	INJ_I8(D, Soldier->bDisplayDamageCount)
	INJ_I8(D, Soldier->fDisplayDamage)
	INJ_I16(D, Soldier->sDamage)
	INJ_I16(D, Soldier->sDamageX)
	INJ_I16(D, Soldier->sDamageY)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->bDoBurst)
	INJ_I16(D, Soldier->usUIMovementMode)
	INJ_SKIP(D, 1)
	INJ_BOOL(D, Soldier->fUIMovementFast)
	INJ_SKIP(D, 2)
	INJ_I32(D, Soldier->BlinkSelCounter)
	INJ_I32(D, Soldier->PortraitFlashCounter)
	INJ_BOOL(D, Soldier->fDeadSoundPlayed)
	INJ_U8(D, Soldier->ubProfile)
	INJ_U8(D, Soldier->ubQuoteRecord)
	INJ_U8(D, Soldier->ubQuoteActionID)
	INJ_U8(D, Soldier->ubBattleSoundID)
	INJ_BOOL(D, Soldier->fClosePanel)
	INJ_BOOL(D, Soldier->fClosePanelToDie)
	INJ_U8(D, Soldier->ubClosePanelFrame)
	INJ_BOOL(D, Soldier->fDeadPanel)
	INJ_U8(D, Soldier->ubDeadPanelFrame)
	INJ_SKIP(D, 2)
	INJ_I16(D, Soldier->sPanelFaceX)
	INJ_I16(D, Soldier->sPanelFaceY)
	INJ_I8(D, Soldier->bNumHitsThisTurn)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usQuoteSaidFlags)
	INJ_I8(D, Soldier->fCloseCall)
	INJ_I8(D, Soldier->bLastSkillCheck)
	INJ_I8(D, Soldier->ubSkillCheckAttempts)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->bStartFallDir)
	INJ_I8(D, Soldier->fTryingToFall)
	INJ_U8(D, Soldier->ubPendingDirection)
	INJ_SKIP(D, 1)
	INJ_U32(D, Soldier->uiAnimSubFlags)
	INJ_U8(D, Soldier->bAimShotLocation)
	INJ_U8(D, Soldier->ubHitLocation)
	INJ_SKIP(D, 2)
	INJ_PTRA(D, Soldier->pEffectShades)
	INJ_U8(D, Soldier->ubPlannedUIAPCost)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sPlannedTargetX)
	INJ_I16(D, Soldier->sPlannedTargetY)
	INJ_I16A(D, Soldier->sSpreadLocations, lengthof(Soldier->sSpreadLocations))
	INJ_BOOL(D, Soldier->fDoSpread)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sStartGridNo)
	INJ_I16(D, Soldier->sEndGridNo)
	INJ_I16(D, Soldier->sForcastGridno)
	INJ_I16(D, Soldier->sZLevelOverride)
	INJ_I8(D, Soldier->bMovedPriorToInterrupt)
	INJ_SKIP(D, 3)
	INJ_I32(D, Soldier->iEndofContractTime)
	INJ_I32(D, Soldier->iStartContractTime)
	INJ_I32(D, Soldier->iTotalContractLength)
	INJ_I32(D, Soldier->iNextActionSpecialData)
	INJ_U8(D, Soldier->ubWhatKindOfMercAmI)
	INJ_I8(D, Soldier->bAssignment)
	INJ_I8(D, Soldier->bOldAssignment)
	INJ_BOOL(D, Soldier->fForcedToStayAwake)
	INJ_I8(D, Soldier->bTrainStat)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sSectorX)
	INJ_I16(D, Soldier->sSectorY)
	INJ_I8(D, Soldier->bSectorZ)
	INJ_SKIP(D, 1)
	INJ_I32(D, Soldier->iVehicleId)
	INJ_PTR(D, Soldier->pMercPath)
	INJ_U8(D, Soldier->fHitByGasFlags)
	INJ_SKIP(D, 1)
	INJ_U16(D, Soldier->usMedicalDeposit)
	INJ_U16(D, Soldier->usLifeInsurance)
	INJ_SKIP(D, 26)
	INJ_I32(D, Soldier->iStartOfInsuranceContract)
	INJ_U32(D, Soldier->uiLastAssignmentChangeMin)
	INJ_I32(D, Soldier->iTotalLengthOfInsuranceContract)
	INJ_U8(D, Soldier->ubSoldierClass)
	INJ_U8(D, Soldier->ubAPsLostToSuppression)
	INJ_BOOL(D, Soldier->fChangingStanceDueToSuppression)
	INJ_SOLDIER(D, Soldier->suppressor)
	INJ_SKIP(D, 4)
	INJ_U8(D, Soldier->ubCivilianGroup)
	INJ_SKIP(D, 3)
	INJ_U32(D, Soldier->uiChangeLevelTime)
	INJ_U32(D, Soldier->uiChangeHealthTime)
	INJ_U32(D, Soldier->uiChangeStrengthTime)
	INJ_U32(D, Soldier->uiChangeDexterityTime)
	INJ_U32(D, Soldier->uiChangeAgilityTime)
	INJ_U32(D, Soldier->uiChangeWisdomTime)
	INJ_U32(D, Soldier->uiChangeLeadershipTime)
	INJ_U32(D, Soldier->uiChangeMarksmanshipTime)
	INJ_U32(D, Soldier->uiChangeExplosivesTime)
	INJ_U32(D, Soldier->uiChangeMedicalTime)
	INJ_U32(D, Soldier->uiChangeMechanicalTime)
	INJ_U32(D, Soldier->uiUniqueSoldierIdValue)
	INJ_I8(D, Soldier->bBeingAttackedCount)
	INJ_I8A(D, Soldier->bNewItemCount, lengthof(Soldier->bNewItemCount))
	INJ_I8A(D, Soldier->bNewItemCycleCount, lengthof(Soldier->bNewItemCycleCount))
	INJ_BOOL(D, Soldier->fCheckForNewlyAddedItems)
	INJ_I8(D, Soldier->bEndDoorOpenCode)
	INJ_U8(D, Soldier->ubScheduleID)
	INJ_I16(D, Soldier->sEndDoorOpenCodeData)
	INJ_I32(D, Soldier->NextTileCounter)
	INJ_BOOL(D, Soldier->fBlockedByAnotherMerc)
	INJ_I8(D, Soldier->bBlockedByAnotherMercDirection)
	INJ_U16(D, Soldier->usAttackingWeapon)
	INJ_I8(D, Soldier->bWeaponMode)
	INJ_SOLDIER(D, Soldier->target)
	INJ_I8(D, Soldier->bAIScheduleProgress)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sOffWorldGridNo)
	INJ_SKIP(D, 2)
	INJ_PTR(D, Soldier->pAniTile)
	INJ_I8(D, Soldier->bCamo)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sAbsoluteFinalDestination)
	INJ_U8(D, Soldier->ubHiResDirection)
	INJ_SKIP(D, 1)
	INJ_U8(D, Soldier->ubLastFootPrintSound)
	INJ_I8(D, Soldier->bVehicleID)
	INJ_I8(D, Soldier->fPastXDest)
	INJ_I8(D, Soldier->fPastYDest)
	INJ_I8(D, Soldier->bMovementDirection)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sOldGridNo)
	INJ_U16(D, Soldier->usDontUpdateNewGridNoOnMoveAnimChange)
	INJ_I16(D, Soldier->sBoundingBoxWidth)
	INJ_I16(D, Soldier->sBoundingBoxHeight)
	INJ_I16(D, Soldier->sBoundingBoxOffsetX)
	INJ_I16(D, Soldier->sBoundingBoxOffsetY)
	INJ_U32(D, Soldier->uiTimeSameBattleSndDone)
	INJ_I8(D, Soldier->bOldBattleSnd)
	INJ_SKIP(D, 1)
	INJ_BOOL(D, Soldier->fContractPriceHasIncreased)
	INJ_SKIP(D, 1)
	INJ_I32(D, Soldier->iBurstSoundID)
	INJ_BOOL(D, Soldier->fFixingSAMSite)
	INJ_BOOL(D, Soldier->fFixingRobot)
	INJ_I8(D, Soldier->bSlotItemTakenFrom)
	INJ_BOOL(D, Soldier->fSignedAnotherContract)
	INJ_SOLDIER(D, Soldier->auto_bandaging_medic)
	INJ_BOOL(D, Soldier->fDontChargeTurningAPs)
	INJ_U8(D, Soldier->ubRobotRemoteHolderID)
	INJ_SKIP(D, 1)
	INJ_U32(D, Soldier->uiTimeOfLastContractUpdate)
	INJ_I8(D, Soldier->bTypeOfLastContract)
	INJ_I8(D, Soldier->bTurnsCollapsed)
	INJ_I8(D, Soldier->bSleepDrugCounter)
	INJ_U8(D, Soldier->ubMilitiaKills)
	INJ_I8A(D, Soldier->bFutureDrugEffect, lengthof(Soldier->bFutureDrugEffect))
	INJ_I8A(D, Soldier->bDrugEffectRate, lengthof(Soldier->bDrugEffectRate))
	INJ_I8A(D, Soldier->bDrugEffect, lengthof(Soldier->bDrugEffect))
	INJ_I8A(D, Soldier->bDrugSideEffectRate, lengthof(Soldier->bDrugSideEffectRate))
	INJ_I8A(D, Soldier->bDrugSideEffect, lengthof(Soldier->bDrugSideEffect))
	INJ_I8A(D, Soldier->bTimesDrugUsedSinceSleep, lengthof(Soldier->bTimesDrugUsedSinceSleep))
	INJ_I8(D, Soldier->bBlindedCounter)
	INJ_BOOL(D, Soldier->fMercCollapsedFlag)
	INJ_BOOL(D, Soldier->fDoneAssignmentAndNothingToDoFlag)
	INJ_BOOL(D, Soldier->fMercAsleep)
	INJ_BOOL(D, Soldier->fDontChargeAPsForStanceChange)
	INJ_SKIP(D, 2)
	INJ_U8(D, Soldier->ubTurnsUntilCanSayHeardNoise)
	INJ_U16(D, Soldier->usQuoteSaidExtFlags)
	INJ_U16(D, Soldier->sContPathLocation)
	INJ_I8(D, Soldier->bGoodContPath)
	INJ_SKIP(D, 1)
	INJ_I8(D, Soldier->bNoiseLevel)
	INJ_I8(D, Soldier->bRegenerationCounter)
	INJ_I8(D, Soldier->bRegenBoostersUsedToday)
	INJ_I8(D, Soldier->bNumPelletsHitBy)
	INJ_I16(D, Soldier->sSkillCheckGridNo)
	INJ_U8(D, Soldier->ubLastEnemyCycledID)
	INJ_U8(D, Soldier->ubPrevSectorID)
	INJ_U8(D, Soldier->ubNumTilesMovesSinceLastForget)
	INJ_I8(D, Soldier->bTurningIncrement)
	INJ_U32(D, Soldier->uiBattleSoundID)
	INJ_BOOL(D, Soldier->fSoldierWasMoving)
	INJ_BOOL(D, Soldier->fSayAmmoQuotePending)
	INJ_U16(D, Soldier->usValueGoneUp)
	INJ_U8(D, Soldier->ubNumLocateCycles)
	INJ_U8(D, Soldier->ubDelayedMovementFlags)
	INJ_BOOL(D, Soldier->fMuzzleFlash)
	INJ_SOLDIER(D, Soldier->CTGTTarget)
	INJ_I32(D, Soldier->PanelAnimateCounter)
	INJ_U32(D, Soldier->uiMercChecksum)
	INJ_I8(D, Soldier->bCurrentCivQuote)
	INJ_I8(D, Soldier->bCurrentCivQuoteDelta)
	INJ_U8(D, Soldier->ubMiscSoldierFlags)
	INJ_U8(D, Soldier->ubReasonCantFinishMove)
	INJ_I16(D, Soldier->sLocationOfFadeStart)
	INJ_U8(D, Soldier->bUseExitGridForReentryDirection)
	INJ_SKIP(D, 1)
	INJ_U32(D, Soldier->uiTimeSinceLastSpoke)
	INJ_U8(D, Soldier->ubContractRenewalQuoteCode)
	INJ_SKIP(D, 1)
	INJ_I16(D, Soldier->sPreTraversalGridNo)
	INJ_U32(D, Soldier->uiXRayActivatedTime)
	INJ_I8(D, Soldier->bTurningFromUI)
	INJ_I8(D, Soldier->bPendingActionData5)
	INJ_I8(D, Soldier->bDelayedStrategicMoraleMod)
	INJ_U8(D, Soldier->ubDoorOpeningNoise)
	INJ_SKIP(D, 4)
	INJ_U8(D, Soldier->ubLeaveHistoryCode)
	INJ_BOOL(D, Soldier->fDontUnsetLastTargetFromTurn)
	INJ_I8(D, Soldier->bOverrideMoveSpeed)
	INJ_BOOL(D, Soldier->fUseMoverrideMoveSpeed)
	INJ_U32(D, Soldier->uiTimeSoldierWillArrive)
	INJ_SKIP(D, 1)
	INJ_BOOL(D, Soldier->fUseLandingZoneForArrival)
	INJ_BOOL(D, Soldier->fFallClockwise)
	INJ_I8(D, Soldier->bVehicleUnderRepairID)
	INJ_I32(D, Soldier->iTimeCanSignElsewhere)
	INJ_I8(D, Soldier->bHospitalPriceModifier)
	INJ_SKIP(D, 3)
	INJ_U32(D, Soldier->uiStartTimeOfInsuranceContract)
	INJ_BOOL(D, Soldier->fRTInNonintAnim)
	INJ_BOOL(D, Soldier->fDoingExternalDeath)
	INJ_I8(D, Soldier->bCorpseQuoteTolerance)
	INJ_SKIP(D, 1)
	INJ_I32(D, Soldier->iPositionSndID)
	INJ_I32(D, Soldier->iTuringSoundID)
	INJ_U8(D, Soldier->ubLastDamageReason)
	INJ_BOOL(D, Soldier->fComplainedThatTired)
	INJ_I16A(D, Soldier->sLastTwoLocations, lengthof(Soldier->sLastTwoLocations))
	INJ_SKIP(D, 2)
	INJ_I32(D, Soldier->uiTimeSinceLastBleedGrunt)
	INJ_SOLDIER(D, Soldier->next_to_previous_attacker)
	INJ_SKIP(D, 39)

#ifdef _WIN32 // XXX HACK000A
	Assert(D == Dst + 2328);
#else
	Assert(D == Dst + 2352);
#endif
}
