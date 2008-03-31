#include "Font.h"
#include "Laptop.h"
#include "Finances.h"
#include "Game_Clock.h"
#include "LoadSaveData.h"
#include "Map_Screen_Interface_Bottom.h"
#include "VObject.h"
#include "WCheck.h"
#include "Debug.h"
#include "Render_Dirty.h"
#include "Cursors.h"
#include "Soldier_Profile.h"
#include "Text.h"
#include "Strategic_Mines.h"
#include "LaptopSave.h"
#include "Campaign_Types.h"
#include "StrategicMap.h"
#include "VSurface.h"
#include "MemMan.h"
#include "Button_System.h"
#include "Font_Control.h"
#include "FileMan.h"


#define FINANCE_HEADER_SIZE 4
#define FINANCE_RECORD_SIZE (1 + 1 + 4 + 4 + 4)


// the financial structure
typedef struct FinanceUnit FinanceUnit;
struct FinanceUnit
{
	UINT8 ubCode; // the code index in the finance code table
	UINT8 ubSecondCode; // secondary code
	UINT32 uiDate; // time in the world in global time
	INT32 iAmount; // the amount of the transaction
	INT32 iBalanceToDate;
	FinanceUnit* Next; // next unit in the list
};


// the global defines

// graphical positions
#define TOP_X 0+LAPTOP_SCREEN_UL_X
#define TOP_Y LAPTOP_SCREEN_UL_Y
#define BLOCK_HEIGHT 10
#define TOP_DIVLINE_Y 102
#define DIVLINE_X 130
#define MID_DIVLINE_Y 205
#define BOT_DIVLINE_Y 180
#define MID_DIVLINE_Y2 263 + 20
#define BOT_DIVLINE_Y2 MID_DIVLINE_Y2 + MID_DIVLINE_Y - BOT_DIVLINE_Y
#define TITLE_X 140
#define TITLE_Y 33
#define TEXT_X 140
#define PAGE_SIZE 17

// yesterdyas/todays income and balance text positions
#define YESTERDAYS_INCOME 114
#define YESTERDAYS_OTHER 138
#define YESTERDAYS_DEBITS 162
#define YESTERDAYS_BALANCE 188
#define TODAYS_INCOME 215
#define TODAYS_OTHER 239
#define TODAYS_DEBITS  263
#define TODAYS_CURRENT_BALANCE  263 + 28
#define TODAYS_CURRENT_FORCAST_INCOME  330
#define TODAYS_CURRENT_FORCAST_BALANCE 354
#define FINANCE_HEADER_FONT FONT14ARIAL
#define FINANCE_TEXT_FONT FONT12ARIAL
#define NUM_RECORDS_PER_PAGE PAGE_SIZE

// records text positions
#define RECORD_CREDIT_WIDTH 106-47
#define RECORD_DEBIT_WIDTH RECORD_CREDIT_WIDTH
#define RECORD_DATE_X TOP_X+10
#define RECORD_TRANSACTION_X RECORD_DATE_X+RECORD_DATE_WIDTH
#define RECORD_TRANSACTION_WIDTH 500-280
#define RECORD_DEBIT_X RECORD_TRANSACTION_X+RECORD_TRANSACTION_WIDTH
#define RECORD_CREDIT_X RECORD_DEBIT_X+RECORD_DEBIT_WIDTH
#define RECORD_Y 107-10
#define RECORD_DATE_WIDTH 47
#define RECORD_BALANCE_X RECORD_DATE_X+385
#define RECORD_BALANCE_WIDTH 479-385
#define RECORD_HEADER_Y 90


#define PAGE_NUMBER_X				TOP_X+297 //345
#define PAGE_NUMBER_Y				TOP_Y+33


// BUTTON defines
enum{
	PREV_PAGE_BUTTON=0,
  NEXT_PAGE_BUTTON,
	FIRST_PAGE_BUTTON,
	LAST_PAGE_BUTTON,
};


// button positions

#define	FIRST_PAGE_X		505
#define NEXT_BTN_X			553//577
#define PREV_BTN_X			529//553
#define	LAST_PAGE_X			577
#define BTN_Y 53



// sizeof one record
#define RECORD_SIZE ( sizeof( UINT32 ) + sizeof( INT32 ) + sizeof( INT32 ) + sizeof( UINT8 ) + sizeof( UINT8 ) )




// the financial record list
static FinanceUnit* pFinanceListHead = NULL;

// current page displayed
static INT32 iCurrentPage = 0;

// video object id's
static SGPVObject* guiTITLE;
static SGPVObject* guiTOP;
static SGPVObject* guiLINE;
static SGPVObject* guiLONGLINE;
static SGPVObject* guiLISTCOLUMNS;

// are in the financial system right now?
static BOOLEAN fInFinancialMode = FALSE;


// the last page altogether
static UINT32 guiLastPageInRecordsList = 0;

// finance screen buttons
static INT32 giFinanceButton[4];
static BUTTON_PICS* giFinanceButtonImage[4];

// internal functions
static void ProcessAndEnterAFinacialRecord(UINT8 ubCode, UINT32 uiDate, INT32 iAmount, UINT8 ubSecondCode, INT32 iBalanceToDate);
static BOOLEAN LoadFinances(void);
static void RemoveFinances(void);
static void ClearFinanceList(void);
static void DrawRecordsColumnHeadersText(void);
static void BtnFinanceDisplayNextPageCallBack(GUI_BUTTON *btn, INT32 reason);
static void BtnFinanceFirstLastPageCallBack(GUI_BUTTON *btn, INT32 reason);
static void BtnFinanceDisplayPrevPageCallBack(GUI_BUTTON *btn, INT32 reason);
static void CreateFinanceButtons(void);
static void DestroyFinanceButtons(void);
static void GetBalanceFromDisk(void);
static BOOLEAN WriteBalanceToDisk(void);
static BOOLEAN AppendFinanceToEndOfFile(void);
static void SetLastPageInRecords(void);
static BOOLEAN LoadInRecords(UINT32 uiPage);
static BOOLEAN LoadPreviousPage(void);
static BOOLEAN LoadNextPage(void);

static INT32 GetPreviousDaysIncome(void);

static void SetFinanceButtonStates(void);
static INT32 GetTodaysBalance(void);
static INT32 GetTodaysDebits(void);
static INT32 GetYesterdaysOtherDeposits(void);
static INT32 GetTodaysOtherDeposits(void);
static INT32 GetYesterdaysDebits(void);


