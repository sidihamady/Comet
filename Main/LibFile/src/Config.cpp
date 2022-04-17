// -------------------------------------------------------------
// LibFile
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------

#define LIBFILE_CORE

#define _CRT_SECURE_NO_DEPRECATE 1

#include "File.h"

LIBFILE_API lfRecord::lfRecord(const char_t *pszKey, const char_t *pszValue)
{
    Tmemset(m_szKey, 0, LM_STRSIZEN);
    Tmemset(m_szValue, 0, LM_STRSIZE);
    update(pszKey, pszValue);
}

// Destructor
LIBFILE_API lfRecord::~lfRecord()
{
}

// Update data
LIBFILE_API void lfRecord::update(const char_t *pszKey, const char_t *pszValue)
{
    if (pszKey && pszValue) {
        Tstrncpy(m_szKey, pszKey, LM_STRSIZEN - 1);
        Tstrncpy(m_szValue, pszValue, LM_STRSIZE - 1);
    }
}

// Getters/Setters
LIBFILE_API void lfRecord::getKey(char_t *pszKey) const
{
    if (pszKey) {
        Tstrncpy(pszKey, static_cast<const char_t *>(m_szKey), LM_STRSIZEN - 1);
    }
}

LIBFILE_API const char_t *lfRecord::getKey(void) const
{
    return static_cast<const char_t *>(m_szKey);
}

LIBFILE_API void lfRecord::getValue(char_t *pszValue) const
{
    if (pszValue) {
        Tstrncpy(pszValue, static_cast<const char_t *>(m_szValue), LM_STRSIZE - 1);
    }
}

LIBFILE_API const char_t *lfRecord::getValue(void) const
{
    return static_cast<const char_t *>(m_szValue);
}

LIBFILE_API void lfRecord::copy(lfRecord *pRrecord)
{
    if ((pRrecord == NULL) || (this == pRrecord)) {
        return;
    }

    pRrecord->getKey(static_cast<char_t *>(m_szKey));
    pRrecord->getValue(static_cast<char_t *>(m_szValue));
}

LIBFILE_API bool lfRecord::isEqual(const char_t *pszKey) const
{
    if (pszKey == NULL) {
        return false;
    }

    // I compare only key, not value.
    return (Tstricmp(static_cast<const char_t *>(m_szKey), pszKey) == 0);
}

LIBFILE_API lfSection::lfSection(const char_t *pszName, int iMaxRecordCount)
{
    Tmemset(m_szName, 0, LM_STRSIZEN);
    m_iRecordCount = 0;
    m_iMaxRecordCount = 0;
    m_ppRecord = NULL;
    if ((iMaxRecordCount > 0) && (iMaxRecordCount <= LF_CONFIG_MAXRECORD)) {
        m_ppRecord = new (std::nothrow) lfRecord *[iMaxRecordCount];
        if (m_ppRecord != NULL) {
            m_iMaxRecordCount = iMaxRecordCount;
            for (int ii = 0; ii < m_iMaxRecordCount; ii++) {
                m_ppRecord[ii] = NULL;
            }
        }
        Tstrncpy(m_szName, pszName, LM_STRSIZEN - 1);
    }
}

// Destructor
LIBFILE_API lfSection::~lfSection()
{
    if (m_ppRecord != NULL) {
        if (m_iMaxRecordCount > 0) {
            for (int ii = 0; ii < m_iMaxRecordCount; ii++) {
                if (m_ppRecord[ii] != NULL) {
                    delete m_ppRecord[ii];
                    m_ppRecord[ii] = NULL;
                }
            }
            m_iMaxRecordCount = 0;
        }
        delete[] m_ppRecord;
        m_ppRecord = NULL;
    }
    m_iMaxRecordCount = 0;
    m_iRecordCount = 0;
}

LIBFILE_API int lfSection::getRecordCount() const
{
    return m_iRecordCount;
}

LIBFILE_API lfRecord *lfSection::getRecord(const char_t *pszKey)
{
    if ((pszKey == NULL) || (m_ppRecord == NULL) || (m_iRecordCount < 1) || (m_iRecordCount > m_iMaxRecordCount)) {
        return NULL;
    }

    for (int ii = 0; ii < m_iRecordCount; ii++) {
        if (m_ppRecord[ii]->isEqual(pszKey)) {
            return m_ppRecord[ii];
        }
    }

    return NULL;
}

