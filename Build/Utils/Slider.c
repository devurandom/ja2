#include "Debug.h"
#include "Input.h"
#include "Line.h"
#include "Local.h"
#include "MemMan.h"
#include "MouseSystem.h"
#include "Render_Dirty.h"
#include "Slider.h"
#include "SysUtil.h"
#include "Types.h"
#include "VObject.h"
#include "VSurface.h"
#include "Video.h"
#include "WCheck.h"


#define WHEEL_MOVE_FRACTION 32

#define SLIDER_VERTICAL     0x00000001

#define DEFUALT_SLIDER_SIZE  7

#define STEEL_SLIDER_WIDTH  42
#define STEEL_SLIDER_HEIGHT 25


struct SLIDER
{
	UINT16                 usPosX;
	UINT16                 usPosY;
	UINT16                 usWidth;
	UINT16                 usHeight;
	UINT16                 usNumberOfIncrements;
	SLIDER_CHANGE_CALLBACK SliderChangeCallback;

	UINT16                 usCurrentIncrement;

	UINT16                 usBackGroundColor;

	MOUSE_REGION           ScrollAreaMouseRegion;

	UINT16                 usCurrentSliderBoxPosition;

	SGPRect                LastRect;

	UINT32                 uiFlags;

	UINT8                  ubSliderWidth;
	UINT8                  ubSliderHeight;

	SLIDER*                pNext;
	SLIDER*                pPrev;
};


static SLIDER* pSliderHead     = NULL;
static SLIDER* gpCurrentSlider = NULL;

static BOOLEAN gfSliderInited    = FALSE;
static UINT32  guiSliderBoxImage = 0;


void InitSlider(void)
{
	// load Slider Box Graphic graphic and add it
	guiSliderBoxImage = AddVideoObjectFromFile("INTERFACE/SliderBox.sti");
	CHECKV(guiSliderBoxImage != NO_VOBJECT);
	gfSliderInited = TRUE;
}


void ShutDownSlider(void)
{
	AssertMsg(gfSliderInited, "Trying to ShutDown the Slider System when it was never inited");

	//Do a cehck to see if there are still active nodes
	for (SLIDER* i = pSliderHead; i != NULL;)
	{
		SLIDER* const remove = i;
		i = i->pNext;
		RemoveSliderBar(remove);

		//Report an error
	}

	//if so report an errror
	gfSliderInited = FALSE;
	DeleteVideoObjectFromIndex(guiSliderBoxImage);
}


static void CalculateNewSliderBoxPosition(SLIDER* pSlider);
static void SelectedSliderButtonCallBack(MOUSE_REGION* r, INT32 iReason);
static void SelectedSliderMovementCallBack(MOUSE_REGION* r, INT32 reason);


SLIDER* AddSlider(UINT8 ubStyle, UINT16 usCursor, UINT16 usPosX, UINT16 usPosY, UINT16 usWidth, UINT16 usNumberOfIncrements, INT8 sPriority, SLIDER_CHANGE_CALLBACK SliderChangeCallback)
{
	AssertMsg(gfSliderInited, "Trying to Add a Slider Bar when the Slider System was never inited");

	if (ubStyle >= NUM_SLIDER_STYLES) return NULL;

	SLIDER* const s = MemAlloc(sizeof(*s));
	if (s == NULL) return NULL;
	memset(s, 0, sizeof(*s));

	// Assign the settings to the current slider
	s->usPosX               = usPosX;
	s->usPosY               = usPosY;
	s->usNumberOfIncrements = usNumberOfIncrements;
	s->SliderChangeCallback = SliderChangeCallback;
	s->usCurrentIncrement   = 0;
	s->usBackGroundColor    = Get16BPPColor(FROMRGB(255, 255, 255));

	UINT16 x = usPosX;
	UINT16 y = usPosY;
	UINT16 w;
	UINT16 h;
	switch (ubStyle)
	{
		case SLIDER_VERTICAL_STEEL:
			s->uiFlags        = SLIDER_VERTICAL;
			s->usWidth        = STEEL_SLIDER_WIDTH;
			s->usHeight       = usWidth;
			s->ubSliderWidth  = STEEL_SLIDER_WIDTH;
			s->ubSliderHeight = STEEL_SLIDER_HEIGHT;
			x -= s->usWidth / 2;
			w  = s->usWidth;
			h  = s->usHeight;
			break;

		default:
		case SLIDER_DEFAULT_STYLE:
			s->uiFlags  = 0;
			s->usWidth  = usWidth;
			s->usHeight = DEFUALT_SLIDER_SIZE;
			y -= DEFUALT_SLIDER_SIZE;
			w  = s->usWidth;
			h  = DEFUALT_SLIDER_SIZE * 2;
			break;
	}

	MOUSE_REGION* const r = &s->ScrollAreaMouseRegion;
	MSYS_DefineRegion(r, x, y, x + w, y + h, sPriority, usCursor, SelectedSliderMovementCallBack, SelectedSliderButtonCallBack);
	MSYS_SetRegionUserPtr(r, s);

	// Add the slider into the list
	if (pSliderHead == NULL)
	{
		pSliderHead = s;
		s->pNext = NULL;
	}
	else
	{
		SLIDER* i = pSliderHead;
		while (i->pNext != NULL) i = i->pNext;

		i->pNext = s;
		s->pPrev = i;
		s->pNext = NULL;
	}

	CalculateNewSliderBoxPosition(s);

	return s;
}