void AddTransactionToPlayersBook(UINT8 ubCode, UINT8 ubSecondCode, UINT32 uiDate, INT32 iAmount)
{
	// adds transaction to player's book(Financial List)
	// outside of the financial system(the code in this .c file), this is the only function you'll ever need

	// read in balance from file

  GetBalanceFromDisk( );
	// process the actual data


	//
	// If this transaction is for the hiring/extending of a mercs contract
	//
	if( ubCode == HIRED_MERC ||
			ubCode == IMP_PROFILE ||
			ubCode == PAYMENT_TO_NPC ||
			ubCode == EXTENDED_CONTRACT_BY_1_DAY ||
			ubCode == EXTENDED_CONTRACT_BY_1_WEEK ||
			ubCode == EXTENDED_CONTRACT_BY_2_WEEKS
		)
	{
		gMercProfiles[ ubSecondCode ].uiTotalCostToDate += -iAmount;
	}

	// clear list
	ClearFinanceList( );

	// update balance
	LaptopSaveInfo.iCurrentBalance += iAmount;

	ProcessAndEnterAFinacialRecord(ubCode, uiDate, iAmount, ubSecondCode, LaptopSaveInfo.iCurrentBalance);

	// write balance to disk
  WriteBalanceToDisk( );

  // append to end of file
	AppendFinanceToEndOfFile();

	// set number of pages
	SetLastPageInRecords( );

	if( !fInFinancialMode )
	{
    ClearFinanceList( );
	}
  else
	{
		SetFinanceButtonStates( );

		// force update
	  fPausedReDrawScreenFlag = TRUE;
	}

	fMapScreenBottomDirty = TRUE;
}


INT32 GetCurrentBalance( void )
{
	// get balance to this minute
	return ( LaptopSaveInfo.iCurrentBalance );
}


INT32 GetProjectedTotalDailyIncome( void )
{
	// return total  projected income, including what is earned today already

	// CJC: I DON'T THINK SO!
	// The point is:  PredictIncomeFromPlayerMines isn't dependant on the time of day
	// (anymore) and this would report income of 0 at midnight!
	/*
  if (GetWorldMinutesInDay() <= 0)
	{
		return ( 0 );
	}
	*/
	// look at we earned today

	// then there is how many deposits have been made, now look at how many mines we have, thier rate, amount of ore left and predict if we still
	// had these mines how much more would we get?

	return( PredictIncomeFromPlayerMines() );
}


void GameInitFinances()
{
  // initialize finances on game start up
	// unlink Finances data file
	if( (FileExists( FINANCES_DATA_FILE ) ) )
	{
		FileClearAttributes( FINANCES_DATA_FILE );
	  FileDelete( FINANCES_DATA_FILE );
	}
	GetBalanceFromDisk( );
}

void EnterFinances()
{
 //entry into finanacial system, load graphics, set variables..draw screen once
 // set the fact we are in the financial display system

  fInFinancialMode=TRUE;

	// reset page we are on
	iCurrentPage = LaptopSaveInfo.iCurrentFinancesPage;


  // get the balance
  GetBalanceFromDisk( );

  // clear the list
  ClearFinanceList( );

  // force redraw of the entire screen
  fReDrawScreenFlag=TRUE;

  // set number of pages
	SetLastPageInRecords( );

  // load graphics into memory
  LoadFinances( );

  // create buttons
  CreateFinanceButtons( );

	// set button state
	SetFinanceButtonStates( );

  RenderFinances( );
}

void ExitFinances( void )
{
	LaptopSaveInfo.iCurrentFinancesPage = iCurrentPage;


	// not in finance system anymore
  fInFinancialMode=FALSE;

	// destroy buttons
	DestroyFinanceButtons( );

  // clear out list
	ClearFinanceList( );


	// remove graphics
	RemoveFinances( );
}

void HandleFinances( void )
{

}


static void DisplayFinancePageNumberAndDateRange(void);
static void DrawAPageOfRecords(void);
static void DrawFinanceTitleText(void);
static void DrawSummary(void);
static void RenderBackGround(void);


void RenderFinances(void)
{
  RenderBackGround();

	// if we are on the first page, draw the summary
	if(iCurrentPage==0)
   DrawSummary( );
	else
   DrawAPageOfRecords( );

	DrawFinanceTitleText( );

  DisplayFinancePageNumberAndDateRange( );

	BltVideoObject(FRAME_BUFFER, guiLaptopBACKGROUND, 0, 108, 23);

	BlitTitleBarIcons(  );
}


static BOOLEAN LoadFinances(void)
{
  // load Finance video objects into memory

	// title bar
	guiTITLE = AddVideoObjectFromFile("LAPTOP/programtitlebar.sti");
	CHECKF(guiTITLE != NO_VOBJECT);

	// top portion of the screen background
	guiTOP = AddVideoObjectFromFile("LAPTOP/Financeswindow.sti");
	CHECKF(guiTOP != NO_VOBJECT);

  // black divider line - long ( 480 length)
	guiLONGLINE = AddVideoObjectFromFile("LAPTOP/divisionline480.sti");
	CHECKF(guiLONGLINE != NO_VOBJECT);

	// the records columns
	guiLISTCOLUMNS = AddVideoObjectFromFile("LAPTOP/recordcolumns.sti");
	CHECKF(guiLISTCOLUMNS != NO_VOBJECT);

  // black divider line - long ( 480 length)
	guiLINE = AddVideoObjectFromFile("LAPTOP/divisionline.sti");
	CHECKF(guiLINE != NO_VOBJECT);

	return (TRUE);
}


static void RemoveFinances(void)
{
	// delete Finance video objects from memory
  DeleteVideoObject(guiLONGLINE);
	DeleteVideoObject(guiLINE);
  DeleteVideoObject(guiLISTCOLUMNS);
	DeleteVideoObject(guiTOP);
	DeleteVideoObject(guiTITLE);
}


static void RenderBackGround(void)
{
	// render generic background for Finance system
	BltVideoObject(FRAME_BUFFER, guiTITLE, 0, TOP_X, TOP_Y -  2);
	BltVideoObject(FRAME_BUFFER, guiTOP,   0, TOP_X, TOP_Y + 22);
}


static void DrawSummaryLines(void);
static void DrawSummaryText(void);


static void DrawSummary(void)
{
	// draw day's summary to screen
  DrawSummaryLines( );
	DrawSummaryText( );
}


static void DrawSummaryLines(void)
{
	// draw divider lines on screen
	// blit summary LINE object to screen
	BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, TOP_DIVLINE_Y);
  BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, TOP_DIVLINE_Y+2);
  //BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, MID_DIVLINE_Y);
  BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, BOT_DIVLINE_Y);
	BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, MID_DIVLINE_Y2);
  //BltVideoObject(FRAME_BUFFER, guiLINE, 0,DIVLINE_X, BOT_DIVLINE_Y2);
}


static void DrawRecordsBackGround(void);
static void DrawRecordsText(void);


static void DrawAPageOfRecords(void)
{
	// this procedure will draw a series of financial records to the screen

	// (re-)render background
	DrawRecordsBackGround( );

	// error check
	if(iCurrentPage==-1)
		return;


	// current page is found, render  from here
	DrawRecordsText( );
}


static void DrawRecordsBackGround(void)
{
	// proceudre will draw the background for the list of financial records
  INT32 iCounter;

	// now the columns
	for (iCounter = 6; iCounter < 35; iCounter++)
	{
    BltVideoObject(FRAME_BUFFER, guiLISTCOLUMNS, 0, TOP_X + 10, TOP_Y + 18 + iCounter * BLOCK_HEIGHT + 1);
	}

	// the divisorLines
  BltVideoObject(FRAME_BUFFER, guiLONGLINE, 0, TOP_X + 10, TOP_Y + 17 + 6 * BLOCK_HEIGHT);
  BltVideoObject(FRAME_BUFFER, guiLONGLINE, 0, TOP_X + 10, TOP_Y + 19 + 6 * BLOCK_HEIGHT);
  BltVideoObject(FRAME_BUFFER, guiLONGLINE, 0, TOP_X + 10, TOP_Y + 19 + iCounter * BLOCK_HEIGHT);

	// the header text
  DrawRecordsColumnHeadersText( );
}


