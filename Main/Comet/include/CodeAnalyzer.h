// -----------------------------------------------------------------------------------
// Comet <Programming Environment for Lua>
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------


#ifndef CODE_ANALYZER_H
#define CODE_ANALYZER_H

#include <vector>

#include <wx/wx.h>

#define ANALYZER_NAMESIZE (LM_STRSIZEN << 1)

struct AnalyzerElement
{
    enum SCOPE
    {
        SCOPE_UNKNOWN = -1,
        SCOPE_GLOBAL = 0,
        SCOPE_LOCAL = 1
    };
    enum ETYPE
    {
        ETYPE_UNKNOWN = -1,
        ETYPE_FUNC = 0,
        ETYPE_FIND = 1
    };

    int id;                         // Index
    int type;                       // Type
    char_t name[ANALYZER_NAMESIZE]; // Function name
    int line;                       // line
    AnalyzerElement::SCOPE scope;   // Scope (local, global)

    void reset(void)
    {
        id = 0;
        type = AnalyzerElement::ETYPE_FUNC;
        Tmemset(name, 0, ANALYZER_NAMESIZE);
        line = 0;
        scope = AnalyzerElement::SCOPE_GLOBAL;
    }

    AnalyzerElement()
    {
        reset();
    }

    AnalyzerElement(int tid, AnalyzerElement::SCOPE tscope, AnalyzerElement::ETYPE ttype, int tline, const char_t *tname)
    {
        id = tid;             // Id
        type = ttype;         // Type
        Tstrcpy(name, tname); // Function name
        line = tline;         // end line
        scope = tscope;       // Scope (local, global)
    }

    ~AnalyzerElement()
    {
    }
};

class CodeAnalyzer
{

private:
    wxString m_strFilename;
    std::vector<AnalyzerElement *> *m_pData;
    int m_iMainLine;

    static bool isSeparator(char_t cT)
    {
        if ((cT == uT(' ')) || (cT == uT('\t')) || (cT == uT('\r')) || (cT == uT('\n'))) {
            return true;
        }
        return false;
    }

    bool isIndexValid(int ii)
    {
        if ((m_pData == NULL) || (ii < 0)) {
            return false;
        }
        return (ii < static_cast<int>(m_pData->size()));
    }

public:
    CodeAnalyzer();
    ~CodeAnalyzer();

    enum LANGUAGE
    {
        LANGUAGE_UNKNOWN = -1,
        LANGUAGE_LUA = 0,
        LANGUAGE_PYTHON = 1,
        LANGUAGE_LATEX = 2,
        LANGUAGE_C = 3,
        LANGUAGE_SOLIS = 4
    };

    static bool isSeparator(const wxString &strT)
    {
        return (strT.IsEmpty());
    }

    void setFilename(wxString strFilename)
    {
        m_strFilename = strFilename;
    }

    static const int MAXCOUNT;

    void reset(void);
    bool analyzeLine(char_t *pszLine, int iLen, int *piId, int *piLine, int *piBalanceFunc, int *piBalanceLoop);
    bool analyze(void);

    const char_t *getName(int ii)
    {
        if (isIndexValid(ii) == false) {
            return NULL;
        }
        return static_cast<const char_t *>(((*m_pData)[ii])->name);
    }

    AnalyzerElement::SCOPE getScope(int ii)
    {
        if (isIndexValid(ii) == false) {
            return AnalyzerElement::SCOPE_UNKNOWN;
        }
        return ((*m_pData)[ii])->scope;
    }

    int getLine(int ii)
    {
        if (isIndexValid(ii) == false) {
            return -1;
        }
        return ((*m_pData)[ii])->line;
    }

    int getLine(const char_t *funcName)
    {
        if ((funcName == NULL) || (*funcName == uT('\0')) || (getCount() < 1)) {
            return -1;
        }
        for (int ii = 0; ii < getCount(); ii++) {
            if (Tstrcmp(funcName, static_cast<const char_t *>(((*m_pData)[ii])->name)) == 0) {
                return ((*m_pData)[ii])->line;
            }
        }
        return -1;
    }

    void setLine(const char_t *funcName, int iLine)
    {
        if ((funcName == NULL) || (*funcName == uT('\0')) || (getCount() < 1)) {
            return;
        }
        for (int ii = 0; ii < getCount(); ii++) {
            if (Tstrcmp(funcName, static_cast<const char_t *>(((*m_pData)[ii])->name)) == 0) {
                ((*m_pData)[ii])->line = iLine;
                return;
            }
        }
        return;
    }

    int getIndex(const char_t *funcName, bool bComplete = true);

    int getCount(void)
    {
        if (m_pData == NULL) {
            return 0;
        }
        return (int)(m_pData->size());
    }

    void setLanguage(CodeAnalyzer::LANGUAGE iLang)
    {
        m_iLang = iLang;
    }

    int getMainLine(void)
    {
        return m_iMainLine;
    }

private:
    LANGUAGE m_iLang;
};

#endif
