#ifndef MESSAGEBOXSCREEN_H
#define MESSAGEBOXSCREEN_H

#include "Button_System.h"
#include "MouseSystem.h"
#include "ScreenIDs.h"


// Message box flags
enum MessageBoxFlags
{
	MSG_BOX_FLAG_OK                    = 0x0002, // Displays OK button
	MSG_BOX_FLAG_YESNO                 = 0x0004, // Displays YES NO buttons
	MSG_BOX_FLAG_CANCEL                = 0x0008, // Displays YES NO buttons
	MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS = 0x0010, // Displays four numbered buttons, 1-4
	MSG_BOX_FLAG_YESNOCONTRACT         = 0x0020, // yes no and contract buttons
	MSG_BOX_FLAG_OKCONTRACT            = 0x0040, // ok and contract buttons
	MSG_BOX_FLAG_YESNOLIE              = 0x0080, // ok and contract buttons
	MSG_BOX_FLAG_CONTINUESTOP          = 0x0100, // continue stop box
	MSG_BOX_FLAG_OKSKIP                = 0x0200, // Displays ok or skip (meanwhile) buttons
	MSG_BOX_FLAG_GENERICCONTRACT       = 0x0400, // displays contract buttoin + 2 user-defined text buttons
	MSG_BOX_FLAG_GENERIC               = 0x0800  // 2 user-defined text buttons
};

// message box return codes
#define MSG_BOX_RETURN_OK       1 // ENTER or on OK button
#define MSG_BOX_RETURN_YES      2 // ENTER or YES button
#define MSG_BOX_RETURN_NO       3 // ESC, Right Click or NO button
#define MSG_BOX_RETURN_CONTRACT 4 // contract button
#define MSG_BOX_RETURN_LIE      5 // LIE BUTTON

// message box style flags
enum MessageBoxStyleID
{
	MSG_BOX_BASIC_STYLE,         // We'll have other styles, like in laptop, etc
	                             // Graphics are all that are required here...
	MSG_BOX_RED_ON_WHITE,
	MSG_BOX_BLUE_ON_GREY,
	MSG_BOX_BASIC_SMALL_BUTTONS,
	MSG_BOX_IMP_STYLE,
	MSG_BOX_LAPTOP_DEFAULT
};


struct MESSAGE_BOX_STRUCT
{
	MessageBoxFlags usFlags;
	ScreenID        uiExitScreen;
	MSGBOX_CALLBACK ExitCallback;
	INT16           sX;
	INT16           sY;
	SGPVSurface*    uiSaveBuffer;
	MOUSE_REGION    BackRegion;
	UINT16          usWidth;
	UINT16          usHeight;
	BUTTON_PICS*    iButtonImages;
	GUIButtonRef    uiOKButton;
	GUIButtonRef    uiYESButton;
	GUIButtonRef    uiNOButton;
	GUIButtonRef    uiButton[4];
	BOOLEAN fRenderBox;
	INT8    bHandled;
	INT32   iBoxId;
};


extern MESSAGE_BOX_STRUCT gMsgBox;
extern BOOLEAN            fRestoreBackgroundForMessageBox;

//this variable can be unset if ur in a non gamescreen and DONT want the msg box to use the save buffer
extern BOOLEAN gfDontOverRideSaveBuffer;

extern wchar_t gzUserDefinedButton1[128];
extern wchar_t gzUserDefinedButton2[128];

/* ubStyle:       Determines the look of graphics including buttons
 * zString:       16-bit string
 * uiExitScreen   The screen to exit to
 * ubFlags        Some flags for button style
 * ReturnCallback Callback for return. Can be NULL. Returns any above return value
 * pCenteringRect Rect to center in. Can be NULL */
void DoMessageBox(MessageBoxStyleID, wchar_t const* zString, ScreenID uiExitScreen, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback, SGPRect const* pCenteringRect);
void DoScreenIndependantMessageBox(const wchar_t* zString, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback);
void DoLowerScreenIndependantMessageBox(const wchar_t* zString, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback);

//wrappers for other screens
void DoMapMessageBoxWithRect(MessageBoxStyleID, wchar_t const* zString, ScreenID uiExitScreen, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback, SGPRect const* pCenteringRect);
void DoOptionsMessageBoxWithRect(               wchar_t const* zString, ScreenID uiExitScreen, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback, SGPRect const* pCenteringRect);
void DoSaveLoadMessageBoxWithRect(              wchar_t const* zString, ScreenID uiExitScreen, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback, SGPRect const* pCenteringRect);

extern BOOLEAN gfInMsgBox;

ScreenID MessageBoxScreenHandle(void);

#endif