static void DrawRecordsColumnHeadersText(void)
{
  // write the headers text for each column

	// font stuff
	SetFont(FINANCE_TEXT_FONT);
  SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	INT16 usX;
	INT16 usY;

	// the date header
  FindFontCenterCoordinates(RECORD_DATE_X,0,RECORD_DATE_WIDTH,0, pFinanceHeaders[0], FINANCE_TEXT_FONT,&usX, &usY);
	mprintf(usX, RECORD_HEADER_Y, pFinanceHeaders[0]);

  // debit header
	FindFontCenterCoordinates(RECORD_DEBIT_X,0,RECORD_DEBIT_WIDTH,0, pFinanceHeaders[1], FINANCE_TEXT_FONT,&usX, &usY);
	mprintf(usX, RECORD_HEADER_Y, pFinanceHeaders[1]);

	// credit header
	FindFontCenterCoordinates(RECORD_CREDIT_X,0,RECORD_CREDIT_WIDTH,0, pFinanceHeaders[2], FINANCE_TEXT_FONT,&usX, &usY);
	mprintf(usX, RECORD_HEADER_Y, pFinanceHeaders[2]);

	// balance header
  FindFontCenterCoordinates(RECORD_BALANCE_X,0,RECORD_BALANCE_WIDTH,0, pFinanceHeaders[4], FINANCE_TEXT_FONT,&usX, &usY);
	mprintf(usX, RECORD_HEADER_Y, pFinanceHeaders[4]);

	// transaction header
  FindFontCenterCoordinates(RECORD_TRANSACTION_X,0,RECORD_TRANSACTION_WIDTH,0, pFinanceHeaders[3], FINANCE_TEXT_FONT,&usX, &usY);
	mprintf(usX, RECORD_HEADER_Y, pFinanceHeaders[3]);

	SetFontShadow(DEFAULT_SHADOW);
}


static void ProcessTransactionString(wchar_t pString[], size_t Length, const FinanceUnit* pFinance);


static void DrawRecordsText(void)
{
  // draws the text of the records
	wchar_t sString[512];

  // setup the font stuff
	SetFont(FINANCE_TEXT_FONT);
  SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
  SetFontShadow(NO_SHADOW);

	const FinanceUnit* pCurFinance = pFinanceListHead;

	// anything to print
	if (pCurFinance == NULL)
	{
		// nothing to print
		return;
	}

	// loop through record list
	for (INT32 iCounter = 0; iCounter < NUM_RECORDS_PER_PAGE; iCounter++)
	{
		// get and write the date
		swprintf(sString, lengthof(sString), L"%d", pCurFinance->uiDate / ( 24*60 ) );

		INT16 usX;
		INT16 usY;

		FindFontCenterCoordinates(RECORD_DATE_X,0,RECORD_DATE_WIDTH,0, sString, FINANCE_TEXT_FONT,&usX, &usY);
		mprintf(usX, 12+RECORD_Y + ( iCounter * ( GetFontHeight( FINANCE_TEXT_FONT ) + 6 ) ), sString);

		// get and write debit/ credit
		if(pCurFinance->iAmount >=0)
		{
			// increase in asset - debit
			SPrintMoney(sString, pCurFinance->iAmount);

		 FindFontCenterCoordinates(RECORD_DEBIT_X,0,RECORD_DEBIT_WIDTH,0, sString, FINANCE_TEXT_FONT,&usX, &usY);
		 mprintf(usX, 12+RECORD_Y + (iCounter * ( GetFontHeight( FINANCE_TEXT_FONT ) + 6 ) ), sString);
		}
		else
		{
			// decrease in asset - credit
		 SetFontForeground(FONT_RED);
			SPrintMoney(sString, -pCurFinance->iAmount);

		 FindFontCenterCoordinates(RECORD_CREDIT_X ,0 , RECORD_CREDIT_WIDTH,0, sString, FINANCE_TEXT_FONT,&usX, &usY);
		 mprintf(usX, 12+RECORD_Y + (iCounter * ( GetFontHeight( FINANCE_TEXT_FONT ) + 6 ) ), sString);
		 SetFontForeground(FONT_BLACK);
		}

		// the balance to this point
    INT32 iBalance = pCurFinance->iBalanceToDate;

		// set font based on balance
		if(iBalance >=0)
		{
      SetFontForeground(FONT_BLACK);
		}
		else
		{
      SetFontForeground(FONT_RED);
			iBalance = ( iBalance ) * ( -1 );
		}

		// transaction string
		ProcessTransactionString(sString, lengthof(sString), pCurFinance);
    FindFontCenterCoordinates(RECORD_TRANSACTION_X,0,RECORD_TRANSACTION_WIDTH,0, sString, FINANCE_TEXT_FONT,&usX, &usY);
		mprintf(usX, 12+RECORD_Y + (iCounter * ( GetFontHeight( FINANCE_TEXT_FONT ) + 6 ) ), sString);


		// print the balance string
		SPrintMoney(sString, iBalance);

		FindFontCenterCoordinates(RECORD_BALANCE_X,0,RECORD_BALANCE_WIDTH,0, sString, FINANCE_TEXT_FONT,&usX, &usY);
		mprintf(usX, 12+RECORD_Y + (iCounter * ( GetFontHeight( FINANCE_TEXT_FONT ) + 6 ) ), sString);

		// restore font color
		SetFontForeground(FONT_BLACK);

		// next finance
		pCurFinance = pCurFinance->Next;

		// last page, no finances left, return
		if( ! pCurFinance )
		{

			// restore shadow
      SetFontShadow(DEFAULT_SHADOW);
			return;
		}

	}

	// restore shadow
  SetFontShadow(DEFAULT_SHADOW);
}


static void DrawFinanceTitleText(void)
{
	// setup the font stuff
	SetFont(FINANCE_HEADER_FONT);
  SetFontForeground(FONT_WHITE);
	SetFontBackground(FONT_BLACK);
  // reset shadow
	SetFontShadow(DEFAULT_SHADOW);

	// draw the pages title
	mprintf(TITLE_X, TITLE_Y, pFinanceTitle);
}


static INT32 GetTodaysDaysIncome(void);
static void SPrintMoneyNoDollarOnZero(wchar_t* Str, INT32 Amount);


