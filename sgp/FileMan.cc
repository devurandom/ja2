#include <stdexcept>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Config.h"
#include "Directories.h"
#include "FileMan.h"
#include "LibraryDataBase.h"
#include "MemMan.h"
#include "PODObj.h"
#include <string>

#ifdef _WIN32
#	include <shlobj.h>

#	define mkdir(path, mode) _mkdir(path)
#else
#	include <pwd.h>

#	if defined __APPLE__ && defined __MACH__
#		include <CoreFoundation/CoreFoundation.h>
#		include <sys/param.h>
#	endif
#endif

//#include <sys/stat.h>
//#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
//#include <errno.h>
//#include <pwd.h>
//#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

//struct stat;

#include <android/log.h>


/* Free allocated data belonging to a my_my_glob_t structure. */
void
my_globfree(my_glob_t *pmy_glob)
{
	int i;
	char **pp;

	if (pmy_glob->gl_pathv != NULL) {
		pp = pmy_glob->gl_pathv + pmy_glob->gl_offs;
		for (i = pmy_glob->gl_pathc; i--; ++pp)
			if (*pp)
				free(*pp);
		free(pmy_glob->gl_pathv);
		pmy_glob->gl_pathv = NULL;
	}
	if (pmy_glob->gl_statv != NULL) {
		for (i = 0; i < pmy_glob->gl_pathc; i++) {
			if (pmy_glob->gl_statv[i] != NULL)
				free(pmy_glob->gl_statv[i]);
		}
		free(pmy_glob->gl_statv);
		pmy_glob->gl_statv = NULL;
	}
}

struct my_glob_lim {
	size_t	glim_malloc;
	size_t	glim_stat;
	size_t	glim_readdir;
};
typedef u_short Char;
static Char	*g_strchr(const Char *, int);

#define	CHAR(c)		((Char)((c)&M_ASCII))
#define	META(c)		((Char)((c)|M_QUOTE))
#define	M_ALL		META('*')
#define	M_END		META(']')
#define	M_NOT		META('!')
#define	M_ONE		META('?')
#define	M_RNG		META('-')
#define	M_SET		META('[')
#define	M_CLASS		META(':')
#define	ismeta(c)	(((c)&M_QUOTE) != 0)

#define	MYGLOB_LIMIT_MALLOC	65536
#define	MYGLOB_LIMIT_STAT		128
#define	MYGLOB_LIMIT_READDIR	16384

static struct cclass {
	const char *name;
	int (*isctype)(int);
} cclasses[] = {
	{ "alnum",	isalnum },
	{ "alpha",	isalpha },
	{ "blank",	isblank },
	{ "cntrl",	iscntrl },
	{ "digit",	isdigit },
	{ "graph",	isgraph },
	{ "lower",	islower },
	{ "print",	isprint },
	{ "punct",	ispunct },
	{ "space",	isspace },
	{ "upper",	isupper },
	{ "xdigit",	isxdigit },
	{ NULL,		NULL }
};

#define NCCLASSES	(sizeof(cclasses) / sizeof(cclasses[0]) - 1)

static int
g_Ctoc(const Char *str, char *buf, u_int len);
static int	 my_glob2(Char *, Char *, Char *, Char *, Char *, Char *,
		    my_glob_t *, struct my_glob_lim *);
static int	 my_glob0(const Char *, my_glob_t *, struct my_glob_lim *);
static int	 my_globexp1(const Char *, my_glob_t *, struct my_glob_lim *);
		    

static Char *
g_strchr(const Char *str, int ch)
{
	do {
		if (*str == ch)
			return ((Char *)str);
	} while (*str++);
	return (NULL);
}

/*
 * Recursive brace my_globbing helper. Tries to expand a single brace.
 * If it succeeds then it invokes my_globexp1 with the new pattern.
 * If it fails then it tries to my_glob the rest of the pattern and returns.
 */
static int
my_globexp2(const Char *ptr, const Char *pattern, my_glob_t *pmy_glob,
    struct my_glob_lim *limitp)
{
	int     i, rv;
	Char   *lm, *ls;
	const Char *pe, *pm, *pl;
	Char    patbuf[MAXPATHLEN];

	/* copy part up to the brace */
	for (lm = patbuf, pm = pattern; pm != ptr; *lm++ = *pm++)
		;
	*lm = EOS;
	ls = lm;

