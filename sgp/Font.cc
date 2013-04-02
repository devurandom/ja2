#include <stdarg.h>
#include "HImage.h"
#include "Local.h"
#include "Types.h"
#include "MemMan.h"
#include "Font.h"
#include "Debug.h"
#include "TranslationTable.h"
#include "VSurface.h"
#include "VObject.h"
#include "VObject_Blitters.h"

#include <android/log.h>


typedef UINT8 GlyphIdx;


// Destination printing parameters
Font                FontDefault      = 0;
static SGPVSurface* FontDestBuffer;
static SGPRect      FontDestRegion   = { 0, 0, g_screen_width, g_screen_height };
static UINT16       FontForeground16 = 0;
static UINT16       FontBackground16 = 0;
static UINT16       FontShadow16     = DEFAULT_SHADOW;

// Temp, for saving printing parameters
static Font         SaveFontDefault      = 0;
static SGPVSurface* SaveFontDestBuffer   = NULL;
static SGPRect      SaveFontDestRegion   = { 0, 0, g_screen_width, g_screen_height };
static UINT16       SaveFontForeground16 = 0;
static UINT16       SaveFontShadow16     = 0;
static UINT16       SaveFontBackground16 = 0;

#ifdef WORKING_VSPRINTF
#else
// careful! does not check for overuns, expects 1024bytes size!!!
void my__wchar2char(const wchar_t* ws, char* s) {
    int lencount = 0;
	for (int wi=0, cj=0; wi<1024; wi++, cj++) {
	    s[cj] = (char) ws[wi];
	    lencount++;
	    if (s[cj] == 0) wi = 1024;
	    //if (lencount == (maxlen-1)) wi = 1024; // up to maxlen -1 chars
	}
}

wchar_t* my__wcsncat(wchar_t * dest, const wchar_t* src, size_t maxlen) {
    // find end of dest string
    int i=0;
    while(dest[i]!=0) i++;
    // i is now pointing to the zero char of dest ".....\0"
    // append src
    int n=0;
    while((src[n]!=0)&&(n<maxlen)) {dest[i+n] = src[n];n++;}
    dest[i+n]=0; // termination char
    return dest;
}

wchar_t* my__wcscat(wchar_t * dest, const wchar_t* src) {
    // find end of dest string
    int i=0;
    while(dest[i]!=0) i++;
    // i is now pointing to the zero char of dest ".....\0"
    // append src
    int n=0;
    while(src[n]!=0) {dest[i+n] = src[n];n++;}
    dest[i+n]=0; // termination char
    return dest;
}

size_t my__wcslen(const wchar_t * ws) {
    int count = 0;
    while (ws[count] != 0) count++;
    return count;
}

wchar_t* my__wcslcpy(wchar_t *dest, const wchar_t * src, size_t maxlen) {
    char cdest[2048];
	char csrc[2048];
	int lencount = 0;
	for (int wi=0, cj=0; wi<2048; wi++, cj++) {
	    csrc[cj] = (char) src[wi];
	    lencount++;
	    if (csrc[cj] == 0) wi = 2048;
	    if (lencount == (maxlen-1)) wi = 2048; // up to maxlen -1 chars
	}
	// csrc is now char version of src
	strlcpy(cdest, csrc, maxlen);
	// copy done
	// recreate wchar string
	for (int i=0; i<2048; i++) {
	    dest[i] = cdest[i];
	    if (dest[i]==0) i=2048;
	}
    //dest[0]='3';
    //dest[1]=0;
	return dest;
}


wchar_t* my__wcscpy(wchar_t *dest, const wchar_t * src) {
    char cdest[2048];
	char csrc[2048];
	for (int wi=0, cj=0; wi<2048; wi++, cj++) {
	    csrc[cj] = (char) src[wi];
	    if (csrc[cj] == 0) wi = 2048;
	}
	// csrc is now char version of src
	strcpy(cdest, csrc);
	// copy done
	// recreate wchar string
	for (int i=0; i<2048; i++) {
	    dest[i] = cdest[i];
	    if (dest[i]==0) i=2048;
	}
	return dest;
}

