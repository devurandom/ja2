#include "Cursors.h"
#include "EMail.h"
#include "Font.h"
#include "HImage.h"
#include "Input.h"
#include "Laptop.h"
#include "AIMMembers.h"
#include "AIM.h"
#include "Local.h"
#include "Timer_Control.h"
#include "WCheck.h"
#include "Debug.h"
#include "SysUtil.h"
#include "Soldier_Profile.h"
#include "Soldier_Control.h"
#include "Interface_Items.h"
#include "Overhead.h"
#include "WordWrap.h"
#include "Finances.h"
#include "VSurface.h"
#include "VObject.h"
#include "Game_Clock.h"
#include "Faces.h"
#include "Dialogue_Control.h"
#include "Text.h"
#include "History.h"
#include "Game_Event_Hook.h"
#include "MercTextBox.h"
#include "Render_Dirty.h"
#include "Soldier_Add.h"
#include "Merc_Hiring.h"
#include "Strategic.h"
#include "AIMFacialIndex.h"
#include "LaptopSave.h"
#include "English.h"
#include "GameSettings.h"
#include "Random.h"
#include "Strategic_Status.h"
#include "Merc_Contract.h"
#include "Strategic_Merc_Handler.h"
#include "Assignments.h"
#include "StrategicMap.h"
#include "Sound_Control.h"
#include "Quests.h"
#include "Button_System.h"
#include "Video.h"
#include "SoundMan.h"
#include "ScreenIDs.h"
#include "Font_Control.h"
#include "Encrypted_File.h"
#include "Strategic_Town_Loyalty.h"

#define	MERCBIOSFILENAME		"BINARYDATA/aimbios.edt"



#define	AIM_M_FONT_PREV_NEXT_CONTACT		FONT14ARIAL
#define	AIM_M_FONT_PREV_NEXT_CONTACT_COLOR_UP		FONT_MCOLOR_DKWHITE
#define	AIM_M_FONT_PREV_NEXT_CONTACT_COLOR_DOWN		138
#define AIM_M_FONT_STATIC_TEXT					FONT12ARIAL
#define AIM_M_COLOR_STATIC_TEXT					146//75
#define AIM_M_FONT_DYNAMIC_TEXT					FONT10ARIAL
#define AIM_M_COLOR_DYNAMIC_TEXT				FONT_MCOLOR_WHITE
#define AIM_M_WEAPON_TEXT_FONT					FONT10ARIAL
#define AIM_M_WEAPON_TEXT_COLOR					FONT_MCOLOR_WHITE
#define AIM_M_NUMBER_FONT								FONT12ARIAL
#define AIM_M_ACTIVE_MEMBER_TITLE_COLOR	AIM_GREEN
#define AIM_M_FEE_CONTRACT_COLOR				AIM_GREEN
#define	AIM_M_VIDEO_TITLE_COLOR					AIM_FONT_GOLD
#define	AIM_M_VIDEO_NAME_COLOR					FONT_MCOLOR_BLACK
#define	AIM_M_VIDEO_NAME_SHADOWCOLOR		AIM_FONT_GOLD


#define	AIM_M_VIDEO_CONTRACT_AMOUNT_FONT	FONT10ARIAL
#define	AIM_M_VIDEO_CONTRACT_AMOUNT_COLOR	183

#define	AIM_POPUP_BOX_FONT							FONT12ARIAL
#define	AIM_POPUP_BOX_COLOR							FONT_MCOLOR_BLACK

#define	HIGH_STAT_COLOR											FONT_MCOLOR_WHITE//FONT_MCOLOR_LTGREEN
#define	MED_STAT_COLOR											FONT_MCOLOR_DKWHITE//FONT_MCOLOR_WHITE
#define	LOW_STAT_COLOR											FONT_MCOLOR_DKWHITE//FONT_MCOLOR_DKGRAY

#define SIZE_MERC_BIO_INFO        400
#define SIZE_MERC_ADDITIONAL_INFO 160

#define	MERC_ANNOYED_WONT_CONTACT_TIME_MINUTES	6 * 60
#define	NUMBER_HATED_MERCS_ONTEAM							3


#define		STATS_X						IMAGE_OFFSET_X + 121
#define		STATS_Y						IMAGE_OFFSET_Y + 66//69

#define		PRICE_X						IMAGE_OFFSET_X + 377
#define		PRICE_Y						STATS_Y
#define		PRICE_WIDTH				116

#define		PORTRAIT_X				IMAGE_OFFSET_X + 8
#define		PORTRAIT_Y				STATS_Y
#define   PORTRAIT_WIDTH		110
#define   PORTRAIT_HEIGHT		126

#define		FACE_X						PORTRAIT_X + 2
#define		FACE_Y						PORTRAIT_Y + 2
#define		FACE_WIDTH				106
#define		FACE_HEIGHT				122


#define		WEAPONBOX_X				IMAGE_OFFSET_X + 6
#define		WEAPONBOX_Y				IMAGE_OFFSET_Y + 296//299
#define		WEAPONBOX_SIZE_X	61
#define		WEAPONBOX_SIZE_Y	31
#define		WEAPONBOX_NUMBER	8

#define		SPACE_BN_LINES		15//13
#define		STATS_FIRST_COL		STATS_X + 9
#define		STATS_SECOND_COL	STATS_FIRST_COL + 129
#define   STAT_NAME_WIDTH    87
#define   STAT_VALUE_DX     102

#define		HEALTH_Y					STATS_Y + 34
#define		AGILITY_Y					HEALTH_Y	+ SPACE_BN_LINES
#define		DEXTERITY_Y				AGILITY_Y + SPACE_BN_LINES
#define		STRENGTH_Y				DEXTERITY_Y + SPACE_BN_LINES
#define		LEADERSHIP_Y			STRENGTH_Y + SPACE_BN_LINES
#define		WISDOM_Y					LEADERSHIP_Y + SPACE_BN_LINES

#define		EXPLEVEL_Y				HEALTH_Y
#define		MARKSMAN_Y				AGILITY_Y
#define		MECHANAICAL_Y			DEXTERITY_Y
#define		EXPLOSIVE_Y				STRENGTH_Y
#define		MEDICAL_Y					LEADERSHIP_Y

#define		NAME_X						STATS_FIRST_COL
#define		NAME_Y						STATS_Y + 7

#define		FEE_X							PRICE_X + 7
#define		FEE_Y							NAME_Y
#define		FEE_WIDTH					37  //33

#define		AIM_CONTRACT_X		PRICE_X + 51
#define		AIM_CONTRACT_Y		FEE_Y
#define		AIM_CONTRACT_WIDTH		59

#define		ONEDAY_X					AIM_CONTRACT_X
#define		ONEWEEK_X					AIM_CONTRACT_X
#define		TWOWEEK_X					AIM_CONTRACT_X

#define PREVIOUS_X 224
#define CONTACT_X  331
#define NEXT_X     431
#define BTN_BOX_Y  (386 + LAPTOP_SCREEN_WEB_DELTA_Y - 4)

#define		AIM_MERC_INFO_X		124
#define		AIM_MERC_INFO_Y		223 + LAPTOP_SCREEN_WEB_DELTA_Y

#define		AIM_MERC_ADD_X		AIM_MERC_ADD_INFO_X
#define		AIM_MERC_ADD_Y		269 + LAPTOP_SCREEN_WEB_DELTA_Y

#define   AIM_MERC_ADD_INFO_X	AIM_MERC_INFO_X
#define   AIM_MERC_ADD_INFO_Y	AIM_MERC_ADD_Y + 15
#define   AIM_MERC_INFO_WIDTH 470

#define		AIM_MEDICAL_DEPOSIT_X		PRICE_X + 5
#define		AIM_MEDICAL_DEPOSIT_Y		LEADERSHIP_Y
#define		AIM_MEDICAL_DEPOSIT_WIDTH		PRICE_WIDTH - 6

#define		AIM_MEMBER_ACTIVE_TEXT_X	IMAGE_OFFSET_X + 149
#define		AIM_MEMBER_ACTIVE_TEXT_Y	AIM_SYMBOL_Y + AIM_SYMBOL_SIZE_Y	- 1	// + 1
#define		AIM_MEMBER_ACTIVE_TEXT_WIDTH	AIM_SYMBOL_WIDTH

#define		AIM_MEMBER_OPTIONAL_GEAR_X		AIM_MERC_INFO_X
#define		AIM_MEMBER_OPTIONAL_GEAR_Y		WEAPONBOX_Y - 13

#define		AIM_MEMBER_WEAPON_NAME_Y			WEAPONBOX_Y + WEAPONBOX_SIZE_Y + 1
#define		AIM_MEMBER_WEAPON_NAME_WIDTH	WEAPONBOX_SIZE_X - 2

//video Conferencing Info
#define		AIM_MEMBER_VIDEO_CONF_TERMINAL_X	125
#define		AIM_MEMBER_VIDEO_CONF_TERMINAL_Y	97 + LAPTOP_SCREEN_WEB_DELTA_Y

#define		AIM_MEMBER_VIDEO_TITLE_BAR_WIDTH			368
#define		AIM_MEMBER_VIDEO_TITLE_BAR_HEIGHT			21
#define		AIM_MEMBER_VIDEO_TITLE_ITERATIONS			18
#define		AIM_MEMBER_VIDEO_TITLE_START_Y				382 + LAPTOP_SCREEN_WEB_DELTA_Y


#define		AIM_MEMBER_VIDEO_CONF_CONTRACT_IMAGE_X	AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 6
#define		AIM_MEMBER_VIDEO_CONF_CONTRACT_IMAGE_Y	AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + 130

#define		AIM_MEMBER_VIDEO_CONF_XCLOSE_X		AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 348
#define		AIM_MEMBER_VIDEO_CONF_XCLOSE_Y		AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + 3

#define		AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y	20

#define		AIM_MEMBER_BUY_CONTRACT_LENGTH_X	AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 113
#define		AIM_MEMBER_BUY_CONTRACT_LENGTH_Y	AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 15

#define		AIM_MEMBER_BUY_EQUIPMENT_GAP			23

#define		AIM_MEMBER_BUY_EQUIPMENT_X				AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 235

#define		AIM_MEMBER_AUTHORIZE_PAY_X				AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 113
#define		AIM_MEMBER_AUTHORIZE_PAY_Y				AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 92
#define		AIM_MEMBER_AUTHORIZE_PAY_GAP			122

#define		AIM_MEMBER_VIDEO_FACE_X						AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 7 + 1
#define		AIM_MEMBER_VIDEO_FACE_Y						AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 6 + 1

#define		AIM_MEMBER_VIDEO_FACE_WIDTH				96
#define		AIM_MEMBER_VIDEO_FACE_HEIGHT			86

#define		AIM_MEMBER_VIDEO_NAME_X						AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 7
#define		AIM_MEMBER_VIDEO_NAME_Y						AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + 5

#define		AIM_CONTRACT_CHARGE_X							AIM_MEMBER_VIDEO_NAME_X
#define		AIM_CONTRACT_CHARGE_Y							AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 98

#define		AIM_CONTRACT_LENGTH_ONE_DAY				0
#define		AIM_CONTRACT_LENGTH_ONE_WEEK			1
#define		AIM_CONTRACT_LENGTH_TWO_WEEKS			2

#define		AIM_SELECT_LIGHT_ON_X							105
#define		AIM_SELECT_LIGHT_ON_Y							8

#define		AIM_SELECT_LIGHT_OFF_X						105
#define		AIM_SELECT_LIGHT_OFF_Y						7

#define		AIM_CONTRACT_CHARGE_AMOUNNT_X			AIM_MEMBER_VIDEO_CONF_TERMINAL_X + 7//8
#define		AIM_CONTRACT_CHARGE_AMOUNNT_Y			AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 111//114

#define		AIM_POPUP_BOX_X										260
#define		AIM_POPUP_BOX_Y										140 + LAPTOP_SCREEN_WEB_DELTA_Y

#define		AIM_POPUP_BOX_WIDTH								162
#define		AIM_POPUP_BOX_STRING1_Y						6
#define		AIM_POPUP_BOX_BUTTON_OFFSET_X			20
#define		AIM_POPUP_BOX_BUTTON_OFFSET_Y			62
#define		AIM_POPUP_BOX_SUCCESS							0
#define		AIM_POPUP_BOX_FAILURE							1

#define		AIM_MEMBER_HANG_UP_X							290
#define		AIM_MEMBER_HANG_UP_Y							AIM_MEMBER_VIDEO_CONF_TERMINAL_Y + AIM_MEMBER_VIDEO_CONF_TITLE_BAR_HEIGHT_Y + 42

#define		VC_CONTACT_STATIC_TIME						30
#define		VC_CONTACT_FUZZY_LINE_TIME				100
#define		VC_NUM_LINES_SNOW									6
#define		VC_NUM_FUZZ_LINES									10
#define		VC_NUM_STRAIGHT_LINES							9

#define		VC_ANSWER_IMAGE_DELAY							100


#define		QUOTE_FIRST_ATTITUDE_TIME				3000
#define		QUOTE_ATTITUDE_TIME							10000

#define		QUOTE_DELAY_SMALL_TALK					1
#define		QUOTE_DELAY_IMPATIENT_TALK			2
#define		QUOTE_DELAY_VERY_IMPATIENT_TALK	3
#define		QUOTE_DELAY_HANGUP_TALK					4
#define		QUOTE_DELAY_NO_ACTION						5
#define		QUOTE_MERC_BUSY									6

#define		TEXT_POPUP_WINDOW_Y								255 + LAPTOP_SCREEN_WEB_DELTA_Y
#define		TEXT_POPUP_STRING_SIZE						512

#define		MINIMUM_TALKING_TIME_FOR_MERC			1500

#define		AIM_TEXT_SPEECH_MODIFIER					80


// enumerated types used for the Video Conferencing Display
enum
{
	AIM_VIDEO_NOT_DISPLAYED_MODE,							// The video popup is not displayed
	AIM_VIDEO_POPUP_MODE,											// The title bar pops up out of the Contact button
	AIM_VIDEO_INIT_MODE,											// When the player first tries to contact the merc, it will be snowy for a bit
	AIM_VIDEO_FIRST_CONTACT_MERC_MODE,				// The popup that is displayed when first contactinf the merc
	AIM_VIDEO_HIRE_MERC_MODE,									// The popup which deals with the contract length, and transfer funds
	AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE,		// The popup which will be instread of the AIM_VIDEO_FIRST_CONTACT_MERC_MODE if the merc is not there
	AIM_VIDEO_MERC_UNAVAILABLE_MODE,					// The popup which will be instread of the AIM_VIDEO_FIRST_CONTACT_MERC_MODE if the merc is unavailable
	AIM_VIDEO_POPDOWN_MODE,										// The title bars pops down to the contact button
};


// Enumerated types used for the Pop Up Box
enum
{
	AIM_POPUP_NOTHING,
	AIM_POPUP_CREATE,
	AIM_POPUP_DISPLAY,
	AIM_POPUP_DELETE,
};

// Enumerated Types used for the different types of video distortion applied to the video face
enum
{
	VC_NO_STATIC,
	VC_FUZZY_LINE,
	VC_STRAIGHTLINE,
	VC_STATIC_IMAGE,
	VC_BW_SNOW,
	VC_PIXELATE,
	VC_TRANS_SNOW_IN,			// fade from clear to snowy
	VC_TRANS_SNOW_OUT,		// fade from snowy to clear
};


//Image Identifiers
static SGPVObject* guiStats;
static SGPVObject* guiPrice;
static SGPVObject* guiPortrait;
static SGPVObject* guiWeaponBox;
//UINT32		guiVideoFace;
//UINT32		guiContactButton;
static SGPVObject* guiVideoConfPopup;
static SGPVObject* guiVideoConfTerminal;
static SGPVObject* guiPopUpBox;
static SGPVSurface* guiVideoFaceBackground;
static SGPVObject* guiBWSnow;
static SGPVObject* guiFuzzLine;
static SGPVObject* guiStraightLine;
static SGPVObject* guiTransSnow;
static SGPVObject* guiVideoContractCharge;
//UINT32		guiAnsweringMachineImage;
static SGPVSurface* guiVideoTitleBar;
static INT32  iAimMembersBoxId = -1;

static UINT8 gbCurrentSoldier = 0;
UINT8        gbCurrentIndex = 0;

UINT8          gubVideoConferencingMode;
static UINT8   gubVideoConferencingPreviousMode;
static BOOLEAN gfJustSwitchedVideoConferenceMode;

static BOOLEAN gfMercIsTalking=FALSE;
static BOOLEAN gfVideoFaceActive=FALSE;

static UINT8     gubPopUpBoxAction = AIM_POPUP_NOTHING;
static BOOLEAN   gfRedrawScreen    = FALSE;
UINT8            gubContractLength;
static BOOLEAN   gfBuyEquipment;
static INT32     giContractAmount  = 0;
static FACETYPE* giMercFaceIndex;
static wchar_t   gsTalkingMercText[TEXT_POPUP_STRING_SIZE];
static UINT32    guiTimeThatMercStartedTalking;
static UINT32    guiLastHandleMercTime;
static BOOLEAN   gfFirstTimeInContactScreen;

static UINT8   gubCurrentCount;
static UINT8   gubCurrentStaticMode;
static UINT32  guiMercAttitudeTime;         // retains the amount of time the user is in a screen, if over a certain time, the merc gets miffed
static UINT8   gubMercAttitudeLevel;        // retains the current level the merc is  P.O.'ed at the caller.
static BOOLEAN gfHangUpMerc;                // if we have to cancel the video conferencing after the merc is finsihed talking
static BOOLEAN gfIsShutUpMouseRegionActive;
static BOOLEAN gfIsAnsweringMachineActive;
static BOOLEAN gfStopMercFromTalking;

static UINT16 usAimMercSpeechDuration=0;

static BOOLEAN gfIsNewMailFlagSet = FALSE;

extern		BOOLEAN fExitDueToMessageBox;

static BOOLEAN gfWaitingForMercToStopTalkingOrUserToClick=FALSE;

static BOOLEAN gfAimMemberDisplayFaceHelpText = FALSE;


static BOOLEAN gfAimMemberCanMercSayOpeningQuote = TRUE;


static BUTTON_PICS* guiPreviousContactNextButtonImage;
static INT32 giPreviousButton;
static INT32 giContactButton;
static INT32 giNextButton;