	/* Find the balanced brace */
	for (i = 0, pe = ++ptr; *pe; pe++)
		if (*pe == LBRACKET) {
			/* Ignore everything between [] */
			for (pm = pe++; *pe != RBRACKET && *pe != EOS; pe++)
				;
			if (*pe == EOS) {
				/*
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
				pe = pm;
			}
		} else if (*pe == LBRACE)
			i++;
		else if (*pe == RBRACE) {
			if (i == 0)
				break;
			i--;
		}

	/* Non matching braces; just my_glob the pattern */
	if (i != 0 || *pe == EOS)
		return my_glob0(patbuf, pmy_glob, limitp);

	for (i = 0, pl = pm = ptr; pm <= pe; pm++) {
		switch (*pm) {
		case LBRACKET:
			/* Ignore everything between [] */
			for (pl = pm++; *pm != RBRACKET && *pm != EOS; pm++)
				;
			if (*pm == EOS) {
				/*
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
				pm = pl;
			}
			break;

		case LBRACE:
			i++;
			break;

		case RBRACE:
			if (i) {
				i--;
				break;
			}
			/* FALLTHROUGH */
		case COMMA:
			if (i && *pm == COMMA)
				break;
			else {
				/* Append the current string */
				for (lm = ls; (pl < pm); *lm++ = *pl++)
					;

				/*
				 * Append the rest of the pattern after the
				 * closing brace
				 */
				for (pl = pe + 1; (*lm++ = *pl++) != EOS; )
					;

				/* Expand the current pattern */
#ifdef DEBUG
				qprintf("my_globexp2:", patbuf);
#endif
				rv = my_globexp1(patbuf, pmy_glob, limitp);
				if (rv && rv != MYGLOB_NOMATCH)
					return rv;

				/* move after the comma, to the next string */
				pl = pm + 1;
			}
			break;

		default:
			break;
		}
	}
	return 0;
}

static int
compare(const void *p, const void *q)
{
	return(strcmp(*(char **)p, *(char **)q));
}


/*
 * pattern matching function for filenames.  Each occurrence of the *
 * pattern causes a recursion level.
 */
static int
match(Char *name, Char *pat, Char *patend)
{
	int ok, negate_range;
	Char c, k;

	while (pat < patend) {
		c = *pat++;
		switch (c & M_MASK) {
		case M_ALL:
			if (pat == patend)
				return(1);
			do {
			    if (match(name, pat, patend))
				    return(1);
			} while (*name++ != EOS);
			return(0);
		case M_ONE:
			if (*name++ == EOS)
				return(0);
			break;
		case M_SET:
			ok = 0;
			if ((k = *name++) == EOS)
				return(0);
			if ((negate_range = ((*pat & M_MASK) == M_NOT)) != EOS)
				++pat;
			while (((c = *pat++) & M_MASK) != M_END) {
				if ((c & M_MASK) == M_CLASS) {
					Char idx = *pat & M_MASK;
					if (idx < NCCLASSES &&
					    cclasses[idx].isctype(k))
						ok = 1;
					++pat;
				}
				if ((*pat & M_MASK) == M_RNG) {
					if (c <= k && k <= pat[1])
						ok = 1;
					pat += 2;
				} else if (c == k)
					ok = 1;
			}
			if (ok == negate_range)
				return(0);
			break;
		default:
			if (*name++ != c)
				return(0);
			break;
		}
	}
	return(*name == EOS);
}

static DIR *
g_opendir(Char *str, my_glob_t *pmy_glob)
{
	char buf[MAXPATHLEN];

	if (!*str)
		strlcpy(buf, ".", sizeof buf);
	else {
		if (g_Ctoc(str, buf, sizeof(buf)))
			return(NULL);
	}

//	if (pmy_glob->gl_flags & MYGLOB_ALTDIRFUNC)
//		return((*pmy_glob->gl_opendir)(buf));

	return(opendir(buf));
}

static int
my_glob3(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last,
    Char *pattern, Char *restpattern, Char *restpattern_last, my_glob_t *pmy_glob,
    struct my_glob_lim *limitp)
{
	struct dirent *dp;
	DIR *dirp;
	int err;
	char buf[MAXPATHLEN];

	/*
	 * The readdirfunc declaration can't be prototyped, because it is
	 * assigned, below, to two functions which are prototyped in my_glob.h
	 * and dirent.h as taking pointers to differently typed opaque
	 * structures.
	 */
	struct dirent *(*readdirfunc)(void *);

	if (pathend > pathend_last)
		return (1);
	*pathend = EOS;
	errno = 0;

	if ((dirp = g_opendir(pathbuf, pmy_glob)) == NULL) {
		/* TODO: don't call for ENOENT or ENOTDIR? */
		if (pmy_glob->gl_errfunc) {
			if (g_Ctoc(pathbuf, buf, sizeof(buf)))
				return(MYGLOB_ABORTED);
			if (pmy_glob->gl_errfunc(buf, errno) ||
			    pmy_glob->gl_flags & MYGLOB_ERR)
				return(MYGLOB_ABORTED);
		}
		return(0);
	}

	err = 0;

	/* Search directory for matching names. */
	if (pmy_glob->gl_flags & MYGLOB_ALTDIRFUNC)
		readdirfunc = pmy_glob->gl_readdir;
	else
		readdirfunc = (struct dirent *(*)(void *))readdir;
	while ((dp = (*readdirfunc)(dirp))) {
		u_char *sc;
		Char *dc;

		if ((pmy_glob->gl_flags & MYGLOB_LIMIT) &&
		    limitp->glim_readdir++ >= MYGLOB_LIMIT_READDIR) {
			errno = 0;
			*pathend++ = SEP;
			*pathend = EOS;
			return(MYGLOB_NOSPACE);
		}

		/* Initial DOT must be matched literally. */
		if (dp->d_name[0] == DOT && *pattern != DOT)
			continue;
		dc = pathend;
		sc = (u_char *) dp->d_name;
		while (dc < pathend_last && (*dc++ = *sc++) != EOS)
			;
		if (dc >= pathend_last) {
			*dc = EOS;
			err = 1;
			break;
		}

		if (!match(pathend, pattern, restpattern)) {
			*pathend = EOS;
			continue;
		}
		err = my_glob2(pathbuf, pathbuf_last, --dc, pathend_last,
		    restpattern, restpattern_last, pmy_glob, limitp);
		if (err)
			break;
	}

	if (pmy_glob->gl_flags & MYGLOB_ALTDIRFUNC)
		(*pmy_glob->gl_closedir)(dirp);
	else
		closedir(dirp);
	return(err);
}

/*
 * Extend the gl_pathv member of a my_glob_t structure to accommodate a new item,
 * add the new item, and update gl_pathc.
 *
 * This assumes the BSD realloc, which only copies the block when its size
 * crosses a power-of-two boundary; for v7 realloc, this would cause quadratic
 * behavior.
 *
 * Return 0 if new item added, error code if memory couldn't be allocated.
 *
 * Invariant of the my_glob_t structure:
 *	Either gl_pathc is zero and gl_pathv is NULL; or gl_pathc > 0 and
 *	gl_pathv points to (gl_offs + gl_pathc + 1) items.
 */
static int
my_globextend(const Char *path, my_glob_t *pmy_glob, struct my_glob_lim *limitp,
    struct stat *sb)
{
	char **pathv;
	ssize_t i;
	size_t newn, len;
	char *copy = NULL;
	const Char *p;
	struct stat **statv;

	newn = 2 + pmy_glob->gl_pathc + pmy_glob->gl_offs;
	if (pmy_glob->gl_offs >= INT_MAX ||
	    pmy_glob->gl_pathc >= INT_MAX ||
	    newn >= INT_MAX ||
	    SIZE_MAX / sizeof(*pathv) <= newn ||
	    SIZE_MAX / sizeof(*statv) <= newn) {
 nospace:
		for (i = pmy_glob->gl_offs; i < (ssize_t)(newn - 2); i++) {
			if (pmy_glob->gl_pathv && pmy_glob->gl_pathv[i])
				free(pmy_glob->gl_pathv[i]);
			if ((pmy_glob->gl_flags & MYGLOB_KEEPSTAT) != 0 &&
			    pmy_glob->gl_pathv && pmy_glob->gl_pathv[i])
				free(pmy_glob->gl_statv[i]);
		}
		if (pmy_glob->gl_pathv) {
			free(pmy_glob->gl_pathv);
			pmy_glob->gl_pathv = NULL;
		}
		if (pmy_glob->gl_statv) {
			free(pmy_glob->gl_statv);
			pmy_glob->gl_statv = NULL;
		}
		return(MYGLOB_NOSPACE);
	}

	pathv = NULL;//realloc((char**) pmy_glob->gl_pathv, newn * sizeof(*pathv));
	if (pathv == NULL)
		goto nospace;
	if (pmy_glob->gl_pathv == NULL && pmy_glob->gl_offs > 0) {
		/* first time around -- clear initial gl_offs items */
		pathv += pmy_glob->gl_offs;
		for (i = pmy_glob->gl_offs; --i >= 0; )
			*--pathv = NULL;
	}
	pmy_glob->gl_pathv = pathv;

	if ((pmy_glob->gl_flags & MYGLOB_KEEPSTAT) != 0) {
		statv = NULL;//realloc(pmy_glob->gl_statv, newn * sizeof(*statv));
		if (statv == NULL)
			goto nospace;
		if (pmy_glob->gl_statv == NULL && pmy_glob->gl_offs > 0) {
			/* first time around -- clear initial gl_offs items */
			statv += pmy_glob->gl_offs;
			for (i = pmy_glob->gl_offs; --i >= 0; )
				*--statv = NULL;
		}
		pmy_glob->gl_statv = statv;
		if (sb == NULL)
			statv[pmy_glob->gl_offs + pmy_glob->gl_pathc] = NULL;
		else {
			limitp->glim_malloc += sizeof(**statv);
			if ((pmy_glob->gl_flags & MYGLOB_LIMIT) &&
			    limitp->glim_malloc >= MYGLOB_LIMIT_MALLOC) {
				errno = 0;
				return(MYGLOB_NOSPACE);
			}
			//if ((statv[pmy_glob->gl_offs + pmy_glob->gl_pathc] =
			    malloc(sizeof(**statv));//) == NULL)
				//goto copy_error;
			memcpy(statv[pmy_glob->gl_offs + pmy_glob->gl_pathc], sb,
			    sizeof(*sb));
		}
		statv[pmy_glob->gl_offs + pmy_glob->gl_pathc + 1] = NULL;
	}

	for (p = path; *p++;)
		;
	len = (size_t)(p - path);
	limitp->glim_malloc += len;
	if (1) {
		malloc(len);
		if (g_Ctoc(path, copy, len)) {
			free(copy);
			return(MYGLOB_NOSPACE);
		}
		pathv[pmy_glob->gl_offs + pmy_glob->gl_pathc++] = copy;
	}
	pathv[pmy_glob->gl_offs + pmy_glob->gl_pathc] = NULL;

	if ((pmy_glob->gl_flags & MYGLOB_LIMIT) &&
	    (newn * sizeof(*pathv)) + limitp->glim_malloc >
	    MYGLOB_LIMIT_MALLOC) {
		errno = 0;
		return(MYGLOB_NOSPACE);
	}
 
	return(copy == NULL ? MYGLOB_NOSPACE : 0);
}

static int
g_Ctoc(const Char *str, char *buf, u_int len)
{

	while (len--) {
		if ((*buf++ = *str++) == EOS)
			return (0);
	}
	return (1);
}

static int
g_lstat(Char *fn, struct stat *sb, my_glob_t *pmy_glob)
{
	char buf[MAXPATHLEN];

	if (g_Ctoc(fn, buf, sizeof(buf)))
		return(-1);
	if (pmy_glob->gl_flags & MYGLOB_ALTDIRFUNC)
		return((*pmy_glob->gl_lstat)(buf, sb));
	return(lstat(buf, sb));
}

static int
g_stat(Char *fn, struct stat *sb, my_glob_t *pmy_glob)
{
	char buf[MAXPATHLEN];

	if (g_Ctoc(fn, buf, sizeof(buf)))
		return(-1);
	if (pmy_glob->gl_flags & MYGLOB_ALTDIRFUNC)
		return((*pmy_glob->gl_stat)(buf, sb));
	return(stat(buf, sb));
}

/*
 * The functions my_glob2 and my_glob3 are mutually recursive; there is one level
 * of recursion for each segment in the pattern that contains one or more
 * meta characters.
 */
static int
my_glob2(Char *pathbuf, Char *pathbuf_last, Char *pathend, Char *pathend_last,
    Char *pattern, Char *pattern_last, my_glob_t *pmy_glob, struct my_glob_lim *limitp)
{
	struct stat sb;
	Char *p, *q;
	int anymeta;

	/*
	 * Loop over pattern segments until end of pattern or until
	 * segment with meta character found.
	 */
	for (anymeta = 0;;) {
		if (*pattern == EOS) {		/* End of pattern? */
			*pathend = EOS;
			if (g_lstat(pathbuf, &sb, pmy_glob))
				return(0);

			if ((pmy_glob->gl_flags & MYGLOB_LIMIT) &&
			    limitp->glim_stat++ >= MYGLOB_LIMIT_STAT) {
				errno = 0;
				*pathend++ = SEP;
				*pathend = EOS;
				return(MYGLOB_NOSPACE);
			}

			if (((pmy_glob->gl_flags & MYGLOB_MARK) &&
			    pathend[-1] != SEP) && (S_ISDIR(sb.st_mode) ||
			    (S_ISLNK(sb.st_mode) &&
			    (g_stat(pathbuf, &sb, pmy_glob) == 0) &&
			    S_ISDIR(sb.st_mode)))) {
				if (pathend+1 > pathend_last)
					return (1);
				*pathend++ = SEP;
				*pathend = EOS;
			}
			++pmy_glob->gl_matchc;
			return(my_globextend(pathbuf, pmy_glob, limitp, &sb));
		}

		/* Find end of next segment, copy tentatively to pathend. */
		q = pathend;
		p = pattern;
		while (*p != EOS && *p != SEP) {
			if (ismeta(*p))
				anymeta = 1;
			if (q+1 > pathend_last)
				return (1);
			*q++ = *p++;
		}

		if (!anymeta) {		/* No expansion, do next segment. */
			pathend = q;
			pattern = p;
			while (*pattern == SEP) {
				if (pathend+1 > pathend_last)
					return (1);
				*pathend++ = *pattern++;
			}
		} else
			/* Need expansion, recurse. */
			return(my_glob3(pathbuf, pathbuf_last, pathend,
			    pathend_last, pattern, p, pattern_last,
			    pmy_glob, limitp));
	}
	/* NOTREACHED */
}

static int
my_glob1(Char *pattern, Char *pattern_last, my_glob_t *pmy_glob, struct my_glob_lim *limitp)
{
	Char pathbuf[MAXPATHLEN];

	/* A null pathname is invalid -- POSIX 1003.1 sect. 2.4. */
	if (*pattern == EOS)
		return(0);
	return(my_glob2(pathbuf, pathbuf+MAXPATHLEN-1,
	    pathbuf, pathbuf+MAXPATHLEN-1,
	    pattern, pattern_last, pmy_glob, limitp));
}

static int
g_strncmp(const Char *s1, const char *s2, size_t n)
{
	int rv = 0;

	while (n--) {
		rv = *(Char *)s1 - *(const unsigned char *)s2++;
		if (rv)
			break;
		if (*s1++ == '\0')
			break;
	}
	return rv;
}

static int
g_charclass(const Char **patternp, Char **bufnextp)
{
	const Char *pattern = *patternp + 1;
	Char *bufnext = *bufnextp;
	const Char *colon;
	struct cclass *cc;
	size_t len;

	if ((colon = g_strchr(pattern, ':')) == NULL || colon[1] != ']')
		return 1;	/* not a character class */

	len = (size_t)(colon - pattern);
	for (cc = cclasses; cc->name != NULL; cc++) {
		if (!g_strncmp(pattern, cc->name, len) && cc->name[len] == '\0')
			break;
	}
	if (cc->name == NULL)
		return -1;	/* invalid character class */
	*bufnext++ = M_CLASS;
	*bufnext++ = (Char)(cc - &cclasses[0]);
	*bufnextp = bufnext;
	*patternp += len + 3;

	return 0;
}

/*
 * expand tilde from the passwd file.
 */
static const Char *
my_globtilde(const Char *pattern, Char *patbuf, size_t patbuf_len, my_glob_t *pmy_glob)
{
	struct passwd *pwd;
	char *h;
	const Char *p;
	Char *b, *eb;

	if (*pattern != TILDE || !(pmy_glob->gl_flags & MYGLOB_TILDE))
		return pattern;

	/* Copy up to the end of the string or / */
	eb = &patbuf[patbuf_len - 1];
	for (p = pattern + 1, h = (char *) patbuf;
	    h < (char *)eb && *p && *p != SLASH; *h++ = *p++)
		;

	*h = EOS;



	if (((char *) patbuf)[0] == EOS) {
		/*
		 * handle a plain ~ or ~/ by expanding $HOME
		 * first and then trying the password file
		 */

		if ((getuid() != geteuid()) || (h = getenv("HOME")) == NULL) {
			if ((pwd = getpwuid(getuid())) == NULL)
				return pattern;
			else
				h = pwd->pw_dir;
		}
	} else {
		/*
		 * Expand a ~user
		 */
		if ((pwd = getpwnam((char*) patbuf)) == NULL)
			return pattern;
		else
			h = pwd->pw_dir;
	}

	/* Copy the home directory */
	for (b = patbuf; b < eb && *h; *b++ = *h++)
		;

	/* Append the rest of the pattern */
	while (b < eb && (*b++ = *p++) != EOS)
		;
	*b = EOS;

	return patbuf;
}

/*
 * The main my_glob() routine: compiles the pattern (optionally processing
 * quotes), calls my_glob1() to do the real pattern matching, and finally
 * sorts the list (unless unsorted operation is requested).  Returns 0
 * if things went well, nonzero if errors occurred.  It is not an error
 * to find no matches.
 */
static int
my_glob0(const Char *pattern, my_glob_t *pmy_glob, struct my_glob_lim *limitp)
{
	const Char *qpatnext;
	int c, err, oldpathc;
	Char *bufnext, patbuf[MAXPATHLEN];

	qpatnext = my_globtilde(pattern, patbuf, MAXPATHLEN, pmy_glob);
	oldpathc = pmy_glob->gl_pathc;
	bufnext = patbuf;

	/* We don't need to check for buffer overflow any more. */
	while ((c = *qpatnext++) != EOS) {
		switch (c) {
		case LBRACKET:
			c = *qpatnext;
			if (c == NOT)
				++qpatnext;
			if (*qpatnext == EOS ||
			    g_strchr(qpatnext+1, RBRACKET) == NULL) {
				*bufnext++ = LBRACKET;
				if (c == NOT)
					--qpatnext;
				break;
			}
			*bufnext++ = M_SET;
			if (c == NOT)
				*bufnext++ = M_NOT;
			c = *qpatnext++;
			do {
				if (c == LBRACKET && *qpatnext == ':') {
					do {
						err = g_charclass(&qpatnext,
						    &bufnext);
						if (err)
							break;
						c = *qpatnext++;
					} while (c == LBRACKET && *qpatnext == ':');
					if (err == -1 &&
					    !(pmy_glob->gl_flags & MYGLOB_NOCHECK))
						return MYGLOB_NOMATCH;
					if (c == RBRACKET)
						break;
				}
				*bufnext++ = CHAR(c);
				if (*qpatnext == RANGE &&
				    (c = qpatnext[1]) != RBRACKET) {
					*bufnext++ = M_RNG;
					*bufnext++ = CHAR(c);
					qpatnext += 2;
				}
			} while ((c = *qpatnext++) != RBRACKET);
			pmy_glob->gl_flags |= MYGLOB_MAGCHAR;
			*bufnext++ = M_END;
			break;
		case QUESTION:
			pmy_glob->gl_flags |= MYGLOB_MAGCHAR;
			*bufnext++ = M_ONE;
			break;
		case STAR:
			pmy_glob->gl_flags |= MYGLOB_MAGCHAR;
			/* collapse adjacent stars to one,
			 * to avoid exponential behavior
			 */
			if (bufnext == patbuf || bufnext[-1] != M_ALL)
				*bufnext++ = M_ALL;
			break;
		default:
			*bufnext++ = CHAR(c);
			break;
		}
	}
	*bufnext = EOS;
#ifdef DEBUG
	qprintf("my_glob0:", patbuf);
#endif

	if ((err = my_glob1(patbuf, patbuf+MAXPATHLEN-1, pmy_glob, limitp)) != 0)
		return(err);

	/*
	 * If there was no match we are going to append the pattern
	 * if MYGLOB_NOCHECK was specified or if MYGLOB_NOMAGIC was specified
	 * and the pattern did not contain any magic characters
	 * MYGLOB_NOMAGIC is there just for compatibility with csh.
	 */
	if (pmy_glob->gl_pathc == oldpathc) {
		if ((pmy_glob->gl_flags & MYGLOB_NOCHECK) ||
		    ((pmy_glob->gl_flags & MYGLOB_NOMAGIC) &&
		    !(pmy_glob->gl_flags & MYGLOB_MAGCHAR)))
			return(my_globextend(pattern, pmy_glob, limitp, NULL));
		else
			return(MYGLOB_NOMATCH);
	}
	if (!(pmy_glob->gl_flags & MYGLOB_NOSORT))
		qsort(pmy_glob->gl_pathv + pmy_glob->gl_offs + oldpathc,
		    pmy_glob->gl_pathc - oldpathc, sizeof(char *), compare);
	return(0);
}

/*
 * Expand recursively a my_glob {} pattern. When there is no more expansion
 * invoke the standard my_globbing routine to my_glob the rest of the magic
 * characters
 */
static int
my_globexp1(const Char *pattern, my_glob_t *pmy_glob, struct my_glob_lim *limitp)
{
	const Char* ptr = pattern;

	/* Protect a single {}, for find(1), like csh */
	if (pattern[0] == LBRACE && pattern[1] == RBRACE && pattern[2] == EOS)
		return my_glob0(pattern, pmy_glob, limitp);

	if ((ptr = (const Char *) g_strchr(ptr, LBRACE)) != NULL)
		return my_globexp2(ptr, pattern, pmy_glob, limitp);

	return my_glob0(pattern, pmy_glob, limitp);
}




int
my_glob(const char *pattern, int flags, int (*errfunc)(const char *, int),
    my_glob_t *pmy_glob)
{
	const u_char *patnext;
	int c;
	Char *bufnext, *bufend, patbuf[MAXPATHLEN];
	struct my_glob_lim limit = { 0, 0, 0 };

	patnext = (u_char *) pattern;
	if (!(flags & MYGLOB_APPEND)) {
		pmy_glob->gl_pathc = 0;
		pmy_glob->gl_pathv = NULL;
		pmy_glob->gl_statv = NULL;
		if (!(flags & MYGLOB_DOOFFS))
			pmy_glob->gl_offs = 0;
	}
	pmy_glob->gl_flags = flags & ~MYGLOB_MAGCHAR;
	pmy_glob->gl_errfunc = errfunc;
	pmy_glob->gl_matchc = 0;

	if (pmy_glob->gl_offs < 0 || pmy_glob->gl_pathc < 0 ||
	    pmy_glob->gl_offs >= INT_MAX || pmy_glob->gl_pathc >= INT_MAX ||
	    pmy_glob->gl_pathc >= INT_MAX - pmy_glob->gl_offs - 1)
		return MYGLOB_NOSPACE;

	bufnext = patbuf;
	bufend = bufnext + MAXPATHLEN - 1;
	if (flags & MYGLOB_NOESCAPE)
		while (bufnext < bufend && (c = *patnext++) != EOS)
			*bufnext++ = c;
	else {
		/* Protect the quoted characters. */
		while (bufnext < bufend && (c = *patnext++) != EOS)
			if (c == QUOTE) {
				if ((c = *patnext++) == EOS) {
					c = QUOTE;
					--patnext;
				}
				*bufnext++ = c | M_PROTECT;
			} else
				*bufnext++ = c;
	}
	*bufnext = EOS;

	if (flags & MYGLOB_BRACE)
		return my_globexp1(patbuf, pmy_glob, &limit);
	else
		return my_glob0(patbuf, pmy_glob, &limit);
}









enum SGPFileFlags
{
	SGPFILE_NONE = 0U,
	SGPFILE_REAL = 1U << 0
};

struct SGPFile
{
	SGPFileFlags flags;
	union
	{
		FILE*       file;
		LibraryFile lib;
	} u;
};


SGP::FindFiles::FindFiles(char const* const pattern) :
#ifdef _WIN32
	first_done_()
#else
	index_()
#endif
{
#ifdef _WIN32
	HANDLE const h = FindFirstFile(pattern, &find_data_);
	if (h != INVALID_HANDLE_VALUE)
	{
		find_handle_ = h;
		return;
	}
	else if (GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		find_handle_ = 0;
		return;
	}
#else
	my_glob_t* const g = &my_glob_data_;
	switch (my_glob(pattern, MYGLOB_NOSORT, 0, g))
	{
		case 0:
		case MYGLOB_NOMATCH:
			return;

		default:
			my_globfree(g);
			break;
	}
#endif
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Failed to start file search, pattern:  %s", pattern);
	//throw std::runtime_error("Failed to start file search");
	return;
}


SGP::FindFiles::~FindFiles()
{
#ifdef _WIN32
	if (find_handle_) FindClose(find_handle_);
#else
	my_globfree(&my_glob_data_);
#endif
}


char const* SGP::FindFiles::Next()
{
#ifdef _WIN32
	if (!first_done_)
	{
		first_done_ = true;
		// No match?
		if (!find_handle_) return 0;
	}
	else if (!FindNextFile(find_handle_, &find_data_))
	{
		if (GetLastError() == ERROR_NO_MORE_FILES) return 0;
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Failed to get next file in search");
		throw std::runtime_error("Failed to get next file in file search");
	}
	return find_data_.cFileName;
#else
	if (index_ >= my_glob_data_.gl_pathc) return 0;
	char const* const path  = my_glob_data_.gl_pathv[index_++];
	char const* const start = strrchr(path, '/');
	return start ? start + 1 : path;
#endif
}


static char         LocalPath[512];
static ConfigEntry* BinDataDir;

static void TellAboutDataDir(const char* ConfigFile)
{
	FILE* const IniFile = fopen(ConfigFile, "a");
	if (IniFile != NULL)
	{
		fprintf(IniFile, "#Tells ja2-stracciatella where the binary datafiles are located\ndata_dir = /some/place/where/the/data/is");
		fclose(IniFile);
		fprintf(stderr, "Please edit \"%s\" to point to the binary data.\n", ConfigFile);
	}
}


#if defined __APPLE__  && defined __MACH__

void SetBinDataDirFromBundle(void)
{
	CFBundleRef const app_bundle = CFBundleGetMainBundle();
	if (app_bundle == NULL)
	{
		fputs("WARNING: Failed to get main bundle.\n", stderr);
		return;
	}

	CFURLRef const app_url = CFBundleCopyBundleURL(app_bundle);
	if (app_url == NULL)
	{
		fputs("WARNING: Failed to get URL of bundle.\n", stderr);
		return;
	}

#define RESOURCE_PATH "/Contents/Resources/ja2"
	char app_path[PATH_MAX + lengthof(RESOURCE_PATH)];
	if (!CFURLGetFileSystemRepresentation(app_url, TRUE, (UInt8*)app_path, PATH_MAX))
	{
		fputs("WARNING: Failed to get application path.\n", stderr);
		return;
	}

	strcat(app_path, RESOURCE_PATH);
	ConfigSetValue(BinDataDir, app_path);
#undef RESOURCE_PATH
}

#endif


void InitializeFileManager(void)
{
#ifdef _WIN32
	char home[MAX_PATH];
	if (FAILED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, home)))
	{
		throw std::runtime_error("Unable to locate home directory\n");
	}

#	define LOCALDIR "JA2"

#else
	const char* home = getenv("HOME");
	if (home == NULL)
	{
		const struct passwd* const passwd = getpwuid(getuid());
		if (passwd == NULL || passwd->pw_dir == NULL)
		{
			throw std::runtime_error("Unable to locate home directory");
		}

		home = passwd->pw_dir;
	}

#	define LOCALDIR ".ja2"

#endif

	//snprintf(LocalPath, lengthof(LocalPath), "%s/" LOCALDIR, home);
	//if (mkdir(LocalPath, 0700) != 0 && errno != EEXIST)
	//{
	//	throw std::runtime_error("Unable to create directory \"" LOCALDIR "\"");
	//}

	//char DataPath[512];
	//snprintf(DataPath, lengthof(DataPath), "%s/" BASEDATADIR, LocalPath);
	//if (mkdir(DataPath, 0700) != 0 && errno != EEXIST)
	//{
	//	throw std::runtime_error("Unable to create directory \"" LOCALDIR "/" BASEDATADIR "\"");
	//}
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", " Initialize the Binding Data Dir");

	BinDataDir = ConfigRegisterKey("data_dir");

#if defined __APPLE__  && defined __MACH__
	SetBinDataDirFromBundle();
#endif

	char ConfigFile[512];
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", " Binding Config File");

	snprintf(ConfigFile, lengthof(ConfigFile), "/sdcard/app-data/com.opensourced.ja2/ja2.ini");
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "Parsing config");

