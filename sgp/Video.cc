#include <stdexcept>

#include "Debug.h"
#include "Fade_Screen.h"
#include "FileMan.h"
#include "HImage.h"
#include "Input.h"
#include "Local.h"
#include "MemMan.h"
#include "RenderWorld.h"
#include "Render_Dirty.h"
#include "Timer.h"
#include "Timer_Control.h"
#include "Types.h"
#include "VObject_Blitters.h"
#include "VSurface.h"
#include "Video.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>


#define BUFFER_READY      0x00
#define BUFFER_DIRTY      0x02

#define MAX_CURSOR_WIDTH  64
#define MAX_CURSOR_HEIGHT 64

#define MAX_DIRTY_REGIONS 128

#define VIDEO_OFF         0x00
#define VIDEO_ON          0x01
#define VIDEO_SUSPENDED   0x04

#define MAX_NUM_FRAMES    25


static BOOLEAN gfVideoCapture = FALSE;
static UINT32  guiFramePeriod = 1000 / 15;
static UINT32  guiLastFrame;
static UINT16* gpFrameData[MAX_NUM_FRAMES];
static INT32   giNumFrames = 0;


// Globals for mouse cursor
static UINT16 gusMouseCursorWidth;
static UINT16 gusMouseCursorHeight;
static INT16  gsMouseCursorXOffset;
static INT16  gsMouseCursorYOffset;

static SDL_Rect MouseBackground = { 0, 0, 0, 0 };

// Refresh thread based variables
static UINT32 guiFrameBufferState;  // BUFFER_READY, BUFFER_DIRTY
static UINT32 guiVideoManagerState; // VIDEO_ON, VIDEO_OFF, VIDEO_SUSPENDED

// Dirty rectangle management variables
static SDL_Rect DirtyRegions[MAX_DIRTY_REGIONS];
static UINT32   guiDirtyRegionCount;
static BOOLEAN  gfForceFullScreenRefresh;


static SDL_Rect DirtyRegionsEx[MAX_DIRTY_REGIONS];
static UINT32   guiDirtyRegionExCount;

// Screen output stuff
static BOOLEAN gfPrintFrameBuffer;
static UINT32  guiPrintFrameBufferIndex;


static SDL_Surface* MouseCursor;
static SDL_Surface* FrameBuffer;
static SDL_Surface* ScreenBuffer;
static Uint32       g_video_flags = SDL_SWSURFACE | SDL_HWPALETTE;


void VideoSetFullScreen(const BOOLEAN enable)
{
	if (enable)
	{
		g_video_flags |= SDL_FULLSCREEN;
	}
	else
	{
		g_video_flags &= ~SDL_FULLSCREEN;
	}
}


void VideoToggleFullScreen(void)
{
	SDL_Surface* const scr = ScreenBuffer;

	// First try using SDL magic to toggle fullscreen
	if (SDL_WM_ToggleFullScreen(scr))
	{
		g_video_flags ^= SDL_FULLSCREEN;
		return;
	}

	// Fallback to manual toggling
	SDL_PixelFormat const& fmt = *scr->format;
	int             const  w   = scr->w;
	int             const  h   = scr->h;
	Uint8           const  bpp = fmt.BitsPerPixel;

	SGP::AutoObj<SDL_Surface, SDL_FreeSurface> tmp(SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bpp, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask));
	if (!tmp) return;

	SDL_BlitSurface(scr, 0, tmp, 0);

	Uint32       const new_vflags = g_video_flags ^ SDL_FULLSCREEN;
	SDL_Surface* const new_scr    = SDL_SetVideoMode(w, h, bpp, new_vflags);
	if (!new_scr) return;

	g_video_flags = new_vflags;
	ScreenBuffer  = new_scr;
	SDL_BlitSurface(tmp, 0, new_scr, 0);
	SDL_UpdateRect(new_scr, 0, 0, 0, 0);
}


static void GetRGBDistribution();