//Video conference buttons
static BUTTON_PICS* guiVideoConferenceButtonImage[3];

//Contract Length Button
static INT32 giContractLengthButton[3];

//BuyEquipment Button
static INT32 giBuyEquipmentButton[2];

//Authorize Payment Button
static INT32 giAuthorizeButton[2];

//Hang up Button
static INT32 giHangUpButton;

// PopupBox button
static UINT32 guiPopUpOkButton;
static BUTTON_PICS* guiPopUpImage;

//Leave Message merc Button
static INT32 giAnsweringMachineButton[2];

//X to Close the video conference Button
static BUTTON_PICS* giXToCloseVideoConfButtonImage;
static INT32 giXToCloseVideoConfButton;


//Mouse Regions
//Clicking on guys Face
static MOUSE_REGION    gSelectedFaceRegion;

//Clicking To shut merc up
static MOUSE_REGION    gSelectedShutUpMercRegion;


void EnterInitAimMembers()
{
	gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	gubVideoConferencingPreviousMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	gfVideoFaceActive = FALSE;
//fShouldMercTalk = FALSE;
	gubPopUpBoxAction = AIM_POPUP_NOTHING;
	gfRedrawScreen = FALSE;
	giContractAmount = 0;
	giMercFaceIndex = NULL;
	guiLastHandleMercTime = GetJA2Clock();
	gubCurrentCount = 0;
	gfFirstTimeInContactScreen = TRUE;

	//reset the variable so a pop up can be displyed this time in laptop
	LaptopSaveInfo.sLastHiredMerc.fHaveDisplayedPopUpInLaptop = FALSE;

		//reset the id of the last merc
	LaptopSaveInfo.sLastHiredMerc.iIdOfMerc = -1;
}


static INT32 MakeButton(const wchar_t* text, INT16 x, GUI_CALLBACK click)
{
	const INT32 btn = CreateIconAndTextButton
	(
		guiPreviousContactNextButtonImage, text, AIM_M_FONT_PREV_NEXT_CONTACT,
		AIM_M_FONT_PREV_NEXT_CONTACT_COLOR_UP,   DEFAULT_SHADOW,
		AIM_M_FONT_PREV_NEXT_CONTACT_COLOR_DOWN, DEFAULT_SHADOW,
		x, BTN_BOX_Y, MSYS_PRIORITY_HIGH, click
	);
	SetButtonCursor(btn, CURSOR_WWW);
	return btn;
}


static void BtnContactButtonCallback( GUI_BUTTON* btn, INT32 reason);
static void BtnNextButtonCallback(    GUI_BUTTON* btn, INT32 reason);
static void BtnPreviousButtonCallback(GUI_BUTTON* btn, INT32 reason);
static BOOLEAN InitDeleteVideoConferencePopUp(void);
static BOOLEAN InitVideoFace(UINT8 ubMercID);
static void SelectFaceMovementRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason);
static void SelectFaceRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason);
static void SelectShutUpMercRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason);


BOOLEAN EnterAIMMembers()
{
	// Create a background video surface to blt the face onto
	guiVideoFaceBackground = AddVideoSurface(AIM_MEMBER_VIDEO_FACE_WIDTH, AIM_MEMBER_VIDEO_FACE_HEIGHT, PIXEL_DEPTH);
	CHECKF(guiVideoFaceBackground != NO_VSURFACE);

	// load the stats graphic and add it
	guiStats = AddVideoObjectFromFile("LAPTOP/stats.sti");
	CHECKF(guiStats != NO_VOBJECT);

	// load the Price graphic and add it
	guiPrice = AddVideoObjectFromFile("LAPTOP/price.sti");
	CHECKF(guiPrice != NO_VOBJECT);

	// load the Portait graphic and add it
	guiPortrait = AddVideoObjectFromFile("LAPTOP/portrait.sti");
	CHECKF(guiPortrait != NO_VOBJECT);

	// load the WeaponBox graphic and add it
	guiWeaponBox = AddVideoObjectFromFile("LAPTOP/weaponbox.sti");
	CHECKF(guiWeaponBox != NO_VOBJECT);

	// load the videoconf Popup graphic and add it
	guiVideoConfPopup = AddVideoObjectFromFile("LAPTOP/VideoConfPopup.sti");
	CHECKF(guiVideoConfPopup != NO_VOBJECT);

	// load the video conf terminal graphic and add it
	guiVideoConfTerminal = AddVideoObjectFromFile("LAPTOP/VideoConfTerminal.sti");
	CHECKF(guiVideoConfTerminal != NO_VOBJECT);

	// load the background snow for the video conf terminal
	guiBWSnow = AddVideoObjectFromFile("LAPTOP/BWSnow.sti");
	CHECKF(guiBWSnow != NO_VOBJECT);

	// load the fuzzy line for the video conf terminal
	guiFuzzLine = AddVideoObjectFromFile("LAPTOP/FuzzLine.sti");
	CHECKF(guiFuzzLine != NO_VOBJECT);

	// load the line distortion for the video conf terminal
	guiStraightLine = AddVideoObjectFromFile("LAPTOP/LineInterference.sti");
	CHECKF(guiStraightLine != NO_VOBJECT);

	// load the translucent snow for the video conf terminal
	guiTransSnow = AddVideoObjectFromFile("LAPTOP/TransSnow.sti");
	CHECKF(guiTransSnow != NO_VOBJECT);

	// load the translucent snow for the video conf terminal
	guiVideoContractCharge = AddVideoObjectFromFile("LAPTOP/VideoContractCharge.sti");
	CHECKF(guiVideoContractCharge != NO_VOBJECT);


	//** Mouse Regions **
	MSYS_DefineRegion( &gSelectedFaceRegion, PORTRAIT_X, PORTRAIT_Y , PORTRAIT_X + PORTRAIT_WIDTH , PORTRAIT_Y + PORTRAIT_HEIGHT, MSYS_PRIORITY_HIGH,
							 CURSOR_WWW, SelectFaceMovementRegionCallBack, SelectFaceRegionCallBack );

	//Set the fast help for the mouse region
//	SetRegionFastHelpText( &gSelectedFaceRegion, AimMemberText[ AIM_MEMBER_CLICK_INSTRUCTIONS ] );


	// if user clicks in the area, the merc will shut up!
	MSYS_DefineRegion( &gSelectedShutUpMercRegion, LAPTOP_SCREEN_UL_X, LAPTOP_SCREEN_WEB_UL_Y ,LAPTOP_SCREEN_LR_X, LAPTOP_SCREEN_WEB_LR_Y, MSYS_PRIORITY_HIGH-1,
							 CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, SelectShutUpMercRegionCallBack);
	//have it disbled at first
  MSYS_DisableRegion(&gSelectedShutUpMercRegion);


	//Button Regions
	giXToCloseVideoConfButtonImage = LoadButtonImage("LAPTOP/x_button.sti", -1,0,-1,1,-1 );


	guiPreviousContactNextButtonImage =  LoadButtonImage("LAPTOP/BottomButtons2.sti", -1,0,-1,1,-1 );

	giPreviousButton = MakeButton(CharacterInfo[AIM_MEMBER_PREVIOUS], PREVIOUS_X, BtnPreviousButtonCallback);
	giContactButton  = MakeButton(CharacterInfo[AIM_MEMBER_CONTACT],  CONTACT_X,  BtnContactButtonCallback);
	giNextButton     = MakeButton(CharacterInfo[AIM_MEMBER_NEXT],     NEXT_X,     BtnNextButtonCallback);

	gbCurrentSoldier = AimMercArray[gbCurrentIndex];

	gfStopMercFromTalking = FALSE;
	gubVideoConferencingMode = (UINT8)giCurrentSubPage;
	gubVideoConferencingPreviousMode = AIM_VIDEO_NOT_DISPLAYED_MODE;

	// if we are re-entering but the video conference should still be up
	if( gubVideoConferencingMode != 0 )
	{
		//if we need to re initialize the talking face
		if( gubVideoConferencingMode !=  AIM_VIDEO_FIRST_CONTACT_MERC_MODE)
			InitVideoFace(gbCurrentSoldier);

		InitDeleteVideoConferencePopUp();
	}




	InitAimMenuBar();
	InitAimDefaults();

	//LoadTextMercPopupImages( BASIC_MERC_POPUP_BACKGROUND, BASIC_MERC_POPUP_BORDER);

	RenderAIMMembers();
	gfIsNewMailFlagSet = FALSE;
	gfAimMemberCanMercSayOpeningQuote = TRUE;

	return( TRUE );
}


static BOOLEAN InitCreateDeleteAimPopUpBox(UINT8 ubFlag, const wchar_t* sString1, const wchar_t* sString2, UINT16 usPosX, UINT16 usPosY, UINT8 ubData);


void ExitAIMMembers()
{
	RemoveAimDefaults();

	//if we are exiting and the transfer of funds popup is enable, make sure we dont come back to it
	if( gubPopUpBoxAction )
		giCurrentSubPage = AIM_VIDEO_NOT_DISPLAYED_MODE;
	else
		giCurrentSubPage = gubVideoConferencingMode;

	gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	InitDeleteVideoConferencePopUp( );

	DeleteVideoSurface(guiVideoFaceBackground);

	DeleteVideoObject(guiStats);
	DeleteVideoObject(guiPrice);
	DeleteVideoObject(guiPortrait);
	DeleteVideoObject(guiWeaponBox);
	DeleteVideoObject(guiVideoConfPopup);
	DeleteVideoObject(guiVideoConfTerminal);
	DeleteVideoObject(guiBWSnow);
	DeleteVideoObject(guiFuzzLine);
	DeleteVideoObject(guiStraightLine);
	DeleteVideoObject(guiTransSnow);
	DeleteVideoObject(guiVideoContractCharge);

	UnloadButtonImage( guiPreviousContactNextButtonImage );
	UnloadButtonImage( giXToCloseVideoConfButtonImage );

	RemoveButton( giPreviousButton );
	RemoveButton( giContactButton );
	RemoveButton( giNextButton );

  MSYS_RemoveRegion( &gSelectedFaceRegion);
	MSYS_RemoveRegion( &gSelectedShutUpMercRegion);

	ExitAimMenuBar();

	InitCreateDeleteAimPopUpBox(AIM_POPUP_DELETE, NULL, NULL, 0, 0, 0);

	RemoveTextMercPopupImages( );

}


static void DelayMercSpeech(UINT8 ubMercID, UINT16 usQuoteNum, UINT16 usDelay, BOOLEAN fNewQuote, BOOLEAN fReset);
static BOOLEAN DisplayTalkingMercFaceForVideoPopUp(const FACETYPE*);
static BOOLEAN HandleCurrentVideoConfMode(void);
static void HandleMercAttitude(void);
static void HandleVideoDistortion(void);
static void StopMercTalking(void);

#ifdef JA2TESTVERSION
static void TempHandleAimMemberKeyBoardInput(void);
#endif


void HandleAIMMembers()
{
	//determine if the merc has a quote that is waiting to be said
	DelayMercSpeech( 0, 0, 0, FALSE, FALSE );

	if( gfHangUpMerc && !gfMercIsTalking)
	{
		if( gubVideoConferencingMode != AIM_VIDEO_NOT_DISPLAYED_MODE )
			gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
		gfHangUpMerc = FALSE;
	}

	if( gfStopMercFromTalking )
	{
		StopMercTalking();
		gfStopMercFromTalking = FALSE;
/*
		//if we were waiting for the merc to stop talking
		if( gfWaitingForMercToStopTalkingOrUserToClick )
		{
			gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
			gfWaitingForMercToStopTalkingOrUserToClick = FALSE;
		}
*/
	}

	// If we have to change video conference modes, change to new mode
	if( gubVideoConferencingMode != gubVideoConferencingPreviousMode && gubPopUpBoxAction != AIM_POPUP_DISPLAY )
	{
		InitDeleteVideoConferencePopUp( );

		//if we are exiting to display a popup box, dont rerender the display
		if( !fExitDueToMessageBox )
			gfRedrawScreen = TRUE;
	}


	// If we have to get rid of the popup box
	if( gubPopUpBoxAction == AIM_POPUP_DELETE )
	{
		InitCreateDeleteAimPopUpBox(AIM_POPUP_DELETE, NULL, NULL, 0, 0, 0);

		//if we are exiting to display a popup box, dont rerender the display
		if( !fExitDueToMessageBox )
			gfRedrawScreen = TRUE;
	}

	// Handle the current video conference screen
	HandleCurrentVideoConfMode();

	// if the face is active, display the talking face
	if( gfVideoFaceActive )
	{
		gfMercIsTalking = DisplayTalkingMercFaceForVideoPopUp( giMercFaceIndex );

		//put the noise lines on the screen
		if( !gfIsAnsweringMachineActive )
			HandleVideoDistortion();

		//to handle when/if the merc is getting po'ed (waiting for player to do something)
		if( !gfMercIsTalking )
			HandleMercAttitude();
	}

	//if we have to rerender the popup, set the flag to render the PostButtonRender function in laptop.c
	if( gubPopUpBoxAction == AIM_POPUP_DISPLAY )
	{
		fReDrawPostButtonRender = TRUE;
	}

	// Gets set in the InitDeleteVideoConferencePopUp() function
	gfJustSwitchedVideoConferenceMode = FALSE;

	if( gfRedrawScreen )
	{
		RenderAIMMembers();
		gfRedrawScreen = FALSE;
	}

	#ifdef JA2TESTVERSION
	TempHandleAimMemberKeyBoardInput();
	#endif


  MarkButtonsDirty( );
}

BOOLEAN RenderAIMMembersTopLevel()
{
	InitCreateDeleteAimPopUpBox( AIM_POPUP_DISPLAY, NULL, NULL, 0, 0, 0);

	return(TRUE);
}


static void DisplayAimMemberClickOnFaceHelpText(void);
static void DisplayMercStats(void);
static void DisplayMercsFace(void);
static BOOLEAN DisplayMercsInventory(UINT8 ubMercID);
static BOOLEAN DisplayVideoConferencingDisplay(void);
static BOOLEAN UpdateMercInfo(void);


BOOLEAN RenderAIMMembers()
{
	UINT16		x, uiPosX;
	wchar_t		wTemp[50];

	DrawAimDefaults();

	BltVideoObject(FRAME_BUFFER, guiStats, 0, STATS_X, STATS_Y);
	BltVideoObject(FRAME_BUFFER, guiPrice, 0, PRICE_X, PRICE_Y);

	uiPosX = WEAPONBOX_X;
	for(x=0; x<WEAPONBOX_NUMBER; x++)
	{
		BltVideoObject(FRAME_BUFFER, guiWeaponBox, 0, uiPosX, WEAPONBOX_Y);
		uiPosX += WEAPONBOX_SIZE_X;
	}

  UpdateMercInfo();

	//Draw fee & contract
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_FEE],      FEE_X,          FEE_Y,          0,                  AIM_M_FONT_PREV_NEXT_CONTACT, AIM_M_FEE_CONTRACT_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_CONTRACT], AIM_CONTRACT_X, AIM_CONTRACT_Y, AIM_CONTRACT_WIDTH, AIM_M_FONT_PREV_NEXT_CONTACT, AIM_M_FEE_CONTRACT_COLOR, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);

	//Draw pay period (day, week, 2 week)
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_1_DAY],   ONEDAY_X,  EXPLEVEL_Y,    AIM_CONTRACT_WIDTH, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_1_WEEK],  ONEWEEK_X, MARKSMAN_Y,    AIM_CONTRACT_WIDTH, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_2_WEEKS], TWOWEEK_X, MECHANAICAL_Y, AIM_CONTRACT_WIDTH, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);

	//Display AIM Member text
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_ACTIVE_MEMBERS], AIM_MEMBER_ACTIVE_TEXT_X, AIM_MEMBER_ACTIVE_TEXT_Y, AIM_MEMBER_ACTIVE_TEXT_WIDTH, AIM_MAINTITLE_FONT, AIM_M_ACTIVE_MEMBER_TITLE_COLOR, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);

	//Display Option Gear Cost text
	DrawTextToScreen(CharacterInfo[AIM_MEMBER_OPTIONAL_GEAR], AIM_MEMBER_OPTIONAL_GEAR_X, AIM_MEMBER_OPTIONAL_GEAR_Y, 0, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

	SPrintMoney(wTemp, gMercProfiles[gbCurrentSoldier].usOptionalGearCost);
	uiPosX = AIM_MEMBER_OPTIONAL_GEAR_X + StringPixLength( CharacterInfo[AIM_MEMBER_OPTIONAL_GEAR], AIM_M_FONT_STATIC_TEXT) + 5;
	DrawTextToScreen(wTemp, uiPosX, AIM_MEMBER_OPTIONAL_GEAR_Y, 0, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_DYNAMIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

	DisableAimButton();

	DisplayMercsInventory(gbCurrentSoldier);


	DisplayMercsFace();

	DisplayMercStats();

	if( gubVideoConferencingMode)
	{
		DisplayVideoConferencingDisplay();
	}
	else
	{
		gubMercAttitudeLevel=0;
		gfIsAnsweringMachineActive = FALSE;
	}

//	InitCreateDeleteAimPopUpBox( AIM_POPUP_DISPLAY, NULL, NULL, 0, 0, 0);

	//check to see if the merc is dead if so disable the contact button
	if( IsMercDead( gbCurrentSoldier ) )
	{
		DisableButton( giContactButton );
	}
	else
	{
		EnableButton( giContactButton );
	}


	//if we are to renbder the 'click face' text
	if(	gfAimMemberDisplayFaceHelpText )
	{
		DisplayAimMemberClickOnFaceHelpText();
	}




	RenderWWWProgramTitleBar( );
	DisplayProgramBoundingBox( TRUE );
	fReDrawScreenFlag = TRUE;

	return(TRUE);
}


BOOLEAN DrawNumeralsToScreen(INT32 iNumber, INT8 bWidth, UINT16 usLocX, UINT16 usLocY, UINT32 ulFont, UINT8 ubColor)
{
	wchar_t		sStr[10];
	swprintf(sStr, lengthof(sStr), L"%d", iNumber);
	DrawTextToScreen(sStr, usLocX, usLocY, bWidth, ulFont, ubColor, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);
	return(TRUE);
}


static BOOLEAN DrawMoneyToScreen(INT32 iNumber, INT8 bWidth, UINT16 usLocX, UINT16 usLocY, UINT32 ulFont, UINT8 ubColor)
{
	wchar_t		sStr[10];
	SPrintMoney(sStr, iNumber);
	DrawTextToScreen(sStr, usLocX, usLocY, bWidth, ulFont, ubColor, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);
	return(TRUE);
}


static void SelectFaceRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		guiCurrentLaptopMode = LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX;
	}
	else if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		//if the merc is not dead, video conference with the merc
		if( !IsMercDead( gbCurrentSoldier ) )
		{
			gubVideoConferencingMode = AIM_VIDEO_POPUP_MODE;
			gfFirstTimeInContactScreen = TRUE;
		}
	}
}