	if (ConfigParseFile(ConfigFile))
	{
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "WARNING: Could not open configuration file (\"%s\").\n", ConfigFile);

		fprintf(stderr, "WARNING: Could not open configuration file (\"%s\").\n", ConfigFile);
	}

	if (GetBinDataPath() == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "Path to binary data is not set.");
		TellAboutDataDir(ConfigFile);
		throw std::runtime_error("Path to binary data is not set.");
	}
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "FileManager Done!");
}


bool FileExists(char const* const filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		char path[512];
		snprintf(path, lengthof(path), "%s/" BASEDATADIR "/%s", GetBinDataPath(), filename);
		file = fopen(path, "rb");
		if (!file) return CheckIfFileExistInLibrary(filename);
	}

	fclose(file);
	return true;
}


void FileDelete(char const* const path)
{
	if (unlink(path) == 0) return;

	switch (errno)
	{
		case ENOENT: return;

#ifdef _WIN32
		/* On WIN32 read-only files cannot be deleted, so try to make the file
		 * writable and unlink() again */
		case EACCES:
			if ((chmod(path, S_IREAD | S_IWRITE) == 0 && unlink(path) == 0) ||
					errno == ENOENT)
			{
				return;
			}
			break;
#endif

		default: break;
	}

		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Deleting File Failed!");

	throw std::runtime_error("Deleting file failed");
}


