
// -------------------------------------------------------------
// LibFile
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
// -------------------------------------------------------------

#ifndef LUAEXT_DEFINE
#define LUAEXT_DEFINE

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <fstream>
#include <map>
#include <clocale>
#include <cstdlib>

typedef wchar_t char_t;
#define uT(x)   L##x

#if defined(__WXMSW__)
#define LM_NEWLINE       uT("\r\n")
#define LM_NEWLINE_CHAR  uT('\n')
#define LM_NEWLINEA      ("\r\n")
#define LM_NEWLINE_CHARA ('\n')
#else
#if defined(__WXMAC__)
#define LM_NEWLINE       uT("\r")
#define LM_NEWLINE_CHAR  uT('\r')
#define LM_NEWLINEA      ("\r")
#define LM_NEWLINE_CHARA ('\r')
#else
#define LM_NEWLINE       uT("\n")
#define LM_NEWLINE_CHAR  uT('\n')
#define LM_NEWLINEA      ("\n")
#define LM_NEWLINE_CHARA ('\n')
#endif
#endif

#ifdef WIN32
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma intrinsic(memcmp, memcpy, memset, abs, fabs)
#endif

#define LM_NONE 0 // Nothing

#define LM_MAX_MEMORY      268435456 // maximum allowed memory = 256 MB
#define LM_MAX_POINTS      33554432  // maximum memory = 256 MB for double float
#define LM_MAX_POINTS_TINY 131072    // maximum memory = 1 MB for double float
#define LM_DEF_POINTS      201

// Constantes fondamentales
#define LM_PI      3.14159265358979323846264338327950
#define LM_2PI     6.28318530717958647692528676655900
#define LM_PI2     1.57079632679489661923132169163975
#define LM_PI4     0.78539816339744830961566084581987
#define LM_SQRTPI2 1.77245385090551602729816748334110
#define LM_SQRT2   1.41421356237309504880168872420969
#define LM_SQRT22  0.70710678118654752440084436210484

// Text size
#define LM_STRSIZET 16   // Tiny
#define LM_STRSIZES 32   // Small
#define LM_STRSIZEN 64   // Normal
#define LM_STRSIZEM 128  // Normal
#define LM_STRSIZE  256  // Normal
#define LM_STRSIZEC 384  // Command
#define LM_STRSIZEW 1024 // Wide
#define LM_STRSIZEL 8192 // Large

#define BINSYMBOL(c) (((c) >= uT('0')) && ((c) <= uT('1')))
#define OCTSYMBOL(c) (((c) >= uT('0')) && ((c) <= uT('7')))
#define DECSYMBOL(c) (((c) >= uT('0')) && ((c) <= uT('9')))
#define HEXSYMBOL(c) ((((c) >= uT('0')) && ((c) <= uT('9'))) || (((c) >= uT('A')) && ((c) <= uT('F'))) || (((c) >= uT('a')) && ((c) <= uT('f'))))

