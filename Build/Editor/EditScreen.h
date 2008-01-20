#include "BuildDefines.h"

#ifdef JA2EDITOR
#ifndef EDITSCREEN_H
#define EDITSCREEN_H

#define EDITOR_LIGHT_MAX		(SHADE_MIN + SHADE_MAX)
#define EDITOR_LIGHT_FAKE		(EDITOR_LIGHT_MAX - SHADE_MAX - 2)

extern BOOLEAN gfFakeLights;

void DisplayWayPoints(void);

void ShowLightPositionHandles( void );

extern void ShowCurrentDrawingMode( void );

void DisplayTilesetPage(void);

BOOLEAN PlaceLight(INT16 sRadius, INT16 iMapX, INT16 iMapY);
BOOLEAN RemoveLight( INT16 iMapX, INT16 iMapY );
extern BOOLEAN gfMercResetUponEditorEntry;

void ProcessSelectionArea(void);

//These go together.  The taskbar has a specific color scheme.
extern UINT16 gusEditorTaskbarColor;
extern UINT16 gusEditorTaskbarHiColor;
extern UINT16 gusEditorTaskbarLoColor;

extern INT32 iOldTaskMode;
extern INT32 iCurrentTaskbar;
extern INT32 iTaskMode;

void ProcessAreaSelection( BOOLEAN fWithLeftButton );

void ShowEntryPoints(void);

extern BOOLEAN			gfConfirmExitFirst;
extern BOOLEAN			gfConfirmExitPending;
extern BOOLEAN			gfIntendOnEnteringEditor;

#endif
#endif