static void CalculateNewSliderIncrement(SLIDER* s, UINT16 usPos);
static void RenderSelectedSliderBar(SLIDER* s);


void RenderAllSliderBars(void)
{
	// set the currently selectd slider bar
	if (gfLeftButtonState && gpCurrentSlider != NULL)
	{
		SLIDER* const s = gpCurrentSlider;
		const UINT16 pos = gusMouseYPos < s->usPosY ? 0 : gusMouseYPos - s->usPosY;
		CalculateNewSliderIncrement(s, pos);
	}
	else
	{
		gpCurrentSlider = NULL;
	}

	for (SLIDER* i = pSliderHead; i != NULL; i = i->pNext)
	{
		RenderSelectedSliderBar(i);
	}
}


static void OptDisplayLine(UINT16 usStartX, UINT16 usStartY, UINT16 EndX, UINT16 EndY, INT16 iColor);
static void RenderSliderBox(SLIDER* s);


static void RenderSelectedSliderBar(SLIDER* s)
{
	if (!(s->uiFlags & SLIDER_VERTICAL))
	{
		// Display the background (the bar)
		const UINT16 x = s->usPosX;
		const UINT16 y = s->usPosY;
		const UINT16 w = s->usWidth;
		const UINT16 c = s->usBackGroundColor;
		OptDisplayLine(x + 1, y - 1, x + w - 1, y - 1, c);
		OptDisplayLine(x,     y,     x + w,     y,     c);
		OptDisplayLine(x + 1, y + 1, x + w - 1, y + 1, c);

		InvalidateRegion(x, y - 2, x + w + 1, y + 2);
	}

	RenderSliderBox(s);
}


static void RenderSliderBox(SLIDER* s)
{
	SGPRect DestRect;
	if (s->uiFlags & SLIDER_VERTICAL)
	{
		//fill out the settings for the current dest rect
		DestRect.iLeft   = s->usPosX                     - s->ubSliderWidth  / 2;
		DestRect.iTop    = s->usCurrentSliderBoxPosition - s->ubSliderHeight / 2;
		DestRect.iRight  = DestRect.iLeft                + s->ubSliderWidth;
		DestRect.iBottom = DestRect.iTop                 + s->ubSliderHeight;

		if (s->LastRect.iTop == DestRect.iTop) return;

		//Restore the old rect
		BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, s->LastRect.iLeft, s->LastRect.iTop, s->ubSliderWidth, s->ubSliderHeight);

		//invalidate the old area
		InvalidateRegion(s->LastRect.iLeft, s->LastRect.iTop, s->LastRect.iRight, s->LastRect.iBottom);

		BltVideoObjectFromIndex(FRAME_BUFFER, guiSliderBoxImage, 0, DestRect.iLeft, DestRect.iTop);

		//invalidate the area
		InvalidateRegion(DestRect.iLeft, DestRect.iTop, DestRect.iRight, DestRect.iBottom);
	}
	else
	{
		//fill out the settings for the current dest rect
		DestRect.iLeft   = s->usCurrentSliderBoxPosition;
		DestRect.iTop    = s->usPosY                     - DEFUALT_SLIDER_SIZE;
		DestRect.iRight  = DestRect.iLeft                + s->ubSliderWidth;
		DestRect.iBottom = DestRect.iTop                 + s->ubSliderHeight;

		if (s->LastRect.iLeft == DestRect.iLeft) return;

		//Restore the old rect
		BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, s->LastRect.iLeft, s->LastRect.iTop, 8, 15);

		BltVideoObjectFromIndex(FRAME_BUFFER, guiSliderBoxImage, 0, DestRect.iLeft, DestRect.iTop);

		//invalidate the area
		InvalidateRegion(DestRect.iLeft, DestRect.iTop, s->usCurrentSliderBoxPosition+9, s->usPosY + DEFUALT_SLIDER_SIZE);
	}

	//Save the new rect location
	s->LastRect = DestRect;
}


void RemoveSliderBar(SLIDER* s)
{
	if (s == pSliderHead) pSliderHead = pSliderHead->pNext;

	if (s->pNext) s->pNext->pPrev = s->pPrev;
	if (s->pPrev) s->pPrev->pNext = s->pNext;

	MSYS_RemoveRegion(&s->ScrollAreaMouseRegion);
	MemFree(s);
}


