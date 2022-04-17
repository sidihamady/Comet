// -------------------------------------------------------------
// LibFile
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
// -------------------------------------------------------------

#ifndef LIBFILE_H
#define LIBFILE_H

#if defined(SIGMA_BUILD_AS_DLL)

#if defined(LIBFILE_CORE)
#define LIBFILE_API __declspec(dllexport)
#else
#define LIBFILE_API __declspec(dllimport)
#endif

#else

#define LIBFILE_API

#endif

#include "Define.h"

#define FILE_VERSION_NUMBER 188
#define FILE_BUILD_NUMBER   2104
#define FILE_VERSION_STRING uT("1.8")
#define FILE_BUILD_STRING   uT("2104") // yymmdd (year, month, day)
#define FILE_HEADER_STRING  uT("# Comet-RC <do not change or remove this header>")

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
#endif

#ifdef WIN32

#define Tunlink _wunlink

#else

#include <unistd.h>
inline int Tunlink(const char_t *pszFilename)
{
    if (NULL == pszFilename) {
        return -1;
    }
    char szT[LM_STRSIZE];
    memset(szT, 0, LM_STRSIZE * sizeof(char));
    const char_t *pszT = pszFilename;
    std::wcsrtombs(szT, &pszT, Tstrlen(pszFilename), NULL);
    return unlink((const char *)szT);
}

#endif

inline char_t *Ttmpnam(char_t *pszFilename)
{
#ifdef WIN32
    char_t *pszT;
    if ((pszT = _wtempnam(uT("c:\\tmp"), uT("lm"))) == NULL) {
        return NULL;
    }
    Tstrcpy(pszFilename, static_cast<const char_t *>(pszT));
    free(pszT);
#else
    char szT[LM_STRSIZE];
    memset(szT, 0, LM_STRSIZE * sizeof(char));
    strcpy(szT, "/tmp/cometXXXXXX");
    int fd = mkstemp(szT);
    if (fd >= 0) {
        close(fd);
        unlink((const char *) szT);
        const char *pszT = (const char *)szT;
        std::mbstate_t tstate = std::mbstate_t();
        std::mbsrtowcs(pszFilename, &pszT, strlen(szT), &tstate);
    }
#endif
    return static_cast<char_t *>(pszFilename);
}

#define Tfgetc    fgetwc
#define Tfputc    fputwc
#define Tfputs    fputws
#define Tfgets    fgetws
#define Tfscanf   fwscanf
#define Tfprintf  fwprintf
#define Tstrftime wcsftime
//

inline char_t *Tnow(char_t *pszTime)
{
    Tmemset(pszTime, 0, LM_STRSIZEN);
    time_t nowT = time(NULL);
    tm *timeT = localtime(&nowT);
    Tstrftime(pszTime, LM_STRSIZEN - 1, uT("%Y-%m-%d %H:%M:%S"), timeT);
    return pszTime;
}

// Datasheet limits
#define LF_MAX_DOCUMENTS 32
#define LF_MAX_COLUMNS   64
#define LF_MAX_CELLS     LM_MAX_POINTS
#define LF_MAX_FILESIZE  LM_MAX_MEMORY

#define LF_SCRIPT_MAXLINES    262144                                // 262144 lines
#define LF_SCRIPT_MAXLINES_SM 4096                                  // 4096 lines
#define LF_SCRIPT_MAXCHARS    (LF_SCRIPT_MAXLINES * LM_STRSIZEW)    // 256 MB
#define LF_SCRIPT_MAXCHARS_SM (LF_SCRIPT_MAXLINES_SM * LM_STRSIZEW) // 4 MB

#define LF_CONFIG_MAXSECTION 32
#define LF_CONFIG_COMMENT    uT('#')
#define LF_CONFIG_MAXRECORD  128
#define LF_CONFIG_MAXLINES   (LF_CONFIG_MAXSECTION * LF_CONFIG_MAXRECORD * 4)

inline bool lf_isWhiteChar(char_t cT)
{
    return ((cT == uT(' ')) || (cT == uT('\t')) || (cT == uT('\r')) || (cT == uT('\n')));
}