static void DrawSummaryText(void)
{
	INT16 usX, usY;
  wchar_t pString[100];
	INT32 iBalance = 0;


	// setup the font stuff
  SetFont(FINANCE_TEXT_FONT);
  SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	// draw summary text to the screen
	mprintf(TEXT_X,YESTERDAYS_INCOME,pFinanceSummary[2]);
  mprintf(TEXT_X,YESTERDAYS_OTHER,pFinanceSummary[3]);
  mprintf(TEXT_X,YESTERDAYS_DEBITS,pFinanceSummary[4]);
  mprintf(TEXT_X,YESTERDAYS_BALANCE,pFinanceSummary[5]);
  mprintf(TEXT_X,TODAYS_INCOME,pFinanceSummary[6]);
  mprintf(TEXT_X,TODAYS_OTHER,pFinanceSummary[7]);
  mprintf(TEXT_X,TODAYS_DEBITS,pFinanceSummary[8]);
	mprintf(TEXT_X,TODAYS_CURRENT_BALANCE, pFinanceSummary[9]);
	mprintf(TEXT_X,TODAYS_CURRENT_FORCAST_INCOME, pFinanceSummary[10]);
	mprintf(TEXT_X,TODAYS_CURRENT_FORCAST_BALANCE, pFinanceSummary[11]);

	// draw the actual numbers



	// yesterdays income
	SPrintMoneyNoDollarOnZero(pString, GetPreviousDaysIncome());
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);
	mprintf(usX,YESTERDAYS_INCOME,pString);

	SetFontForeground( FONT_BLACK );

	// yesterdays other
	SPrintMoneyNoDollarOnZero(pString, GetYesterdaysOtherDeposits());
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,YESTERDAYS_OTHER,pString);

	SetFontForeground( FONT_RED );

	// yesterdays debits
	iBalance =  GetYesterdaysDebits( );
	if( iBalance < 0 )
	{
		SetFontForeground( FONT_RED );
		iBalance *= -1;
	}

	SPrintMoneyNoDollarOnZero(pString, iBalance);
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,YESTERDAYS_DEBITS,pString);

	SetFontForeground( FONT_BLACK );

	// yesterdays balance..ending balance..so todays balance then
	iBalance =  GetTodaysBalance( );

	if( iBalance < 0 )
	{
		SetFontForeground( FONT_RED );
		iBalance *= -1;
	}

	SPrintMoneyNoDollarOnZero(pString, iBalance);
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,YESTERDAYS_BALANCE,pString);

	SetFontForeground( FONT_BLACK );

	// todays income
	SPrintMoneyNoDollarOnZero(pString, GetTodaysDaysIncome());
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,TODAYS_INCOME,pString);

	SetFontForeground( FONT_BLACK );

	// todays other
	SPrintMoneyNoDollarOnZero(pString, GetTodaysOtherDeposits());
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,TODAYS_OTHER,pString);

	SetFontForeground( FONT_RED );

	// todays debits
	iBalance =  GetTodaysDebits( );

	// absolute value
	if( iBalance < 0 )
	{
		iBalance *= ( -1 );
	}

	SPrintMoneyNoDollarOnZero(pString, iBalance);
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,TODAYS_DEBITS,pString);

	SetFontForeground( FONT_BLACK );

	// todays current balance
	iBalance = GetCurrentBalance( );
	if( iBalance < 0 )
	{
		iBalance *= -1;
		SetFontForeground( FONT_RED );
	}

	SPrintMoneyNoDollarOnZero(pString, iBalance);
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);
	mprintf(usX,TODAYS_CURRENT_BALANCE,pString);
  SetFontForeground( FONT_BLACK );


	// todays forcast income
	SPrintMoneyNoDollarOnZero(pString, GetProjectedTotalDailyIncome());
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);

	mprintf(usX,TODAYS_CURRENT_FORCAST_INCOME,pString);

	SetFontForeground( FONT_BLACK );


	// todays forcast balance
	iBalance = GetCurrentBalance( ) + GetProjectedTotalDailyIncome( );
	if( iBalance < 0 )
	{
		iBalance *= -1;
		SetFontForeground( FONT_RED );
	}

	SPrintMoneyNoDollarOnZero(pString, iBalance);
	FindFontRightCoordinates(0,0,580,0,pString,FINANCE_TEXT_FONT, &usX, &usY);
	mprintf(usX,TODAYS_CURRENT_FORCAST_BALANCE,pString);
  SetFontForeground( FONT_BLACK );



	// reset the shadow
	SetFontShadow(DEFAULT_SHADOW);
}


static void ClearFinanceList(void)
{
	// remove each element from list of transactions
	FinanceUnit* pFinanceList = pFinanceListHead;
	FinanceUnit* pFinanceNode = pFinanceList;

	// while there are elements in the list left, delete them
	while( pFinanceList )
	{
    // set node to list head
		pFinanceNode=pFinanceList;

		// set list head to next node
		pFinanceList=pFinanceList->Next;

		// delete current node
		MemFree(pFinanceNode);
	}
  pFinanceListHead = NULL;
}


static void ProcessAndEnterAFinacialRecord(const UINT8 ubCode, const UINT32 uiDate, const INT32 iAmount, const UINT8 ubSecondCode, const INT32 iBalanceToDate)
{
	FinanceUnit* const fu = MALLOC(FinanceUnit);
	fu->Next           = NULL;
	fu->ubCode         = ubCode;
	fu->ubSecondCode   = ubSecondCode;
	fu->uiDate         = uiDate;
	fu->iAmount        = iAmount;
	fu->iBalanceToDate = iBalanceToDate;

	// Append to end of list
	FinanceUnit** i = &pFinanceListHead;
	while (*i != NULL) i = &(*i)->Next;
	*i = fu;
}


static void CreateFinanceButtons(void)
{
  giFinanceButtonImage[PREV_PAGE_BUTTON] =  LoadButtonImage( "LAPTOP/arrows.sti" ,-1,0,-1,1,-1  );
	giFinanceButton[PREV_PAGE_BUTTON] = QuickCreateButton(giFinanceButtonImage[PREV_PAGE_BUTTON], PREV_BTN_X, BTN_Y, MSYS_PRIORITY_HIGHEST - 1, BtnFinanceDisplayPrevPageCallBack);

	giFinanceButtonImage[NEXT_PAGE_BUTTON]=  UseLoadedButtonImage( giFinanceButtonImage[PREV_PAGE_BUTTON] ,-1,6,-1,7,-1 );
	giFinanceButton[NEXT_PAGE_BUTTON] = QuickCreateButton(giFinanceButtonImage[NEXT_PAGE_BUTTON], NEXT_BTN_X, BTN_Y, MSYS_PRIORITY_HIGHEST - 1, BtnFinanceDisplayNextPageCallBack);

	//button to go to the first page
  giFinanceButtonImage[FIRST_PAGE_BUTTON]=  UseLoadedButtonImage( giFinanceButtonImage[PREV_PAGE_BUTTON], -1,3,-1,4,-1  );
	giFinanceButton[FIRST_PAGE_BUTTON] = QuickCreateButton(giFinanceButtonImage[FIRST_PAGE_BUTTON], FIRST_PAGE_X, BTN_Y, MSYS_PRIORITY_HIGHEST - 1, BtnFinanceFirstLastPageCallBack);
	MSYS_SetBtnUserData(giFinanceButton[FIRST_PAGE_BUTTON], 0);

	//button to go to the last page
  giFinanceButtonImage[LAST_PAGE_BUTTON]=  UseLoadedButtonImage( giFinanceButtonImage[PREV_PAGE_BUTTON], -1,9,-1,10,-1  );
	giFinanceButton[LAST_PAGE_BUTTON] = QuickCreateButton(giFinanceButtonImage[LAST_PAGE_BUTTON], LAST_PAGE_X, BTN_Y, MSYS_PRIORITY_HIGHEST - 1, BtnFinanceFirstLastPageCallBack);
	MSYS_SetBtnUserData(giFinanceButton[LAST_PAGE_BUTTON], 1);

	SetButtonCursor(giFinanceButton[0], CURSOR_LAPTOP_SCREEN);
  SetButtonCursor(giFinanceButton[1], CURSOR_LAPTOP_SCREEN);
  SetButtonCursor(giFinanceButton[2], CURSOR_LAPTOP_SCREEN);
  SetButtonCursor(giFinanceButton[3], CURSOR_LAPTOP_SCREEN);
}