static void SelectFaceMovementRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if( iReason & MSYS_CALLBACK_REASON_LOST_MOUSE )
	{
		gfAimMemberDisplayFaceHelpText = FALSE;
		gfRedrawScreen = TRUE;
	}
	else if( iReason & MSYS_CALLBACK_REASON_GAIN_MOUSE )
	{
		gfAimMemberDisplayFaceHelpText = TRUE;
		gfRedrawScreen = TRUE;
	}
}


static BOOLEAN LoadMercBioInfo(UINT8 ubIndex, wchar_t* pInfoString, wchar_t* pAddInfo);


static BOOLEAN UpdateMercInfo(void)
{
	//Display the salaries
	DrawMoneyToScreen(gMercProfiles[gbCurrentSoldier].sSalary, FEE_WIDTH, FEE_X, HEALTH_Y, AIM_M_NUMBER_FONT, AIM_M_COLOR_DYNAMIC_TEXT	);
	DrawMoneyToScreen(gMercProfiles[gbCurrentSoldier].uiWeeklySalary, FEE_WIDTH, FEE_X, AGILITY_Y, AIM_M_NUMBER_FONT, AIM_M_COLOR_DYNAMIC_TEXT	);
	DrawMoneyToScreen(gMercProfiles[gbCurrentSoldier].uiBiWeeklySalary, FEE_WIDTH, FEE_X, DEXTERITY_Y, AIM_M_NUMBER_FONT, AIM_M_COLOR_DYNAMIC_TEXT	);

	//if medical deposit is required
	if( gMercProfiles[gbCurrentSoldier].bMedicalDeposit )
	{
		wchar_t	zTemp[40];
		wchar_t	sMedicalString[40];

		SPrintMoney(zTemp, gMercProfiles[gbCurrentSoldier].sMedicalDepositAmount);
		swprintf( sMedicalString, lengthof(sMedicalString), L"%ls %ls", zTemp, CharacterInfo[AIM_MEMBER_MEDICAL_DEPOSIT_REQ] );

		// If the string will be displayed in more then 2 lines, recenter the string
		if (DisplayWrappedString(0, 0, AIM_MEDICAL_DEPOSIT_WIDTH, 2, AIM_FONT12ARIAL, AIM_M_COLOR_DYNAMIC_TEXT, sMedicalString, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED | DONT_DISPLAY_TEXT) / GetFontHeight(AIM_FONT12ARIAL) > 2)
		{
			DisplayWrappedString(AIM_MEDICAL_DEPOSIT_X, AIM_MEDICAL_DEPOSIT_Y - GetFontHeight(AIM_FONT12ARIAL), AIM_MEDICAL_DEPOSIT_WIDTH, 2, AIM_FONT12ARIAL, AIM_M_COLOR_DYNAMIC_TEXT, sMedicalString, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
		}
		else
			DisplayWrappedString(AIM_MEDICAL_DEPOSIT_X, AIM_MEDICAL_DEPOSIT_Y, AIM_MEDICAL_DEPOSIT_WIDTH, 2, AIM_FONT12ARIAL, AIM_M_COLOR_DYNAMIC_TEXT, sMedicalString, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
	}

	wchar_t MercInfoString[SIZE_MERC_BIO_INFO];
	wchar_t AdditionalInfoString[SIZE_MERC_BIO_INFO];
	LoadMercBioInfo( gbCurrentSoldier, MercInfoString, AdditionalInfoString);
	if( MercInfoString[0] != 0)
	{
		DisplayWrappedString(AIM_MERC_INFO_X, AIM_MERC_INFO_Y, AIM_MERC_INFO_WIDTH, 2, AIM_M_FONT_DYNAMIC_TEXT, AIM_FONT_MCOLOR_WHITE, MercInfoString, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	}
	if( AdditionalInfoString[0] != 0)
	{
		DrawTextToScreen(CharacterInfo[AIM_MEMBER_ADDTNL_INFO], AIM_MERC_ADD_X, AIM_MERC_ADD_Y, 0, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
		DisplayWrappedString(AIM_MERC_ADD_INFO_X, AIM_MERC_ADD_INFO_Y, AIM_MERC_INFO_WIDTH, 2, AIM_M_FONT_DYNAMIC_TEXT, AIM_FONT_MCOLOR_WHITE, AdditionalInfoString, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	}

	return(TRUE);
}


static BOOLEAN LoadMercBioInfo(const UINT8 ubIndex, wchar_t* const pInfoString, wchar_t* const pAddInfo)
{
	UINT32 uiStartSeekAmount = (SIZE_MERC_BIO_INFO + SIZE_MERC_ADDITIONAL_INFO) * ubIndex;
	return
		LoadEncryptedDataFromFile(MERCBIOSFILENAME, pInfoString, uiStartSeekAmount,                      SIZE_MERC_BIO_INFO) &&
		LoadEncryptedDataFromFile(MERCBIOSFILENAME, pAddInfo,    uiStartSeekAmount + SIZE_MERC_BIO_INFO, SIZE_MERC_ADDITIONAL_INFO);
}


static BOOLEAN DisplayMercsInventory(UINT8 ubMercID)
{
	UINT8				ubItemCount=0;

	//if the mercs inventory has already been purchased, dont display the inventory
	if( gMercProfiles[ ubMercID ].ubMiscFlags & PROFILE_MISC_FLAG_ALREADY_USED_ITEMS )
		return( TRUE );

	INT16 PosY = WEAPONBOX_Y;
	INT16 PosX = WEAPONBOX_X + 3; // + 3 (1 to take care of the shadow, +2 to get past the weapon box border)
	for (UINT8 i = 0; i < NUM_INV_SLOTS; i++)
	{
		UINT16 usItem = gMercProfiles[ubMercID].inv[i];

		//if its a valid item AND we are only displaying less then 8 items
		if( usItem && ubItemCount < WEAPONBOX_NUMBER )
		{
			//increase the item count
			ubItemCount++;

			const INVTYPE* pItem = &Item[usItem];
			const SGPVObject* const item_vo = GetInterfaceGraphicForItem(pItem);
			const ETRLEObject* pTrav = GetVideoObjectETRLESubregionProperties(item_vo, pItem->ubGraphicNum);

			UINT32 usHeight = pTrav->usHeight;
			UINT32 usWidth  = pTrav->usWidth;

			INT16 sCenX = PosX + abs(WEAPONBOX_SIZE_X - 3 - usWidth)  / 2 - pTrav->sOffsetX;
			INT16 sCenY = PosY + abs(WEAPONBOX_SIZE_Y     - usHeight) / 2 - pTrav->sOffsetY;

			//blt the shadow of the item
			BltVideoObjectOutlineShadow(FRAME_BUFFER, item_vo, pItem->ubGraphicNum, sCenX - 2, sCenY + 2);
			//blt the item
			BltVideoObjectOutline(      FRAME_BUFFER, item_vo, pItem->ubGraphicNum, sCenX,     sCenY, TRANSPARENT);

			//if there are more then 1 piece of equipment in the current slot, display how many there are
			if( gMercProfiles[ubMercID].bInvNumber[ i ] > 1 )
			{
				wchar_t zTempStr[ 32 ];
//				UINT16	usWidthOfNumber;

				swprintf( zTempStr, lengthof(zTempStr), L"x%d", gMercProfiles[ ubMercID ].bInvNumber[ i ] );

				DrawTextToScreen(zTempStr, PosX - 1, PosY + 20, AIM_MEMBER_WEAPON_NAME_WIDTH, AIM_M_FONT_DYNAMIC_TEXT, AIM_M_WEAPON_TEXT_COLOR, FONT_MCOLOR_BLACK, RIGHT_JUSTIFIED);
			}
			else
			{
			}

			const wchar_t* ItemName = ShortItemNames[usItem];
			//if this will only be a single line, center it in the box
			if (DisplayWrappedString(PosX - 1, AIM_MEMBER_WEAPON_NAME_Y, AIM_MEMBER_WEAPON_NAME_WIDTH, 2, AIM_M_WEAPON_TEXT_FONT, AIM_M_WEAPON_TEXT_COLOR, ItemName, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED | DONT_DISPLAY_TEXT) / GetFontHeight(AIM_M_WEAPON_TEXT_FONT) == 1)
				DisplayWrappedString(PosX - 1, AIM_MEMBER_WEAPON_NAME_Y + GetFontHeight(AIM_M_WEAPON_TEXT_FONT) / 2, AIM_MEMBER_WEAPON_NAME_WIDTH, 2, AIM_M_WEAPON_TEXT_FONT, AIM_M_WEAPON_TEXT_COLOR, ItemName, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
			else
				DisplayWrappedString(PosX - 1, AIM_MEMBER_WEAPON_NAME_Y, AIM_MEMBER_WEAPON_NAME_WIDTH, 2, AIM_M_WEAPON_TEXT_FONT, AIM_M_WEAPON_TEXT_COLOR, ItemName, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);

			PosX += WEAPONBOX_SIZE_X;
		}
	}

	return(TRUE);
}


static void BtnPreviousButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		InitCreateDeleteAimPopUpBox(AIM_POPUP_DELETE, NULL, NULL, 0, 0, 0);

		gbCurrentIndex =
			(gbCurrentIndex > 0 ? gbCurrentIndex - 1 : MAX_NUMBER_MERCS - 1);

		gfRedrawScreen = TRUE;
		gbCurrentSoldier = AimMercArray[gbCurrentIndex];
		gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	}
}


static void BtnContactButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		// if we are not already in the video conferemce mode, go in to it
		if (!gubVideoConferencingMode)
		{
			gubVideoConferencingMode = AIM_VIDEO_POPUP_MODE;
//				gubVideoConferencingMode = AIM_VIDEO_INIT_MODE;
			gfFirstTimeInContactScreen = TRUE;
		}

		InitCreateDeleteAimPopUpBox(AIM_POPUP_DELETE, NULL, NULL, 0, 0, 0);
	}
}


static void BtnNextButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		InitCreateDeleteAimPopUpBox(AIM_POPUP_DELETE, NULL, NULL, 0, 0, 0);

		gbCurrentIndex =
			(gbCurrentIndex < MAX_NUMBER_MERCS - 1 ? gbCurrentIndex + 1 : 0);

		gbCurrentSoldier = AimMercArray[gbCurrentIndex];
		gfRedrawScreen = TRUE;
		gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	}
}


static void DisplayMercsFace(void)
{
	// see if the merc is currently hired
	const ProfileID          id = gbCurrentSoldier;
	const SOLDIERTYPE* const s  = FindSoldierByProfileIDOnPlayerTeam(id);

	// Portrait Frame
	BltVideoObject(FRAME_BUFFER, guiPortrait, 0, PORTRAIT_X, PORTRAIT_Y);

	// load the face graphic
	char sTemp[100];
  sprintf(sTemp, "FACES/BIGFACES/%02d.sti", id);
	SGPVObject* const face = AddVideoObjectFromFile(sTemp);
	CHECKV(face != NO_VOBJECT);

	BOOLEAN        shaded;
	const wchar_t* text;
	if (IsMercDead(id))
	{
		// the merc is dead, so shade the face red
		face->pShades[0] = Create16BPPPaletteShaded(face->pPaletteEntry, DEAD_MERC_COLOR_RED, DEAD_MERC_COLOR_GREEN, DEAD_MERC_COLOR_BLUE, TRUE);
		SetObjectShade(face, 0);
		shaded = FALSE;
		text   = AimPopUpText[AIM_MEMBER_DEAD];
	}
	else if (gMercProfiles[id].bMercStatus == MERC_FIRED_AS_A_POW || (s && s->bAssignment == ASSIGNMENT_POW))
	{
		// the merc is currently a POW or, the merc was fired as a pow
		shaded = TRUE;
		text   = pPOWStrings[0];
	}
	else if (s != NULL)
	{
		// the merc has already been hired
		shaded = TRUE;
		text   = MercInfo[MERC_FILES_ALREADY_HIRED];
	}
	else if (!IsMercHireable(id))
	{
		// the merc has a text file and the merc is not away
		shaded = TRUE;
		text   = AimPopUpText[AIM_MEMBER_ON_ASSIGNMENT];
	}
	else
	{
		shaded = FALSE;
		text   = NULL;
	}

	BltVideoObject(FRAME_BUFFER, face, 0, FACE_X, FACE_Y);
	DeleteVideoObject(face);

	if (shaded)
	{
		ShadowVideoSurfaceRect(FRAME_BUFFER, FACE_X, FACE_Y, FACE_X + FACE_WIDTH, FACE_Y + FACE_HEIGHT);
	}

	if (text != NULL)
	{
		DrawTextToScreen(text, FACE_X + 1, FACE_Y + 107, FACE_WIDTH, FONT14ARIAL, 145, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
	}
}


static void DisplayDots(UINT16 usNameX, UINT16 usNameY, UINT16 usStatX, const wchar_t* pString);


static void DrawStatColoured(const UINT16 x, const UINT16 y, const wchar_t* const stat, const INT32 val, const UINT8 colour)
{
	DrawTextToScreen(stat, x, y, 0, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	DisplayDots(x, y, x + STAT_NAME_WIDTH, stat);
	DrawNumeralsToScreen(val, 3, x + STAT_VALUE_DX, y, AIM_M_NUMBER_FONT, colour);
}


static void DrawStat(const UINT16 x, const UINT16 y, const wchar_t* const stat, const INT32 val)
{
	const UINT8 colour = (val >= 80 ? HIGH_STAT_COLOR : (val >= 50 ? MED_STAT_COLOR : LOW_STAT_COLOR));
	DrawStatColoured(x, y, stat, val, colour);
}


static void DisplayMercStats(void)
{
	const MERCPROFILESTRUCT* const p = GetProfile(gbCurrentSoldier);

	//Name
	DrawTextToScreen(p->zName, NAME_X, NAME_Y, 0, FONT14ARIAL, AIM_M_COLOR_DYNAMIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

	//First column in stats box.  Health, Agility, dexterity, strength, leadership, wisdom
	const UINT16 x1 = STATS_FIRST_COL;
	DrawStat(x1, HEALTH_Y,     str_stat_health,     p->bLife      );
	DrawStat(x1, AGILITY_Y,    str_stat_agility,    p->bAgility   );
	DrawStat(x1, DEXTERITY_Y,  str_stat_dexterity,  p->bDexterity );
	DrawStat(x1, STRENGTH_Y,   str_stat_strength,   p->bStrength  );
	DrawStat(x1, LEADERSHIP_Y, str_stat_leadership, p->bLeadership);
	DrawStat(x1, WISDOM_Y,     str_stat_wisdom,     p->bWisdom    );

	//Second column in stats box.  Exp.Level, Markmanship, mechanical, explosive, medical
	const UINT16 x2 = STATS_SECOND_COL;
	DrawStatColoured(x2, EXPLEVEL_Y,    str_stat_exp_level,    p->bExpLevel, FONT_MCOLOR_WHITE);
	DrawStat(        x2, MARKSMAN_Y,    str_stat_marksmanship, p->bMarksmanship);
	DrawStat(        x2, MECHANAICAL_Y, str_stat_mechanical,   p->bMechanical  );
	DrawStat(        x2, EXPLOSIVE_Y,   str_stat_explosive,    p->bExplosive   );
	DrawStat(        x2, MEDICAL_Y,     str_stat_medical,      p->bMedical     );
}


//displays the dots between the stats and the stat name
static void DisplayDots(UINT16 usNameX, UINT16 usNameY, UINT16 usStatX, const wchar_t* pString)
{
	UINT16 usStringLength = StringPixLength(pString, AIM_M_FONT_STATIC_TEXT);
	INT16	 i;
	UINT16 usPosX;

	usPosX = usStatX;
	for(i=usNameX + usStringLength; i <= usPosX; usPosX-=7)
	{
		DrawTextToScreen(L".", usPosX, usNameY, 0, AIM_M_FONT_STATIC_TEXT, AIM_M_COLOR_STATIC_TEXT, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	}
}


static void DisplayMercChargeAmount(void);
static void DisplaySelectLights(BOOLEAN fContractDown, BOOLEAN fBuyEquipDown);


static void BtnContractLengthButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		UINT8	ubRetValue = MSYS_GetBtnUserData(btn);

		gubContractLength = ubRetValue;
		DisplaySelectLights(TRUE, FALSE);
	}
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		DisplaySelectLights(FALSE, FALSE);
		guiMercAttitudeTime = GetJA2Clock();
		DisplayMercChargeAmount();
	}
}


static void BtnBuyEquipmentButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		gfBuyEquipment = MSYS_GetBtnUserData(btn);
		DisplaySelectLights(FALSE, TRUE);
	}
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		DisplaySelectLights(FALSE, FALSE);
		DisplayMercChargeAmount();
		guiMercAttitudeTime = GetJA2Clock();
	}
}


static INT8 AimMemberHireMerc(void);
static BOOLEAN CanMercBeHired(void);
static BOOLEAN EnableDisableCurrentVideoConferenceButtons(BOOLEAN fEnable);


