#include "SGPStrings.h"

#if defined(__linux__) || defined(_WIN32)

#include <sys/types.h>

#include <android/log.h>



/*	$OpenBSD: wcslcpy.c,v 1.4 2006/05/05 15:27:38 millert Exp $	*/
/*	$NetBSD: wcslcpy.c,v 1.2 2001/01/03 14:33:02 lukem Exp $	*/
/*	from OpenBSD: strlcpy.c,v 1.4 1999/05/01 18:56:41 millert Exp 	*/

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns wcslen(src); if retval >= siz, truncation occurred.
 */
size_t
wcslcpy(wchar_t *dst, const wchar_t *src, size_t siz)
{
    __android_log_print(ANDROID_LOG_INFO, "==TEST==", "wcslcpy called with widestring: %ls", src);
    __android_log_print(ANDROID_LOG_INFO, "==TEST==", "wcslcpy called with charstring: %s", (char*)src);
	wchar_t *d = dst;
	const wchar_t *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == L'\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = L'\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

    __android_log_print(ANDROID_LOG_INFO, "==TEST==", "wcslcpy result: widestring %ls", dst);
    __android_log_print(ANDROID_LOG_INFO, "==TEST==", "wcslcpy result: charstring %ls", (char*)dst);
	return(s - src - 1);	/* count does not include NUL */
}

/*	$OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $	*/

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
my_strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

#endif
#ifdef _WIN32

//------------------------------------- TORVALDS BLOCK
//------------------------------------- TORVALDS BLOCK
//------------------------------------- TORVALDS BLOCK

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */


#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long long) n) % (unsigned) base; \
n = ((unsigned long long) n) / (unsigned) base; \
__res; })

int iswdigit(const wchar_t wc) {
    if ((wc >= 48) || (wc <= 57)) return 1;
    return 0;
}


static int skip_atoi(const wchar_t **s)
{
	int i=0;

	while (iswdigit(**s))
		i = i*10 + *((*s)++) - L'0';
	return i;
}


static wchar_t *
number (wchar_t *str, long long num, int base, int size, int precision,
	int type)
{
   wchar_t c,sign,tmp[66];
   const wchar_t *digits = L"0123456789abcdefghijklmnopqrstuvwxyz";
   int i;

   if (type & LARGE)
     digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   if (type & LEFT)
     type &= ~ZEROPAD;
   if (base < 2 || base > 36)
     return 0;

   c = (type & ZEROPAD) ? L'0' : L' ';
   sign = 0;

   if (type & SIGN)
     {
	if (num < 0)
	  {
	     sign = L'-';
	     num = -num;
	     size--;
	  }
	else if (type & PLUS)
	  {
	     sign = L'+';
	     size--;
	  }
	else if (type & SPACE)
	  {
	     sign = L' ';
	     size--;
	  }
     }

   if (type & SPECIAL)
     {
	if (base == 16)
	  size -= 2;
	else if (base == 8)
	  size--;
     }

   i = 0;
   if (num == 0)
     tmp[i++]='0';
   else while (num != 0)
     tmp[i++] = digits[do_div(num,base)];
   if (i > precision)
     precision = i;
   size -= precision;
   if (!(type&(ZEROPAD+LEFT)))
     while(size-->0)
       *str++ = L' ';
   if (sign)
     *str++ = sign;
   if (type & SPECIAL)
     {
	if (base==8)
	  {
	     *str++ = L'0';
	  }
	else if (base==16)
	  {
	     *str++ = L'0';
	     *str++ = digits[33];
	  }
     }
   if (!(type & LEFT))
     while (size-- > 0)
       *str++ = c;
   while (i < precision--)
     *str++ = '0';
   while (i-- > 0)
     *str++ = tmp[i];
   while (size-- > 0)
     *str++ = L' ';
   return str;
}