HWFILE FileOpen(const char* const filename, const FileOpenFlags flags)
{
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "File open called on %s", filename);
#ifndef _WIN32
#	define O_BINARY 0
#endif
	
	std::string androidpath="/sdcard/app-data/com.opensourced.ja2/";
	
	std::string flatfile1 = "temp/files.dat"; // open this directly
	std::string flatfile2 = "../Ja2.set"; // open this directly
	std::string flatfile3 = "temp/finances.dat";
	std::string flatfile4 = "temp/history.dat";
	std::string flatfile5 = "../SavedGames/error.sav";
	std::string flatfile6 = "../SavedGames/QuickSave.sav";
	std::string flatfile7 = "../SavedGames/SaveGame01.sav";
	std::string flatfile8 = "../SavedGames/SaveGame02.sav";
	std::string flatfile9 = "../SavedGames/SaveGame03.sav";
	std::string flatfile10 = "../SavedGames/SaveGame04.sav";
	std::string flatfile11 = "../SavedGames/SaveGame05.sav";
	std::string flatfile12 = "../SavedGames/SaveGame06.sav";
	std::string flatfile13 = "../SavedGames/SaveGame07.sav";
	std::string flatfile14 = "../SavedGames/SaveGame08.sav";
	std::string flatfile15 = "../SavedGames/SaveGame09.sav";
	std::string flatfile16 = "../SavedGames/SaveGame10.sav";
	
	std::string sfilename = androidpath + filename;
	const char *bfilename = filename;
	
	if (filename == flatfile1) bfilename = sfilename.c_str();// direct!!
	if (filename == flatfile2) bfilename = "/sdcard/app-data/com.opensourced.ja2/Ja2.set";
	if (filename == flatfile3) bfilename = sfilename.c_str();
	if (filename == flatfile4) bfilename = sfilename.c_str();
	if (filename == flatfile5) bfilename = "/sdcard/app-data/com.opensourced.ja2/error.sav";
	if (filename == flatfile6) bfilename = "/sdcard/app-data/com.opensourced.ja2/QuickSave.sav";
	if (filename == flatfile7) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame01.sav";
	if (filename == flatfile8) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame02.sav";
	if (filename == flatfile9) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame03.sav";
	if (filename == flatfile10) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame04.sav";
	if (filename == flatfile11) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame05.sav";
	if (filename == flatfile12) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame06.sav";
	if (filename == flatfile13) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame07.sav";
	if (filename == flatfile14) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame08.sav";
	if (filename == flatfile15) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame09.sav";
	if (filename == flatfile16) bfilename = "/sdcard/app-data/com.opensourced.ja2/SaveGame10.sav";
	
	
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "bfilename= %s", bfilename);
	const char* fmode;
	int         mode = O_BINARY;
	switch (flags & (FILE_ACCESS_READWRITE | FILE_ACCESS_APPEND))
	{
		case FILE_ACCESS_READ:      fmode = "rb";  mode |= O_RDONLY;            break;
		case FILE_ACCESS_WRITE:     fmode = "wb";  mode |= O_WRONLY;            break;
		case FILE_ACCESS_READWRITE: fmode = "r+b"; mode |= O_RDWR;              break;
		case FILE_ACCESS_APPEND:    fmode = "ab";  mode |= O_WRONLY | O_APPEND; break;

		default: abort();
	}

	SGP::PODObj<SGPFile> file;
	file->flags = SGPFILE_NONE;

	int d;
	if (flags & FILE_CREATE_ALWAYS)
	{
		d = open(bfilename, mode | O_CREAT | O_TRUNC, 0600);
	}
	else if (flags & (FILE_ACCESS_WRITE | FILE_ACCESS_APPEND))
	{
		if (flags & FILE_OPEN_ALWAYS) mode |= O_CREAT;
		d = open(bfilename, mode, 0600);
	}
	else
	{
		d = open(bfilename, mode);
		if (d < 0)
		{
			char path[512];
			snprintf(path, lengthof(path), "%s/" BASEDATADIR "/%s", GetBinDataPath(), bfilename);
			d = open(path, mode);
			if (d < 0)
			{
				if (OpenFileFromLibrary(bfilename, &file->u.lib)) return file.Release();

				if (flags & FILE_OPEN_ALWAYS)
				{
					d = open(bfilename, mode | O_CREAT, 0600);
				}
			}
		}
	}

	std::string sError("Opening file failed: ");
	sError += filename;
	sError += " (from fd)";
	if (d < 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Opening file failed! %s", bfilename);
		throw std::runtime_error(sError);
	}

	FILE* const f = fdopen(d, fmode);
	if (!f)
	{
		close(d);
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Opening file failed! %s", bfilename);
		throw std::runtime_error(sError);
	}

	file->flags  = SGPFILE_REAL;
	file->u.file = f;
	return file.Release();
}