//Transfer funds button callback
static void BtnAuthorizeButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		UINT8	ubRetValue = MSYS_GetBtnUserData(btn);

		gfStopMercFromTalking = TRUE;
		gubMercAttitudeLevel = QUOTE_DELAY_NO_ACTION;

		//If we try to hire the merc
		if (ubRetValue == 0)
		{
			StopMercTalking();

			//can the merc be hired?  (does he like/not like people on the team
//			if( CanMercBeHired() )
			{
				//Was the merc hired
				if (AimMemberHireMerc())
				{
					// if merc was hired
					InitCreateDeleteAimPopUpBox(AIM_POPUP_CREATE, AimPopUpText[AIM_MEMBER_FUNDS_TRANSFER_SUCCESFUL], NULL, AIM_POPUP_BOX_X, AIM_POPUP_BOX_Y, AIM_POPUP_BOX_SUCCESS);
					DelayMercSpeech( gbCurrentSoldier, QUOTE_CONTRACT_ACCEPTANCE, 750, TRUE, FALSE );

					//Disable the buttons behind the message box
					EnableDisableCurrentVideoConferenceButtons( TRUE );

					SpecifyDisabledButtonStyle( giBuyEquipmentButton[0], DISABLED_STYLE_NONE );
					SpecifyDisabledButtonStyle( giBuyEquipmentButton[1], DISABLED_STYLE_NONE );
				}
			}
#if 0 // XXX was commented out
			else
			{
				//else the merc doesnt like a player on the team, hang up when the merc is done complaining

				//reset ( in case merc was going to say something
				DelayMercSpeech( 0, 0, 0, FALSE, TRUE );

				gubVideoConferencingMode = AIM_VIDEO_HIRE_MERC_MODE;
			}
#endif
		}
		else
		{
			// else we cancel
			gubVideoConferencingMode = AIM_VIDEO_FIRST_CONTACT_MERC_MODE;
		}
	}
}


static INT8 AimMemberHireMerc(void)
{
	MERC_HIRE_STRUCT HireMercStruct;
	UINT8		ubCurrentSoldier = AimMercArray[gbCurrentIndex];
	INT8		bReturnCode;
	INT8		bTypeOfContract=0;


	if( LaptopSaveInfo.iCurrentBalance < giContractAmount )
	{
		//wasnt hired because of lack of funds
		InitCreateDeleteAimPopUpBox(AIM_POPUP_CREATE, AimPopUpText[AIM_MEMBER_FUNDS_TRANSFER_FAILED], AimPopUpText[AIM_MEMBER_NOT_ENOUGH_FUNDS], AIM_POPUP_BOX_X, AIM_POPUP_BOX_Y, AIM_POPUP_BOX_FAILURE);

		//Disable the buttons behind the message box
		EnableDisableCurrentVideoConferenceButtons( TRUE );

		SpecifyDisabledButtonStyle( giBuyEquipmentButton[0], DISABLED_STYLE_NONE );
		SpecifyDisabledButtonStyle( giBuyEquipmentButton[1], DISABLED_STYLE_NONE );

		DelayMercSpeech( gbCurrentSoldier, QUOTE_REFUSAL_TO_JOIN_LACK_OF_FUNDS, 750, TRUE, FALSE );

		return(FALSE);
	}

	memset(&HireMercStruct, 0, sizeof(MERC_HIRE_STRUCT));

	HireMercStruct.ubProfileID = ubCurrentSoldier;

	//DEF: temp
	HireMercStruct.sSectorX = gsMercArriveSectorX;
	HireMercStruct.sSectorY = gsMercArriveSectorY;
	HireMercStruct.fUseLandingZoneForArrival = TRUE;
	HireMercStruct.ubInsertionCode	= INSERTION_CODE_ARRIVING_GAME;

	HireMercStruct.fCopyProfileItemsOver = gfBuyEquipment;
	//if the players is buyibng the equipment
	if( gfBuyEquipment )
	{
		gMercProfiles[ ubCurrentSoldier ].ubMiscFlags |= PROFILE_MISC_FLAG_ALREADY_USED_ITEMS;
	}

		//If 1 day
	if( gubContractLength == AIM_CONTRACT_LENGTH_ONE_DAY)
	{
		bTypeOfContract = CONTRACT_EXTEND_1_DAY;
		HireMercStruct.iTotalContractLength = 1;
	}
	else if( gubContractLength == AIM_CONTRACT_LENGTH_ONE_WEEK)
	{
		bTypeOfContract = CONTRACT_EXTEND_1_WEEK;
		HireMercStruct.iTotalContractLength = 7;
	}
	else if( gubContractLength == AIM_CONTRACT_LENGTH_TWO_WEEKS)
	{
		bTypeOfContract = CONTRACT_EXTEND_2_WEEK;
		HireMercStruct.iTotalContractLength = 14;
	}

	//specify when the merc should arrive
	HireMercStruct.uiTimeTillMercArrives = GetMercArrivalTimeOfDay( );// + ubCurrentSoldier

	//Set the time and ID of the last hired merc will arrive
//	LaptopSaveInfo.sLastHiredMerc.iIdOfMerc = HireMercStruct.ubProfileID;
//	LaptopSaveInfo.sLastHiredMerc.uiArrivalTime = HireMercStruct.uiTimeTillMercArrives;

	//if we succesfully hired the merc
	bReturnCode = HireMerc( &HireMercStruct );
	if( bReturnCode == MERC_HIRE_OVER_20_MERCS_HIRED )
	{
		//display a warning saying u cant hire more then 20 mercs
		DoLapTopMessageBox( MSG_BOX_LAPTOP_DEFAULT, AimPopUpText[ AIM_MEMBER_ALREADY_HAVE_20_MERCS ], LAPTOP_SCREEN, MSG_BOX_FLAG_OK, NULL);
		return(FALSE);
	}
	else if( bReturnCode == MERC_HIRE_FAILED )
	{
		return(FALSE);
	}

	//Set the type of contract the merc is on
	SOLDIERTYPE* const s = FindSoldierByProfileIDOnPlayerTeam(ubCurrentSoldier);
	if (s == NULL) return FALSE;
	s->bTypeOfLastContract = bTypeOfContract;

	//add an entry in the finacial page for the hiring of the merc
	AddTransactionToPlayersBook(HIRED_MERC, ubCurrentSoldier, GetWorldTotalMin(), -( giContractAmount - gMercProfiles[gbCurrentSoldier].sMedicalDepositAmount ) );

	if( gMercProfiles[ gbCurrentSoldier ].bMedicalDeposit )
	{
		//add an entry in the finacial page for the medical deposit
		AddTransactionToPlayersBook(	MEDICAL_DEPOSIT, ubCurrentSoldier, GetWorldTotalMin(), -(gMercProfiles[gbCurrentSoldier].sMedicalDepositAmount) );
	}

	//add an entry in the history page for the hiring of the merc
	AddHistoryToPlayersLog(HISTORY_HIRED_MERC_FROM_AIM, ubCurrentSoldier, GetWorldTotalMin(), -1, -1 );
	return(TRUE);
}


static BOOLEAN DisplayMercsVideoFace(void);


static BOOLEAN DisplayVideoConferencingDisplay(void)
{
	wchar_t		sMercName[128];

	if( ( gubVideoConferencingMode == AIM_VIDEO_NOT_DISPLAYED_MODE ) || ( gubVideoConferencingMode == AIM_VIDEO_POPUP_MODE ) )
		return(FALSE);

	DisplayMercsVideoFace();

	//Title & Name
	if( gubVideoConferencingMode == AIM_VIDEO_INIT_MODE)
	{
		wcslcpy(sMercName, VideoConfercingText[AIM_MEMBER_CONNECTING], lengthof(sMercName));
		DrawTextToScreen(sMercName, AIM_MEMBER_VIDEO_NAME_X, AIM_MEMBER_VIDEO_NAME_Y, 0, FONT12ARIAL, AIM_M_VIDEO_TITLE_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	}
	else
	{
		swprintf( sMercName, lengthof(sMercName), L"%ls %ls",  VideoConfercingText[AIM_MEMBER_VIDEO_CONF_WITH], gMercProfiles[gbCurrentSoldier].zName);
		DrawTextToScreen(sMercName, AIM_MEMBER_VIDEO_NAME_X, AIM_MEMBER_VIDEO_NAME_Y, 0, FONT12ARIAL, AIM_M_VIDEO_TITLE_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	}

	//Display Contract charge text
	if( gubVideoConferencingMode == AIM_VIDEO_HIRE_MERC_MODE )
	{
		// Display the contract charge
		SetFontShadow(AIM_M_VIDEO_NAME_SHADOWCOLOR);
		DrawTextToScreen(VideoConfercingText[AIM_MEMBER_CONTRACT_CHARGE], AIM_CONTRACT_CHARGE_X, AIM_CONTRACT_CHARGE_Y, 0, FONT12ARIAL, AIM_M_VIDEO_NAME_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
		SetFontShadow(DEFAULT_SHADOW);
	}

	DisplayMercChargeAmount();

//	if( gfMercIsTalking && !gfIsAnsweringMachineActive)
	if( gfMercIsTalking && gGameSettings.fOptions[ TOPTION_SUBTITLES ] )
	{
		UINT16 usActualWidth;
		UINT16 usActualHeight;
		UINT16 usPosX;

		iAimMembersBoxId = PrepareMercPopupBox( iAimMembersBoxId ,BASIC_MERC_POPUP_BACKGROUND, BASIC_MERC_POPUP_BORDER, gsTalkingMercText, 300, 0, 0, 0, &usActualWidth, &usActualHeight);

		usPosX = ( LAPTOP_SCREEN_LR_X - usActualWidth ) / 2 ;

		RenderMercPopUpBoxFromIndex( iAimMembersBoxId, usPosX, TEXT_POPUP_WINDOW_Y, FRAME_BUFFER);

		if( RemoveMercPopupBoxFromIndex( iAimMembersBoxId ) )
		{
			iAimMembersBoxId = -1;
		}
	}

  InvalidateRegion(LAPTOP_SCREEN_UL_X,LAPTOP_SCREEN_WEB_UL_Y,LAPTOP_SCREEN_LR_X,LAPTOP_SCREEN_WEB_LR_Y);

	return(TRUE);
}


static BOOLEAN DisplayMercsVideoFace(void)
{
	const ETRLEObject* e = GetVideoObjectETRLESubregionProperties(guiVideoConfTerminal, 0);
	const INT32 x = AIM_MEMBER_VIDEO_CONF_TERMINAL_X;
	const INT32 y = AIM_MEMBER_VIDEO_CONF_TERMINAL_Y;
	const INT32 w = e->usWidth;
	const INT32 h = e->usHeight;

	// Draw a drop shadow
	ShadowVideoSurfaceRect(FRAME_BUFFER, x + 3, y + h, x + w,     y + h + 3); // Horizontal
	ShadowVideoSurfaceRect(FRAME_BUFFER, x + w, y + 3, x + w + 3, y + h);     // Vertical

	BltVideoObject(FRAME_BUFFER, guiVideoConfTerminal, 0, x, y);

	//Display the Select light on the merc
	if(gubVideoConferencingMode == AIM_VIDEO_HIRE_MERC_MODE)
		DisplaySelectLights(FALSE, FALSE);

	return(TRUE);
}


static void DisplaySelectLights(BOOLEAN fContractDown, BOOLEAN fBuyEquipDown)
{
	UINT16 i, usPosY, usPosX;

	//First draw the select light for the contract length buttons
	usPosY = AIM_MEMBER_BUY_CONTRACT_LENGTH_Y;
	for(i=0; i<3; i++)
	{
		// if the if is true, the light is on
		if( gubContractLength == i)
		{
			if( fContractDown)
			{
				usPosX = AIM_MEMBER_BUY_CONTRACT_LENGTH_X + AIM_SELECT_LIGHT_ON_X;
				ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_ON_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_ON_Y+8, Get16BPPColor( FROMRGB( 0, 255, 0 ) ) );
			}
			else
			{
				usPosX = AIM_MEMBER_BUY_CONTRACT_LENGTH_X + AIM_SELECT_LIGHT_OFF_X;
				ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_OFF_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_OFF_Y+8, Get16BPPColor( FROMRGB( 0, 255, 0 ) ) );
			}
		}
		else
		{
			usPosX = AIM_MEMBER_BUY_CONTRACT_LENGTH_X + AIM_SELECT_LIGHT_OFF_X;
			ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_OFF_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_OFF_Y+8, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );
		}
		usPosY += AIM_MEMBER_BUY_EQUIPMENT_GAP;
	}

	//draw the select light for the buy equipment buttons
	usPosY = AIM_MEMBER_BUY_CONTRACT_LENGTH_Y;
	for(i=0; i<2; i++)
	{
		if( gfBuyEquipment == i)
		{
			if( fBuyEquipDown)
			{
				usPosX = AIM_MEMBER_BUY_EQUIPMENT_X + AIM_SELECT_LIGHT_ON_X;
				ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_ON_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_ON_Y+8, Get16BPPColor( FROMRGB( 0, 255, 0 ) ) );
			}
			else
			{
				usPosX = AIM_MEMBER_BUY_EQUIPMENT_X + AIM_SELECT_LIGHT_OFF_X;
				ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_OFF_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_OFF_Y+8, Get16BPPColor( FROMRGB( 0, 255, 0 ) ) );
			}
		}
		else
		{
			usPosX = AIM_MEMBER_BUY_EQUIPMENT_X + AIM_SELECT_LIGHT_OFF_X;
			ColorFillVideoSurfaceArea( FRAME_BUFFER, usPosX, usPosY+AIM_SELECT_LIGHT_OFF_Y, usPosX+8,	usPosY+AIM_SELECT_LIGHT_OFF_Y+8, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );
		}
		usPosY += AIM_MEMBER_BUY_EQUIPMENT_GAP;
	}
  InvalidateRegion(LAPTOP_SCREEN_UL_X,LAPTOP_SCREEN_WEB_UL_Y,LAPTOP_SCREEN_LR_X,LAPTOP_SCREEN_WEB_LR_Y);
}


static void DisplayMercChargeAmount(void)
{
	if (gubVideoConferencingMode != AIM_VIDEO_HIRE_MERC_MODE) return;

	// Display the 'black hole' for the contract charge  in the video conference terminal
	BltVideoObject(FRAME_BUFFER, guiVideoContractCharge, 0, AIM_MEMBER_VIDEO_CONF_CONTRACT_IMAGE_X, AIM_MEMBER_VIDEO_CONF_CONTRACT_IMAGE_Y);

	const MERCPROFILESTRUCT* const p = GetProfile(gbCurrentSoldier);

	if (FindSoldierByProfileIDOnPlayerTeam(gbCurrentSoldier) == NULL)
	{
		//the contract charge amount
		INT32 amount;

		// Get the salary rate
		switch (gubContractLength)
		{
			case AIM_CONTRACT_LENGTH_ONE_DAY:   amount = p->sSalary;          break;
			case AIM_CONTRACT_LENGTH_ONE_WEEK:  amount = p->uiWeeklySalary;   break;
			case AIM_CONTRACT_LENGTH_TWO_WEEKS: amount = p->uiBiWeeklySalary; break;
			default:                            amount = 0;                   break;
		}

		// If there is a medical deposit, add it in
		if (p->bMedicalDeposit) amount += p->sMedicalDepositAmount;

		// If hired with the equipment, add it in aswell
		if (gfBuyEquipment) amount += p->usOptionalGearCost;

		giContractAmount = amount;
	}

	wchar_t wDollarTemp[50];
	SPrintMoney(wDollarTemp, giContractAmount);

	wchar_t wTemp[50];
	if (p->bMedicalDeposit)
	{
		swprintf(wTemp, lengthof(wTemp), L"%ls %ls", wDollarTemp, VideoConfercingText[AIM_MEMBER_WITH_MEDICAL]);
	}
	else
	{
		wcslcpy(wTemp, wDollarTemp, lengthof(wTemp));
	}
	DrawTextToScreen(wTemp, AIM_CONTRACT_CHARGE_AMOUNNT_X + 1, AIM_CONTRACT_CHARGE_AMOUNNT_Y + 3, 0, AIM_M_VIDEO_CONTRACT_AMOUNT_FONT, AIM_M_VIDEO_CONTRACT_AMOUNT_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
}


static void BtnPopUpOkButtonCallback(GUI_BUTTON* btn, INT32 reason);


static BOOLEAN InitCreateDeleteAimPopUpBox(UINT8 ubFlag, const wchar_t* sString1, const wchar_t* sString2, UINT16 usPosX, UINT16 usPosY, UINT8 ubData)
{
	static UINT16				usPopUpBoxPosX, usPopUpBoxPosY;
	static wchar_t				sPopUpString1[400], sPopUpString2[400];
	static BOOLEAN fPopUpBoxActive = FALSE;

	switch( ubFlag )
	{
		case AIM_POPUP_CREATE:
		{
			if( fPopUpBoxActive )
				return(FALSE);

			//Disable the 'X' to close the pop upi video
			DisableButton( giXToCloseVideoConfButton );

			if(sString1 != NULL)
				wcscpy(sPopUpString1, sString1);
			else
				sPopUpString1[0] = L'\0';

			if(sString2 != NULL)
				wcscpy(sPopUpString2, sString2);
			else
				sPopUpString2[0] = L'\0';

			usPopUpBoxPosX = usPosX;
			usPopUpBoxPosY = usPosY;

			// load the popup box graphic
			guiPopUpBox = AddVideoObjectFromFile("LAPTOP/VideoConfPopUp.sti");
			CHECKF(guiPopUpBox != NO_VOBJECT);

			BltVideoObject(FRAME_BUFFER, guiPopUpBox, 0, usPosX, usPosY);

			//Create the popup boxes button
			guiPopUpImage = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,2,-1,3,-1 );
			guiPopUpOkButton = CreateIconAndTextButton( guiPopUpImage, VideoConfercingText[AIM_MEMBER_OK],
															 FONT14ARIAL,
															 AIM_POPUP_BOX_COLOR, AIM_M_VIDEO_NAME_SHADOWCOLOR,
															 AIM_POPUP_BOX_COLOR, AIM_M_VIDEO_NAME_SHADOWCOLOR,
															 usPosX + AIM_POPUP_BOX_BUTTON_OFFSET_X, usPosY + AIM_POPUP_BOX_BUTTON_OFFSET_Y, MSYS_PRIORITY_HIGH + 5,
															 BtnPopUpOkButtonCallback);
			SetButtonCursor(guiPopUpOkButton, CURSOR_LAPTOP_SCREEN);
			MSYS_SetBtnUserData(guiPopUpOkButton, ubData);

			fPopUpBoxActive = TRUE;
			gubPopUpBoxAction = AIM_POPUP_DISPLAY;


			// Disable the current video conference buttons
			//EnableDisableCurrentVideoConferenceButtons(TRUE);
			if( gubVideoConferencingPreviousMode == AIM_VIDEO_HIRE_MERC_MODE )
			{
				// Enable the current video conference buttons
				EnableDisableCurrentVideoConferenceButtons(FALSE);
			}



//
//	Create a new flag for the PostButtonRendering function
//
			fReDrawPostButtonRender = TRUE;
		}
		break;

		case AIM_POPUP_DISPLAY:
		{
			UINT16				usTempPosY = usPopUpBoxPosY;

			if( gubPopUpBoxAction != AIM_POPUP_DISPLAY)
				return(FALSE);

			BltVideoObject(FRAME_BUFFER, guiPopUpBox, 0, usPopUpBoxPosX, usPopUpBoxPosY);

			SetFontShadow(AIM_M_VIDEO_NAME_SHADOWCOLOR);

			usTempPosY += AIM_POPUP_BOX_STRING1_Y;
			if( sPopUpString1[0]  != L'\0')
				usTempPosY += DisplayWrappedString(usPopUpBoxPosX, usTempPosY, AIM_POPUP_BOX_WIDTH, 2, AIM_POPUP_BOX_FONT, AIM_POPUP_BOX_COLOR, sPopUpString1, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
			if( sPopUpString2[0] != L'\0')
				DisplayWrappedString(usPopUpBoxPosX, usTempPosY + 4, AIM_POPUP_BOX_WIDTH, 2, AIM_POPUP_BOX_FONT, AIM_POPUP_BOX_COLOR, sPopUpString2, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);

			SetFontShadow(DEFAULT_SHADOW);

			InvalidateRegion(LAPTOP_SCREEN_UL_X,LAPTOP_SCREEN_WEB_UL_Y,LAPTOP_SCREEN_LR_X,LAPTOP_SCREEN_WEB_LR_Y);

		}
		break;

		case AIM_POPUP_DELETE:
		{
			if( !fPopUpBoxActive )
				return(FALSE);

			//Disable the 'X' to close the pop upi video
			EnableButton( giXToCloseVideoConfButton );

			UnloadButtonImage( guiPopUpImage );
			RemoveButton( guiPopUpOkButton );
			DeleteVideoObject(guiPopUpBox);

			fPopUpBoxActive = FALSE;
			gubPopUpBoxAction = AIM_POPUP_NOTHING;

			if( gubVideoConferencingPreviousMode == AIM_VIDEO_HIRE_MERC_MODE )
			{
				// Enable the current video conference buttons
				EnableDisableCurrentVideoConferenceButtons(FALSE);
			}
			else if( gubVideoConferencingPreviousMode == AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE )
			{
				EnableButton( giAnsweringMachineButton[1]);
			}
		}
		break;
	}

	return(TRUE);
}


static void WaitForMercToFinishTalkingOrUserToClick(void);


static void BtnPopUpOkButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	static BOOLEAN fInCallback = TRUE;

	if (fInCallback)
	{
		if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
		{
			UINT8 ubCurPageNum = MSYS_GetBtnUserData(btn);

			fInCallback = FALSE;

//			gfStopMercFromTalking = TRUE;

			gubPopUpBoxAction = AIM_POPUP_DELETE;

			if (gubVideoConferencingMode != AIM_VIDEO_NOT_DISPLAYED_MODE)
			{
				if (ubCurPageNum == AIM_POPUP_BOX_SUCCESS)
				{
					gubVideoConferencingMode = AIM_VIDEO_HIRE_MERC_MODE;
					WaitForMercToFinishTalkingOrUserToClick();
				}
					//				gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
				else
					gubVideoConferencingMode = AIM_VIDEO_HIRE_MERC_MODE;
			}

			fInCallback = TRUE;
		}
	}
}


