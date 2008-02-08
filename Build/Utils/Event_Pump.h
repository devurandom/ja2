#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

// Enumerate all events for JA2
enum
{
	S_SETDESIREDDIRECTION,
	S_BEGINFIREWEAPON,
	S_FIREWEAPON,
	S_WEAPONHIT,
	S_NOISE,
	S_GETNEWPATH,

	NUM_EVENTS
};

// This definition is used to denote events with a special delay value;
// it indicates that these events will not be processed until specifically
// called for in a special loop.
#define DEMAND_EVENT_DELAY 0xFFFF


typedef struct
{
	UINT16	 usSoldierID;
	UINT32	 uiUniqueId;
	INT16		 sDestGridNo;
	UINT16		usMovementAnim;
} EV_S_GETNEWPATH;

typedef struct
{
	UINT16  usSoldierID;
	UINT32  uiUniqueId;
	UINT16	usDesiredDirection;
} EV_S_SETDESIREDDIRECTION;


typedef struct
{
	UINT16  usSoldierID;
	UINT32  uiUniqueId;
	INT16		sTargetGridNo;
	INT8		bTargetLevel;
	INT8		bTargetCubeLevel;
} EV_S_BEGINFIREWEAPON;


typedef struct
{
	UINT16  usSoldierID;
	UINT32  uiUniqueId;
	INT16		sTargetGridNo;
	INT8		bTargetLevel;
	INT8		bTargetCubeLevel;
} EV_S_FIREWEAPON;

typedef struct
{
	UINT16  usSoldierID;
	UINT16	usWeaponIndex;
	INT16		sDamage;
	INT16		sBreathLoss;
	UINT16	usDirection;
	INT16		sXPos;
	INT16		sYPos;
	INT16		sZPos;
	INT16		sRange;
	UINT8		ubAttackerID;
	UINT8		ubSpecial;
	UINT8		ubLocation;
} EV_S_WEAPONHIT;

typedef struct
{
	UINT8 ubNoiseMaker;
	INT16 sGridNo;
	UINT8 bLevel;
	UINT8 ubTerrType;
	UINT8 ubVolume;
	UINT8 ubNoiseType;
} EV_S_NOISE;


BOOLEAN AddGameEvent(UINT32 uiEvent, UINT16 usDelay, PTR pEventData);
BOOLEAN DequeAllGameEvents(void);
BOOLEAN DequeueAllDemandGameEvents(void);

// clean out the event queue
BOOLEAN ClearEventQueue(void);

#endif