void InitializeVideoManager(void)
{
	DebugMsg(TOPIC_VIDEO, DBG_LEVEL_0, "Initializing the video manager");

	SDL_WM_SetCaption(APPLICATION_NAME, NULL);
	ClippingRect.iLeft = 0;
	ClippingRect.iTop = 0;
	ClippingRect.iRight = g_screen_width;
	ClippingRect.iBottom = g_screen_height;

	ScreenBuffer = SDL_SetVideoMode(g_screen_width, g_screen_height, PIXEL_DEPTH, g_video_flags);
	if (!ScreenBuffer) throw std::runtime_error("Failed to set up video mode");

	Uint32 Rmask = ScreenBuffer->format->Rmask;
	Uint32 Gmask = ScreenBuffer->format->Gmask;
	Uint32 Bmask = ScreenBuffer->format->Bmask;
	Uint32 Amask = ScreenBuffer->format->Amask;

	FrameBuffer = SDL_CreateRGBSurface(
		SDL_SWSURFACE, g_screen_width, g_screen_height, PIXEL_DEPTH,
		Rmask, Gmask, Bmask, Amask
	);

	MouseCursor = SDL_CreateRGBSurface(
		SDL_SWSURFACE, MAX_CURSOR_WIDTH, MAX_CURSOR_HEIGHT, PIXEL_DEPTH,
		Rmask, Gmask, Bmask, Amask
	);
	SDL_SetColorKey(MouseCursor, SDL_SRCCOLORKEY, 0);

	SDL_ShowCursor(SDL_DISABLE);

	// Initialize state variables
	guiFrameBufferState      = BUFFER_DIRTY;
	guiVideoManagerState     = VIDEO_ON;
	guiDirtyRegionCount      = 0;
	gfForceFullScreenRefresh = TRUE;
	gfPrintFrameBuffer       = FALSE;
	guiPrintFrameBufferIndex = 0;

	// This function must be called to setup RGB information
	GetRGBDistribution();
}


void ShutdownVideoManager(void)
{
	DebugMsg(TOPIC_VIDEO, DBG_LEVEL_0, "Shutting down the video manager");

	/* Toggle the state of the video manager to indicate to the refresh thread
	 * that it needs to shut itself down */

	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	guiVideoManagerState = VIDEO_OFF;

	// ATE: Release mouse cursor!
	FreeMouseCursor();
}


void SuspendVideoManager(void)
{
	guiVideoManagerState = VIDEO_SUSPENDED;
}


BOOLEAN RestoreVideoManager(void)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	// Make sure the video manager is indeed suspended before moving on

	if (guiVideoManagerState == VIDEO_SUSPENDED)
	{
		// Set the video state to VIDEO_ON

		guiFrameBufferState = BUFFER_DIRTY;
		gfForceFullScreenRefresh = TRUE;
		guiVideoManagerState = VIDEO_ON;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif
}


void InvalidateRegion(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom)
{
	if (gfForceFullScreenRefresh)
	{
		// There's no point in going on since we are forcing a full screen refresh
		return;
	}

	if (guiDirtyRegionCount < MAX_DIRTY_REGIONS)
	{
		// Well we haven't broken the MAX_DIRTY_REGIONS limit yet, so we register the new region

		// DO SOME PREMIMARY CHECKS FOR VALID RECTS
		if (iLeft < 0) iLeft = 0;
		if (iTop  < 0) iTop  = 0;

		if (iRight  > g_screen_width)  iRight  = g_screen_width;
		if (iBottom > g_screen_height) iBottom = g_screen_height;

		if (iRight - iLeft <= 0) return;
		if (iBottom - iTop <= 0) return;

		DirtyRegions[guiDirtyRegionCount].x = iLeft;
		DirtyRegions[guiDirtyRegionCount].y = iTop;
		DirtyRegions[guiDirtyRegionCount].w = iRight  - iLeft;
		DirtyRegions[guiDirtyRegionCount].h = iBottom - iTop;
		guiDirtyRegionCount++;
	}
	else
	{
		// The MAX_DIRTY_REGIONS limit has been exceeded. Therefore we arbitrarely invalidate the entire
		// screen and force a full screen refresh
		guiDirtyRegionExCount = 0;
		guiDirtyRegionCount = 0;
		gfForceFullScreenRefresh = TRUE;
	}
}


static void AddRegionEx(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom);


void InvalidateRegionEx(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom)
{
	// Check if we are spanning the rectangle - if so slit it up!
	if (iTop <= gsVIEWPORT_WINDOW_END_Y && iBottom > gsVIEWPORT_WINDOW_END_Y)
	{
		// Add new top region
		AddRegionEx(iLeft, iTop, iRight, gsVIEWPORT_WINDOW_END_Y);

		// Add new bottom region
		AddRegionEx(iLeft, gsVIEWPORT_WINDOW_END_Y, iRight, iBottom);
	}
	else
	{
		AddRegionEx(iLeft, iTop, iRight, iBottom);
	}
}


