//------------------------------------------------------------------------------
// Cinematics Module
//
//
//	Stolen from Nemesis by Derek Beland.
//	Originally by Derek Beland and Bret Rowden.
//
//------------------------------------------------------------------------------

#include "Cinematics.h"
#include "Debug.h"
#include "FileMan.h"
#include "Intro.h"
#include "Local.h"
//#include "Smack.h" // XXX
#include "Smack_Stub.h" // XXX
#include "SoundMan.h"
#include "Types.h"
#include "VSurface.h"
#include "Video.h"


struct SMKFLIC
{
	HWFILE    hFileHandle;
	Smack*    SmackHandle;
	SmackBuf* SmackBuffer;
	UINT32    uiFlags;
	UINT32    uiLeft;
	UINT32    uiTop;
};


// SMKFLIC uiFlags
#define SMK_FLIC_OPEN      0x00000001 // Flic is open
#define SMK_FLIC_PLAYING   0x00000002 // Flic is playing
#define SMK_FLIC_AUTOCLOSE 0x00000008 // Close when done


static SMKFLIC SmkList[4];
static UINT32  guiSmackPixelFormat = SMACKBUFFER565;


BOOLEAN SmkPollFlics(void)
{
	BOOLEAN fFlicStatus = FALSE;
	for (SMKFLIC* i = SmkList; i != endof(SmkList); ++i)
	{
		if (!(i->uiFlags & SMK_FLIC_PLAYING)) continue;
		fFlicStatus = TRUE;

		Smack* const smk = i->SmackHandle;
		if (SmackWait(smk)) continue;

		UINT32        Pitch;
		UINT16* const Dest = (UINT16*)LockVideoSurface(FRAME_BUFFER, &Pitch);
		SmackToBuffer(smk, i->uiLeft, i->uiTop, Pitch, smk->Height, Dest, guiSmackPixelFormat);
		SmackDoFrame(smk);
		UnLockVideoSurface(FRAME_BUFFER);

		// Check to see if the flic is done the last frame
		if (smk->FrameNum == smk->Frames - 1)
		{
			if (i->uiFlags & SMK_FLIC_AUTOCLOSE) SmkCloseFlic(i);
		}
		else
		{
			SmackNextFrame(smk);
		}
	}

	return fFlicStatus;
}


void SmkInitialize(void)
{
	// Wipe the flic list clean
	memset(SmkList, 0, sizeof(SmkList));

	// Use MMX acceleration, if available
	SmackUseMMX(1);
}


void SmkShutdown(void)
{
	// Close and deallocate any open flics
	for (SMKFLIC* i = SmkList; i != endof(SmkList); ++i)
	{
		if (i->uiFlags & SMK_FLIC_OPEN) SmkCloseFlic(i);
	}
}


static SMKFLIC* SmkOpenFlic(const char* filename);


SMKFLIC* SmkPlayFlic(const char* const filename, const UINT32 left, const UINT32 top, const BOOLEAN auto_close)
{
	SMKFLIC* const sf = SmkOpenFlic(filename);
	if (sf == NULL) return NULL;

	// Set the blitting position on the screen
	sf->uiLeft = left;
	sf->uiTop  = top;

	// We're now playing, flag the flic for the poller to update
	sf->uiFlags |= SMK_FLIC_PLAYING;
	if (auto_close) sf->uiFlags |= SMK_FLIC_AUTOCLOSE;

	return sf;
}


static SMKFLIC* SmkGetFreeFlic(void);
static void SmkSetupVideo(void);


static SMKFLIC* SmkOpenFlic(const char* const filename)
{
	SMKFLIC* const sf = SmkGetFreeFlic();
	if (!sf)
	{
		FastDebugMsg("SMK ERROR: Out of flic slots, cannot open another");
		return NULL;
	}

	AutoSGPFile file(FileOpen(filename, FILE_ACCESS_READ));
	if (!file)
	{
		FastDebugMsg("SMK ERROR: Can't open the SMK file");
		return NULL;
	}

	FILE* const f = GetRealFileHandleFromFileManFileHandle(file);

	// Allocate a Smacker buffer for video decompression
	sf->SmackBuffer = SmackBufferOpen(SMACKAUTOBLIT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	if (sf->SmackBuffer == NULL)
	{
		FastDebugMsg("SMK ERROR: Can't allocate a Smacker decompression buffer");
		return NULL;
	}

	sf->SmackHandle = SmackOpen((char*)f, SMACKFILEHANDLE | SMACKTRACKS, SMACKAUTOEXTRA);
	if (!sf->SmackHandle)
	{
		FastDebugMsg("SMK ERROR: Smacker won't open the SMK file");
		return NULL;
	}

	// Make sure we have a video surface
	SmkSetupVideo();

	sf->hFileHandle  = file.Release();
	sf->uiFlags     |= SMK_FLIC_OPEN;
	return sf;
}


void SmkCloseFlic(SMKFLIC* const sf)
{
	FileClose(sf->hFileHandle);
	SmackBufferClose(sf->SmackBuffer);
	SmackClose(sf->SmackHandle);
	memset(sf, 0, sizeof(*sf));
}


static SMKFLIC* SmkGetFreeFlic(void)
{
	for (SMKFLIC* i = SmkList; i != endof(SmkList); ++i)
	{
		if (!(i->uiFlags & SMK_FLIC_OPEN)) return i;
	}
	return NULL;
}


static void SmkSetupVideo(void)
{
	UINT32 red;
	UINT32 green;
	UINT32 blue;
	GetPrimaryRGBDistributionMasks(&red, &green, &blue);
	if (red == 0xf800 && green == 0x07e0 && blue == 0x001f)
	{
		guiSmackPixelFormat = SMACKBUFFER565;
	}
	else
	{
		guiSmackPixelFormat = SMACKBUFFER555;
	}
}