//vswprintf(str, lengthof(str), fmt, ap);
size_t my__swprintf(wchar_t * dest, size_t length, const wchar_t* fmt, ...) {
    //wchar_t string[2048];
	char cstr[2048];
	char cfmt[2048];
	for (int wi=0, cj=0; wi<2048; wi++, cj++) {
	    cfmt[cj] = (char) fmt[wi];
	    if (cfmt[cj] == 0) wi = 2048;
	}
    va_list ap;
	va_start(ap, fmt);
	//vswprintf(str, lengthof(str), fmt, ap); // does this buddy break it?! YES IT DOES
	vsprintf(cstr, cfmt, ap);
	//custom_vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	//str[0] = L's';
	//str[1] = L'h';
	//str[2] = L'i';
	//str[3] = L't';
	size_t wchar_count = 0;
	for (int i=0; i<2048; i++) {
	    dest[i] = cstr[i];
	    wchar_count++;
	    if (dest[i]==0) i=2048;
	}
	return wchar_count;
}
#endif

/* Sets both the foreground and the background colors of the current font. The
 * top byte of the parameter word is the background color, and the bottom byte
 * is the foreground. */
void SetFontColors(UINT16 usColors)
{
	UINT8 ubForeground = usColors & 0xFF;
	UINT8 ubBackground = (usColors >> 8) & 0xFF;

	SetFontForeground(ubForeground);
	SetFontBackground(ubBackground);
}


/* Sets the foreground color of the currently selected font. The parameter is
 * the index into the 8-bit palette. In 16BPP mode, the RGB values from the
 * palette are used to create the pixel color. Note that if you change fonts,
 * the selected foreground/background colors will stay at what they are
 * currently set to. */
void SetFontForeground(UINT8 ubForeground)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubForeground];
	FontForeground16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));
}


void SetFontShadow(UINT8 ubShadow)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubShadow];
	FontShadow16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));

	if (ubShadow != 0 && FontShadow16 == 0) FontShadow16 = 1;
}


/* Sets the Background color of the currently selected font. The parameter is
 * the index into the 8-bit palette. In 16BPP mode, the RGB values from the
 * palette are used to create the pixel color. If the background value is zero,
 * the background of the font will be transparent.  Note that if you change
 * fonts, the selected foreground/background colors will stay at what they are
 * currently set to. */
void SetFontBackground(UINT8 ubBackground)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubBackground];
	FontBackground16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));
}


/* Loads a font from an ETRLE file */
Font LoadFontFile(const char *filename)
{
	Font const font = AddVideoObjectFromFile(filename);
	if (!FontDefault) FontDefault = font;
	return font;
}


/* Deletes the video object of a particular font. Frees up the memory and
 * resources allocated for it. */
void UnloadFont(Font const font)
{
	Assert(font);
	DeleteVideoObject(font);
}


/* Returns the width of a given character in the font. */
static UINT32 GetWidth(HVOBJECT const hSrcVObject, GlyphIdx const ssIndex)
{
	// Get Offsets from Index into structure
	ETRLEObject const& pTrav = hSrcVObject->SubregionProperties(ssIndex);
	return pTrav.usWidth + pTrav.sOffsetX;
}


/* Returns the length of a string in pixels, depending on the font given. */
INT16 StringPixLength(wchar_t const* const string, Font const font)
{
	if (!string) return 0;

	UINT32 w = 0;
	for (wchar_t const* c = string; *c != L'\0'; ++c)
	{
		w += GetCharWidth(font, *c);
	}
	return w;
}


/* Saves the current font printing settings into temporary locations. */
void SaveFontSettings(void)
{
	SaveFontDefault      = FontDefault;
	SaveFontDestBuffer   = FontDestBuffer;
	SaveFontDestRegion   = FontDestRegion;
	SaveFontForeground16 = FontForeground16;
	SaveFontShadow16     = FontShadow16;
	SaveFontBackground16 = FontBackground16;
}


/* Restores the last saved font printing settings from the temporary lactions */
void RestoreFontSettings(void)
{
	FontDefault      = SaveFontDefault;
	FontDestBuffer   = SaveFontDestBuffer;
	FontDestRegion   = SaveFontDestRegion;
	FontForeground16 = SaveFontForeground16;
	FontShadow16     = SaveFontShadow16;
	FontBackground16 = SaveFontBackground16;
}