static void DestroyFinanceButtons(void)
{
	UINT32 uiCnt;

	for( uiCnt=0; uiCnt<4; uiCnt++ )
	{
		RemoveButton( giFinanceButton[ uiCnt ] );
		UnloadButtonImage( giFinanceButtonImage[ uiCnt ] );
	}
}


static void BtnFinanceDisplayPrevPageCallBack(GUI_BUTTON *btn, INT32 reason)
{

   if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	 {
		 // if greater than page zero, we can move back, decrement iCurrentPage counter
		 LoadPreviousPage( );

		 // set button state
	   SetFinanceButtonStates( );
		 fReDrawScreenFlag=TRUE;
	 }

}


static void BtnFinanceDisplayNextPageCallBack(GUI_BUTTON *btn, INT32 reason)
{
	if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		 // increment currentPage
     LoadNextPage( );

		 // set button state
	   SetFinanceButtonStates( );

		 // redraw screen
		 fReDrawScreenFlag=TRUE;
	}
}


static void BtnFinanceFirstLastPageCallBack(GUI_BUTTON *btn, INT32 reason)
{
	if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		UINT32 uiButton = MSYS_GetBtnUserData(btn);

		//if its the first page button
		if( uiButton == 0 )
		{
			iCurrentPage = 0;
			LoadInRecords( iCurrentPage );
		}

		//else its the last page button
		else
		{
			LoadInRecords( guiLastPageInRecordsList + 1 );

			iCurrentPage = guiLastPageInRecordsList + 1;
		}

		// set button state
		SetFinanceButtonStates( );

		// redraw screen
		fReDrawScreenFlag=TRUE;
	}
}


static void ProcessTransactionString(wchar_t pString[], size_t Length, const FinanceUnit* pFinance)
{
	const wchar_t* s;
	switch (pFinance->ubCode)
	{
		case ACCRUED_INTEREST:  s = pTransactionText[ACCRUED_INTEREST];  goto copy;
		case ANONYMOUS_DEPOSIT: s = pTransactionText[ANONYMOUS_DEPOSIT]; goto copy;
		case TRANSACTION_FEE:   s = pTransactionText[TRANSACTION_FEE];   goto copy;

		case HIRED_MERC: s = pMessageStrings[MSG_HIRED_MERC]; goto copy_name;

		case BOBBYR_PURCHASE: s = pTransactionText[BOBBYR_PURCHASE]; goto copy;

		case PAY_SPECK_FOR_MERC: s = pTransactionText[PAY_SPECK_FOR_MERC]; goto copy;

		case MEDICAL_DEPOSIT: s = pTransactionText[MEDICAL_DEPOSIT]; goto copy_name;

		case IMP_PROFILE: s = pTransactionText[IMP_PROFILE]; goto copy;

		case PURCHASED_INSURANCE: s = pTransactionText[PURCHASED_INSURANCE]; goto copy_name;
		case REDUCED_INSURANCE:   s = pTransactionText[REDUCED_INSURANCE];   goto copy_name;
		case EXTENDED_INSURANCE:  s = pTransactionText[EXTENDED_INSURANCE];  goto copy_name;
		case CANCELLED_INSURANCE: s = pTransactionText[CANCELLED_INSURANCE]; goto copy_name;
		case INSURANCE_PAYOUT:    s = pTransactionText[INSURANCE_PAYOUT];    goto copy_name;

		case EXTENDED_CONTRACT_BY_1_DAY:   s = pTransactionAlternateText[0]; goto copy_name;
		case EXTENDED_CONTRACT_BY_1_WEEK:  s = pTransactionAlternateText[1]; goto copy_name;
		case EXTENDED_CONTRACT_BY_2_WEEKS: s = pTransactionAlternateText[2]; goto copy_name;

		case DEPOSIT_FROM_GOLD_MINE:
		case DEPOSIT_FROM_SILVER_MINE: s = pTransactionText[16]; goto copy;

		case PURCHASED_FLOWERS: s = pTransactionText[PURCHASED_FLOWERS]; goto copy;

		case FULL_MEDICAL_REFUND:    s = pTransactionText[FULL_MEDICAL_REFUND];    goto copy_name;
		case PARTIAL_MEDICAL_REFUND: s = pTransactionText[PARTIAL_MEDICAL_REFUND]; goto copy_name;
		case NO_MEDICAL_REFUND:      s = pTransactionText[NO_MEDICAL_REFUND];      goto copy_name;

		case TRANSFER_FUNDS_TO_MERC:   s = pTransactionText[TRANSFER_FUNDS_TO_MERC];   goto copy_name;
		case TRANSFER_FUNDS_FROM_MERC: s = pTransactionText[TRANSFER_FUNDS_FROM_MERC]; goto copy_name;
		case PAYMENT_TO_NPC:           s = pTransactionText[PAYMENT_TO_NPC];           goto copy_name;

		case TRAIN_TOWN_MILITIA:
		{
			wchar_t str[128];
			UINT8 ubSectorX = SECTORX(pFinance->ubSecondCode);
			UINT8 ubSectorY = SECTORY(pFinance->ubSecondCode);
			GetSectorIDString(ubSectorX, ubSectorY, 0, str, lengthof(str), TRUE);
			swprintf(pString, Length, pTransactionText[TRAIN_TOWN_MILITIA], str);
			break;
		}

		case PURCHASED_ITEM_FROM_DEALER: s = pTransactionText[PURCHASED_ITEM_FROM_DEALER]; goto copy_name;

		case MERC_DEPOSITED_MONEY_TO_PLAYER_ACCOUNT: s = pTransactionText[MERC_DEPOSITED_MONEY_TO_PLAYER_ACCOUNT]; goto copy_name;

copy:
		wcslcpy(pString, s, Length);
		break;

copy_name:
		swprintf(pString, Length, s, gMercProfiles[pFinance->ubSecondCode].zNickname);
		break;
	}
}


static void DisplayFinancePageNumberAndDateRange(void)
{
  // setup the font stuff
	SetFont(FINANCE_TEXT_FONT);
  SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
  SetFontShadow(NO_SHADOW);

	mprintf(PAGE_NUMBER_X, PAGE_NUMBER_Y, L"%ls %d / %d", pFinanceHeaders[5], iCurrentPage + 1, guiLastPageInRecordsList + 2);

	SetFontShadow(DEFAULT_SHADOW);
}


static BOOLEAN WriteBalanceToDisk(void)
{
	// will write the current balance to disk
	const HWFILE hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_WRITE | FILE_OPEN_ALWAYS);

	// write balance to disk
	FileWrite(hFileHandle, &LaptopSaveInfo.iCurrentBalance, sizeof(INT32));

  FileClose( hFileHandle );
  return( TRUE );
}


