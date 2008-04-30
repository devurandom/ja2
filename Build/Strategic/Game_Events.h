#ifndef __GAME_EVENTS_H
#define __GAME_EVENTS_H

#include "Game_Event_Hook.h"


#define SEF_DELETION_PENDING	0x02

typedef struct STRATEGICEVENT
{
	struct STRATEGICEVENT	 *next;
	UINT32									uiTimeStamp;
	UINT32									uiParam;
	UINT32									uiTimeOffset;
	UINT8										ubEventType;
	UINT8										ubCallbackID;
	UINT8										ubFlags;
	INT8										bPadding[6];
} STRATEGICEVENT;
CASSERT(sizeof(STRATEGICEVENT) == 28)


enum
{
	ONETIME_EVENT,
	RANGED_EVENT,
	ENDRANGED_EVENT,
	EVERYDAY_EVENT,
	PERIODIC_EVENT,
	QUEUED_EVENT
};

//part of the game.sav files (not map files)
BOOLEAN SaveStrategicEventsToSavedGame( HWFILE hFile );
BOOLEAN LoadStrategicEventsFromSavedGame( HWFILE hFile );

STRATEGICEVENT* AddAdvancedStrategicEvent( UINT8 ubEventType, UINT8 ubCallbackID, UINT32 uiTimeStamp, UINT32 uiParam );

BOOLEAN ExecuteStrategicEvent( STRATEGICEVENT *pEvent );

extern BOOLEAN gfEventDeletionPending;

extern STRATEGICEVENT *gpEventList;

/* Determines if there are any events that will be processed between the current
 * global time, and the beginning of the next global time. */
BOOLEAN GameEventsPending(UINT32 uiAdjustment);

/* If there are any events pending, they are processed, until the time limit is
 * reached, or a major event is processed (one that requires the player's
 * attention). */
void ProcessPendingGameEvents(UINT32 uiAdjustment, UINT8 ubWarpCode);

#endif