static void AddRegionEx(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom)
{
	if (guiDirtyRegionExCount < MAX_DIRTY_REGIONS)
	{
		// DO SOME PRELIMINARY CHECKS FOR VALID RECTS
		if (iLeft < 0) iLeft = 0;
		if (iTop  < 0) iTop  = 0;

		if (iRight  > g_screen_width)  iRight  = g_screen_width;
		if (iBottom > g_screen_height) iBottom = g_screen_height;

		if (iRight - iLeft <= 0) return;
		if (iBottom - iTop <= 0) return;

		DirtyRegionsEx[guiDirtyRegionExCount].x = iLeft;
		DirtyRegionsEx[guiDirtyRegionExCount].y = iTop;
		DirtyRegionsEx[guiDirtyRegionExCount].w = iRight  - iLeft;
		DirtyRegionsEx[guiDirtyRegionExCount].h = iBottom - iTop;
		guiDirtyRegionExCount++;
	}
	else
	{
		guiDirtyRegionExCount = 0;
		guiDirtyRegionCount = 0;
		gfForceFullScreenRefresh = TRUE;
	}
}


void InvalidateScreen(void)
{
	// W A R N I N G ---- W A R N I N G ---- W A R N I N G ---- W A R N I N G ---- W A R N I N G ----
	//
	// This function is intended to be called by a thread which has already locked the
	// FRAME_BUFFER_MUTEX mutual exclusion section. Anything else will cause the application to
	// yack

	guiDirtyRegionCount = 0;
	guiDirtyRegionExCount = 0;
	gfForceFullScreenRefresh = TRUE;
	guiFrameBufferState = BUFFER_DIRTY;
}


//#define SCROLL_TEST

static void ScrollJA2Background(INT16 sScrollXIncrement, INT16 sScrollYIncrement)
{
	SDL_Surface* Frame  = FrameBuffer;
	SDL_Surface* Source = ScreenBuffer; // Primary
	SDL_Surface* Dest   = ScreenBuffer; // Back
	SDL_Rect     SrcRect;
	SDL_Rect     DstRect;
	SDL_Rect     StripRegions[2];
	UINT16       NumStrips = 0;

	const UINT16 usWidth  = g_screen_width;
	const UINT16 usHeight = gsVIEWPORT_WINDOW_END_Y - gsVIEWPORT_WINDOW_START_Y;

	if (sScrollXIncrement < 0)
	{
		SrcRect.x = 0;
		SrcRect.w = usWidth + sScrollXIncrement;
		DstRect.x = -sScrollXIncrement;
		StripRegions[0].x = gsVIEWPORT_START_X;
		StripRegions[0].y = gsVIEWPORT_WINDOW_START_Y;
		StripRegions[0].w = -sScrollXIncrement;
		StripRegions[0].h = usHeight;
		++NumStrips;
	}
	else if (sScrollXIncrement > 0)
	{
		SrcRect.x = sScrollXIncrement;
		SrcRect.w = usWidth - sScrollXIncrement;
		DstRect.x = 0;
		StripRegions[0].x = gsVIEWPORT_END_X - sScrollXIncrement;
		StripRegions[0].y = gsVIEWPORT_WINDOW_START_Y;
		StripRegions[0].w = sScrollXIncrement;
		StripRegions[0].h = usHeight;
		++NumStrips;
	}
	else
	{
		SrcRect.x = 0;
		SrcRect.w = usWidth;
		DstRect.x = 0;
	}

	if (sScrollYIncrement < 0)
	{
		SrcRect.y = gsVIEWPORT_WINDOW_START_Y;
		SrcRect.h = usHeight + sScrollYIncrement;
		DstRect.y = gsVIEWPORT_WINDOW_START_Y - sScrollYIncrement;
		StripRegions[NumStrips].x = DstRect.x;
		StripRegions[NumStrips].y = gsVIEWPORT_WINDOW_START_Y;
		StripRegions[NumStrips].w = SrcRect.w;
		StripRegions[NumStrips].h = -sScrollYIncrement;
		++NumStrips;
	}
	else if (sScrollYIncrement > 0)
	{
		SrcRect.y = gsVIEWPORT_WINDOW_START_Y + sScrollYIncrement;
		SrcRect.h = usHeight - sScrollYIncrement;
		DstRect.y = gsVIEWPORT_WINDOW_START_Y;
		StripRegions[NumStrips].x = DstRect.x;
		StripRegions[NumStrips].y = gsVIEWPORT_WINDOW_END_Y - sScrollYIncrement;
		StripRegions[NumStrips].w = SrcRect.w;
		StripRegions[NumStrips].h = sScrollYIncrement;
		++NumStrips;
	}
	else
	{
		SrcRect.y = gsVIEWPORT_WINDOW_START_Y;
		SrcRect.h = usHeight;
		DstRect.y = gsVIEWPORT_WINDOW_START_Y;
	}

	SDL_BlitSurface(Source, &SrcRect, Dest, &DstRect);
#if defined __GNUC__ && defined i386
		__asm__ __volatile__("cld"); // XXX HACK000D
#endif

#ifdef SCROLL_TEST
	SDL_FillRect(Dest, NULL, 0);
#endif

	for (UINT i = 0; i < NumStrips; i++)
	{
		UINT x = StripRegions[i].x;
		UINT y = StripRegions[i].y;
		UINT w = StripRegions[i].w;
		UINT h = StripRegions[i].h;
		for (UINT j = y; j < y + h; ++j)
		{
			memset(gpZBuffer + j * g_screen_width + x, 0, w * sizeof(*gpZBuffer));
		}

		RenderStaticWorldRect(x, y, x + w, y + h, TRUE);
		SDL_BlitSurface(Frame, &StripRegions[i], Dest, &StripRegions[i]);
	}

	// RESTORE SHIFTED
	RestoreShiftedVideoOverlays(sScrollXIncrement, sScrollYIncrement);

	// SAVE NEW
	SaveVideoOverlaysArea(BACKBUFFER);

	// BLIT NEW
	ExecuteVideoOverlaysToAlternateBuffer(BACKBUFFER);

	SDL_UpdateRect
	(
		Dest,
		gsVIEWPORT_START_X,
		gsVIEWPORT_WINDOW_START_Y,
		gsVIEWPORT_END_X - gsVIEWPORT_START_X,
		gsVIEWPORT_WINDOW_END_Y - gsVIEWPORT_WINDOW_START_Y
	);
}