static void GetBalanceFromDisk(void)
{
	// will grab the current blanace from disk
	// assuming file already openned
  // this procedure will open and read in data to the finance list
  HWFILE hFileHandle;

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		LaptopSaveInfo.iCurrentBalance = 0;
		return;
  }

	// get balance from disk first
  FileRead(hFileHandle, &LaptopSaveInfo.iCurrentBalance, sizeof(INT32));

  FileClose( hFileHandle );
}


static BOOLEAN AppendFinanceToEndOfFile(void)
{
  	// will write the current finance to disk
	FinanceUnit* pFinanceList = pFinanceListHead;

	const HWFILE hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS);
	if (!hFileHandle)
	{
    return FALSE;
	}

	// write finance to disk
  // now write date and amount, and code
	FileWrite(hFileHandle, &pFinanceList->ubCode,         sizeof(UINT8));
	FileWrite(hFileHandle, &pFinanceList->ubSecondCode,   sizeof(UINT8));
	FileWrite(hFileHandle, &pFinanceList->uiDate,         sizeof(UINT32));
	FileWrite(hFileHandle, &pFinanceList->iAmount,        sizeof(INT32));
	FileWrite(hFileHandle, &pFinanceList->iBalanceToDate, sizeof(INT32));

  FileClose( hFileHandle );
  return( TRUE );
}


// This function will read in the last unit in the finance list, to grab its id number
static UINT32 ReadInLastElementOfFinanceListAndReturnIdNumber(void)
{
	const HWFILE f = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!f) return 0;

  const UINT32 size = FileGetSize(f);
	FileClose(f);

	// make sure file is at least balance size
	if (size < FINANCE_HEADER_SIZE) return 0;
	return (size - FINANCE_HEADER_SIZE) / FINANCE_RECORD_SIZE;
}


// Grabs the size of the file and interprets number of pages it will take up
static void SetLastPageInRecords(void)
{
	const HWFILE f = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!f)
	{
		LaptopSaveInfo.iCurrentBalance = 0;
		return;
	}

	// make sure file is more than 0 length
	if (FileGetSize(f) == 0)
	{
		FileClose(f);
		guiLastPageInRecordsList = 1;
		return;
	}

	FileClose(f);

	guiLastPageInRecordsList = (ReadInLastElementOfFinanceListAndReturnIdNumber() - 1) / NUM_RECORDS_PER_PAGE;
}


static BOOLEAN LoadPreviousPage(void)
{

	// clear out old list of records, and load in previous page worth of records
  ClearFinanceList( );

	// load previous page
	if( ( iCurrentPage == 1 )||( iCurrentPage == 0 ) )
	{
		iCurrentPage = 0;
		return ( FALSE );
	}

	// now load in previous page's records, if we can
  if ( LoadInRecords( iCurrentPage - 1 ) )
	{
		iCurrentPage--;
		return ( TRUE );
	}
	else
	{
    LoadInRecords( iCurrentPage );
		return ( FALSE );
	}
}


static BOOLEAN LoadNextPage(void)
{

	// clear out old list of records, and load in previous page worth of records
  ClearFinanceList( );



	// now load in previous page's records, if we can
  if ( LoadInRecords( iCurrentPage + 1 ) )
	{
		iCurrentPage++;
	  return ( TRUE );
	}
	else
	{
		LoadInRecords( iCurrentPage );
	  return ( FALSE );
	}

}


// Loads in records belonging to page
static BOOLEAN LoadInRecords(const UINT32 page)
{
	if (page == 0) return FALSE; // check if bad page

	const HWFILE f = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!f) return FALSE;

	BOOLEAN      ret  = FALSE;
	const UINT32 size = FileGetSize(f);
	if (size >= FINANCE_HEADER_SIZE)
	{
		UINT32       records      = (size - FINANCE_HEADER_SIZE) / FINANCE_RECORD_SIZE;
		const UINT32 skip_records = NUM_RECORDS_PER_PAGE * (page - 1);
		if (records > skip_records)
		{
			records -= skip_records;
			FileSeek(f, FINANCE_HEADER_SIZE + FINANCE_RECORD_SIZE * skip_records, FILE_SEEK_FROM_START);

			if (records > NUM_RECORDS_PER_PAGE) records = NUM_RECORDS_PER_PAGE;
			for (; records > 0; --records)
			{
				BYTE data[FINANCE_RECORD_SIZE];
				FileRead(f, data, sizeof(data));

				UINT8  code;
				UINT8  second_code;
				UINT32 date;
				INT32  amount;
				INT32  balance_to_date;
				const BYTE* d = data;
				EXTR_U8(d, code);
				EXTR_U8(d, second_code);
				EXTR_U32(d, date);
				EXTR_I32(d, amount);
				EXTR_I32(d, balance_to_date);
				Assert(d == endof(data));

				ProcessAndEnterAFinacialRecord(code, date, amount, second_code, balance_to_date);
			}
			ret = TRUE;
		}
	}
	FileClose(f);
	return ret;
}


static void InternalSPrintMoney(wchar_t* Str, INT32 Amount)
{
	if (Amount == 0)
	{
		*Str++ = L'0';
		*Str   = L'\0';
	}
	else
	{
		if (Amount < 0)
		{
			*Str++ = L'-';
			Amount = -Amount;
		}

		UINT32 Digits = 0;
		for (INT32 Tmp = Amount; Tmp != 0; Tmp /= 10) ++Digits;
		Str += Digits + (Digits - 1) / 3;
		*Str-- = L'\0';
		Digits = 0;
		do
		{
			if (Digits != 0 && Digits % 3 == 0) *Str-- = L',';
			++Digits;
			*Str-- = L'0' + Amount % 10;
			Amount /= 10;
		}
		while (Amount != 0);
	}
}


void SPrintMoney(wchar_t* Str, INT32 Amount)
{
	*Str++ = L'$';
	InternalSPrintMoney(Str, Amount);
}


static void SPrintMoneyNoDollarOnZero(wchar_t* Str, INT32 Amount)
{
	if (Amount != 0) *Str++ = L'$';
	InternalSPrintMoney(Str, Amount);
}


// find out what today is, then go back 2 days, get balance for that day
static INT32 GetPreviousDaysBalance(void)
{
	const UINT32 date_in_minutes = GetWorldTotalMin() - 60 * 24;
	const UINT32 date_in_days    = date_in_minutes / (24 * 60);

	if (date_in_days < 2) return 0;

	const HWFILE f = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!f) return 0;
	const UINT32 size = FileGetSize(f);

	INT32 balance = 0;
  // start at the end, move back until Date / 24 * 60 on the record equals date_in_days - 2
  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	INT32 iCounter = 1;
	for (UINT32 iByteCount = FINANCE_HEADER_SIZE; iByteCount < size; iByteCount += RECORD_SIZE)
	{
		FileSeek(f, RECORD_SIZE * iCounter++, FILE_SEEK_FROM_END);

		BYTE data[RECORD_SIZE];
		FileRead(f, data, sizeof(data));

		UINT32 date;
		INT32 balance_to_date;
		const BYTE* d = data;
		EXTR_SKIP(d, 2);
		EXTR_U32(d, date);
		EXTR_SKIP(d, 4);
		EXTR_I32(d, balance_to_date);
		Assert(d == endof(data));

		// check to see if we are far enough
		if (date / (24 * 60) == date_in_days - 2)
		{
			balance = balance_to_date;
			break;
		}

		// there are no entries for the previous day
		if (date / (24 * 60) < date_in_days - 2) break;
	}

	FileClose(f);
	return balance;
}