#include <thread>
#include <chrono>
inline void Tsleep(int iMilliseconds)
{
#if defined(WIN32) || (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
    std::this_thread::sleep_for(std::chrono::milliseconds(iMilliseconds));
#else
#include <unistd.h> // for usleep
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = iMilliseconds / 1000;
    ts.tv_nsec = (iMilliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(iMilliseconds * 1000);
#endif
#endif
}

// Unicode support
#define Tmemset(psz, c, len) memset(psz, c, len * sizeof(char_t))
#define Tmemcpy(pl, pm, len) memcpy(pl, pm, len * sizeof(char_t))
#define Tstrncpy             wcsncpy
#define Tstrcpy              wcscpy
#define Tstrcat              wcscat
#define Tstrncat             wcsncat
#define Tsprintf             swprintf
#define Tsscanf              swscanf
#define Tstrlen              wcslen
#define Tstrcmp              wcscmp
#define Tstrstr              wcsstr
#define Tstrrchr             wcsrchr
#define Tstrchr              wcschr
#define Tstrtod              wcstod
#define Tisalpha             iswalpha
#define Tisalnum             iswalnum
#define Tisspace             iswspace
#define Tprintf              wprintf
#define Tscanf               wscanf
#define Tstrtol              wcstol
#define Tfputs               fputws
inline int Tputs(const char_t *szT)
{
    // this version of putws do not append newline
    return Tfputs(szT, stdout);
}

inline size_t Tstartwith(const char_t *pszT, const char_t *pszItem)
{
    size_t iLenT = Tstrlen(pszT),
           iLenItem = Tstrlen(pszItem);
    if (iLenT < iLenItem) {
        return 0;
    }
    for (size_t ii = 0; ii < iLenItem; ii++) {
        if (pszT[ii] != pszItem[ii]) {
            return 0;
        }
    }
    return iLenItem;
}

inline const char *Tltoa(long ivalue, char *szT, int ibase)
{
    switch (ibase) {
        case 10:
            sprintf(szT, "%ld", ivalue);
            break;
        case 16:
            sprintf(szT, "%X", static_cast<unsigned int>(ivalue));
            break;
        default:
            break;
    }
    return static_cast<const char *>(szT);
}

inline const char *Tltoa(int ivalue, char *szT, int ibase)
{
    return Tltoa(static_cast<long>(ivalue), szT, ibase);
}

#ifdef WIN32

#define Tstricmp  wcsicmp
#define Tsnprintf _snwprintf
#define Tfopen    _wfopen

#else

#include <locale.h>

#define Tstricmp  wcscmp
#define Tsnprintf swprintf

inline int doWC2UTF8(char *tUTF8, wchar_t tWC, size_t tcount)
{
    int tlen = 0;
    unsigned int iWC = (unsigned int)tWC;

    if (iWC < 0x80) {
        if (tcount >= 1) {
            tUTF8[tlen++] = (char)iWC;
        }
    }
    else if (iWC < 0x800) {
        if (tcount >= 2) {
            tUTF8[tlen++] = 0xc0 | (iWC >> 6);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x10000) {
        if (tcount >= 3) {
            tUTF8[tlen++] = 0xe0 | (iWC >> 12);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x200000) {
        if (tcount >= 4) {
            tUTF8[tlen++] = 0xf0 | (iWC >> 18);
            tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x4000000) {
        if (tcount >= 5) {
            tUTF8[tlen++] = 0xf8 | (iWC >> 24);
            tUTF8[tlen++] = 0x80 | ((iWC >> 18) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x80000000) {
        if (tcount >= 6) {
            tUTF8[tlen++] = 0xfc | (iWC >> 30);
            tUTF8[tlen++] = 0x80 | ((iWC >> 24) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 18) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else {
        tlen = -1;
    }

    return tlen;
}

inline int WC2UTF8(char *tUTF8, const wchar_t *tWC, size_t tcount)
{
    char *pUTF8 = tUTF8;
    int nn;

    while (*tWC) {
        nn = doWC2UTF8(pUTF8, *tWC++, tcount);
        if (nn <= 0) {
            break;
        }
        pUTF8 += nn;
        tcount -= nn;
    }

    if (nn == 0) {
        while (tcount--) {
            *pUTF8++ = 0;
        }
    }
    else if (tcount) {
        *pUTF8 = 0;
    }

    if (nn == -1) {
        return -1;
    }

    return (pUTF8 - tUTF8);
}

inline FILE *Tfopen(const char_t *pszFilename, const char_t *pszMode)
{
    char szFilenameA[LM_STRSIZEW];
    char szModeA[LM_STRSIZET];

    memset(szModeA, 0, LM_STRSIZET * sizeof(char));
    for (size_t ii = 0; ii < Tstrlen(pszMode) + 1; ii++) {
        szModeA[ii] = (char)pszMode[ii];
    }

    memset(szFilenameA, 0, LM_STRSIZEW * sizeof(char));
    WC2UTF8(szFilenameA, pszFilename, LM_STRSIZEW - 1);

    return fopen((const char *)szFilenameA, (const char *)szModeA);
}

#endif

//

#ifdef WIN32
#define snprintf _snprintf
#endif

// Datasheet limits
#define LM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define LM_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LM_SWAP(a, b)      \
    {                      \
        double sffs = (a); \
        (a) = (b);         \
        (b) = sffs;        \
    }
#define LM_SWAPI(a, b)  \
    {                   \
        int sffs = (a); \
        (a) = (b);      \
        (b) = sffs;     \
    }
#define LM_SWAPL(a, b)   \
    {                    \
        long sffs = (a); \
        (a) = (b);       \
        (b) = sffs;      \
    }
#define LM_REAL_DEFAULT 28.4375

// Numeric format
#define LM_FORMAT_DATE   uT("%d-%m-%Y %H:%M:%S")
#define LM_FORMAT_FSHORT uT("%.6f")
#define LM_FORMAT_FLONG  uT("%.12f")
#define LM_FORMAT_ESHORT uT("%.4e")
#define LM_FORMAT_ELONG  uT("%.8e")

// Error handling
#define LM_SIGMA_INFO  0
#define LM_SIGMA_ERROR 1

// >> Fixed size for disk storing
#ifdef _MSC_VER

typedef unsigned __int8 lmByte;
typedef unsigned __int16 lmUShort;
typedef unsigned __int32 lmUInt;
typedef unsigned __int32 lmULong;
typedef __int16 lmShort;
typedef __int32 lmInt;
typedef __int32 lmLong;
typedef double lmDouble;
typedef unsigned __int32 lmChar;

#else

#include <stdint.h>
typedef uint8_t lmByte;
typedef uint16_t lmUShort;
typedef uint32_t lmUInt;
typedef uint32_t lmULong;
typedef int16_t lmShort;
typedef int32_t lmInt;
typedef int32_t lmLong;
typedef double lmDouble;
typedef uint32_t lmChar;

#endif
// <<

// Attention : 0xGBR (0xFF0000 is green and 0x0000FF is red)
#define LM_COLOR(r, g, b) ((unsigned long)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((unsigned short)(unsigned char)(b)) << 16)))
#define LM_COLORB(rgb)    ((unsigned char)((((unsigned short)(rgb >> 16))) & 0xFF))
#define LM_COLORG(rgb)    ((unsigned char)((((unsigned short)(rgb >> 8))) & 0xFF))
#define LM_COLORR(rgb)    ((unsigned char)(((unsigned short)(rgb)) & 0xFF))

#define LM_BLOCK_START uT('{')
#define LM_BLOCK_END   uT('}')

// Defined as macros, then do not need wxWidgets headers to be included
#define LM_CSTR(strT)   static_cast<const char_t *>(strT.c_str())
#define LM_U8STR(strT)  static_cast<const char *>(strT.mb_str(wxConvUTF8))
#define LM_U8TOWC(pszT) wxString(static_cast<const char *>(pszT), wxConvUTF8)
//

#if (__GNUC__ > 8) || (__GNUC__ == 8 && __GNUC_MINOR__ >= 1)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
#endif

inline bool lm_isEqual(double tx, double ty)
{
    if (tx == ty) {
        return true;
    }
    return (fabs(tx - ty) <= (DBL_EPSILON * (fabs(tx) + fabs(ty))));
}

inline bool lm_isNaN(double tx)
{
    return ((tx != tx) || ((tx == tx) && ((tx - tx) != 0.0)));
}

inline bool lm_isNaN(int ti)
{
    return ((ti != ti) || ((ti == ti) && ((ti - ti) != 0)));
}

#if (__GNUC__ > 8) || (__GNUC__ == 8 && __GNUC_MINOR__ >= 1)
#pragma GCC diagnostic pop
#endif

inline int lm_iSqrt(int inumber)
{
    register int iop, ires, ione;

    iop = inumber;
    ires = 0;

    ione = 1 << 30;
    while (ione > iop) {
        ione >>= 2;
    }
    while (ione != 0) {
        if (iop >= ires + ione) {
            iop -= ires + ione;
            ires += (ione << 1);
        }
        ires >>= 1;
        ione >>= 2;
    }
    return ires;
}

#endif