static void WriteTGAHeader(FILE* const f)
{
	/*
	 *  0 byte ID length
	 *  1 byte colour map type
	 *  2 byte targa type
	 *  3 word colour map origin
	 *  5 word colour map length
	 *  7 byte colour map entry size
	 *  8 word origin x
	 * 10 word origin y
	 * 12 word image width
	 * 14 word image height
	 * 16 byte bits per pixel
	 * 17 byte image descriptor
	 */
	static const BYTE data[] =
	{
		0,
		0,
		2,
		0, 0,
		0, 0,
		0,
		0, 0,
		0, 0,
		g_screen_width  % 256, g_screen_width  / 256,
		g_screen_height % 256, g_screen_height / 256,
		PIXEL_DEPTH,
		0
	};
	fwrite(data, sizeof(data), 1, f);
}


/* Create a file for a screenshot, which is guaranteed not to exist yet. */
static FILE* CreateScreenshotFile(void)
{
	const char* const exec_dir = GetExecutableDirectory();
	do
	{
		char filename[2048];
		sprintf(filename, "%s/SCREEN%03d.TGA", exec_dir, guiPrintFrameBufferIndex++);
#ifndef _WIN32
#	define O_BINARY 0
#endif
		int const fd = open(filename, O_WRONLY | O_CREAT | O_EXCL | O_BINARY, 0644);
		if (fd >= 0)
		{
			FILE* const f = fdopen(fd, "wb");
			if (f == NULL) close(fd);
			return f;
		}
	}
	while (errno == EEXIST);
	return NULL;
}


static void TakeScreenshot()
{
	FILE* const f = CreateScreenshotFile();
	if (!f) return;

	WriteTGAHeader(f);

	// If not 5/5/5, create buffer
	UINT16* buf = 0;
	if (gusRedMask != 0x7C00 || gusGreenMask != 0x03E0 || gusBlueMask != 0x001F)
	{
		buf = MALLOCN(UINT16, g_screen_width);
	}

	UINT16 const* src = static_cast<UINT16 const*>(ScreenBuffer->pixels);
	for (INT32 y = g_screen_height - 1; y >= 0; --y)
	{
		if (buf)
		{ // ATE: Fix this such that it converts pixel format to 5/5/5
			memcpy(buf, src + y * g_screen_width, g_screen_width * sizeof(*buf));
			ConvertRGBDistribution565To555(buf, g_screen_width);
			fwrite(buf, sizeof(*buf), g_screen_width, f);
		}
		else
		{
			fwrite(src + y * g_screen_width, g_screen_width * 2, 1, f);
		}
	}

	if (buf) MemFree(buf);

	fclose(f);
}