// we first contact merc.  We either go to hire him or cancel the call
static void BtnFirstContactButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		UINT8	ubRetValue = MSYS_GetBtnUserData(btn);

//		gfStopMercFromTalking = TRUE;
		StopMercTalking();

		gfAimMemberCanMercSayOpeningQuote = FALSE;

		if (ubRetValue == 0)
		{
			if (CanMercBeHired())
			{
				gubVideoConferencingMode = AIM_VIDEO_HIRE_MERC_MODE;
			}
		}
		else
		{
			gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
		}
	}
}


static void BtnAnsweringMachineButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		UINT8	ubRetValue = MSYS_GetBtnUserData(btn);

		if (ubRetValue == 0)
		{
			//Set a flag indicating that the merc has a message
			gMercProfiles[gbCurrentSoldier].ubMiscFlags3 |= PROFILE_MISC_FLAG3_PLAYER_LEFT_MSG_FOR_MERC_AT_AIM;
			WaitForMercToFinishTalkingOrUserToClick();

			//Display a message box displaying a messsage that the message was recorded
//			DoLapTopMessageBox(10, AimPopUpText[AIM_MEMBER_MESSAGE_RECORDED], LAPTOP_SCREEN, MSG_BOX_FLAG_OK, NULL);
			InitCreateDeleteAimPopUpBox(AIM_POPUP_CREATE, L" ", AimPopUpText[AIM_MEMBER_MESSAGE_RECORDED], AIM_POPUP_BOX_X, AIM_POPUP_BOX_Y, AIM_POPUP_BOX_SUCCESS);


			SpecifyDisabledButtonStyle(giAnsweringMachineButton[1], DISABLED_STYLE_NONE);
			DisableButton(giAnsweringMachineButton[1]);
			DisableButton(giAnsweringMachineButton[0]);
		}
		else
		{
			gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
//			gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
		}
	}
}


static void BtnHangUpButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
//		gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
		gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
	}
}


// InitVideoFace() is called once to initialize things
static BOOLEAN InitVideoFace(UINT8 ubMercID)
{
	//Create the facial index
	giMercFaceIndex = InitFace(ubMercID, NULL, 0);

	SetAutoFaceActive( guiVideoFaceBackground, FACE_AUTO_RESTORE_BUFFER , giMercFaceIndex, 0, 0);

	RenderAutoFace( giMercFaceIndex );

	gubCurrentStaticMode = VC_NO_STATIC;

	gfVideoFaceActive = TRUE;

	guiMercAttitudeTime = GetJA2Clock();

	return(TRUE);
}


// InitVideoFaceTalking() is called to start a merc speaking a particular message
static BOOLEAN InitVideoFaceTalking(UINT8 ubMercID, UINT16 usQuoteNum)
{

	//Starts the merc talking
	if(!CharacterDialogue( ubMercID, usQuoteNum, giMercFaceIndex, DIALOGUE_CONTACTPAGE_UI, FALSE , FALSE) )
	{
		return(FALSE);
	}

	//Enables it so if a player clicks, he will shutup the merc
	MSYS_EnableRegion(&gSelectedShutUpMercRegion);

	gfIsShutUpMouseRegionActive = TRUE;
	gfMercIsTalking = TRUE;
	guiTimeThatMercStartedTalking	= GetJA2Clock();
	return(TRUE);
}


static BOOLEAN DisplayTalkingMercFaceForVideoPopUp(const FACETYPE* const face)
{
	static BOOLEAN fWasTheMercTalking=FALSE;
	BOOLEAN		fIsTheMercTalking;
	SGPRect		SrcRect;
	SGPRect		DestRect;


	//Test
	SrcRect.iLeft = 0;
	SrcRect.iTop = 0;
	SrcRect.iRight = 48;
	SrcRect.iBottom = 43;

	DestRect.iLeft = AIM_MEMBER_VIDEO_FACE_X;
	DestRect.iTop = AIM_MEMBER_VIDEO_FACE_Y;
	DestRect.iRight = DestRect.iLeft + AIM_MEMBER_VIDEO_FACE_WIDTH;
	DestRect.iBottom = DestRect.iTop + AIM_MEMBER_VIDEO_FACE_HEIGHT;

	//If the answering machine graphics is up, dont handle the faces
	if (gfIsAnsweringMachineActive) giMercFaceIndex->fInvalidAnim = TRUE;

	HandleDialogue();
	HandleAutoFaces( );
	HandleTalkingAutoFaces( );

	//If the answering machine is up, dont display the face
//	if( !gfIsAnsweringMachineActive )
	{
		//Blt the face surface to the video background surface
		if (!BltStretchVideoSurface(FRAME_BUFFER, guiVideoFaceBackground, &SrcRect, &DestRect))
			return(FALSE);

		//if the merc is not at home and the players is leaving a message, shade the players face
		if( gfIsAnsweringMachineActive )
			ShadowVideoSurfaceRect( FRAME_BUFFER, DestRect.iLeft, DestRect.iTop, DestRect.iRight-1, DestRect.iBottom-1 );


		//If the answering machine graphics is up, place a message on the screen
		if( gfIsAnsweringMachineActive )
		{
			//display a message over the mercs face
			DisplayWrappedString(AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y + 20, AIM_MEMBER_VIDEO_FACE_WIDTH, 2, FONT14ARIAL, 145, AimPopUpText[AIM_MEMBER_PRERECORDED_MESSAGE], FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
		}


		InvalidateRegion(AIM_MEMBER_VIDEO_FACE_X,AIM_MEMBER_VIDEO_FACE_Y, AIM_MEMBER_VIDEO_FACE_X+AIM_MEMBER_VIDEO_FACE_WIDTH,AIM_MEMBER_VIDEO_FACE_Y+AIM_MEMBER_VIDEO_FACE_HEIGHT);
	}

	fIsTheMercTalking = face->fTalking;

	//if the merc is talking, reset their attitude time
	if( fIsTheMercTalking )
	{
		//def added 3/18/99
		guiMercAttitudeTime = GetJA2Clock();
	}

	//if the text the merc is saying is really short, extend the time that it is on the screen
	if( ( GetJA2Clock() - guiTimeThatMercStartedTalking ) > usAimMercSpeechDuration )
	{
		//if the merc just stopped talking
		if(fWasTheMercTalking && !fIsTheMercTalking )
		{
			fWasTheMercTalking = FALSE;

			gfRedrawScreen = TRUE;
			guiMercAttitudeTime = GetJA2Clock();

			StopMercTalking();
		}
	}
	else if( fIsTheMercTalking )
	{
		fWasTheMercTalking = fIsTheMercTalking;
	}

	return(fIsTheMercTalking);
}


void DisplayTextForMercFaceVideoPopUp(const wchar_t* const pString)
{
	swprintf(gsTalkingMercText, lengthof(gsTalkingMercText), L"\"%ls\"", pString);

	//Set the minimum time for the dialogue text to be present
	usAimMercSpeechDuration =  wcslen( gsTalkingMercText ) * AIM_TEXT_SPEECH_MODIFIER;

	if( usAimMercSpeechDuration < MINIMUM_TALKING_TIME_FOR_MERC )
		usAimMercSpeechDuration = MINIMUM_TALKING_TIME_FOR_MERC;


	gfRedrawScreen = TRUE;
}


static void SelectShutUpMercRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	BOOLEAN fInCallBack=TRUE;

	if(fInCallBack)
	{
		if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
		{
			gfStopMercFromTalking = TRUE;
		}
		else if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
		{
			fInCallBack = FALSE;

			gfStopMercFromTalking = TRUE;
			fInCallBack = TRUE;
		}
	}
}


static UINT8 WillMercAcceptCall(void)
{
	//if merc has hung up on the player twice within a period of time (MERC_ANNOYED_WONT_CONTACT_TIME_MINUTES )the merc cant ber hired
	if( gMercProfiles[ gbCurrentSoldier ].bMercStatus == MERC_ANNOYED_WONT_CONTACT )
	{
		return(AIM_VIDEO_MERC_UNAVAILABLE_MODE);
	}

	//if the merc is currently on contract, the answering machine will pick up.
	if( (gMercProfiles[ gbCurrentSoldier ].bMercStatus > 0 ) || (gMercProfiles[ gbCurrentSoldier ].bMercStatus == MERC_HAS_NO_TEXT_FILE  ) || (gMercProfiles[ gbCurrentSoldier ].bMercStatus == MERC_HIRED_BUT_NOT_ARRIVED_YET  ) )
	{
		return(AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE);
	}

	// if the merc is at home, or if the merc is only slightly annoyed at the player,  he will greet the player
	if( IsMercHireable( gbCurrentSoldier ) )
	{
		return(AIM_VIDEO_FIRST_CONTACT_MERC_MODE);
	}
	else
		return(AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE);
}


static BOOLEAN CanMercBeHired(void)
{
	UINT8	i,j;
	INT8	bMercID;
	BOOLEAN fRetVal = FALSE;
	BOOLEAN	fBuddyOnTeam=FALSE;



	StopMercTalking();

	// if the merc recently came back with poor morale, and hasn't gotten over it yet
	if (gMercProfiles[ gbCurrentSoldier ].ubDaysOfMoraleHangover > 0)
	{
		// then he refuses with a lame excuse.  Buddy or no buddy.
		WaitForMercToFinishTalkingOrUserToClick();
		InitVideoFaceTalking( gbCurrentSoldier, QUOTE_LAME_REFUSAL );
		return( FALSE );
	}


	// loop through the list of people the merc hates
	for(i=0; i< NUMBER_HATED_MERCS_ONTEAM; i++)
	{
		//see if someone the merc hates is on the team
		bMercID = gMercProfiles[ gbCurrentSoldier ].bHated[i];

		if( bMercID < 0 )
			continue;

		//if the hated merc is dead
		if( IsMercDead( bMercID ) )
		{
			//ignore the merc
			continue;
		}

		if( IsMercOnTeamAndInOmertaAlready( bMercID ) )
		{
			//if the merc hates someone on the team, see if a buddy is on the team
			for(j=0; j< NUMBER_HATED_MERCS_ONTEAM; j++)
			{
				//if a buddy is on the team, the merc will join
				bMercID = gMercProfiles[ gbCurrentSoldier ].bBuddy[j];

				if( bMercID < 0 )
					continue;

				if( IsMercOnTeam( bMercID ) && !IsMercDead( bMercID ) )
				{
					if(j == 0 )
					{
						InitVideoFaceTalking(gbCurrentSoldier, QUOTE_JOINING_CAUSE_BUDDY_1_ON_TEAM);
					}
					else if(j == 1 )
					{
						InitVideoFaceTalking(gbCurrentSoldier, QUOTE_JOINING_CAUSE_BUDDY_2_ON_TEAM);
					}
					else
					{
						InitVideoFaceTalking(gbCurrentSoldier, QUOTE_JOINING_CAUSE_LEARNED_TO_LIKE_BUDDY_ON_TEAM);
					}

					return(TRUE);
				}
			}

			// the merc doesnt like anybody on the team
			//if merc doesnt like first hated merc
			if( i == 0)
			{
				if( gMercProfiles[ gbCurrentSoldier ].bHatedTime[ i ] < 24 )
				{
					WaitForMercToFinishTalkingOrUserToClick();
					InitVideoFaceTalking(gbCurrentSoldier, QUOTE_HATE_MERC_1_ON_TEAM);
					fRetVal = FALSE;
				}
				else
				{
					InitVideoFaceTalking(gbCurrentSoldier, QUOTE_PERSONALITY_BIAS_WITH_MERC_1);
					fRetVal = TRUE;
				}
			}
			else if( i == 1)
			{
				if( gMercProfiles[ gbCurrentSoldier ].bHatedTime[ i ] < 24 )
				{
					WaitForMercToFinishTalkingOrUserToClick();
					InitVideoFaceTalking(gbCurrentSoldier, QUOTE_HATE_MERC_2_ON_TEAM);
					fRetVal = FALSE;
				}
				else
				{
					InitVideoFaceTalking(gbCurrentSoldier, QUOTE_PERSONALITY_BIAS_WITH_MERC_2);
//					DelayMercSpeech( gbCurrentSoldier, QUOTE_PERSONALITY_BIAS_WITH_MERC_2, 750, TRUE, FALSE );
					fRetVal = TRUE;
				}
			}
			else
			{
				WaitForMercToFinishTalkingOrUserToClick();
				InitVideoFaceTalking(gbCurrentSoldier, QUOTE_LEARNED_TO_HATE_MERC_ON_TEAM);
				fRetVal = FALSE;
			}

			return( fRetVal );
		}
	}

	//Is a buddy working on the team
	fBuddyOnTeam = DoesMercHaveABuddyOnTheTeam( gbCurrentSoldier );

	//If the merc doesnt have a buddy on the team
	if( !fBuddyOnTeam )
	{
		// Check the players Death rate
		if( MercThinksDeathRateTooHigh( gbCurrentSoldier ) )
		{
			WaitForMercToFinishTalkingOrUserToClick();
			InitVideoFaceTalking( gbCurrentSoldier, QUOTE_DEATH_RATE_REFUSAL );
			return( FALSE );
		}

		// Check the players Reputation
		if( MercThinksBadReputationTooHigh( gbCurrentSoldier ) )
		{
			WaitForMercToFinishTalkingOrUserToClick();
			InitVideoFaceTalking( gbCurrentSoldier, QUOTE_REPUTATION_REFUSAL );
			return( FALSE );
		}
	}

	return(TRUE);
}


static BOOLEAN DisplaySnowBackground(void)
{
	UINT32		uiCurrentTime = 0;
	UINT8	ubCount;

	uiCurrentTime = GetJA2Clock();

	if(gubCurrentCount < VC_NUM_LINES_SNOW)
	{
		ubCount = gubCurrentCount;
	}
	else if( gubCurrentCount < VC_NUM_LINES_SNOW*2 )
	{
		ubCount = gubCurrentCount - VC_NUM_LINES_SNOW;
	}
	else
	{
		gfFirstTimeInContactScreen = FALSE;
		gubCurrentCount = 0;
		ubCount = 0;

		if( gubVideoConferencingMode == AIM_VIDEO_FIRST_CONTACT_MERC_MODE && gfAimMemberCanMercSayOpeningQuote )
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_GREETING);

		return(TRUE);
	}

	// if it is time to update the snow image
	if( (uiCurrentTime - guiLastHandleMercTime) > VC_CONTACT_STATIC_TIME)
	{
		gubCurrentCount++;
		guiLastHandleMercTime = uiCurrentTime;
	}
	BltVideoObject(FRAME_BUFFER, guiBWSnow, ubCount,AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y);

  InvalidateRegion(AIM_MEMBER_VIDEO_FACE_X,AIM_MEMBER_VIDEO_FACE_Y, AIM_MEMBER_VIDEO_FACE_X+AIM_MEMBER_VIDEO_FACE_WIDTH,AIM_MEMBER_VIDEO_FACE_Y+AIM_MEMBER_VIDEO_FACE_HEIGHT);

	return(FALSE);
}