void FileClose(const HWFILE f)
{
	if (f->flags & SGPFILE_REAL)
	{
		fclose(f->u.file);
	}
	else
	{
		CloseLibraryFile(&f->u.lib);
	}
	MemFree(f);
}


#ifdef JA2TESTVERSION
#	include "Timer_Control.h"
extern UINT32 uiTotalFileReadTime;
extern UINT32 uiTotalFileReadCalls;
#endif

void FileRead(HWFILE const f, void* const pDest, UINT32 const uiBytesToRead)
{
#ifdef JA2TESTVERSION
	const UINT32 uiStartTime = GetJA2Clock();
#endif

	BOOLEAN ret;
	if (f->flags & SGPFILE_REAL)
	{
		ret = fread(pDest, uiBytesToRead, 1, f->u.file) == 1;
	}
	else
	{
		ret = LoadDataFromLibrary(&f->u.lib, pDest, uiBytesToRead);
	}

#ifdef JA2TESTVERSION
	//Add the time that we spent in this function to the total.
	uiTotalFileReadTime += GetJA2Clock() - uiStartTime;
	uiTotalFileReadCalls++;
#endif

	if (!ret) {
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: reading from file failed!");
		throw std::runtime_error("Reading from file failed");
	}
}


void FileWrite(HWFILE const f, void const* const pDest, UINT32 const uiBytesToWrite)
{
	if (!(f->flags & SGPFILE_REAL)) {
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Tried to write to library file");
		throw std::logic_error("Tried to write to library file");
	}
	if (fwrite(pDest, uiBytesToWrite, 1, f->u.file) != 1) {
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Writing to file failed");
		throw std::runtime_error("Writing to file failed");
	}
}