static INT32 GetTodaysBalance(void)
{
	// find out what today is, then go back 2 days, get balance for that day
  HWFILE hFileHandle;
  UINT32 iDateInMinutes = 0;
	BOOLEAN fOkToContinue = FALSE;
	UINT32 iByteCount = 0;
  INT32 iCounter = 1;
	UINT8 ubCode;
	UINT8 ubSecondCode;
	UINT32 uiDate;
	INT32 iAmount;
	INT32 iBalanceToDate;
  BOOLEAN fGoneTooFar= FALSE;



	// what day is it?
  iDateInMinutes = GetWorldTotalMin( );

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		return 0;
  }

  // start at the end, move back until Date / 24 * 60 on the record is =  ( iDateInMinutes /  ( 24 * 60 ) ) - 2
  iByteCount+= sizeof( INT32 );

  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	while( ( iByteCount < FileGetSize( hFileHandle ) ) && ( ! fOkToContinue ) && ( ! fGoneTooFar ) )
	{
		FileSeek( hFileHandle,  RECORD_SIZE * iCounter , FILE_SEEK_FROM_END );

    // incrment byte count
    iByteCount += RECORD_SIZE;

		FileRead(hFileHandle, &ubCode,         sizeof(UINT8));
		FileRead(hFileHandle, &ubSecondCode,   sizeof(UINT8));
		FileRead(hFileHandle, &uiDate,         sizeof(UINT32));
	  FileRead(hFileHandle, &iAmount,        sizeof(INT32));
    FileRead(hFileHandle, &iBalanceToDate, sizeof(INT32));

		// check to see if we are far enough
		if( ( uiDate / ( 24 * 60 ) ) == ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			fOkToContinue = TRUE;
		}

		iCounter++;
	}


  FileClose( hFileHandle );

	// not found ?
	if( fOkToContinue == FALSE )
	{
		iBalanceToDate = 0;
	}

	// reached 3 days ago, or beginning of file
	return iBalanceToDate;
}


static INT32 GetPreviousDaysIncome(void)
{
  // will return the income from the previous day
  // which is todays starting balance - yesterdays starting balance
  HWFILE hFileHandle;
  UINT32 iDateInMinutes = 0;
	BOOLEAN fOkToContinue = FALSE;
	BOOLEAN fOkToIncrement = FALSE;
	UINT32 iByteCount = 0;
  INT32 iCounter = 1;
	UINT8 ubCode;
	UINT8 ubSecondCode;
	UINT32 uiDate;
	INT32 iAmount;
	INT32 iBalanceToDate;
  BOOLEAN fGoneTooFar = FALSE;
	INT32 iTotalPreviousIncome = 0;

	// what day is it?
  iDateInMinutes = GetWorldTotalMin( );

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		return 0;
  }

  // start at the end, move back until Date / 24 * 60 on the record is =  ( iDateInMinutes /  ( 24 * 60 ) ) - 2
  iByteCount+= sizeof( INT32 );

  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	while( ( iByteCount < FileGetSize( hFileHandle ) ) && ( ! fOkToContinue ) &&( !fGoneTooFar ) )
	{
		FileSeek( hFileHandle,  RECORD_SIZE * iCounter , FILE_SEEK_FROM_END );

    // incrment byte count
    iByteCount += RECORD_SIZE;

		FileRead(hFileHandle, &ubCode,         sizeof(UINT8));
		FileRead(hFileHandle, &ubSecondCode,   sizeof(UINT8));
		FileRead(hFileHandle, &uiDate,         sizeof(UINT32));
	  FileRead(hFileHandle, &iAmount,        sizeof(INT32));
    FileRead(hFileHandle, &iBalanceToDate, sizeof(INT32));

		// check to see if we are far enough
		if( ( uiDate / ( 24 * 60 ) )== ( iDateInMinutes / ( 24 * 60 ) ) - 2 )
		{
			fOkToContinue = TRUE;
		}

			// there are no entries for the previous day
		if(  ( uiDate / ( 24 * 60 ) ) < ( iDateInMinutes / ( 24 * 60 ) ) - 2 )
    {
			fGoneTooFar = TRUE;

		}

		if( ( uiDate / ( 24 * 60 ) ) == ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			// now ok to increment amount
			fOkToIncrement = TRUE;
		}

		if( ( fOkToIncrement ) && ( ( ubCode == DEPOSIT_FROM_GOLD_MINE ) || ( ubCode == DEPOSIT_FROM_SILVER_MINE) ) )
		{
			// increment total
			iTotalPreviousIncome += iAmount;
		}

		iCounter++;
	}


	// now run back one more day and add up the total of deposits

	FileClose( hFileHandle );
	return( iTotalPreviousIncome );
}


static INT32 GetTodaysDaysIncome(void)
{
  // will return the income from the previous day
  // which is todays starting balance - yesterdays starting balance
  HWFILE hFileHandle;
  UINT32 iDateInMinutes = 0;
	BOOLEAN fOkToContinue = FALSE;
	BOOLEAN fOkToIncrement = FALSE;
	UINT32 iByteCount = 0;
  INT32 iCounter = 1;
	UINT8 ubCode;
	UINT8 ubSecondCode;
	UINT32 uiDate;
	INT32 iAmount;
	INT32 iBalanceToDate;
  BOOLEAN fGoneTooFar = FALSE;
	INT32 iTotalIncome = 0;

	// what day is it?
  iDateInMinutes = GetWorldTotalMin( );

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		return 0;
  }

  // start at the end, move back until Date / 24 * 60 on the record is =  ( iDateInMinutes /  ( 24 * 60 ) ) - 2
  iByteCount+=sizeof( INT32 );

  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	while( ( iByteCount < FileGetSize( hFileHandle ) ) && ( ! fOkToContinue ) &&( !fGoneTooFar ) )
	{
		FileSeek( hFileHandle,  RECORD_SIZE * iCounter , FILE_SEEK_FROM_END );

    // incrment byte count
    iByteCount += RECORD_SIZE;

		FileRead(hFileHandle, &ubCode,         sizeof(UINT8));
		FileRead(hFileHandle, &ubSecondCode,   sizeof(UINT8));
		FileRead(hFileHandle, &uiDate,         sizeof(UINT32));
	  FileRead(hFileHandle, &iAmount,        sizeof(INT32));
    FileRead(hFileHandle, &iBalanceToDate, sizeof(INT32));

		// check to see if we are far enough
		if( ( uiDate / ( 24 * 60 ) ) == ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			fOkToContinue = TRUE;
		}

		if( ( uiDate / ( 24 * 60 ) ) > ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			// now ok to increment amount
			fOkToIncrement = TRUE;
		}

		if( ( fOkToIncrement ) && ( ( ubCode == DEPOSIT_FROM_GOLD_MINE ) || ( ubCode == DEPOSIT_FROM_SILVER_MINE) ) )
		{
			// increment total
			iTotalIncome += iAmount;
			fOkToIncrement = FALSE;
		}

		iCounter++;
	}

		// no entries, return nothing - no income for the day
	if( fGoneTooFar == TRUE )
	{
    FileClose( hFileHandle );
		return 0;
	}

	// now run back one more day and add up the total of deposits

	FileClose( hFileHandle );
	return( iTotalIncome );
}