static BOOLEAN DisplayBlackBackground(UINT8 ubMaxNumOfLoops)
{
	UINT32		uiCurrentTime = 0;

	uiCurrentTime = GetJA2Clock();

	if (gubCurrentCount >= ubMaxNumOfLoops)
	{
		gubCurrentCount = 0;
		return(TRUE);
	}

	// if it is time to update the snow image
	if( (uiCurrentTime - guiLastHandleMercTime) > VC_CONTACT_STATIC_TIME)
	{
		gubCurrentCount++;
		guiLastHandleMercTime = uiCurrentTime;
	}
	// Blit color to screen
	ColorFillVideoSurfaceArea( FRAME_BUFFER, AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y, AIM_MEMBER_VIDEO_FACE_X+AIM_MEMBER_VIDEO_FACE_WIDTH,	AIM_MEMBER_VIDEO_FACE_Y+AIM_MEMBER_VIDEO_FACE_HEIGHT, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );
  InvalidateRegion(AIM_MEMBER_VIDEO_FACE_X,AIM_MEMBER_VIDEO_FACE_Y, AIM_MEMBER_VIDEO_FACE_X+AIM_MEMBER_VIDEO_FACE_WIDTH,AIM_MEMBER_VIDEO_FACE_Y+AIM_MEMBER_VIDEO_FACE_HEIGHT);

	return(FALSE);
}


static UINT8 DisplayDistortionLine(UINT8 ubMode, const SGPVObject* image, UINT8 ubMaxImages);
static UINT8 DisplayPixelatedImage(UINT8 ubMaxImages);
static UINT8 DisplayTransparentSnow(UINT8 ubMode, const SGPVObject* image, UINT8 ubMaxImages, BOOLEAN bForward);


static void HandleVideoDistortion(void)
{
	static UINT32	uiStaticNoiseSound = NO_SAMPLE;
	UINT8		ubOldMode = gubCurrentStaticMode;

	// if we are just entering the contact page, display a snowy background
	if( gfFirstTimeInContactScreen && !gfIsAnsweringMachineActive)
	{
		DisplaySnowBackground();

		//if it is time to start playing another sound
		if( uiStaticNoiseSound == NO_SAMPLE )
		{
			uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static4.wav", LOWVOLUME, 1, MIDDLEPAN);
		}
	}
	else
	{
		switch( gubCurrentStaticMode )
		{
			case VC_NO_STATIC:
				{
					static UINT32 uiCurTime=0;
					UINT8	ubNum;

					//if the sound is playing, stop it
					if( uiStaticNoiseSound != NO_SAMPLE )
					{
						SoundStop( uiStaticNoiseSound );
						uiStaticNoiseSound = NO_SAMPLE;
					}

					//DECIDE WHICH ONE TO BLIT NEXT
					if( (GetJA2Clock() - uiCurTime) > 2500)
					{
						ubNum = (UINT8)Random( 200 );//125;

						if( ubNum < 15)
							gubCurrentStaticMode = VC_FUZZY_LINE;

						else if( ubNum < 25)
							gubCurrentStaticMode = VC_STRAIGHTLINE;

						else if( ubNum < 35)
							gubCurrentStaticMode = VC_BW_SNOW;

						else if( ubNum < 40)
							gubCurrentStaticMode = VC_PIXELATE;

						else if( ubNum < 80)
							gubCurrentStaticMode = VC_TRANS_SNOW_OUT;

						else if( ubNum < 100)
							gubCurrentStaticMode = VC_TRANS_SNOW_IN;

						uiCurTime = GetJA2Clock();
					}
				}
				break;

			case VC_FUZZY_LINE:
				gubCurrentStaticMode = DisplayDistortionLine(VC_FUZZY_LINE, guiFuzzLine, VC_NUM_FUZZ_LINES);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static1.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;

			case VC_STRAIGHTLINE:
				gubCurrentStaticMode = DisplayDistortionLine(VC_STRAIGHTLINE, guiStraightLine, VC_NUM_STRAIGHT_LINES);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static5.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;

			case VC_BW_SNOW:
				gubCurrentStaticMode = DisplayDistortionLine(VC_BW_SNOW, guiBWSnow, 5);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static6.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;

			case VC_PIXELATE:
				gubCurrentStaticMode = DisplayPixelatedImage(4);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static3.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;

			case VC_TRANS_SNOW_OUT:
				gubCurrentStaticMode = DisplayTransparentSnow(VC_TRANS_SNOW_OUT, guiTransSnow, 7, FALSE);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static5.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;

			case VC_TRANS_SNOW_IN:
				gubCurrentStaticMode = DisplayTransparentSnow(VC_TRANS_SNOW_IN, guiTransSnow, 7, TRUE);

				//if it is time to start playing another sound
				if( uiStaticNoiseSound == NO_SAMPLE )
				{
					uiStaticNoiseSound = PlayJA2SampleFromFile("LAPTOP/static4.wav", LOWVOLUME, 1, MIDDLEPAN);
				}
				break;
		}

		if( ubOldMode != gubCurrentStaticMode )
		{
			uiStaticNoiseSound = NO_SAMPLE;
		}
	}
}


//returns true when done. else false
static UINT8 DisplayTransparentSnow(const UINT8 ubMode, const SGPVObject* const image, const UINT8 ubMaxImages, const BOOLEAN bForward)
{
	static INT8	bCount= 0;
	UINT32		uiCurrentTime = 0;
	static UINT32	uiLastTime=0;

	uiCurrentTime = GetJA2Clock();

	if( (uiCurrentTime - uiLastTime) > 100)
	{
		if( bForward )
		{
			if( bCount > ubMaxImages-1 )
				bCount = 0;
			else
				bCount++;
		}
		else
		{
			if( bCount <= 0 )
				bCount = ubMaxImages-1;
			else
				bCount--;
		}
		uiLastTime = uiCurrentTime;
	}

	if( bCount >= ubMaxImages)
		bCount = ubMaxImages - 1;

	BltVideoObject(FRAME_BUFFER, image, bCount, AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y);

	if( bForward )
	{
		if( bCount == ubMaxImages-1)
		{
			bCount = 0;
			return(VC_BW_SNOW);
		}
		else
			return(ubMode);
	}
	else
	{
		if( bCount == 0)
		{
			bCount = 0;
			return(VC_NO_STATIC);
		}
		else
			return(ubMode);
	}
}


//returns true when done. else false
static UINT8 DisplayDistortionLine(const UINT8 ubMode, const SGPVObject* const image, const UINT8 ubMaxImages)
{
	static UINT8	ubCount=255;
	UINT32		uiCurrentTime = 0;
	static UINT32	uiLastTime=0;

	uiCurrentTime = GetJA2Clock();

	if( (uiCurrentTime - uiLastTime) > VC_CONTACT_FUZZY_LINE_TIME)
	{
		if( ubCount >= ubMaxImages-1 )
			ubCount = 0;
		else
			ubCount++;
		uiLastTime = uiCurrentTime;
	}

	if( ubCount >= ubMaxImages)
		ubCount = ubMaxImages - 1;

	BltVideoObject(FRAME_BUFFER, image, ubCount,AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y);

	if( ubCount == ubMaxImages-1)
	{
		ubCount = 0;
		if( ubMode == VC_BW_SNOW)
			return(VC_TRANS_SNOW_OUT);
		else
			return(VC_NO_STATIC);
	}
	else
		return(ubMode);
}


static UINT8 DisplayPixelatedImage(UINT8 ubMaxImages)
{
	static UINT8	ubCount=255;
	UINT32		uiCurrentTime = 0;
	static UINT32	uiLastTime=0;

	uiCurrentTime = GetJA2Clock();

	if( (uiCurrentTime - uiLastTime) > VC_CONTACT_FUZZY_LINE_TIME)
	{
		if( ubCount >= ubMaxImages-1 )
			ubCount = 0;
		else
			ubCount++;
		uiLastTime = uiCurrentTime;
	}

	ShadowVideoSurfaceRect( FRAME_BUFFER, AIM_MEMBER_VIDEO_FACE_X, AIM_MEMBER_VIDEO_FACE_Y, AIM_MEMBER_VIDEO_FACE_X+AIM_MEMBER_VIDEO_FACE_WIDTH-1, AIM_MEMBER_VIDEO_FACE_Y+AIM_MEMBER_VIDEO_FACE_HEIGHT-1);

	if( ubCount == ubMaxImages-1)
	{
		ubCount = 0;
		return(VC_NO_STATIC);
	}
	else
		return(VC_PIXELATE);
}


static void HandleMercAttitude(void)
{
	UINT32		uiCurrentTime = 0;

	uiCurrentTime = GetJA2Clock();

	if( ( gubMercAttitudeLevel <= 1 && ( ( uiCurrentTime -  guiMercAttitudeTime ) > QUOTE_FIRST_ATTITUDE_TIME ) ) ||
			( ( uiCurrentTime -  guiMercAttitudeTime ) > QUOTE_ATTITUDE_TIME ) )
	{

		if( gubMercAttitudeLevel == QUOTE_DELAY_SMALL_TALK)
		{
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_SMALL_TALK);
		}
		else if( gubMercAttitudeLevel == QUOTE_DELAY_IMPATIENT_TALK)
		{
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_IMPATIENT_QUOTE);
		}
		else if( gubMercAttitudeLevel == QUOTE_DELAY_VERY_IMPATIENT_TALK)
		{
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_PRECEDENT_TO_REPEATING_ONESELF);
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_IMPATIENT_QUOTE);
		}
		else if( gubMercAttitudeLevel == QUOTE_DELAY_HANGUP_TALK)
		{
			UINT32	uiResetTime;
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_COMMENT_BEFORE_HANG_UP);

			//if the merc is going to hang up disable the buttons, so user cant press any buttons
//			EnableDisableCurrentVideoConferenceButtons( FALSE);
			if( gubVideoConferencingPreviousMode == AIM_VIDEO_HIRE_MERC_MODE )
			{
				// Enable the current video conference buttons
				EnableDisableCurrentVideoConferenceButtons(FALSE);
			}


			//increments the merc 'annoyance' at the player
			if( gMercProfiles[ gbCurrentSoldier ].bMercStatus == 0 )
				gMercProfiles[ gbCurrentSoldier ].bMercStatus = MERC_ANNOYED_BUT_CAN_STILL_CONTACT;
			else if( gMercProfiles[ gbCurrentSoldier ].bMercStatus == MERC_ANNOYED_BUT_CAN_STILL_CONTACT )
				gMercProfiles[ gbCurrentSoldier ].bMercStatus = MERC_ANNOYED_WONT_CONTACT;

			// add an event so we can reset the 'annoyance factor'
			uiResetTime =  ( Random( 600 ) );
			uiResetTime += GetWorldTotalMin() + MERC_ANNOYED_WONT_CONTACT_TIME_MINUTES;
			AddStrategicEvent( EVENT_AIM_RESET_MERC_ANNOYANCE, uiResetTime, gbCurrentSoldier );

			gfHangUpMerc = TRUE;
		}

		if( gubMercAttitudeLevel == QUOTE_MERC_BUSY )
		{
			InitVideoFaceTalking(gbCurrentSoldier, QUOTE_LAME_REFUSAL);
			gfHangUpMerc = TRUE;
		}
		else if( gubMercAttitudeLevel != QUOTE_DELAY_NO_ACTION )
			gubMercAttitudeLevel++;

		guiMercAttitudeTime = GetJA2Clock();
	}
}


static void StopMercTalking(void)
{
	if( gfIsShutUpMouseRegionActive )
	{
	  MSYS_DisableRegion(&gSelectedShutUpMercRegion);

		ShutupaYoFace( giMercFaceIndex );
		gfMercIsTalking = FALSE;
		guiMercAttitudeTime = GetJA2Clock();
		gfIsShutUpMouseRegionActive = FALSE;
		gfRedrawScreen = TRUE;
	}
}


static void BtnXToCloseVideoConfButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gubVideoConferencingMode = AIM_VIDEO_POPDOWN_MODE;
//		gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;
	}
}


static INT32 MakeButtonVideo(BUTTON_PICS* const img, const wchar_t* const text, const INT16 x, const INT16 y, const GUI_CALLBACK click)
{
	const INT16 txt_col    = AIM_M_VIDEO_NAME_COLOR;
	const INT16 shadow_col = AIM_M_VIDEO_NAME_SHADOWCOLOR;
	const INT32 btn = CreateIconAndTextButton(img, text, FONT12ARIAL, txt_col, shadow_col, txt_col, shadow_col, x, y, MSYS_PRIORITY_HIGH, click);
	SetButtonCursor(btn, CURSOR_LAPTOP_SCREEN);
	return btn;
}


static BOOLEAN DeleteVideoConfPopUp(void);


static BOOLEAN InitDeleteVideoConferencePopUp(void)
{
	static BOOLEAN	fXRegionActive = FALSE;
	UINT8	i;
	UINT16	usPosX, usPosY;

	//remove the face help text
	gfAimMemberDisplayFaceHelpText = FALSE;

	//Gets reset to FALSE in the HandleCurrentVideoConfMode() function
	gfJustSwitchedVideoConferenceMode = TRUE;


	//remove old mode
	DeleteVideoConfPopUp();

	//reset ( in case merc was going to say something
	DelayMercSpeech( 0, 0, 0, FALSE, TRUE );

	//if the video conferencing is currently displayed, put the 'x' to close it in the top right corner
	//and disable the ability to click on the BIG face to go to different screen
	if( ( gubVideoConferencingMode != AIM_VIDEO_NOT_DISPLAYED_MODE) && ( gubVideoConferencingMode != AIM_VIDEO_POPUP_MODE) )
	{
		if( !fXRegionActive )
		{
			giXToCloseVideoConfButton = QuickCreateButton(giXToCloseVideoConfButtonImage, AIM_MEMBER_VIDEO_CONF_XCLOSE_X, AIM_MEMBER_VIDEO_CONF_XCLOSE_Y, MSYS_PRIORITY_HIGH, BtnXToCloseVideoConfButtonCallback);
			SetButtonCursor(giXToCloseVideoConfButton, CURSOR_LAPTOP_SCREEN);
			SpecifyDisabledButtonStyle( giXToCloseVideoConfButton, DISABLED_STYLE_NONE );
			fXRegionActive = TRUE;

			MSYS_DisableRegion(&gSelectedFaceRegion);
		}
	}



	// The video conference is not displayed
	if( gubVideoConferencingMode ==  AIM_VIDEO_NOT_DISPLAYED_MODE )
	{
		gubVideoConferencingPreviousMode = gubVideoConferencingMode;
		gfRedrawScreen = TRUE;

		if( gfVideoFaceActive )
		{
			StopMercTalking();

			//Get rid of the talking face
			DeleteFace(giMercFaceIndex);
		}

		//if the ansering machine is currently on, turn it off
		if( gfIsAnsweringMachineActive)
			gfIsAnsweringMachineActive = FALSE;

		gfVideoFaceActive = FALSE;

		if( fXRegionActive )
		{

			RemoveButton(giXToCloseVideoConfButton );
			fXRegionActive = FALSE;
		}

		MSYS_DisableRegion(&gSelectedShutUpMercRegion);

		//Enable the ability to click on the BIG face to go to different screen
	  MSYS_EnableRegion(&gSelectedFaceRegion);

//		EnableDisableCurrentVideoConferenceButtons(FALSE);
			if( gubVideoConferencingPreviousMode == AIM_VIDEO_HIRE_MERC_MODE )
			{
				// Enable the current video conference buttons
				EnableDisableCurrentVideoConferenceButtons(FALSE);
			}

		fNewMailFlag = gfIsNewMailFlagSet;
		gfIsNewMailFlagSet = FALSE;

	}


	if( gubVideoConferencingMode == AIM_VIDEO_POPUP_MODE )
	{

		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		if( gfJustSwitchedVideoConferenceMode )
		{
			// load the answering machine graphic and add it

			// Create a background video surface to blt the face onto
			guiVideoTitleBar = AddVideoSurface(AIM_MEMBER_VIDEO_TITLE_BAR_WIDTH, AIM_MEMBER_VIDEO_TITLE_BAR_HEIGHT, PIXEL_DEPTH);
			CHECKF(guiVideoTitleBar != NO_VSURFACE);

			CHECKF(BltVideoObjectOnce(guiVideoTitleBar, "LAPTOP/VideoTitleBar.sti", 0, 0, 0));

			gfAimMemberCanMercSayOpeningQuote = TRUE;
		}
	}


	// The opening animation of the vc (fuzzy screen, then goes to black)
	if( gubVideoConferencingMode == AIM_VIDEO_INIT_MODE )
	{
		gubVideoConferencingPreviousMode = gubVideoConferencingMode;
 		gubMercAttitudeLevel = 0;
		gubContractLength = AIM_CONTRACT_LENGTH_ONE_WEEK;

		if( gMercProfiles[gbCurrentSoldier].usOptionalGearCost == 0 )
			gfBuyEquipment = FALSE;
		else
			gfBuyEquipment = TRUE;

		gfMercIsTalking = FALSE;
		gfVideoFaceActive = FALSE;
		guiLastHandleMercTime = 0;
		gfHangUpMerc = FALSE;
	}

	// The screen in which you first contact the merc, you have the option to hang up or goto hire merc screen
	if( gubVideoConferencingMode ==  AIM_VIDEO_FIRST_CONTACT_MERC_MODE )
	{
		//if the last screen was the init screen, then we need to initialize the video face
		if( ( gubVideoConferencingPreviousMode == AIM_VIDEO_INIT_MODE) || ( gubVideoConferencingPreviousMode == AIM_VIDEO_NOT_DISPLAYED_MODE) )
		{
			//Put the merc face up on the screen
			InitVideoFace(gbCurrentSoldier);

//			if( gubVideoConferencingPreviousMode == AIM_VIDEO_INIT_MODE)
//				InitVideoFaceTalking(gbCurrentSoldier, QUOTE_GREETING);
		}

		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		// Hang up button
		usPosX = AIM_MEMBER_AUTHORIZE_PAY_X;
		guiVideoConferenceButtonImage[2] = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,2,-1,3,-1 );
		for(i=0; i<2; i++)
		{
			giAuthorizeButton[i] = MakeButtonVideo(guiVideoConferenceButtonImage[2], VideoConfercingText[AIM_MEMBER_HIRE + i], usPosX, AIM_MEMBER_HANG_UP_Y, BtnFirstContactButtonCallback);
			MSYS_SetBtnUserData(giAuthorizeButton[i], i);
			usPosX += AIM_MEMBER_AUTHORIZE_PAY_GAP;
		}

		if( gfWaitingForMercToStopTalkingOrUserToClick )
		{
			DisableButton( giAuthorizeButton[0] );
			gfWaitingForMercToStopTalkingOrUserToClick = FALSE;

			//Display a popup msg box telling the user when and where the merc will arrive
//			DisplayPopUpBoxExplainingMercArrivalLocationAndTime();
		}
	}



		// The screen in which you set the contract length, and the ability to buy equipment..
	if( gubVideoConferencingMode == AIM_VIDEO_HIRE_MERC_MODE)
	{
		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		// Contract Length button
		guiVideoConferenceButtonImage[0] = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,0,-1,1,-1 );
		usPosY = AIM_MEMBER_BUY_CONTRACT_LENGTH_Y;
		for(i=0; i<3; i++)
		{
			giContractLengthButton[i] = MakeButtonVideo(guiVideoConferenceButtonImage[0], VideoConfercingText[AIM_MEMBER_ONE_DAY + i], AIM_MEMBER_BUY_CONTRACT_LENGTH_X, usPosY, BtnContractLengthButtonCallback);
			SpecifyButtonTextJustification(giContractLengthButton[i], BUTTON_TEXT_LEFT);
			MSYS_SetBtnUserData(giContractLengthButton[i], i);
			SpecifyDisabledButtonStyle( giContractLengthButton[i], DISABLED_STYLE_NONE );
			usPosY += AIM_MEMBER_BUY_EQUIPMENT_GAP;
		}

		// BuyEquipment button
		usPosY = AIM_MEMBER_BUY_CONTRACT_LENGTH_Y;
		for(i=0; i<2; i++)
		{
			giBuyEquipmentButton[i] = MakeButtonVideo(guiVideoConferenceButtonImage[0], VideoConfercingText[AIM_MEMBER_NO_EQUIPMENT + i], AIM_MEMBER_BUY_EQUIPMENT_X, usPosY, BtnBuyEquipmentButtonCallback);
			SpecifyButtonTextJustification(giBuyEquipmentButton[i], BUTTON_TEXT_LEFT);
			MSYS_SetBtnUserData(giBuyEquipmentButton[i], i);
			SpecifyDisabledButtonStyle( giBuyEquipmentButton[i], DISABLED_STYLE_SHADED );
			usPosY += AIM_MEMBER_BUY_EQUIPMENT_GAP;
		}
		if( gMercProfiles[gbCurrentSoldier].usOptionalGearCost == 0 )
			DisableButton( giBuyEquipmentButton[1] );


		// Authorize button
		usPosX = AIM_MEMBER_AUTHORIZE_PAY_X;
		guiVideoConferenceButtonImage[1] = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,2,-1,3,-1 );
		for(i=0; i<2; i++)
		{
			giAuthorizeButton[i] = MakeButtonVideo(guiVideoConferenceButtonImage[1], VideoConfercingText[AIM_MEMBER_TRANSFER_FUNDS + i], usPosX, AIM_MEMBER_AUTHORIZE_PAY_Y, BtnAuthorizeButtonCallback);
			MSYS_SetBtnUserData(giAuthorizeButton[i], i);
			SpecifyDisabledButtonStyle( giAuthorizeButton[i], DISABLED_STYLE_NONE );
			usPosX += AIM_MEMBER_AUTHORIZE_PAY_GAP;
		}