/* Returns the height of a given character in the font. */
static UINT32 GetHeight(HVOBJECT hSrcVObject, INT16 ssIndex)
{
	// Get Offsets from Index into structure
	ETRLEObject const& pTrav = hSrcVObject->SubregionProperties(ssIndex);
	return pTrav.usHeight + pTrav.sOffsetY;
}


/* Returns the height of the first character in a font. */
UINT16 GetFontHeight(Font const font)
{
	return GetHeight(font, 0);
}


#if defined RUSSIAN
#	define ZERO_GLYPH_CHAR L' '
#else
#	define ZERO_GLYPH_CHAR L'A'
#endif


bool IsPrintableChar(wchar_t const c)
{
	if (lengthof(TranslationTable) <= c) return false;
	return TranslationTable[c] != 0 || c == ZERO_GLYPH_CHAR;
}


/* Given a wide char, this function returns the index of the glyph. If no glyph
 * exists for the requested wide char, the glyph index of '?' is returned. */
static GlyphIdx GetGlyphIndex(wchar_t const c)
{
	if (c < lengthof(TranslationTable))
	{
		GlyphIdx const idx = TranslationTable[c];
		if (idx != 0 || c == ZERO_GLYPH_CHAR) return idx;
	}
	//DebugMsg(TOPIC_FONT_HANDLER, DBG_LEVEL_0, String("Error: Invalid character given U+%04X", c));
	//__android_log_print(ANDROID_LOG_INFO, "==TEST==", "Error: Invalid character given U+%04X Tablesize:%d", c, lengthof(TranslationTable));
	return TranslationTable[L'|']; // pipe stands for error here and stops the string
}


UINT32 GetCharWidth(HVOBJECT Font, wchar_t c)
{
	return GetWidth(Font, GetGlyphIndex(c));
}


/* Sets the current font number. */
void SetFont(Font const font)
{
	Assert(font);
	FontDefault = font;
}


void SetFontAttributes(Font const font, UINT8 const foreground, UINT8 const shadow, UINT8 const background)
{
	SetFont(font);
	SetFontForeground(foreground);
	SetFontShadow(shadow);
	SetFontBackground(background);
}


void SetFontDestBuffer(SGPVSurface* const dst, const INT32 x1, const INT32 y1, const INT32 x2, const INT32 y2)
{
	Assert(x2 > x1);
	Assert(y2 > y1);

	FontDestBuffer         = dst;
	FontDestRegion.iLeft   = x1;
	FontDestRegion.iTop    = y1;
	FontDestRegion.iRight  = x2;
	FontDestRegion.iBottom = y2;
}


void SetFontDestBuffer(SGPVSurface* const dst)
{
	SetFontDestBuffer(dst, 0, 0, dst->Width(), dst->Height());
}


void FindFontRightCoordinates(INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t* pStr, Font const font, INT16* psNewX, INT16* psNewY)
{
	// Compute the coordinates to right justify the text
	INT16 xp = sWidth - StringPixLength(pStr, font) + sLeft;
	INT16 yp = (sHeight - GetFontHeight(font)) / 2 + sTop;

	*psNewX = xp;
	*psNewY = yp;
}


void FindFontCenterCoordinates(INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t* pStr, Font const font, INT16* psNewX, INT16* psNewY)
{
	// Compute the coordinates to center the text
	INT16 xp = (sWidth - StringPixLength(pStr, font) + 1) / 2 + sLeft;
	INT16 yp = (sHeight - GetFontHeight(font)) / 2 + sTop;

	*psNewX = xp;
	*psNewY = yp;
}