static void SnapshotSmall(void);


void RefreshScreen(void)
{
	if (guiVideoManagerState != VIDEO_ON) return;

	SDL_BlitSurface(FrameBuffer, &MouseBackground, ScreenBuffer, &MouseBackground);

	const BOOLEAN scrolling = (gsScrollXIncrement != 0 || gsScrollYIncrement != 0);

	if (guiFrameBufferState == BUFFER_DIRTY)
	{
		if (gfFadeInitialized && gfFadeInVideo)
		{
			gFadeFunction();
		}
		else
		{
			if (gfForceFullScreenRefresh)
			{
				SDL_BlitSurface(FrameBuffer, NULL, ScreenBuffer, NULL);
			}
			else
			{
				for (UINT32 i = 0; i < guiDirtyRegionCount; i++)
				{
					SDL_BlitSurface(FrameBuffer, &DirtyRegions[i], ScreenBuffer, &DirtyRegions[i]);
				}

				for (UINT32 i = 0; i < guiDirtyRegionExCount; i++)
				{
					SDL_Rect* r = &DirtyRegionsEx[i];
					if (scrolling)
					{
						// Check if we are completely out of bounds
						if (r->y <= gsVIEWPORT_WINDOW_END_Y && r->y + r->h <= gsVIEWPORT_WINDOW_END_Y)
						{
							continue;
						}
					}
					SDL_BlitSurface(FrameBuffer, r, ScreenBuffer, r);
				}
			}
		}
		if (scrolling)
		{
			ScrollJA2Background(gsScrollXIncrement, gsScrollYIncrement);
			gsScrollXIncrement = 0;
			gsScrollYIncrement = 0;
		}
		gfIgnoreScrollDueToCenterAdjust = FALSE;
		guiFrameBufferState = BUFFER_READY;
	}

	if (gfVideoCapture)
	{
		UINT32 uiTime = GetClock();
		if (uiTime < guiLastFrame || uiTime > guiLastFrame + guiFramePeriod)
		{
			SnapshotSmall();
			guiLastFrame = uiTime;
		}
	}

	if (gfPrintFrameBuffer)
	{
		TakeScreenshot();
		gfPrintFrameBuffer = FALSE;
	}

	SGPPoint MousePos;
	GetMousePos(&MousePos);
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = gusMouseCursorWidth;
	src.h = gusMouseCursorHeight;
	SDL_Rect dst;
	dst.x = MousePos.iX - gsMouseCursorXOffset;
	dst.y = MousePos.iY - gsMouseCursorYOffset;
	SDL_BlitSurface(MouseCursor, &src, ScreenBuffer, &dst);
	SDL_UpdateRects(ScreenBuffer, 1, &dst);
	SDL_UpdateRects(ScreenBuffer, 1, &MouseBackground);
	MouseBackground = dst;

	if (gfForceFullScreenRefresh)
	{
		SDL_UpdateRect(ScreenBuffer, 0, 0, 0, 0);
	}
	else
	{
		SDL_UpdateRects(ScreenBuffer, guiDirtyRegionCount, DirtyRegions);

		for (UINT32 i = 0; i < guiDirtyRegionExCount; i++)
		{
			SDL_Rect* r = &DirtyRegionsEx[i];
			if (scrolling)
			{
				if (r->y <= gsVIEWPORT_WINDOW_END_Y && r->y + r->h <= gsVIEWPORT_WINDOW_END_Y)
				{
					continue;
				}
			}
			SDL_UpdateRects(ScreenBuffer, 1, r);
		}
	}

	gfForceFullScreenRefresh = FALSE;
	guiDirtyRegionCount = 0;
	guiDirtyRegionExCount = 0;
}


SDL_Surface* GetBackBufferObject(void)
{
	if (!ScreenBuffer) throw std::logic_error("Tried to access screen surface before its creation");
	return ScreenBuffer;
}


SDL_Surface* GetFrameBufferObject(void)
{
	if (!FrameBuffer) throw std::logic_error("Tried to access frame surface before its creation");
	return FrameBuffer;
}


SDL_Surface* GetMouseBufferObject(void)
{
	if (!MouseCursor) throw std::logic_error("Tried to access mouse surface before its creation");
	return MouseCursor;
}