//		InitVideoFaceTalking(gbCurrentSoldier, QUOTE_LENGTH_OF_CONTRACT);
		DelayMercSpeech( gbCurrentSoldier, QUOTE_LENGTH_OF_CONTRACT, 750, TRUE, FALSE );
	}




		// The merc is not home and the player gets the answering machine
	if( gubVideoConferencingMode ==  AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE )
	{
		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		gfIsAnsweringMachineActive = TRUE;

		// Leave msg button
		usPosX = AIM_MEMBER_AUTHORIZE_PAY_X;
		guiVideoConferenceButtonImage[2] = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,2,-1,3,-1 );

		giAnsweringMachineButton[0] = MakeButtonVideo(guiVideoConferenceButtonImage[2], VideoConfercingText[AIM_MEMBER_LEAVE_MESSAGE], usPosX, AIM_MEMBER_HANG_UP_Y, BtnAnsweringMachineButtonCallback);
		MSYS_SetBtnUserData(giAnsweringMachineButton[0], 0);

		//if the user has already left a message, disable the button
		if( gMercProfiles[ gbCurrentSoldier ].ubMiscFlags3 & PROFILE_MISC_FLAG3_PLAYER_LEFT_MSG_FOR_MERC_AT_AIM )
			DisableButton( giAnsweringMachineButton[0] );

		usPosX += AIM_MEMBER_AUTHORIZE_PAY_GAP;

		giAnsweringMachineButton[1] = MakeButtonVideo(guiVideoConferenceButtonImage[2], VideoConfercingText[AIM_MEMBER_HANG_UP], usPosX, AIM_MEMBER_HANG_UP_Y, BtnAnsweringMachineButtonCallback);
		MSYS_SetBtnUserData(giAnsweringMachineButton[1], 1);

		//The face must be inited even though the face wont appear.  It is so the voice is played
		InitVideoFace(gbCurrentSoldier);

		//Make sure the merc doesnt ramble away to the player
		gubMercAttitudeLevel = QUOTE_DELAY_NO_ACTION;


//
//DEF: TEST
//
/*		// load the answering machine graphic and add it
		guiAnsweringMachineImage = AddVideoObjectFromFile("LAPTOP/explosion.sti");
		CHECKF(guiAnsweringMachineImage != NO_VOBJECT);
*/
		gubCurrentStaticMode = VC_NO_STATIC;
	}




	// The merc is home but for some reason doesnt want to work for player
	if( gubVideoConferencingMode == AIM_VIDEO_MERC_UNAVAILABLE_MODE)
	{
		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		// The hangup button
		guiVideoConferenceButtonImage[2] = LoadButtonImage("LAPTOP/VideoConfButtons.sti", -1,2,-1,3,-1 );
		giHangUpButton = MakeButtonVideo(guiVideoConferenceButtonImage[2], VideoConfercingText[AIM_MEMBER_HANG_UP], AIM_MEMBER_HANG_UP_X, AIM_MEMBER_HANG_UP_Y, BtnHangUpButtonCallback);
		MSYS_SetBtnUserData(giHangUpButton, 1);

		//set the flag saying specifying that merc is busy
		gubMercAttitudeLevel = QUOTE_MERC_BUSY;

		InitVideoFace(gbCurrentSoldier);
	}

	if( gubVideoConferencingMode == 	AIM_VIDEO_POPDOWN_MODE )
	{
		if( gubPopUpBoxAction == AIM_POPUP_DISPLAY )
		{
			return( TRUE );
		}

		gubVideoConferencingPreviousMode = gubVideoConferencingMode;

		gfIsAnsweringMachineActive = FALSE;

		// load the Video conference background graphic and add it

		// Create a background video surface to blt the face onto
		guiVideoTitleBar = AddVideoSurface(AIM_MEMBER_VIDEO_TITLE_BAR_WIDTH, AIM_MEMBER_VIDEO_TITLE_BAR_HEIGHT, PIXEL_DEPTH);
		CHECKF(guiVideoTitleBar != NO_VSURFACE);

		CHECKF(BltVideoObjectOnce(guiVideoTitleBar, "LAPTOP/VideoTitleBar.sti", 0, 0, 0));
	}

//	gfWaitingForMercToStopTalkingOrUserToClick = FALSE;

	//reset the time in which the merc will get annoyed
	guiMercAttitudeTime = GetJA2Clock();
	return(TRUE);
}


static BOOLEAN DeleteVideoConfPopUp(void)
{
	UINT16 i;

	//reset ( in case merc was going to say something
	DelayMercSpeech( 0, 0, 0, FALSE, TRUE );

	switch(	gubVideoConferencingPreviousMode )
	{
		// The video conference is not displayed
		case AIM_VIDEO_NOT_DISPLAYED_MODE:
		{

			break;
		}

		case AIM_VIDEO_POPUP_MODE:
			DeleteVideoSurface(guiVideoTitleBar);
			break;

		// The opening animation of the vc (fuzzy screen, then goes to black)
		case AIM_VIDEO_INIT_MODE:
		{

			break;
		}



		// The screen in which you first contact the merc, you have the option to hang up or goto hire merc screen
		case AIM_VIDEO_FIRST_CONTACT_MERC_MODE:
		{
			//Remove the video conf buttons images
			UnloadButtonImage(guiVideoConferenceButtonImage[2]);

			//Remove the Hangup  buttons
			for(i=0; i<2; i++)
				RemoveButton(giAuthorizeButton[i] );

			break;
		}



		// The screen in which you set the contract length, and the ability to buy equipment..
		case AIM_VIDEO_HIRE_MERC_MODE:
		{
			//Remove the video conf buttons images
			for(i=0; i<2; i++)
				UnloadButtonImage(guiVideoConferenceButtonImage[i]);

			//Remove the Contracy Length button
			for(i=0; i<3; i++)
				RemoveButton(giContractLengthButton[i] );

			for(i=0; i<2; i++)
				RemoveButton(giBuyEquipmentButton[i] );

			for(i=0; i<2; i++)
				RemoveButton(giAuthorizeButton[i] );

			break;
		}




		// The merc is not home and the player gets the answering machine
		case AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE:
		{
			if( gubPopUpBoxAction == AIM_POPUP_DISPLAY )
			{
//				return( TRUE );
			}

			//Remove the video conf buttons images
			UnloadButtonImage(guiVideoConferenceButtonImage[2]);

			//Remove the Answering machine buttons
			for(i=0; i<2; i++)
				RemoveButton(giAnsweringMachineButton[i] );

//			DeleteVideoObject(guiAnsweringMachineImage);
			break;
		}




		// The merc is home but doesnt want to work for player
		case AIM_VIDEO_MERC_UNAVAILABLE_MODE:
		{
			RemoveButton(giHangUpButton );
			UnloadButtonImage(guiVideoConferenceButtonImage[2]);
			break;
		}

		case AIM_VIDEO_POPDOWN_MODE:
		{
			if( gubPopUpBoxAction == AIM_POPUP_DISPLAY )
			{
				return( TRUE );
			}

			if( gfWaitingForMercToStopTalkingOrUserToClick )
			{
				gfWaitingForMercToStopTalkingOrUserToClick = FALSE;

//				DisplayPopUpBoxExplainingMercArrivalLocationAndTime();
			}

			gfWaitingForMercToStopTalkingOrUserToClick = FALSE;
			DeleteVideoSurface(guiVideoTitleBar);
			break;
		}
	}
	return(TRUE);
}


static BOOLEAN DisplayMovingTitleBar(BOOLEAN fForward);


static BOOLEAN HandleCurrentVideoConfMode(void)
{
	switch(	gubVideoConferencingMode )
	{
		// The video conference is not displayed
		case AIM_VIDEO_NOT_DISPLAYED_MODE:
		{
			gfWaitingForMercToStopTalkingOrUserToClick = FALSE;

			break;
		}


		case AIM_VIDEO_POPUP_MODE:
			if (DisplayMovingTitleBar(TRUE))
			{
				gubVideoConferencingMode = AIM_VIDEO_INIT_MODE;
			}
			break;

		// The opening animation of the vc (fuzzy screen, then goes to black)
		case AIM_VIDEO_INIT_MODE:
		{
			static UINT8 ubCurMode = 0;
			BOOLEAN	fDone;

			if( ubCurMode == 0 )
			{
				fDone = DisplayBlackBackground( 10 );
				if( fDone )
					ubCurMode = 1;
			}
			else
				fDone = DisplaySnowBackground();

			if( fDone && ubCurMode )
			{
				ubCurMode = 0;

				gubVideoConferencingMode = WillMercAcceptCall();
			}

			break;
		}

		// The screen in which you first contact the merc, you have the option to hang up or goto hire merc screen
		case AIM_VIDEO_FIRST_CONTACT_MERC_MODE:
		{
			//if the merc is at home, play his greeting
//			if( gfJustSwitchedVideoConferenceMode )
//				InitVideoFaceTalking(gbCurrentSoldier, QUOTE_GREETING);

			break;
		}

		// The screen in which you set the contract length, and the ability to buy equipment..
		case AIM_VIDEO_HIRE_MERC_MODE:
		{

			break;
		}

		// The merc is not home and the player gets the answering machine
		case AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE:
		{
			//if the merc is not at home, play his answering machine
			if( gfJustSwitchedVideoConferenceMode )
			{
				InitVideoFaceTalking(gbCurrentSoldier, QUOTE_ANSWERING_MACHINE_MSG);
			}

			break;
		}

		// The merc is home but doesnt want to work for player
		case AIM_VIDEO_MERC_UNAVAILABLE_MODE:
		{

			break;
		}

		case AIM_VIDEO_POPDOWN_MODE:
			if (DisplayMovingTitleBar(FALSE))
			{
				gubVideoConferencingMode = AIM_VIDEO_NOT_DISPLAYED_MODE;

				//display the popup telling the user when the just hired merc is going to land
				DisplayPopUpBoxExplainingMercArrivalLocationAndTime();

				//render the screen immediately to get rid of the pop down stuff
				InitDeleteVideoConferencePopUp( );
				RenderAIMMembers();
				gfVideoFaceActive = FALSE;
			}
			break;
	}

	//Gets set in the InitDeleteVideoConferencePopUp() function
//	gfJustSwitchedVideoConferenceMode = FALSE;

	return(TRUE);
}


static BOOLEAN EnableDisableCurrentVideoConferenceButtons(BOOLEAN fEnable)
{
	INT8	i;
	static BOOLEAN fCreated = FALSE;
	if(!fEnable)
	{
		if( fCreated )
		{
			//enable buttons behind the acknowlegde button

			for( i=0; i<3; i++)
				EnableButton( giContractLengthButton[i] );

			for(i=0; i<2; i++)
				EnableButton( giBuyEquipmentButton[i] );

			for(i=0; i<2; i++)
				EnableButton( giAuthorizeButton[i] );

			fCreated = FALSE;
		}
	}
	else
	{
		if( !fCreated )
		{
			//disable buttons behind the acknowlegde button
			for( i=0; i<3; i++)
				DisableButton( giContractLengthButton[i] );

			for(i=0; i<2; i++)
				DisableButton( giBuyEquipmentButton[i] );

			for(i=0; i<2; i++)
				DisableButton( giAuthorizeButton[i] );


			fCreated = TRUE;
		}
	}
	return(TRUE);
}


void ResetMercAnnoyanceAtPlayer( UINT8 ubMercID )
{
	//if merc is still annoyed, reset back to 0

	if ( ubMercID == LARRY_NORMAL  )
	{
		if ( CheckFact( FACT_LARRY_CHANGED, 0 ) )
		{
			ubMercID = LARRY_DRUNK;
		}
	}
	else if ( ubMercID == LARRY_DRUNK )
	{
		if ( CheckFact( FACT_LARRY_CHANGED, 0 ) == FALSE )
		{
			ubMercID = LARRY_NORMAL;
		}
	}
	if( ( gMercProfiles[ ubMercID ].bMercStatus == MERC_ANNOYED_WONT_CONTACT ) || ( gMercProfiles[ ubMercID ].bMercStatus == MERC_ANNOYED_BUT_CAN_STILL_CONTACT ) )
		gMercProfiles[ ubMercID ].bMercStatus = 0;
}


void DisableNewMailMessage(void)
{
	if( fNewMailFlag && gubVideoConferencingMode )
	{
		gfIsNewMailFlagSet = TRUE;
		fNewMailFlag = FALSE;
		gfRedrawScreen = TRUE;
	}
}


static INT32 Interpolate(INT32 start, INT32 end, INT32 step)
{
	return start + (end - start) * step / AIM_MEMBER_VIDEO_TITLE_ITERATIONS;
}


static BOOLEAN DisplayMovingTitleBar(BOOLEAN fForward)
{
	static UINT8   ubCount;
	static SGPRect LastRect;

	if (gfJustSwitchedVideoConferenceMode)
	{
		ubCount = (fForward ? 1 : AIM_MEMBER_VIDEO_TITLE_ITERATIONS - 1);
	}

	const UINT16 usPosX      = Interpolate(331, 125, ubCount);
	const UINT16 usPosRightX = Interpolate(405, 490, ubCount);

	const UINT16 usPosY       = Interpolate(AIM_MEMBER_VIDEO_TITLE_START_Y, 96, ubCount);
	const UINT16 usPosBottomY = usPosY + AIM_MEMBER_VIDEO_TITLE_BAR_HEIGHT;

	SGPRect SrcRect;
	SrcRect.iLeft   = 0;
	SrcRect.iTop    = 0;
	SrcRect.iRight  = AIM_MEMBER_VIDEO_TITLE_BAR_WIDTH;
	SrcRect.iBottom = AIM_MEMBER_VIDEO_TITLE_BAR_HEIGHT;

	SGPRect DestRect;
	DestRect.iLeft   = usPosX;
	DestRect.iTop    = usPosY;
	DestRect.iRight  = usPosRightX;
	DestRect.iBottom = usPosBottomY;

	if (fForward)
	{
		//Restore the old rect
		if (ubCount > 2)
		{
			const UINT16 usWidth  = LastRect.iRight  - LastRect.iLeft;
			const UINT16 usHeight = LastRect.iBottom - LastRect.iTop;
			BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, LastRect.iLeft, LastRect.iTop, usWidth, usHeight);
		}

		//Save rectangle
		if (ubCount > 1)
		{
			const UINT16 usWidth  = DestRect.iRight  - DestRect.iLeft;
			const UINT16 usHeight = DestRect.iBottom - DestRect.iTop;
			BlitBufferToBuffer(FRAME_BUFFER, guiSAVEBUFFER, DestRect.iLeft, DestRect.iTop, usWidth, usHeight);
		}
	}
	else
	{
		//Restore the old rect
		if (ubCount < AIM_MEMBER_VIDEO_TITLE_ITERATIONS - 2)
		{
			const UINT16 usWidth  = LastRect.iRight  - LastRect.iLeft;
			const UINT16 usHeight = LastRect.iBottom - LastRect.iTop;
			BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, LastRect.iLeft, LastRect.iTop, usWidth, usHeight);
		}

		//Save rectangle
		if (ubCount < AIM_MEMBER_VIDEO_TITLE_ITERATIONS - 1)
		{
			const UINT16 usWidth  = DestRect.iRight  - DestRect.iLeft;
			const UINT16 usHeight = DestRect.iBottom - DestRect.iTop;
			BlitBufferToBuffer(FRAME_BUFFER, guiSAVEBUFFER, DestRect.iLeft, DestRect.iTop, usWidth, usHeight);
		}
	}

	BltStretchVideoSurface(FRAME_BUFFER, guiVideoTitleBar, &SrcRect, &DestRect);

	InvalidateRegion(DestRect.iLeft, DestRect.iTop, DestRect.iRight, DestRect.iBottom);
	InvalidateRegion(LastRect.iLeft, LastRect.iTop, LastRect.iRight, LastRect.iBottom);

	LastRect = DestRect;

	if (fForward)
	{
		ubCount++;
		return ubCount == AIM_MEMBER_VIDEO_TITLE_ITERATIONS - 1;
	}
	else
	{
		ubCount--;
		return ubCount == 0;
	}
}