static void SetFinanceButtonStates(void)
{
	// this function will look at what page we are viewing, enable and disable buttons as needed

	if( iCurrentPage == 0 )
	{
		// first page, disable left buttons
		DisableButton( 	giFinanceButton[PREV_PAGE_BUTTON] );
		DisableButton( 	giFinanceButton[FIRST_PAGE_BUTTON] );
	}
	else
	{
		// enable buttons
		EnableButton( giFinanceButton[PREV_PAGE_BUTTON] );
		EnableButton( giFinanceButton[FIRST_PAGE_BUTTON] );
	}

	if( LoadNextPage( ) )
	{
		// decrement page
    LoadPreviousPage( );


		// enable buttons
		EnableButton( giFinanceButton[ NEXT_PAGE_BUTTON ] );
		EnableButton( giFinanceButton[ LAST_PAGE_BUTTON ] );

	}
	else
	{
    DisableButton( 	giFinanceButton[ NEXT_PAGE_BUTTON ] );
    DisableButton( 	giFinanceButton[ LAST_PAGE_BUTTON ] );
	}
}


static INT32 GetTodaysOtherDeposits(void)
{
	// grab todays other deposits
  HWFILE hFileHandle;
  UINT32 iDateInMinutes = 0;
	BOOLEAN fOkToContinue = FALSE;
	BOOLEAN fOkToIncrement = FALSE;
	UINT32 iByteCount = 0;
  INT32 iCounter = 1;
	UINT8 ubCode;
	UINT8 ubSecondCode;
	UINT32 uiDate;
	INT32 iAmount;
	INT32 iBalanceToDate;
  BOOLEAN fGoneTooFar = FALSE;
	INT32 iTotalIncome = 0;

	// what day is it?
  iDateInMinutes = GetWorldTotalMin( );

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		return 0;
  }

  // start at the end, move back until Date / 24 * 60 on the record is =  ( iDateInMinutes /  ( 24 * 60 ) ) - 2
  iByteCount+= sizeof( INT32 );

  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	while( ( iByteCount < FileGetSize( hFileHandle ) ) && ( ! fOkToContinue ) &&( !fGoneTooFar ) )
	{
		FileSeek( hFileHandle,  RECORD_SIZE * iCounter , FILE_SEEK_FROM_END );

    // incrment byte count
    iByteCount += RECORD_SIZE;

		FileRead(hFileHandle, &ubCode,         sizeof(UINT8));
		FileRead(hFileHandle, &ubSecondCode,   sizeof(UINT8));
		FileRead(hFileHandle, &uiDate,         sizeof(UINT32));
	  FileRead(hFileHandle, &iAmount,        sizeof(INT32));
    FileRead(hFileHandle, &iBalanceToDate, sizeof(INT32));

		// check to see if we are far enough
		if( ( uiDate / ( 24 * 60 ) ) == ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			fOkToContinue = TRUE;
		}

		if( ( uiDate / ( 24 * 60 ) ) > ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			// now ok to increment amount
			fOkToIncrement = TRUE;
		}

		if( ( fOkToIncrement ) && ( ( ubCode != DEPOSIT_FROM_GOLD_MINE ) && ( ubCode != DEPOSIT_FROM_SILVER_MINE) ) )
		{
			if( iAmount > 0 )
			{
				// increment total
				iTotalIncome += iAmount;
				fOkToIncrement = FALSE;
			}
		}

		iCounter++;
	}

		// no entries, return nothing - no income for the day
	if( fGoneTooFar == TRUE )
	{
    FileClose( hFileHandle );
		return 0;
	}

	// now run back one more day and add up the total of deposits

	FileClose( hFileHandle );
	return( iTotalIncome );
}


static INT32 GetYesterdaysOtherDeposits(void)
{
  HWFILE hFileHandle;
  UINT32 iDateInMinutes = 0;
	BOOLEAN fOkToContinue = FALSE;
	BOOLEAN fOkToIncrement = FALSE;
	UINT32 iByteCount = 0;
  INT32 iCounter = 1;
	UINT8 ubCode;
	UINT8 ubSecondCode;
	UINT32 uiDate;
	INT32 iAmount;
	INT32 iBalanceToDate;
  BOOLEAN fGoneTooFar = FALSE;
	INT32 iTotalPreviousIncome = 0;

	// what day is it?
  iDateInMinutes = GetWorldTotalMin( );

	hFileHandle = FileOpen(FINANCES_DATA_FILE, FILE_ACCESS_READ);
	if (!hFileHandle)
	{
		return 0;
  }

  // start at the end, move back until Date / 24 * 60 on the record is =  ( iDateInMinutes /  ( 24 * 60 ) ) - 2
  iByteCount+= sizeof( INT32 );

  // loop, make sure we don't pass beginning of file, if so, we have an error, and check for condifition above
	while( ( iByteCount < FileGetSize( hFileHandle ) ) && ( ! fOkToContinue ) &&( !fGoneTooFar ) )
	{
		FileSeek( hFileHandle,  RECORD_SIZE * iCounter , FILE_SEEK_FROM_END );

    // incrment byte count
    iByteCount += RECORD_SIZE;

		FileRead(hFileHandle, &ubCode,         sizeof(UINT8));
		FileRead(hFileHandle, &ubSecondCode,   sizeof(UINT8));
		FileRead(hFileHandle, &uiDate,         sizeof(UINT32));
	  FileRead(hFileHandle, &iAmount,        sizeof(INT32));
    FileRead(hFileHandle, &iBalanceToDate, sizeof(INT32));

		// check to see if we are far enough
		if( ( uiDate / ( 24 * 60 ) )== ( iDateInMinutes / ( 24 * 60 ) ) - 2 )
		{
			fOkToContinue = TRUE;
		}

			// there are no entries for the previous day
		if(  ( uiDate / ( 24 * 60 ) ) < ( iDateInMinutes / ( 24 * 60 ) ) - 2 )
    {
			fGoneTooFar = TRUE;

		}

		if( ( uiDate / ( 24 * 60 ) ) == ( iDateInMinutes / ( 24 * 60 ) ) - 1 )
		{
			// now ok to increment amount
			fOkToIncrement = TRUE;
		}

		if( ( fOkToIncrement ) && ( ( ubCode != DEPOSIT_FROM_GOLD_MINE ) && ( ubCode != DEPOSIT_FROM_SILVER_MINE) ) )
		{
			if( iAmount > 0 )
			{
				// increment total
				iTotalPreviousIncome += iAmount;
			}
		}

		iCounter++;
	}

	FileClose( hFileHandle );
	return( iTotalPreviousIncome );
}


static INT32 GetTodaysDebits(void)
{
	// return the expenses for today

	// currentbalance - todays balance - Todays income - other deposits

	return( GetCurrentBalance( ) - GetTodaysBalance( ) - GetTodaysDaysIncome( ) - GetTodaysOtherDeposits( ) );
}


static INT32 GetYesterdaysDebits(void)
{
	// return the expenses for yesterday

	return( GetTodaysBalance( ) - GetPreviousDaysBalance( ) - GetPreviousDaysIncome( ) - GetYesterdaysOtherDeposits( ) );
}