LIBFILE_API lfRecord *lfSection::getRecord(int ii)
{
    if ((ii < 0) || (ii >= m_iRecordCount) || (m_ppRecord == NULL) || (m_iRecordCount < 1) || (m_iRecordCount > m_iMaxRecordCount)) {
        return NULL;
    }

    return m_ppRecord[ii];
}
// Update data
LIBFILE_API void lfSection::setName(const char_t *pszName)
{
    if (pszName) {
        Tstrncpy(m_szName, pszName, LM_STRSIZEN - 1);
    }
}
LIBFILE_API void lfSection::getName(char_t *pszName) const
{
    if (pszName) {
        Tstrncpy(pszName, static_cast<const char_t *>(m_szName), LM_STRSIZEN - 1);
    }
}
LIBFILE_API const char_t *lfSection::getName(void) const
{
    return static_cast<const char_t *>(m_szName);
}

LIBFILE_API bool lfSection::setValue(const char_t *pszKey, const char_t *pszValue)
{
    if ((m_ppRecord == NULL) || (m_iRecordCount < 0) || (m_iMaxRecordCount < 1) || (m_iRecordCount > m_iMaxRecordCount)) {
        return NULL;
    }

    if (pszKey && pszValue) {
        if (m_iRecordCount > 0) {
            for (int ii = 0; ii < m_iRecordCount; ii++) {
                if (m_ppRecord[ii]->isEqual(pszKey)) {
                    m_ppRecord[ii]->update(pszKey, pszValue);
                    return true;
                }
            }
        }
        if (m_iRecordCount < m_iMaxRecordCount) {
            m_ppRecord[m_iRecordCount] = new (std::nothrow) lfRecord(pszKey, pszValue);
            if (m_ppRecord[m_iRecordCount] == NULL) {
                return false;
            }
            m_iRecordCount += 1;
            return true;
        }
    }
    return false;
}

LIBFILE_API bool lfSection::getValue(const char_t *pszKey, char_t *pszValue) const
{
    if (pszKey && pszValue) {
        if (m_iRecordCount > 0) {
            for (int ii = 0; ii < m_iRecordCount; ii++) {
                if (m_ppRecord[ii]->isEqual(pszKey)) {
                    m_ppRecord[ii]->getValue(pszValue);
                    return true;
                }
            }
        }
    }
    return false;
}

LIBFILE_API bool lfSection::removeKey(const char_t *pszKey)
{
    // :IMPLEMENT: not yet implemented
    if (pszKey) {
    }
    return false;
}

LIBFILE_API void lfSection::copy(lfSection *pSection)
{
    if ((pSection == NULL) || (this == pSection)) {
        return;
    }

    // :IMPLEMENT: not yet implemented
}

LIBFILE_API bool lfSection::isEqual(const char_t *szSection) const
{
    return (Tstricmp(static_cast<const char_t *>(m_szName), szSection) == 0);
}

// Constructors

LIBFILE_API lfConfig::lfConfig(int iMaxSectionCount)
{
    m_iSectionCount = 0;
    m_iMaxSectionCount = 0;
    m_ppSection = NULL;
    if ((iMaxSectionCount > 0) && (iMaxSectionCount <= LF_CONFIG_MAXRECORD)) {
        m_ppSection = new (std::nothrow) lfSection *[iMaxSectionCount];
        if (m_ppSection != NULL) {
            m_iMaxSectionCount = iMaxSectionCount;
            for (int ii = 0; ii < m_iMaxSectionCount; ii++) {
                m_ppSection[ii] = NULL;
            }
        }
    }
}

// Destructor
LIBFILE_API lfConfig::~lfConfig()
{
    if (m_ppSection != NULL) {
        if (m_iMaxSectionCount > 0) {
            for (int ii = 0; ii < m_iMaxSectionCount; ii++) {
                if (m_ppSection[ii] != NULL) {
                    delete m_ppSection[ii];
                    m_ppSection[ii] = NULL;
                }
            }
            m_iMaxSectionCount = 0;
        }
        delete[] m_ppSection;
        m_ppSection = NULL;
    }
    m_iMaxSectionCount = 0;
    m_iSectionCount = 0;
}