inline bool lf_isEOL(char_t cT)
{
    return ((cT == uT('\r')) || (cT == uT('\n')));
}

// Endian functions
inline lmByte lf_getEndian(void)
{
    int iT = 1;
    lmByte *pp = reinterpret_cast<lmByte *>(&iT);

    if (pp[0] == 1) {
        return 0x10; // Little-endian
    }
    return 0x20; // Big-endian
}

inline void lf_swapIt(lmByte *pdat, int isizeof)
{
    register int ii;
    register int jj;
    lmByte bt;

    ii = 0;
    jj = isizeof - 1;
    while (ii < jj) {
        bt = pdat[ii];
        pdat[ii] = pdat[jj];
        pdat[jj] = bt;
        ii++;
        jj--;
    }
}
#define lm_swapEndian(dat) lf_swapIt((lmByte *)(&(dat)), (int)sizeof(dat))

LIBFILE_API bool lf_convertUTF16to32(const char_t *pszSource, lmChar *pTarget, int iLen);
LIBFILE_API bool lf_convertUTF32to16(const lmChar *pSource, char_t *pszTarget, int iLen);

inline long lf_getFileSize(const char_t *pszFilename)
{
    if (pszFilename == NULL) {
        return 0L;
    }

    FILE *pf = Tfopen(pszFilename, uT("rb"));
    if (pf == NULL) {
        return -1L;
    }

    fseek(pf, 0L, SEEK_END);
    long fileSize = ftell(pf);
    fseek(pf, 0L, SEEK_SET);
    fclose(pf);

    return fileSize;
}

inline long lf_getFileSizeA(const char *pszFilename)
{
    if (pszFilename == NULL) {
        return 0L;
    }

    FILE *pf = fopen(pszFilename, ("r"));
    if (pf == NULL) {
        return -1L;
    }

    fseek(pf, 0L, SEEK_END);
    long fileSize = ftell(pf);
    fseek(pf, 0L, SEEK_SET);
    fclose(pf);

    return fileSize;
}

inline int lf_countNewline(const char_t *pszLine, int iLen)
{
    if (iLen < 1) {
        return 0;
    }

    int iC = 0, ii = 0;

    for (;;) {

        // Windows CRLF
        if (pszLine[ii] == uT('\r')) {
            iC += 1;
            ii += 1;

            if (ii < iLen) {
                if (pszLine[ii] == uT('\n')) {
                    ii += 1;
                }
            }

            if (ii >= iLen) {
                break;
            }

            continue;
        }

        // Unix LF
        if (pszLine[ii] == uT('\n')) {
            iC += 1;
        }

        ii += 1;

        if (ii >= iLen) {
            break;
        }
    }

    return iC;
}

LIBFILE_API int lf_getShortFilename(char_t *pszFilenameShort, const char_t *pfilename);

/*    -------------------------------------------------------------
    lfRecord
    Managing file storing
    ------------------------------------------------------------- */
class lfRecord
{
private:
    // disable the copy constructor
    lfRecord(const lfRecord &);
    lfRecord &operator=(lfRecord const &);

    // disable the default constructor
    lfRecord();

    // :SPEED: :MEMORY:
    // Must be kept as small as possible
    char_t m_szKey[LM_STRSIZEN];
    char_t m_szValue[LM_STRSIZE];
    //

public:
    // Constructors
    LIBFILE_API lfRecord(const char_t *pszKey, const char_t *pszValue);

    // Destructor
    LIBFILE_API ~lfRecord();

    // Update data
    LIBFILE_API void update(const char_t *pszKey, const char_t *pszValue);

    // Getters/Setters
    LIBFILE_API void getKey(char_t *pszKey) const;

    LIBFILE_API const char_t *getKey(void) const;

    LIBFILE_API void getValue(char_t *pszValue) const;

    LIBFILE_API const char_t *getValue(void) const;

    LIBFILE_API void copy(lfRecord *pRecord);

    LIBFILE_API bool isEqual(const char_t *pszKey) const;
};

/*    -------------------------------------------------------------
    lfSection
    Managing file storing
    ------------------------------------------------------------- */