void FileSeek(HWFILE const f, INT32 distance, FileSeekMode const how)
{
	bool success;
	if (f->flags & SGPFILE_REAL)
	{
		int whence;
		switch (how)
		{
			case FILE_SEEK_FROM_START: whence = SEEK_SET; break;
			case FILE_SEEK_FROM_END:   whence = SEEK_END; break;
			default:                   whence = SEEK_CUR; break;
		}

		success = fseek(f->u.file, distance, whence) == 0;
	}
	else
	{
		success = LibraryFileSeek(&f->u.lib, distance, how);
	}
	if (!success) {
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Seek in file failed!");
		throw std::runtime_error("Seek in file failed");
	}
}


INT32 FileGetPos(const HWFILE f)
{
	return f->flags & SGPFILE_REAL ? ftell(f->u.file) : f->u.lib.uiFilePosInFile;
}


UINT32 FileGetSize(const HWFILE f)
{
	if (f->flags & SGPFILE_REAL)
	{
		struct stat sb;
		if (fstat(fileno(f->u.file), &sb) != 0)
		{
			__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: getting filesize failed!");
			throw std::runtime_error("Getting file size failed");
		}
		return sb.st_size;
	}
	else
	{
		return f->u.lib.pFileHeader->uiFileLength;
	}
}