void gprintf(INT32 x, INT32 const y, wchar_t const* fmt, ...)
{

#ifdef WORKING_VSPRINTF
	va_list ap;
	va_start(ap, fmt);
	wchar_t	string[512];
	vswprintf(string, lengthof(string), fmt, ap);
	va_end(ap);

#else
	wchar_t string[2048];
	char cstr[2048];
	char cfmt[2048];
	for (int wi=0, cj=0; wi<2048; wi++, cj++) {
	    cfmt[cj] = (char) fmt[wi];
	    if (cfmt[cj] == 0) wi = 2048;
	}
    va_list ap;
	va_start(ap, fmt);
	//vswprintf(str, lengthof(str), fmt, ap); // does this buddy break it?! YES IT DOES
	vsprintf(cstr, cfmt, ap);
	//custom_vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	//str[0] = L's';
	//str[1] = L'h';
	//str[2] = L'i';
	//str[3] = L't';
	for (int i=0; i<2048; i++) {
	    string[i] = cstr[i];
	    if (string[i]==0) i=2048;
	}
#endif


	SGPVSurface::Lock l(FontDestBuffer);
	UINT16* const buf   = l.Buffer<UINT16>();
	UINT32  const pitch = l.Pitch();
	Font    const font  = FontDefault;
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "gprintf called with: %s %s %s ...", string, (char*)string+1,(char*)string+2);
	for (wchar_t const* i = string; *i != L'\0'; ++i)
	{
		GlyphIdx const glyph = GetGlyphIndex(*i);
		Blt8BPPDataTo16BPPBufferTransparentClip(buf, pitch, font, x, y, glyph, &FontDestRegion);
		x += GetWidth(font, glyph);
	}
}


UINT32 MPrintChar(INT32 const x, INT32 const y, wchar_t const c)
{
	GlyphIdx const glyph = GetGlyphIndex(c);
	Font     const font  = FontDefault;
	{ SGPVSurface::Lock l(FontDestBuffer);
		Blt8BPPDataTo16BPPBufferMonoShadowClip(l.Buffer<UINT16>(), l.Pitch(), font, x, y, glyph, &FontDestRegion, FontForeground16, FontBackground16, FontShadow16);
	}
	return GetWidth(font, glyph);
}


void MPrintBuffer(UINT16* const pDestBuf, UINT32 const uiDestPitchBYTES, INT32 x, INT32 const y, wchar_t const* str)
{
	//__android_log_print(ANDROID_LOG_INFO, "==TEST==", "MPrintBuffer called with: %ls %s %s %s ...", str, (char*)str+1,(char*)str+2,(char*)str+3);
    //str = L"BROKEN"; <--- this string is displayed correctly, so somwhere else the shit is coming from
	Font const font = FontDefault;
	//char * shortchars = (char*)str;
	//str = ((wchar_t*)(shortchars-1)); // shift string

	// NOTE: sometimes a normal char string seems to work its way
	// to this function and is rendered shitty.

	// first character:
	//GlyphIdx const glyph = GetGlyphIndex(*shortchars);
	//Blt8BPPDataTo16BPPBufferMonoShadowClip(pDestBuf, uiDestPitchBYTES, font, x, y, glyph, &FontDestRegion, FontForeground16, FontBackground16, FontShadow16);
    //x += GetWidth(font, glyph);
    //shortchars += 2; // increase POINTER by 2 to skip invalid zero character

	for (; *str != L'\0'; ++str)
	//for (; *str != 0; ((char*)str)++)
	//for (; *shortchars != 0; shortchars++)
	{
		GlyphIdx const glyph = GetGlyphIndex(*str);
		//memcpy( &i, p, sizeof(int) );
		//wchar_t c;
		//memcpy(&c, str, sizeof(wchar_t));
		//GlyphIdx const glyph = GetGlyphIndex(c);
		if (glyph != L'|') {
		    Blt8BPPDataTo16BPPBufferMonoShadowClip(pDestBuf, uiDestPitchBYTES, font, x, y, glyph, &FontDestRegion, FontForeground16, FontBackground16, FontShadow16);
            x += GetWidth(font, glyph);
		} else continue; // ignore errors

	}
}


void MPrint(INT32 const x, INT32 const y, wchar_t const* const str)
{
	SGPVSurface::Lock l(FontDestBuffer);
	MPrintBuffer(l.Buffer<UINT16>(), l.Pitch(), x, y, str);
}

void custom_vswprintf(wchar_t * dest, size_t maxlen, const wchar_t* fmt, wchar_t* wstring) {
    return;
}

/* Prints to the currently selected destination buffer, at the X/Y coordinates
 * specified, using the currently selected font. Other than the X/Y coordinates,
 * the parameters are identical to printf. The resulting string may be no longer
 * than 512 word-characters. Uses monochrome font color settings */