class lfSection
{
private:
    // disable the copy constructor
    lfSection(const lfSection &);
    lfSection &operator=(lfSection const &);

    // disable the default constructor
    lfSection();

    // :SPEED: :MEMORY:
    // Must be kept as small as possible (otherwise, the memory usage increases ...)
    char_t m_szName[LM_STRSIZEN];
    lfRecord **m_ppRecord;
    int m_iRecordCount;
    int m_iMaxRecordCount;
    //

public:
    // Constructors
    LIBFILE_API explicit lfSection(const char_t *pszName, int iMaxRecordCount);

    // Destructor
    LIBFILE_API ~lfSection();

    LIBFILE_API int getRecordCount() const;

    LIBFILE_API lfRecord *getRecord(const char_t *pszKey);

    LIBFILE_API lfRecord *getRecord(int ii);

    // Update data
    LIBFILE_API void setName(const char_t *pszName);

    LIBFILE_API void getName(char_t *pszName) const;

    LIBFILE_API const char_t *getName(void) const;

    LIBFILE_API bool setValue(const char_t *pszKey, const char_t *pszValue);

    LIBFILE_API bool getValue(const char_t *pszKey, char_t *pszValue) const;

    LIBFILE_API bool removeKey(const char_t *pszKey);

    LIBFILE_API bool isEqual(const char_t *szSection) const;

    LIBFILE_API void copy(lfSection *pSection);
};

class lfConfig
{
private:
    // disable the copy constructor
    lfConfig(const lfConfig &);
    lfConfig &operator=(lfConfig const &);

    // disable the default constructor
    lfConfig();

protected:
    lfSection **m_ppSection;
    int m_iSectionCount;
    int m_iMaxSectionCount;

public:
    // Constructors
    LIBFILE_API explicit lfConfig(int iMaxSectionCount);

    // Destructor
    LIBFILE_API ~lfConfig();

    LIBFILE_API int trim(char_t *szBuffer);

    LIBFILE_API int getSectionCount() const;

    LIBFILE_API lfSection *getSection(const char_t *pszSection);

    LIBFILE_API lfSection *getSection(int ii);

    LIBFILE_API lfSection *addSection(const char_t *pszSection, int iMaxRecordCount);

    LIBFILE_API void removeAll();

    LIBFILE_API bool setValue(const char_t *pszSection, const char_t *pszKey, const char_t *pszValue);

    LIBFILE_API bool getValue(const char_t *pszSection, const char_t *pszKey, char_t *pszValue) const;
};

#ifdef WIN32

LIBFILE_API void win32ClearConsole(void);
LIBFILE_API void win32EnableConsole(void);
LIBFILE_API void win32SendEnterKey(void);

#endif

inline void lf_println(void)
{
#ifdef WIN32
    win32SendEnterKey();
#else
    printf("\n");
#endif
}

LIBFILE_API bool lf_redirectConsole(const char_t *pszFilename, FILE **fpout, FILE **fperr);

LIBFILE_API int lf_fileExists(const char *pszFilename);
LIBFILE_API int lf_dirExists(const char *pszDirname);
LIBFILE_API int lf_mkdir(const char *pszDirname);
LIBFILE_API bool lf_fileReadonly(const char_t *pszFilename);
LIBFILE_API bool lf_fileReadWrite(const char_t *pszFilename);
LIBFILE_API char *lf_realpath(const char *pszRelPath, char *pszFullPath);
LIBFILE_API bool lf_getCurrentDir(char *pszCurDir);
LIBFILE_API bool lf_createOutputDir(const char *pszInputFilename,
                                    const char_t *pszLogFilenameW, char_t *pszInputFilenameW, char_t *pszOutputDirW, char_t *pszLogFileW);
LIBFILE_API bool lf_comparePath(const char_t *pszPathA, const char_t *pszPathB, char_t *pszPathAA);

class lmMessageSender
{
protected:
    void *m_ptr;

public:
    LIBFILE_API lmMessageSender()
    {
        m_ptr = NULL; // allocated/deleted elsewhere
    }

    LIBFILE_API virtual ~lmMessageSender()
    {
        m_ptr = NULL; // allocated/deleted elsewhere
    }