#ifdef JA2TESTVERSION
//TEMP:
void TempHiringOfMercs( UINT8 ubNumberOfMercs, BOOLEAN fReset )
{
	static const ProfileID MercID[] = { RED, GUS, MAGIC, SCOPE, LYNX, SHADOW, BUNS, GRUNTY, IVAN, REAPER, BARRY, BLOOD, GRIZZLY, VICKY, TREVOR, STEROID, IGOR, FIDEL, FOX, SIDNEY, ICE, SPIDER };

	INT16	i;
	MERC_HIRE_STRUCT HireMercStruct;
	static BOOLEAN	fHaveCalledBefore=FALSE;

	//if we should reset the global variable
	if( fReset )
	{
		fHaveCalledBefore = FALSE;
		return;
	}


	if( fHaveCalledBefore )
		return;

	if( guiCurrentLaptopMode != LAPTOP_MODE_NONE )
		return;

	fHaveCalledBefore = TRUE;

	for( i=0; i<ubNumberOfMercs; i++)
	{
		memset(&HireMercStruct, 0, sizeof(MERC_HIRE_STRUCT));

		if( !IsMercHireable( MercID[i] ) )
		{
			ubNumberOfMercs++;
			continue;
		}

		HireMercStruct.ubProfileID = MercID[i];

		//DEF: temp
		HireMercStruct.sSectorX = gsMercArriveSectorX;
		HireMercStruct.sSectorY = gsMercArriveSectorY;
		HireMercStruct.fUseLandingZoneForArrival = TRUE;
		HireMercStruct.ubInsertionCode	= INSERTION_CODE_ARRIVING_GAME;

		HireMercStruct.fCopyProfileItemsOver = TRUE;
		gMercProfiles[ MercID[i] ].ubMiscFlags |= PROFILE_MISC_FLAG_ALREADY_USED_ITEMS;


		if( gfKeyState[ ALT ] )
			HireMercStruct.iTotalContractLength = 14;
		else if( gfKeyState[ CTRL ] )
			HireMercStruct.iTotalContractLength = 7;
		else
			HireMercStruct.iTotalContractLength = 1;

		//specify when the merc should arrive
		HireMercStruct.uiTimeTillMercArrives = GetMercArrivalTimeOfDay( );// + MercID[i];

		//since this is only a testing function, make the merc available
		gMercProfiles[ MercID[i] ].bMercStatus = 0;

		//if we succesfully hired the merc
		HireMerc( &HireMercStruct );

		//add an entry in the finacial page for the hiring of the merc
		AddTransactionToPlayersBook(HIRED_MERC, MercID[i], GetWorldTotalMin(), -(INT32)( gMercProfiles[MercID[i]].sSalary ) );

		if( gMercProfiles[ MercID[i] ].bMedicalDeposit )
		{
				//add an entry in the finacial page for the medical deposit
			AddTransactionToPlayersBook(	MEDICAL_DEPOSIT, MercID[i], GetWorldTotalMin(), -(gMercProfiles[MercID[i]].sMedicalDepositAmount) );
		}

		//add an entry in the history page for the hiring of the merc
		AddHistoryToPlayersLog(HISTORY_HIRED_MERC_FROM_AIM, MercID[i], GetWorldTotalMin(), -1, -1 );
	}
}

#endif


static void DelayMercSpeech(UINT8 ubMercID, UINT16 usQuoteNum, UINT16 usDelay, BOOLEAN fNewQuote, BOOLEAN fReset)
{
	static UINT32		uiLastTime=0;
	UINT32					uiCurTime;
	static UINT16		usCurQuoteNum;
	static UINT16		usCurDelay;
	static BOOLEAN	fQuoteWaiting = FALSE;		//a quote is waiting to be said
	static UINT8		ubCurMercID;
	static BOOLEAN	fHangUpAfter=FALSE;

	uiCurTime = GetJA2Clock();

	if( fReset )
		fQuoteWaiting = FALSE;

	if( fNewQuote )
	{
		//set up the counters
		uiLastTime = uiCurTime;

		ubCurMercID = ubMercID;
		usCurQuoteNum = usQuoteNum;
		usCurDelay = usDelay;

		if( gfHangUpMerc )
		{
			gfHangUpMerc = FALSE;
			fHangUpAfter = TRUE;
		}


		fQuoteWaiting = TRUE;
	}


	if( fQuoteWaiting )
	{
		if( ( uiCurTime - uiLastTime ) > usCurDelay )
		{
			InitVideoFaceTalking( ubCurMercID, usCurQuoteNum );
			fQuoteWaiting = FALSE;

			if( fHangUpAfter )
			{
				gfHangUpMerc = TRUE;
				fHangUpAfter = FALSE;
			}
		}
	}
}





#ifdef JA2TESTVERSION

extern void SetFlagToForceHireMerc(BOOLEAN fForceHire);


//TEMP!!!
static BOOLEAN QuickHireMerc(void)
{
	INT8	bReturnCode;
	MERC_HIRE_STRUCT HireMercStruct;
	UINT8		ubCurrentSoldier = AimMercArray[gbCurrentIndex];

	giContractAmount = 0;

//	if( !IsMercHireable( ubCurrentSoldier ) )
//		return( FALSE );
	if (FindSoldierByProfileIDOnPlayerTeam(ubCurrentSoldier) != NULL) return FALSE;

	HireMercStruct.ubProfileID = ubCurrentSoldier;

	//DEF: temp
	HireMercStruct.sSectorX = gsMercArriveSectorX;
	HireMercStruct.sSectorY = gsMercArriveSectorY;
	HireMercStruct.bSectorZ = 0;
	HireMercStruct.fUseLandingZoneForArrival = TRUE;
	HireMercStruct.ubInsertionCode	= INSERTION_CODE_ARRIVING_GAME;

	HireMercStruct.fCopyProfileItemsOver = TRUE;
	gMercProfiles[ ubCurrentSoldier ].ubMiscFlags |= PROFILE_MISC_FLAG_ALREADY_USED_ITEMS;



	if( gfKeyState[ ALT ] )
		HireMercStruct.iTotalContractLength = 14;
	else if( gfKeyState[ CTRL ] )
		HireMercStruct.iTotalContractLength = 7;
	else
		HireMercStruct.iTotalContractLength = 1;


	//specify when the merc should arrive
	HireMercStruct.uiTimeTillMercArrives = GetMercArrivalTimeOfDay( );// + ubCurrentSoldier;

	SetFlagToForceHireMerc( TRUE );
	bReturnCode = HireMerc( &HireMercStruct );
	SetFlagToForceHireMerc( FALSE );
	if( bReturnCode == MERC_HIRE_OVER_20_MERCS_HIRED )
	{
		//display a warning saying u cant hire more then 20 mercs
		DoLapTopMessageBox( MSG_BOX_LAPTOP_DEFAULT, AimPopUpText[ AIM_MEMBER_ALREADY_HAVE_20_MERCS ], LAPTOP_SCREEN, MSG_BOX_FLAG_OK, NULL);
		return(FALSE);
	}
	else if( bReturnCode == MERC_HIRE_FAILED )
	{
		return(FALSE);
	}

	//add an entry in the finacial page for the hiring of the merc
	giContractAmount = gMercProfiles[gbCurrentSoldier].sSalary;

	AddTransactionToPlayersBook(HIRED_MERC, ubCurrentSoldier, GetWorldTotalMin(), -( giContractAmount ) );//- gMercProfiles[gbCurrentSoldier].sMedicalDepositAmount

	if( gMercProfiles[ gbCurrentSoldier ].bMedicalDeposit )
	{
		//add an entry in the finacial page for the medical deposit
		AddTransactionToPlayersBook(	MEDICAL_DEPOSIT, ubCurrentSoldier, GetWorldTotalMin(), -(gMercProfiles[gbCurrentSoldier].sMedicalDepositAmount) );
	}

	//add an entry in the history page for the hiring of the merc
	AddHistoryToPlayersLog(HISTORY_HIRED_MERC_FROM_AIM, ubCurrentSoldier, GetWorldTotalMin(), -1, -1 );

	gfRedrawScreen = TRUE;

	return( TRUE );
}


static void TempHandleAimMemberKeyBoardInput(void)
{
	InputAtom					InputEvent;

	while (DequeueEvent(&InputEvent) == TRUE)
	{//!HandleTextInput( &InputEvent ) &&
		if( InputEvent.usEvent == KEY_DOWN )
		{
			switch (InputEvent.usParam)
			{
#ifdef JA2TESTVERSION
				case SDLK_SPACE: QuickHireMerc(); break;

				case '~':
					// to test going on other assignments, unhired merc improvements & deaths
					if (guiDay == 1) guiDay++;
					MercDailyUpdate();
					gfRedrawScreen = TRUE;
					break;
#endif

				default:
					HandleKeyBoardShortCutsForLapTop( InputEvent.usEvent, InputEvent.usParam, InputEvent.usKeyState );
					break;
			}
		}
	}
}

#endif


static void WaitForMercToFinishTalkingOrUserToClick(void)
{
	//if the region is not active
	if( !gfIsShutUpMouseRegionActive )
	{
		//Enables it so if a player clicks, he will shutup the merc
		MSYS_EnableRegion(&gSelectedShutUpMercRegion);
		gfIsShutUpMouseRegionActive = TRUE;
	}

	if( gfIsAnsweringMachineActive )
		gubVideoConferencingMode = AIM_VIDEO_MERC_ANSWERING_MACHINE_MODE;
	else
		gubVideoConferencingMode = AIM_VIDEO_FIRST_CONTACT_MERC_MODE;

	gfWaitingForMercToStopTalkingOrUserToClick = TRUE;
	gfHangUpMerc = TRUE;
	gfStopMercFromTalking = FALSE;
}


#if defined ( JA2TESTVERSION ) || defined ( JA2DEMO )

void DemoHiringOfMercs( )
{
#ifdef GERMAN
	const ProfileID MercID[] = { IVAN, SHADOW, VICKY, FOX, BUBBA };
#else
	const ProfileID MercID[] = { IVAN, SHADOW, VICKY, GASKET, DR_Q };
#endif

	INT16	i;
	MERC_HIRE_STRUCT HireMercStruct;
	#ifndef JA2DEMO
		static BOOLEAN	fHaveCalledBefore=FALSE;

		if( fHaveCalledBefore )
			return;

		fHaveCalledBefore = TRUE;

		if( guiCurrentLaptopMode != LAPTOP_MODE_NONE )
			return;
	#endif

	for( i=0; i<5; i++)
	{
		memset(&HireMercStruct, 0, sizeof(MERC_HIRE_STRUCT));

		HireMercStruct.ubProfileID = MercID[i];

		//DEF: temp
		HireMercStruct.sSectorX = 1;
		HireMercStruct.sSectorY = 16;
		HireMercStruct.ubInsertionCode	= INSERTION_CODE_ARRIVING_GAME;

		HireMercStruct.fCopyProfileItemsOver = TRUE;
		gMercProfiles[ MercID[i] ].ubMiscFlags |= PROFILE_MISC_FLAG_ALREADY_USED_ITEMS;


		HireMercStruct.iTotalContractLength = 60;

		//specify when the merc should arrive
		HireMercStruct.uiTimeTillMercArrives = GetMercArrivalTimeOfDay( );// + MercID[i];

		//since this is only a testing function, make the merc available
		gMercProfiles[ MercID[i] ].bMercStatus = 0;

		//if we succesfully hired the merc
		HireMerc( &HireMercStruct );

		//add an entry in the finacial page for the hiring of the merc
		AddTransactionToPlayersBook(HIRED_MERC, MercID[i], GetWorldTotalMin(), -(INT32)( gMercProfiles[MercID[i]].sSalary ) );

		if( gMercProfiles[ MercID[i] ].bMedicalDeposit )
		{
				//add an entry in the finacial page for the medical deposit
			AddTransactionToPlayersBook(	MEDICAL_DEPOSIT, MercID[i], GetWorldTotalMin(), -(gMercProfiles[MercID[i]].sMedicalDepositAmount) );
		}

		//add an entry in the history page for the hiring of the merc
		AddHistoryToPlayersLog(HISTORY_HIRED_MERC_FROM_AIM, MercID[i], GetWorldTotalMin(), -1, -1 );
	}
}

#endif


static void DisplayPopUpBoxExplainingMercArrivalLocationAndTimeCallBack(UINT8 bExitValue);


void DisplayPopUpBoxExplainingMercArrivalLocationAndTime(void)
{
	CHAR16	szLocAndTime[512];
	CHAR16		zTimeString[128];
	CHAR16		zSectorIDString[512];
	UINT32		uiHour;

	//if the id of the merc is invalid, dont display the pop up
	if( LaptopSaveInfo.sLastHiredMerc.iIdOfMerc == -1 )
		return;

	//if the pop up has already been displayed, dont display it again for this occurence of laptop
	if( LaptopSaveInfo.sLastHiredMerc.fHaveDisplayedPopUpInLaptop )
		return;

	SOLDIERTYPE* const pSoldier = FindSoldierByProfileIDOnPlayerTeam(LaptopSaveInfo.sLastHiredMerc.iIdOfMerc);
	if( pSoldier == NULL )
		return;

	//calc the approximate hour the mercs will arrive at
	uiHour = ( ( LaptopSaveInfo.sLastHiredMerc.uiArrivalTime ) - ( ( ( LaptopSaveInfo.sLastHiredMerc.uiArrivalTime ) / 1440 ) * 1440 ) ) / 60;

	//create the time string
	swprintf( zTimeString, lengthof(zTimeString), L"%02d:%02d", uiHour, 0 );

	//get the id string
	GetSectorIDString( gsMercArriveSectorX, gsMercArriveSectorY, 0, zSectorIDString, lengthof(zSectorIDString), FALSE );

	//create the string to display to the user, looks like....
	//	L"%ls should arrive at the designated drop-off point ( sector %d:%d %s ) on day %d, at approximately %ls.",		//first %ls is mercs name, next is the sector location and name where they will be arriving in, lastely is the day an the time of arrival

#ifdef GERMAN
	//Germans version has a different argument order
	swprintf( szLocAndTime, lengthof(szLocAndTime), pMessageStrings[ MSG_JUST_HIRED_MERC_ARRIVAL_LOCATION_POPUP ],
							gMercProfiles[ pSoldier->ubProfile ].zNickname,
							LaptopSaveInfo.sLastHiredMerc.uiArrivalTime / 1440,
							zTimeString,
							zSectorIDString );
#else
	swprintf( szLocAndTime, lengthof(szLocAndTime), pMessageStrings[ MSG_JUST_HIRED_MERC_ARRIVAL_LOCATION_POPUP ],
							gMercProfiles[ pSoldier->ubProfile ].zNickname,
							zSectorIDString,
							LaptopSaveInfo.sLastHiredMerc.uiArrivalTime / 1440,
							zTimeString );
#endif



	//display the message box
	DoLapTopMessageBox( MSG_BOX_LAPTOP_DEFAULT, szLocAndTime, LAPTOP_SCREEN, MSG_BOX_FLAG_OK, DisplayPopUpBoxExplainingMercArrivalLocationAndTimeCallBack );

	//reset the id of the last merc
	LaptopSaveInfo.sLastHiredMerc.iIdOfMerc = -1;

	//set the fact that the pop up has been displayed this time in laptop
	LaptopSaveInfo.sLastHiredMerc.fHaveDisplayedPopUpInLaptop = TRUE;
}


static void DisplayPopUpBoxExplainingMercArrivalLocationAndTimeCallBack(UINT8 bExitValue)
{
	//unset the flag so the msgbox WONT dislay its save buffer
	gfDontOverRideSaveBuffer = FALSE;

	if( guiCurrentLaptopMode == LAPTOP_MODE_AIM_MEMBERS )
	{
		//render the screen
		gfRedrawScreen = TRUE;
		RenderAIMMembers();
	}
}


static void DisplayAimMemberClickOnFaceHelpText(void)
{
	//display the 'left and right click' onscreen help msg
	DrawTextToScreen(AimMemberText[0], AIM_FI_LEFT_CLICK_TEXT_X, AIM_FI_LEFT_CLICK_TEXT_Y,                                   AIM_FI_CLICK_TEXT_WIDTH, AIM_FI_HELP_TITLE_FONT, AIM_FONT_MCOLOR_WHITE, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
	DrawTextToScreen(AimMemberText[1], AIM_FI_LEFT_CLICK_TEXT_X, AIM_FI_LEFT_CLICK_TEXT_Y + AIM_FI_CLICK_DESC_TEXT_Y_OFFSET, AIM_FI_CLICK_TEXT_WIDTH, AIM_FI_HELP_FONT,       AIM_FONT_MCOLOR_WHITE, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);

	DrawTextToScreen(AimMemberText[2], AIM_FI_RIGHT_CLICK_TEXT_X, AIM_FI_LEFT_CLICK_TEXT_Y,                                   AIM_FI_CLICK_TEXT_WIDTH, AIM_FI_HELP_TITLE_FONT, AIM_FONT_MCOLOR_WHITE, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
	DrawTextToScreen(AimMemberText[3], AIM_FI_RIGHT_CLICK_TEXT_X, AIM_FI_LEFT_CLICK_TEXT_Y + AIM_FI_CLICK_DESC_TEXT_Y_OFFSET, AIM_FI_CLICK_TEXT_WIDTH, AIM_FI_HELP_FONT,       AIM_FONT_MCOLOR_WHITE, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
}