static void SelectedSliderMovementCallBack(MOUSE_REGION* r, INT32 reason)
{
	//if we already have an anchored slider bar
	if (gpCurrentSlider != NULL) return;

	if (!gfLeftButtonState) return;

	SLIDER* s;
	if (reason & MSYS_CALLBACK_REASON_LOST_MOUSE)
	{
		s = MSYS_GetRegionUserPtr(r);

		// set the currently selectd slider bar
		gpCurrentSlider = s;
	}
	else if (reason & MSYS_CALLBACK_REASON_GAIN_MOUSE ||
			reason & MSYS_CALLBACK_REASON_MOVE)
	{
		s = MSYS_GetRegionUserPtr(r);
	}
	else
	{
		return;
	}

	const UINT16 pos = s->uiFlags & SLIDER_VERTICAL ? r->RelativeYPos : r->RelativeXPos;
	CalculateNewSliderIncrement(s, pos);
}


static void SetSliderPos(SLIDER* s, INT32 pos)
{
	if (pos == s->usCurrentIncrement) return;
	s->usCurrentIncrement = pos;
	CalculateNewSliderBoxPosition(s);
	if (s->uiFlags & SLIDER_VERTICAL) pos = s->usNumberOfIncrements - pos;
	s->SliderChangeCallback(pos);
}


static void SelectedSliderButtonCallBack(MOUSE_REGION* r, INT32 iReason)
{
	//if we already have an anchored slider bar
	if (gpCurrentSlider != NULL) return;

	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_DWN ||
			iReason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT)
	{
 		SLIDER* const s = MSYS_GetRegionUserPtr(r);
		const UINT16 pos = s->uiFlags & SLIDER_VERTICAL ? r->RelativeYPos : r->RelativeXPos;
		CalculateNewSliderIncrement(s, pos);
	}
	else if (iReason & MSYS_CALLBACK_REASON_WHEEL_UP)
	{
		SLIDER* const s = MSYS_GetRegionUserPtr(r);
		const INT32 step = (s->usNumberOfIncrements + WHEEL_MOVE_FRACTION - 1) / WHEEL_MOVE_FRACTION;
		INT32 pos = s->usCurrentIncrement - step;
		pos = max(0, pos);
		SetSliderPos(s, pos);
	}
	else if (iReason & MSYS_CALLBACK_REASON_WHEEL_DOWN)
	{
		SLIDER* const s = MSYS_GetRegionUserPtr(r);
		const INT32 step = (s->usNumberOfIncrements + WHEEL_MOVE_FRACTION - 1) / WHEEL_MOVE_FRACTION;
		INT32 pos = s->usCurrentIncrement + step;
		pos = min(pos, s->usNumberOfIncrements);
		SetSliderPos(s, pos);
	}
}


static void CalculateNewSliderIncrement(SLIDER* s, UINT16 usPos)
{
	INT32 pos;
	if (s->uiFlags & SLIDER_VERTICAL)
	{
		if (usPos <= s->usHeight)
		{
			pos = s->usNumberOfIncrements * usPos / s->usHeight;
		}
		else
		{
			pos = s->usNumberOfIncrements;
		}
	}
	else
	{
		pos = s->usNumberOfIncrements * usPos /  s->usWidth;
	}

	SetSliderPos(s, pos);
}


static void OptDisplayLine(UINT16 usStartX, UINT16 usStartY, UINT16 EndX, UINT16 EndY, INT16 iColor)
{
  UINT32 uiDestPitchBYTES;
  UINT8* pDestBuf = LockVideoSurface(FRAME_BUFFER, &uiDestPitchBYTES);

	SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  // draw the line
	LineDraw(FALSE, usStartX, usStartY, EndX, EndY, iColor, pDestBuf);

	UnLockVideoSurface(FRAME_BUFFER);
}


static void CalculateNewSliderBoxPosition(SLIDER* s)
{
	UINT16 pos;
	UINT16 usMaxPos;
	if (s->uiFlags & SLIDER_VERTICAL)
	{
		//if the box is in the last position
		if (s->usCurrentIncrement >= s->usNumberOfIncrements)
		{
			pos = s->usPosY + s->usHeight;
		}
		else
		{
			pos = s->usPosY + s->usHeight * s->usCurrentIncrement / s->usNumberOfIncrements;
		}
		usMaxPos = s->usPosY + s->usHeight;
	}
	else
	{
		//if the box is in the last position
		if (s->usCurrentIncrement == s->usNumberOfIncrements)
		{
			pos = s->usPosX + s->usWidth - 8 + 1;	// - minus box width
		}
		else
		{
			pos = s->usPosX + s->usWidth * s->usCurrentIncrement / s->usNumberOfIncrements;
		}
		usMaxPos = s->usPosX + s->usWidth - 8 + 1;
	}

	//if the box is past the edge, move it back
	s->usCurrentSliderBoxPosition = min(pos, usMaxPos);
}


void SetSliderValue(SLIDER* s, UINT32 uiNewValue)
{
	if (uiNewValue >= s->usNumberOfIncrements) return;

	if (s->uiFlags & SLIDER_VERTICAL)
	{
		s->usCurrentIncrement = s->usNumberOfIncrements - uiNewValue;
	}
	else
	{
		s->usCurrentIncrement = uiNewValue;
	}

	CalculateNewSliderBoxPosition(s);
}