    LIBFILE_API virtual void send(const char_t *pszMessage) = 0;
    LIBFILE_API virtual bool stop(void) = 0;
    LIBFILE_API virtual void set(void *ptr) = 0;
};

class lmSigma
{

private:
    // disable the copy constructor
    lmSigma(const lmSigma &);
    lmSigma &operator=(lmSigma const &);

    size_t m_iMaxUsedMemory;                 // Max allocated memory (bytes)
    size_t m_iUsedMemory;                    // Allocated memory (bytes)
    std::map<lmByte *, size_t> m_MemoryBt;   // Map containing pointer/size pairs allocated on the heap (Bytes)
    std::map<char_t *, size_t> m_MemoryStr;  // Map containing pointer/size pairs allocated on the heap (Unicode chars)
    std::map<char *, size_t> m_MemoryStrA;   // Map containing pointer/size pairs allocated on the heap (Ansi chars)
    std::map<int *, size_t> m_MemoryInt;     // Map containing pointer/size pairs allocated on the heap (integers)
    std::map<double *, size_t> m_MemoryDbl;  // Map containing pointer/size pairs allocated on the heap (Doubles)
    std::map<double **, size_t> m_MemoryPtr; // Map containing pointer/size pairs allocated on the heap (Double pointers)

    int m_iCode;                    // Code du message
    char_t m_szMessage[LM_STRSIZE]; // Message (erreur, warning, info, ...)
    char_t m_szSource[LM_STRSIZEN]; // Source du message
    char_t m_szAdvice[LM_STRSIZE];  // Conseil (pour éventuellement corriger une erreur ...)

    lmChar m_iOutputType;                  // Output type: 0x10 = screen, 0x01 = file, 0x11 = both
    char_t m_szOutputFilename[LM_STRSIZE]; // Output filename

    lmMessageSender *m_pMessageSender;
    bool m_bStop;

    bool m_bDebug;

    bool m_bSaveFull;

    // freeAll only called by the destructor
    LIBFILE_API void freeAll(void);

public:
    LIBFILE_API lmSigma()
    {
        m_iMaxUsedMemory = 0; // Max allocated memory (bytes)
        m_iUsedMemory = 0;    // Allocated memory (bytes)
        m_iOutputType = 0x10;
        Tmemset(m_szOutputFilename, 0, LM_STRSIZE);
        m_bDebug = false;
        m_pMessageSender = NULL;
        m_bStop = false;
        m_bSaveFull = false;
        reset();
    }

    LIBFILE_API ~lmSigma()
    {
        // free unused memory, if necessary
        freeAll();

        m_pMessageSender = NULL;
    }

    // La gestion de la mémoire se fait via ces méthodes
    LIBFILE_API lmByte *mallocBt(int iDim, bool bZeroMem = true);
    LIBFILE_API lmByte *reallocBt(lmByte *ptr, int iNewDim);
    LIBFILE_API char_t *mallocChar(int iDim, bool bZeroMem = true);
    LIBFILE_API char *mallocCharA(int iDim, bool bZeroMem = true);
    LIBFILE_API int *mallocInt(int iDim, bool bZeroMem = true);
    LIBFILE_API double *mallocDbl(int iDim, bool bZeroMem = true);
    LIBFILE_API double *reallocDbl(double *ptr, int iNewDim);
    LIBFILE_API double **mallocMat(int iRows, int iCols, bool bZeroMem = true);
    LIBFILE_API void freeBt(lmByte **pm);
    LIBFILE_API void freeStr(char_t **pm);
    LIBFILE_API void freeStrA(char **pm);
    LIBFILE_API void freeInt(int **pm);
    LIBFILE_API void freeDbl(double **pm);
    LIBFILE_API void freeMat(double ***pm);
    LIBFILE_API size_t getUsedMemory(void) const
    {
        return m_iUsedMemory;
    }
    LIBFILE_API size_t getMaxUsedMemory(void) const
    {
        return m_iMaxUsedMemory;
    }

    // Reset data
    void reset(void)
    {
        m_iCode = LM_SIGMA_INFO;
        Tmemset(m_szSource, 0, LM_STRSIZEN);
        Tmemset(m_szMessage, 0, LM_STRSIZE);
        Tmemset(m_szAdvice, 0, LM_STRSIZE);
    }