void SetFileManCurrentDirectory(char const* const pcDirectory)
{
#if 1 // XXX TODO
	if (chdir(pcDirectory) != 0)
#else
	if (!SetCurrentDirectory(pcDirectory))
#endif
	{
		__android_log_print(ANDROID_LOG_INFO, "==TEST==", "RUNTIME ERROR: Change directory failed");
		throw std::runtime_error("Changing directory failed");
	}
}


void MakeFileManDirectory(char const* const path)
{
	if (mkdir("/sdcard/app-data/com.opensourced.ja2/temp", 0755) == 0) return;

	if (errno == EEXIST)
	{
		FileAttributes const attr = FileGetAttributes(path);
		if (attr != FILE_ATTR_ERROR && attr & FILE_ATTR_DIRECTORY) return;
	}
	__android_log_print(ANDROID_LOG_INFO, "==TEST==", "create directory failed: %s", path);
	//throw std::runtime_error("Failed to create directory");
}


void EraseDirectory(char const* const path)
{
	char pattern[512];
	snprintf(pattern, lengthof(pattern), "%s/*", path);

	SGP::FindFiles find(pattern);
	while (char const* const find_filename = find.Next())
	{
		char filename[512];
		snprintf(filename, lengthof(filename), "%s/%s", path, find_filename);

		try
		{
			FileDelete(filename);
		}
		catch (...)
		{
			const FileAttributes attr = FileGetAttributes(filename);
			if (attr != FILE_ATTR_ERROR && attr & FILE_ATTR_DIRECTORY) continue;
			throw;
		}
	}
}