int _vsnwprintf(wchar_t *buf, size_t cnt, const wchar_t *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	wchar_t * str;
	const char *s;
	const wchar_t *sw;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', 'L', 'w' or 'I' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != L'%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case L'-': flags |= LEFT; goto repeat;
				case L'+': flags |= PLUS; goto repeat;
				case L' ': flags |= SPACE; goto repeat;
				case L'#': flags |= SPECIAL; goto repeat;
				case L'0': flags |= ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (iswdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == L'*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == L'.') {
			++fmt;
			if (iswdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == L'*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'w') {
			qualifier = *fmt;
			++fmt;
		} else if (*fmt == 'I' && *(fmt+1) == '6' && *(fmt+2) == '4') {
			qualifier = *fmt;
			fmt += 3;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case L'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = L' ';
			if (qualifier == 'h')
				*str++ = (wchar_t) va_arg(args, int);
			else
				*str++ = (wchar_t) va_arg(args, int);
			while (--field_width > 0)
				*str++ = L' ';
			continue;

		case L'C':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = L' ';
			if (qualifier == 'l' || qualifier == 'w')
				*str++ = (wchar_t) va_arg(args, int);
			else
				*str++ = (wchar_t) va_arg(args, int);
			while (--field_width > 0)
				*str++ = L' ';
			continue;

		case L's':
			if (qualifier == 'h') {
				/* print ascii string */
				s = va_arg(args, char *);
				if (s == NULL)
					s = "<NULL>";

				len = strlen (s);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = (wchar_t)(*s++);
				while (len < field_width--)
					*str++ = L' ';
			} else {
				/* print unicode string */
				sw = va_arg(args, wchar_t *);
				if (sw == NULL)
					sw = L"<NULL>";

				len = wcslen (sw);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = *sw++;
				while (len < field_width--)
					*str++ = L' ';
			}
			continue;

		case L'S':
			if (qualifier == 'l' || qualifier == 'w') {
				/* print unicode string */
				sw = va_arg(args, wchar_t *);
				if (sw == NULL)
					sw = L"<NULL>";

				len = wcslen (sw);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = *sw++;
				while (len < field_width--)
					*str++ = L' ';
			} else {
				/* print ascii string */
				s = va_arg(args, char *);
				if (s == NULL)
					s = "<NULL>";

				len = strlen (s);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = (wchar_t)(*s++);
				while (len < field_width--)
					*str++ = L' ';
			}
			continue;

//		case L'Z':
//			if (qualifier == 'h') {
//				/* print counted ascii string */
//				PANSI_STRING pus = va_arg(args, PANSI_STRING);
//				if ((pus == NULL) || (pus->Buffer == NULL)) {
//					sw = L"<NULL>";
//					while ((*sw) != 0)
//						*str++ = *sw++;
//				} else {
//					for (i = 0; pus->Buffer[i] && i < pus->Length; i++)
//						*str++ = (wchar_t)(pus->Buffer[i]);
//				}
//			} else {
//				/* print counted unicode string */
//				PUNICODE_STRING pus = va_arg(args, PUNICODE_STRING);
//				if ((pus == NULL) || (pus->Buffer == NULL)) {
//					sw = L"<NULL>";
//					while ((*sw) != 0)
//						*str++ = *sw++;
//				} else {
//					for (i = 0; pus->Buffer[i] && i < pus->Length / sizeof(WCHAR); i++)
//						*str++ = pus->Buffer[i];
//				}
//			}
//			continue;

		case L'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;

		case L'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		/* integer number formats - set up the flags and "break" */
		case L'o':
			base = 8;
			break;

		case L'b':
			base = 2;
			break;

		case L'X':
			flags |= LARGE;
		case L'x':
			base = 16;
			break;

		case L'd':
		case L'i':
			flags |= SIGN;
		case L'u':
			break;

		default:
			if (*fmt != L'%')
				*str++ = L'%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}

		if (qualifier == 'I')
			num = va_arg(args, unsigned long long);
		else if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			if (flags & SIGN)
				num = va_arg(args, int);
			else
				num = va_arg(args, unsigned int);
		}
		else {
			if (flags & SIGN)
				num = va_arg(args, int);
			else
				num = va_arg(args, unsigned int);
		}
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = L'\0';
	return str-buf;
}


int swprintf(wchar_t *buf, const wchar_t *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=_vsnwprintf(buf,INT_MAX,fmt,args);
	va_end(args);
	return i;
}


int _snwprintf(wchar_t *buf, size_t cnt, const wchar_t *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=_vsnwprintf(buf,cnt,fmt,args);
	va_end(args);
	return i;
}


int vswprintf(wchar_t *buf, const wchar_t *fmt, va_list args)
{
	return _vsnwprintf(buf,INT_MAX,fmt,args);
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




int WINsnprintf(char* const s, size_t const n, const char* const fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	int const ret = vsnprintf(s, n, fmt, arg);
	va_end(arg);
	if (n != 0) s[n - 1] = '\0'; // _vsnprintf() does not guarantee NUL termination
	return ret;
}


int WINswprintf(wchar_t* const s, size_t const n, const wchar_t* const fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	int const ret = WINvswprintf(s, n, fmt, arg);
	va_end(arg);
	return ret;
}


int WINvswprintf(wchar_t* const s, size_t const n, const wchar_t* const fmt, va_list const arg)
{
	int const ret = _vsnwprintf(s, n, fmt, arg);
	if (n != 0) s[n - 1] = L'\0'; // _vsnwprintf() does not guarantee NUL termination
	return ret;
}

#endif


void ReplacePath(char* const buf, size_t const size, char const* path, char const* const filename, char const* const ext)
{
	char const* base    = filename;
	char const* old_ext = 0;
	for (char const* i = filename;; ++i)
	{
		switch (*i)
		{
			case '.':
				old_ext = i;
				break;

			case '/':
				base    = i + 1;
				old_ext = 0;
				break;

			case '\0':
				if (!path)
				{
					base = filename;
					path = "";
				}
				int const n = (old_ext ? old_ext : i) - base;
				snprintf(buf, size, "%s%.*s%s", path, n, base, ext);
				return;
		}
	}
}