    LIBFILE_API void copy(const lmSigma &tSigma)
    {
        m_iUsedMemory = tSigma.getUsedMemory(); // Allocated memory (bytes)
        m_iOutputType = tSigma.getOutputType();
        getOutputFilename(m_szOutputFilename);
        m_bDebug = tSigma.isDebug();
        reset();
    }

    // Update data
    LIBFILE_API void update(const char_t *pszMessage, int iCode,
                            const char_t *pszSource = NULL, const char_t *pszAdvice = NULL);

    // Getters/Setters
    LIBFILE_API void getMessage(char_t *pszMessage)
    {
        if (pszMessage) {
            Tstrncpy(pszMessage, static_cast<const char_t *>(m_szMessage), LM_STRSIZE - 1);
        }
    }
    LIBFILE_API char_t *getMessage(void)
    {
        return static_cast<char_t *>(m_szMessage);
    }
    LIBFILE_API int getCode(void) const
    {
        return m_iCode;
    }
    LIBFILE_API void getSource(char_t *pszSource)
    {
        if (pszSource) {
            Tstrncpy(pszSource, static_cast<const char_t *>(m_szSource), LM_STRSIZEN - 1);
        }
    }
    LIBFILE_API char_t *getSource(void)
    {
        return static_cast<char_t *>(m_szSource);
    }
    LIBFILE_API void getAdvice(char_t *pszAdvice)
    {
        if (pszAdvice) {
            Tstrncpy(pszAdvice, static_cast<const char_t *>(m_szAdvice), LM_STRSIZE - 1);
        }
    }
    LIBFILE_API char_t *getAdvice(void)
    {
        return static_cast<char_t *>(m_szAdvice);
    }

    LIBFILE_API void setMessageSender(lmMessageSender *pMessageSender)
    {
        m_pMessageSender = pMessageSender;
    }

    LIBFILE_API bool isMessageSender(void) const
    {
        return (m_pMessageSender != NULL);
    }

    LIBFILE_API void setOutputType(lmByte iOutputType)
    {
        m_iOutputType = iOutputType;
    }
    LIBFILE_API lmByte getOutputType(void) const
    {
        return m_iOutputType;
    }
    LIBFILE_API char_t *getOutputFilename(void)
    {
        return static_cast<char_t *>(m_szOutputFilename);
    }
    LIBFILE_API void getOutputFilename(char_t *pszFilename) const
    {
        if (pszFilename) {
            Tstrncpy(pszFilename, static_cast<const char_t *>(m_szOutputFilename), LM_STRSIZE - 1);
        }
    }
    LIBFILE_API void setOutputFilename(const char_t *pszFilename)
    {
        if (pszFilename) {
            Tstrncpy(m_szOutputFilename, pszFilename, LM_STRSIZE - 1);
        }
    }

    // Methods
    LIBFILE_API bool isOK(void) const
    {
        return (m_iCode == LM_SIGMA_INFO);
    }

    LIBFILE_API bool stopIt(void)
    {
        if (m_bStop) {
            return true;
        }

        if (m_pMessageSender != NULL) {
            m_bStop = m_pMessageSender->stop();
        }

        return m_bStop;
    }

    LIBFILE_API bool saveFull(void) const
    {
        return m_bSaveFull;
    }
    LIBFILE_API void setSaveFull(bool bSaveFull)
    {
        m_bSaveFull = bSaveFull;
    }

    // Debugging functions
    LIBFILE_API bool isDebug(void) const
    {
        return m_bDebug;
    }
    LIBFILE_API void setDebug(bool bDebug)
    {
        m_bDebug = bDebug;
    }
    LIBFILE_API void print(void);
};

/*    -------------------------------------------------------------
    lmDate
    Gestion simplifiée des dates (UTC)
    ------------------------------------------------------------- */
class lmDate
{

private:
    int m_iYear;
    int m_iMonth;
    int m_iDay;
    int m_iHour;
    int m_iMinute;
    int m_iSecond;
    int m_iMillisecond;

public:
    // Constructor
    LIBFILE_API lmDate()
    {
        reset();
    }
    LIBFILE_API lmDate(const lmDate &dateT)
    {
        reset();
        copy(dateT);
    }