const char* GetExecutableDirectory(void)
{
	return LocalPath;
}


FileAttributes FileGetAttributes(const char* const filename)
{
	FileAttributes attr = FILE_ATTR_NONE;
#ifndef _WIN32 // XXX TODO
	struct stat sb;
	if (stat(filename, &sb) != 0) return FILE_ATTR_ERROR;

	if (S_ISDIR(sb.st_mode))     attr |= FILE_ATTR_DIRECTORY;
	if (!(sb.st_mode & S_IWUSR)) attr |= FILE_ATTR_READONLY;
#else
	const UINT32 w32attr = GetFileAttributes(filename);
	if (w32attr == INVALID_FILE_ATTRIBUTES) return FILE_ATTR_ERROR;

	if (w32attr & FILE_ATTRIBUTE_READONLY)  attr |= FILE_ATTR_READONLY;
	if (w32attr & FILE_ATTRIBUTE_DIRECTORY) attr |= FILE_ATTR_DIRECTORY;
#endif
	return attr;
}


BOOLEAN FileClearAttributes(const char* const filename)
{
#if 1 // XXX TODO
#	if defined WITH_FIXMES
	fprintf(stderr, "===> %s:%d: IGNORING %s(\"%s\")\n", __FILE__, __LINE__, __func__, filename);
#	endif
	return FALSE;
	UNIMPLEMENTED
#else
	return SetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);
#endif
}


BOOLEAN GetFileManFileTime(const HWFILE f, SGP_FILETIME* const pCreationTime, SGP_FILETIME* const pLastAccessedTime, SGP_FILETIME* const pLastWriteTime)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	//Initialize the passed in variables
	memset(pCreationTime,     0, sizeof(*pCreationTime));
	memset(pLastAccessedTime, 0, sizeof(*pLastAccessedTime));
	memset(pLastWriteTime,    0, sizeof(*pLastWriteTime));

	if (f->flags & SGPFILE_REAL)
	{
		const HANDLE hRealFile = f->u.file;

		//Gets the UTC file time for the 'real' file
		SGP_FILETIME sCreationUtcFileTime;
		SGP_FILETIME sLastAccessedUtcFileTime;
		SGP_FILETIME sLastWriteUtcFileTime;
		GetFileTime(hRealFile, &sCreationUtcFileTime, &sLastAccessedUtcFileTime, &sLastWriteUtcFileTime);

		//converts the creation UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sCreationUtcFileTime, pCreationTime);

		//converts the accessed UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sLastAccessedUtcFileTime, pLastAccessedTime);

		//converts the write UTC file time to the current time used for the file
		FileTimeToLocalFileTime(&sLastWriteUtcFileTime, pLastWriteTime);
		return TRUE;
	}
	else
	{
		return GetLibraryFileTime(&f->u.lib, pLastWriteTime);
	}
#endif
}


INT32	CompareSGPFileTimes(const SGP_FILETIME* const pFirstFileTime, const SGP_FILETIME* const pSecondFileTime)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	return CompareFileTime(pFirstFileTime, pSecondFileTime);
#endif
}


FILE* GetRealFileHandleFromFileManFileHandle(const HWFILE f)
{
	return f->flags & SGPFILE_REAL ? f->u.file : f->u.lib.lib->hLibraryHandle;
}


static UINT32 GetFreeSpaceOnHardDrive(const char* pzDriveLetter);


UINT32 GetFreeSpaceOnHardDriveWhereGameIsRunningFrom(void)
{
#if 1 // XXX TODO
	FIXME
	return 1024 * 1024 * 1024; // XXX TODO return an arbitrary number for now
#else
	//get the drive letter from the exec dir
  STRING512 zDrive;
	_splitpath(GetExecutableDirectory(), zDrive, NULL, NULL, NULL);

	sprintf(zDrive, "%s\\", zDrive);
	return GetFreeSpaceOnHardDrive(zDrive);
#endif
}


static UINT32 GetFreeSpaceOnHardDrive(const char* const pzDriveLetter)
{
#if 1 // XXX TODO
	UNIMPLEMENTED
#else
	UINT32 uiSectorsPerCluster     = 0;
	UINT32 uiBytesPerSector        = 0;
	UINT32 uiNumberOfFreeClusters  = 0;
	UINT32 uiTotalNumberOfClusters = 0;
	if (!GetDiskFreeSpace(pzDriveLetter, &uiSectorsPerCluster, &uiBytesPerSector, &uiNumberOfFreeClusters, &uiTotalNumberOfClusters))
	{
		const UINT32 uiLastError = GetLastError();
		char zString[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, uiLastError, 0, zString, 1024, NULL);
		return TRUE;
	}

	return uiBytesPerSector * uiNumberOfFreeClusters * uiSectorsPerCluster;
#endif
}


const char* GetBinDataPath(void)
{
	return ConfigGetValue(BinDataDir);
}