void mprintf(INT32 const x, INT32 const y, wchar_t const* const fmt, ...)
{
    __android_log_print(ANDROID_LOG_INFO, "==TEST==", "mprintf called with %ls / %s", fmt, (char*)fmt);
	//MPrint(x,y-15,fmt); // string arrives correctly!!! this one is rendered!
	// TEST: expand fmt to 4-byte wchar_t
//	unsigned char my_buffer[2048];
//	int a = 0;
//	int b = 0;
//	//wchar_t* my_fmt = fmt;
//	while (fmt[b]!=0){
//	    my_buffer[a] = 0;
//	    my_buffer[a+1] = 0;
//	    my_buffer[a+2] = (unsigned char)((fmt[b]<<8)>>8);
//	    my_buffer[a+3] = (unsigned char)(fmt[b]>>8);
//	    a+=4;
//	    b++;
//	}
	// mybuffer should now be fmt in 4-byte wchar_t
	//fmt = (wchar_t*) my_buffer; // switch pointers

	wchar_t str[512];
#ifdef WORKING_VSPRINTF
    va_list ap;
	va_start(ap, fmt);
	vswprintf(str, lengthof(str), fmt, ap); // does this buddy break it?! YES IT DOES
	va_end(ap);
#else
	char cstr[512];
	char cfmt[512];
	for (int wi=0, cj=0; wi<512; wi++, cj++) {
        //if ((uint16_t)fmt[wi] >=256) {
        //    cfmt[cj] = 'U';
        //    cj++;
        //    cfmt[cj] = '+';
        //    cj++;
        //    cfmt[cj] = ((char)(fmt[wi]>>8));
        //    cj++;
        //}
	    cfmt[cj] = (char) fmt[wi];
	    if (cfmt[cj] == 0) wi = 512;
	}
    //str[0] = L'A';
    //str[1] = L'B';
    //str[2] = L'C';

	va_list ap;
	va_start(ap, fmt);
	//vswprintf(str, lengthof(str), fmt, ap); // does this buddy break it?! YES IT DOES
	vsprintf(cstr, cfmt, ap); // fix %ls here?
	//custom_vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	//str[0] = L's';
	//str[1] = L'h';
	//str[2] = L'i';
	//str[3] = L't';
	for (int i=0; i<512; i++) {
	    str[i] = cstr[i];
	    if (str[i]==0) i=512;
	}
#endif
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "mprintf result %X %X %X %X %X %X %X", str[0], str[1], str[2], str[3], str[4], str[5], str[6]);
	MPrint(x,y,str);
	//MPrint(x, y, L"this is string"); THIS WORKS!!!!!!

}


void mprintf_buffer(UINT16* const pDestBuf, UINT32 const uiDestPitchBYTES, INT32 const x, INT32 const y, wchar_t const* const fmt, ...)
{
#ifdef WORKING_VSPRINTF
	wchar_t str[512];
	va_list ap;
	va_start(ap, fmt);
	vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
#else
	wchar_t str[2048];
	char cstr[2048];
	char cfmt[2048];
	for (int wi=0, cj=0; wi<2048; wi++, cj++) {
	    cfmt[cj] = (char) fmt[wi];
	    if (cfmt[cj] == 0) wi = 2048;
	}
    va_list ap;
	va_start(ap, fmt);
	//vswprintf(str, lengthof(str), fmt, ap); // does this buddy break it?! YES IT DOES
	vsprintf(cstr, cfmt, ap);
	//custom_vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	//str[0] = L's';
	//str[1] = L'h';
	//str[2] = L'i';
	//str[3] = L't';
	for (int i=0; i<2048; i++) {
	    str[i] = cstr[i];
	    if (str[i]==0) i=2048;
	}
#endif

	MPrintBuffer(pDestBuf, uiDestPitchBYTES, x, y, str);
}


void InitializeFontManager(void)
{
	FontDefault    = 0;
	FontDestBuffer = BACKBUFFER;

	FontDestRegion.iLeft   = 0;
	FontDestRegion.iTop    = 0;
	FontDestRegion.iRight  = g_screen_width;
	FontDestRegion.iBottom = g_screen_height;
}