    // Destructor
    LIBFILE_API ~lmDate()
    {
    }

    // Reset data
    LIBFILE_API void reset(void)
    {
        m_iYear = 0;
        m_iMonth = 0;
        m_iDay = 0;
        m_iHour = 0;
        m_iMinute = 0;
        m_iSecond = 0;
        m_iMillisecond = 0;

        now();
    }

    // Update Data
    LIBFILE_API void update(int iYear, int iMonth, int iDay, int iHour,
                            int iMinute, int iSecond, int iMillisecond)
    {
        if ((iYear >= 1900) && (iYear <= 3000) && (iMonth >= 1) && (iMonth <= 12) && (iDay >= 1) && (iDay <= 31) && (iHour >= 0) && (iHour <= 24) && (iMinute >= 0) && (iMinute <= 60) && (iSecond >= 0) && (iSecond <= 60) && (iMillisecond >= 0) && (iMillisecond <= 1000)) {

            m_iYear = iYear;
            m_iMonth = iMonth;
            m_iDay = iDay;
            m_iHour = iHour;
            m_iMinute = iMinute;
            m_iSecond = iSecond;
            m_iMillisecond = iMillisecond;
        }
    }

    // Getters/Setters
    LIBFILE_API int getYear(void) const
    {
        return m_iYear;
    }
    LIBFILE_API void setYear(int iYear)
    {
        if ((iYear >= 1900) && (iYear <= 3000)) {
            m_iYear = iYear;
        }
    }

    LIBFILE_API int getMonth(void) const
    {
        return m_iMonth;
    }
    LIBFILE_API void setMonth(int iMonth)
    {
        if ((iMonth >= 1) && (iMonth <= 12)) {
            m_iMonth = iMonth;
        }
    }

    LIBFILE_API int getDay(void) const
    {
        return m_iDay;
    }
    LIBFILE_API void setDay(int iDay)
    {
        if ((iDay >= 1) && (iDay <= 31)) {
            m_iDay = iDay;
        }
    }

    LIBFILE_API int getHour(void) const
    {
        return m_iHour;
    }
    LIBFILE_API void setHour(int iHour)
    {
        if ((iHour >= 0) && (iHour <= 24)) {
            m_iHour = iHour;
        }
    }

    LIBFILE_API int getMinute(void) const
    {
        return m_iMinute;
    }
    LIBFILE_API void setMinute(int iMinute)
    {
        if ((iMinute >= 0) && (iMinute <= 60)) {
            m_iMinute = iMinute;
        }
    }

    LIBFILE_API int getSecond(void) const
    {
        return m_iSecond;
    }
    LIBFILE_API void setSecond(int iSecond)
    {
        if ((iSecond >= 0) && (iSecond <= 60)) {
            m_iSecond = iSecond;
        }
    }

    LIBFILE_API int getMillisecond(void) const
    {
        return m_iMillisecond;
    }
    LIBFILE_API void setMillisecond(int iMillisecond)
    {
        if ((iMillisecond >= 0) && (iMillisecond <= 1000)) {
            m_iMillisecond = iMillisecond;
        }
    }

    // Methods
    LIBFILE_API bool isOK(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMillisecond) const
    {
        return ((iYear >= 1900) && (iYear <= 3000) && (iMonth >= 1) && (iMonth <= 12) && (iDay >= 1) && (iDay <= 31) && (iHour >= 0) && (iHour <= 24) && (iMinute >= 0) && (iMinute <= 60) && (iSecond >= 0) && (iSecond <= 60) && (iMillisecond >= 0) && (iMillisecond <= 1000));
    }

    LIBFILE_API bool isOK(void) const
    {
        return isOK(m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute, m_iSecond, m_iMillisecond);
    }

    LIBFILE_API void now(void)
    {
        time_t tmT;
        time(&tmT);
    }