LIBFILE_API int lfConfig::trim(char_t *szBuffer)
{
    return lm_trim(szBuffer);
}

LIBFILE_API int lfConfig::getSectionCount() const
{
    return m_iSectionCount;
}

LIBFILE_API lfSection *lfConfig::getSection(const char_t *pszSection)
{
    if ((pszSection == NULL) || (m_ppSection == NULL) || (m_iSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return NULL;
    }

    for (int ii = 0; ii < m_iSectionCount; ii++) {
        if (m_ppSection[ii]->isEqual(pszSection)) {
            return m_ppSection[ii];
        }
    }

    return NULL;
}

LIBFILE_API lfSection *lfConfig::getSection(int ii)
{
    if ((ii < 0) || (ii >= m_iSectionCount) || (m_ppSection == NULL) || (m_iSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return NULL;
    }

    return m_ppSection[ii];
}

LIBFILE_API lfSection *lfConfig::addSection(const char_t *pszSection, int iMaxRecordCount)
{
    if ((pszSection == NULL) || (m_ppSection == NULL) || (m_iSectionCount < 0) || (m_iMaxSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount) || (iMaxRecordCount < 1) || (iMaxRecordCount > LF_CONFIG_MAXRECORD)) {
        return NULL;
    }

    int iLen = (int)Tstrlen(pszSection);
    if ((iLen < 1) || (iLen > (LM_STRSIZEN - 1))) {
        return NULL;
    }

    if (m_iSectionCount == 0) {
        m_ppSection[m_iSectionCount] = new (std::nothrow) lfSection(pszSection, iMaxRecordCount);
        if (m_ppSection[m_iSectionCount] != NULL) {
            m_iSectionCount += 1;
            return m_ppSection[m_iSectionCount - 1];
        }
    }

    // Section already exists ?
    for (int ii = 0; ii < m_iSectionCount; ii++) {
        if (m_ppSection[ii]->isEqual(pszSection)) {
            return m_ppSection[ii];
        }
    }

    if (m_iSectionCount < m_iMaxSectionCount) {
        m_ppSection[m_iSectionCount] = new (std::nothrow) lfSection(pszSection, iMaxRecordCount);
        if (m_ppSection[m_iSectionCount] != NULL) {
            m_iSectionCount += 1;
            return m_ppSection[m_iSectionCount - 1];
        }
    }

    return NULL;
}

LIBFILE_API bool lfConfig::setValue(const char_t *pszSection, const char_t *pszKey, const char_t *pszValue)
{
    if ((pszSection == NULL) || (pszKey == NULL) || (pszValue == NULL) || (m_ppSection == NULL) || (m_iSectionCount < 1) || (m_iMaxSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return false;
    }

    for (int ii = 0; ii < m_iSectionCount; ii++) {
        if (Tstricmp(pszSection, m_ppSection[ii]->getName()) == 0) {
            return m_ppSection[ii]->setValue(pszKey, pszValue);
        }
    }
    return false;
}

LIBFILE_API bool lfConfig::getValue(const char_t *pszSection, const char_t *pszKey, char_t *pszValue) const
{
    if ((pszSection == NULL) || (pszKey == NULL) || (pszValue == NULL) || (m_ppSection == NULL) || (m_iSectionCount < 1) || (m_iMaxSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return false;
    }

    for (int ii = 0; ii < m_iSectionCount; ii++) {
        if (Tstricmp(pszSection, m_ppSection[ii]->getName()) == 0) {
            return m_ppSection[ii]->getValue(pszKey, pszValue);
        }
    }
    return false;
}

LIBFILE_API void lfConfig::removeAll()
{
    if ((m_ppSection == NULL) || (m_iSectionCount < 1) || (m_iMaxSectionCount < 1) || (m_iSectionCount > m_iMaxSectionCount)) {
        return;
    }

    if (m_iSectionCount > 0) {
        for (int ii = 0; ii < m_iSectionCount; ii++) {
            if (m_ppSection[ii] != NULL) {
                delete m_ppSection[ii];
                m_ppSection[ii] = NULL;
            }
        }
        m_iSectionCount = 0;
    }
}