static void GetRGBDistribution()
{
	SDL_PixelFormat const& f = *ScreenBuffer->format;
	UINT32          const  r = f.Rmask;
	UINT32          const  g = f.Gmask;
	UINT32          const  b = f.Bmask;

	/* Mask the highest bit of each component. This is used for alpha blending. */
	guiTranslucentMask = r & r >> 1 | g & g >> 1 | b & b >> 1;

	gusRedMask   = r;
	gusGreenMask = g;
	gusBlueMask  = b;

	gusRedShift   = f.Rshift - f.Rloss;
	gusGreenShift = f.Gshift - f.Gloss;
	gusBlueShift  = f.Bshift - f.Bloss;
}


void GetPrimaryRGBDistributionMasks(UINT32* const  RedBitMask, UINT32* const GreenBitMask, UINT32* const BlueBitMask)
{
	*RedBitMask   = gusRedMask;
	*GreenBitMask = gusGreenMask;
	*BlueBitMask  = gusBlueMask;
}


void SetMouseCursorProperties(INT16 sOffsetX, INT16 sOffsetY, UINT16 usCursorHeight, UINT16 usCursorWidth)
{
	gsMouseCursorXOffset = sOffsetX;
	gsMouseCursorYOffset = sOffsetY;
	gusMouseCursorWidth  = usCursorWidth;
	gusMouseCursorHeight = usCursorHeight;
}


void EndFrameBufferRender(void)
{
	guiFrameBufferState = BUFFER_DIRTY;
}


void PrintScreen(void)
{
	gfPrintFrameBuffer = TRUE;
}


/*******************************************************************************
 * SnapshotSmall
 *
 * Grabs a screen from the primary surface, and stuffs it into a 16-bit
 * (RGB 5,5,5), uncompressed Targa file. Each time the routine is called, it
 * increments the file number by one. The files are create in the current
 * directory, usually the EXE directory. This routine produces 1/4 sized images.
 *
 ******************************************************************************/


static void RefreshMovieCache(void);


static void SnapshotSmall(void)
{
	// Get the write pointer
	const UINT16* pVideo = (UINT16*)ScreenBuffer->pixels;

	UINT16* pDest = gpFrameData[giNumFrames];

	for (INT32 iCountY = g_screen_height - 1; iCountY >= 0; iCountY--)
	{
		for (INT32 iCountX = 0; iCountX < g_screen_width; iCountX++)
		{
			pDest[iCountY * g_screen_width + iCountX] = pVideo[iCountY * g_screen_width + iCountX];
		}
	}

	giNumFrames++;

	if (giNumFrames == MAX_NUM_FRAMES) RefreshMovieCache();
}


void VideoCaptureToggle(void)
{
#ifdef JA2TESTVERSION
	gfVideoCapture = !gfVideoCapture;
	if (gfVideoCapture)
	{
		for (INT32 cnt = 0; cnt < MAX_NUM_FRAMES; cnt++)
		{
			gpFrameData[cnt] = MALLOCN(UINT16, g_screen_width * g_screen_height);
		}
		guiLastFrame = GetClock();
	}
	else
	{
		RefreshMovieCache();

		for (INT32 cnt = 0; cnt < MAX_NUM_FRAMES; cnt++)
		{
			if (gpFrameData[cnt] != NULL) MemFree(gpFrameData[cnt]);
		}
	}
	giNumFrames = 0;
#endif
}


static void RefreshMovieCache(void)
{
	static UINT32 uiPicNum = 0;

	PauseTime(TRUE);

	const char* ExecDir = GetExecutableDirectory();

	for (INT32 cnt = 0; cnt < giNumFrames; cnt++)
	{
		CHAR8 cFilename[2048];
		sprintf(cFilename, "%s/JA%5.5d.TGA", ExecDir, uiPicNum++);

		FILE* disk = fopen(cFilename, "wb");
		if (disk == NULL) return;

		WriteTGAHeader(disk);

		UINT16* pDest = gpFrameData[cnt];

		for (INT32 iCountY = g_screen_height - 1; iCountY >= 0; iCountY -= 1)
		{
			for (INT32 iCountX = 0; iCountX < g_screen_width; iCountX ++)
			{
				fwrite(pDest + iCountY * g_screen_width + iCountX, sizeof(UINT16), 1, disk);
			}
		}

		fclose(disk);
	}

	PauseTime(FALSE);

	giNumFrames = 0;
}