    LIBFILE_API void copy(const lmDate &dateT)
    {
        m_iYear = dateT.getYear();
        m_iMonth = dateT.getMonth();
        m_iDay = dateT.getDay();
        m_iHour = dateT.getHour();
        m_iMinute = dateT.getMinute();
        m_iSecond = dateT.getSecond();
        m_iMillisecond = dateT.getMillisecond();
    }

    // Operators
    LIBFILE_API lmDate &operator=(const lmDate &dateT)
    {
        if (this == &dateT) {
            return (*this);
        }

        copy(dateT);

        return (*this);
    }
    LIBFILE_API bool operator==(lmDate &dateT) const
    {
        return ((m_iYear == dateT.getYear()) && (m_iMonth == dateT.getMonth()) && (m_iDay == dateT.getDay()) && (m_iHour == dateT.getHour()) && (m_iMinute == dateT.getMinute()) && (m_iSecond == dateT.getSecond()) && (m_iMillisecond == dateT.getMillisecond()));
    }
    LIBFILE_API bool operator!=(lmDate &dateT) const
    {
        return ((m_iYear != dateT.getYear()) || (m_iMonth != dateT.getMonth()) || (m_iDay != dateT.getDay()) || (m_iHour != dateT.getHour()) || (m_iMinute != dateT.getMinute()) || (m_iSecond != dateT.getSecond()) || (m_iMillisecond != dateT.getMillisecond()));
    }
};

LIBFILE_API double lm_toDouble(const char_t *szInput, lmByte *pError = NULL, char_t **pszEnd = NULL);
LIBFILE_API double lm_toDoubleA(const char *szInput, lmByte *pError = NULL, char **pszEnd = NULL);
LIBFILE_API double lm_round(double *fVal, bool bUp = true);
LIBFILE_API double lm_round(long double *fVal, bool bUp = true);
LIBFILE_API int lm_removeSpace(char_t *szBuffer, int *nlen);
LIBFILE_API int lm_getFormat(const char_t *szFmt, int *prefix, int *suffix);
LIBFILE_API int lm_reformatAuto(double fVal, char_t *szBuffer, int nCount, char_t *pszFormat = NULL, int *pFmt = NULL);

inline int lm_trim(char_t *szBuffer)
{
    char_t *psz = NULL;
    int iLen = static_cast<int>(Tstrlen(static_cast<const char_t *>(szBuffer)));
    if ((iLen < 2) || (iLen >= LM_STRSIZEL)) {
        return 0;
    }

    char_t szT[LM_STRSIZEL];
    szT[LM_STRSIZEL - 1] = uT('\0');
    Tstrncpy(szT, static_cast<const char_t *>(szBuffer), iLen);
    szT[iLen] = uT('\0');

    int izl = 0, izr = 0;

    psz = szT + iLen - 1;
    while (psz >= szT) {
        if (((*psz != uT(' ')) && (*psz != uT('\t')) && (*psz != uT('\r')) && (*psz != uT('\n')))) {
            break;
        }
        *psz = uT('\0');
        psz -= 1;
        izr += 1;
    }

    if (psz <= szT) {
        return 0;
    }

    psz = szT;
    while (psz <= (szT + iLen - 1)) {
        if (((*psz != uT(' ')) && (*psz != uT('\t')) && (*psz != uT('\r')) && (*psz != uT('\n')))) {
            break;
        }
        psz += 1;
        izl += 1;
    }

    if ((izl == 0) && (izr == 0)) {
        return iLen;
    }

    Tstrcpy(szBuffer, static_cast<const char_t *>(psz));

    return (iLen - izr - izl);
}

inline unsigned int lm_hash(const char *szT)
{
    unsigned int ihash = 7;
    unsigned int ilen = (unsigned int)(strlen(szT));
    for (unsigned int ii = 0; ii < ilen; ii++) {
        ihash = (ihash * 31) + (unsigned int)szT[ii];
    }
    return ihash;
}

inline unsigned int lm_hash(const char_t *szT)
{
    unsigned int ihash = 7;
    unsigned int ilen = (unsigned int)(Tstrlen(szT));
    for (unsigned int ii = 0; ii < ilen; ii++) {
        ihash = (ihash * 31) + (unsigned int)szT[ii];
    }
    return ihash;
}

#endif
