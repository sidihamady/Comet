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


#include "Identifiers.h"

#include "CometApp.h"
#include "CometFrame.h"
#include "ScriptEdit.h"

static const unsigned int UTF8FROM_CP1252_TABLE[32] = {
    0xe282ac, 0xefbfbd, 0xe2809a, 0xc692,
    0xe2809e, 0xe280a6, 0xe280a0, 0xe280a1,
    0xcb86, 0xe280b0, 0xc5a0, 0xe280b9,
    0xc592, 0xefbfbd, 0xc5bd, 0xefbfbd,
    0xefbfbd, 0xe28098, 0xe28099, 0xe2809c,
    0xe2809d, 0xe280a2, 0xe28093, 0xe28094,
    0xcb9c, 0xe284a2, 0xc5a1, 0xe280ba,
    0xc593, 0xefbfbd, 0xc5be, 0xc5b8
};

const uint8_t ScriptEdit::UTF32_BOM_LE[] = { uint8_t(0xFF), uint8_t(0xFE), uint8_t(0x00), uint8_t(0x00) };
const uint8_t ScriptEdit::UTF32_BOM_BE[] = { uint8_t(0x00), uint8_t(0x00), uint8_t(0xFE), uint8_t(0xFF) };
const uint8_t ScriptEdit::UTF16_BOM_LE[] = { uint8_t(0xFF), uint8_t(0xFE) };
const uint8_t ScriptEdit::UTF16_BOM_BE[] = { uint8_t(0xFE), uint8_t(0xFF) };
const uint8_t ScriptEdit::UTF8_BOM[] = { uint8_t(0xEF), uint8_t(0xBB), uint8_t(0xBF) };
const uint8_t ScriptEdit::UTF16_BYTEMARK[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

bool ScriptEdit::isCP1252(uint8_t *pBufferCP, size_t sizeCP)
{
    if ((sizeCP < 4) || (sizeCP > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    uint8_t cT;

    size_t ii = 0, izu = 0;
    size_t it = (sizeCP > UNICODE_CHECKLEN) ? UNICODE_CHECKLEN : sizeCP;

    // trailing zeros?
    if ((pBufferCP[it - 1] == 0) && (pBufferCP[it - 2] == 0)) {
        ii = it - 2;
        while (pBufferCP[ii] == 0) {
            if ((ii <= 0) || (it <= 0)) {
                break;
            }
            --it;
            --ii;
        }
        if (it <= 0) {
            return false;
        }
    }
    //

    for (ii = 0; ii < it; ii++) {

        cT = pBufferCP[ii];

        // Zeros Count...
        if (cT == 0x00) {
            izu += 1;
        }
        if (izu > 1) {
            return false;
        }

        //  81, 8D, 8F, 90, and 9D unused
        if ((cT == 0x81) || (cT == 0x8d) || (cT == 0x8f) || (cT == 0x90) || (cT == 0x9d)) {
            return false;
        }
    }

    return true;
}

bool ScriptEdit::isUTF8(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBOM)
{
    size_t ii, is, jj, it, izu, inextchars;

    uint8_t *pSource = pBufferUTF;
    uint8_t cT;

    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    *bBOM = ScriptEdit::isUTF8BOM(pBufferUTF, sizeUTF);

    // is UTF-8?
    izu = 0;
    inextchars = 0;
    is = *bBOM ? 3 : 0;
    it = (sizeUTF > UNICODE_CHECKLEN) ? UNICODE_CHECKLEN : sizeUTF;

    // trailing zeros?
    if ((pSource[it - 1] == 0) && (pSource[it - 2] == 0)) {
        ii = it - 2;
        while (pSource[ii] == 0) {
            if ((ii <= 0) || (it <= 0)) {
                break;
            }
            --it;
            --ii;
        }
        if (it <= is) {
            return false;
        }
    }
    //

    ii = is;
    for (;;) {

        cT = pSource[ii];

        // Zeros Count...
        if (cT == 0x00) {
            izu += 1;
        }
        if (izu > 1) {
            return false;
        }

        if (cT <= 0x7f) {
            inextchars = 0;
        }
        else if ((cT >= 0xc2) && (cT <= 0xdf)) {
            inextchars = 1;
        }
        else if ((cT >= 0xe0) && (cT <= 0xef)) {
            inextchars = 2;
        }
        else if ((cT >= 0xf0) && (cT <= 0xf4)) {
            inextchars = 3;
        }
        else {
            return false;
        }

        if (inextchars > 0) {
            for (jj = ii + 1; (jj <= (ii + inextchars)) && (jj < it); jj++) {
                cT = pSource[jj];
                if ((cT < 0x80) || (cT > 0xbf)) {
                    return false;
                }
            }
            ii += inextchars;
        }

        ii += 1;

        if (ii >= it) {
            break;
        }
    }

    return true;
}

bool ScriptEdit::isUTF8BOM(uint8_t *pBufferUTF, size_t sizeUTF)
{
    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    if ((pBufferUTF[0] == ScriptEdit::UTF8_BOM[0]) && (pBufferUTF[1] == ScriptEdit::UTF8_BOM[1]) && (pBufferUTF[2] == ScriptEdit::UTF8_BOM[2])) {
        return true;
    }

    return false;
}

bool ScriptEdit::isUTF16(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian, bool *bBOM)
{
    size_t ii, is, it, ile, ibe, izbe, izle;

    uint8_t *pSource = pBufferUTF;
    uint8_t cTa, cTb;

    bool bUTF16 = false;

    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    *bBOM = ScriptEdit::isUTF16BOM(pBufferUTF, sizeUTF, bBigEndian);

    // is UTF-16? one method is to look for linefeed 000A
    ile = 0;
    ibe = 0;
    izbe = 0;
    izle = 0;
    is = *bBOM ? 2 : 0;
    it = (sizeUTF > UNICODE_CHECKLEN) ? UNICODE_CHECKLEN : sizeUTF;

    // trailing zeros?
    if ((pSource[it - 1] == 0) && (pSource[it - 2] == 0)) {
        ii = it - 2;
        while (pSource[ii] == 0) {
            if ((ii <= 0) || (it <= 0)) {
                break;
            }
            --it;
            --ii;
        }
        if (it <= is) {
            return false;
        }
    }
    //

    for (ii = is; ii < it; ii += 2) {
        cTa = pSource[ii];
        cTb = pSource[ii + 1];

        if (cTa == 0x00) {
            if ((cTb == 0x0a) || (cTb == 0x0d)) {
                ibe += 1;
            }
            izbe += 1;
        }
        else if (cTb == 0x00) {
            if ((cTa == 0x0a) || (cTa == 0x0d)) {
                ile += 1;
            }
            izle += 1;
        }

        if ((ibe > 0) && (ile > 0)) {
            return false;
        }
    }

    *bBigEndian = (ibe > 0);
    bUTF16 = (*bBigEndian || (ile > 0));

    if (bUTF16 == false) {
        if ((izle > 1) || (izbe > 1)) {
            if (izle > (izbe + 2)) {
                *bBigEndian = false;
                bUTF16 = true;
            }
            else if (izbe > (izle + 2)) {
                *bBigEndian = true;
                bUTF16 = true;
            }
        }
    }

    // false UTF-16 sometimes happen... check Windows encoding
    if (bUTF16) {
        bUTF16 = (ScriptEdit::isCP1252(pBufferUTF, sizeUTF) == false);
    }

    return bUTF16;
}

bool ScriptEdit::isUTF16BOM(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian)
{
    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    if ((pBufferUTF[0] == ScriptEdit::UTF16_BOM_LE[0]) && (pBufferUTF[1] == ScriptEdit::UTF16_BOM_LE[1])) {
        *bBigEndian = false;
        return true;
    }
    if ((pBufferUTF[0] == ScriptEdit::UTF16_BOM_BE[0]) && (pBufferUTF[1] == ScriptEdit::UTF16_BOM_BE[1])) {
        *bBigEndian = true;
        return true;
    }

    return false;
}

bool ScriptEdit::isUTF32(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian, bool *bBOM)
{
    size_t ii, is, it, ile, ibe, izle, izbe;

    uint8_t *pSource = pBufferUTF;
    uint8_t cTa, cTb, cTc, cTd;

    bool bUTF32 = false;

    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    *bBOM = ScriptEdit::isUTF32BOM(pBufferUTF, sizeUTF, bBigEndian);

    // is UTF-32?
    ile = 0;
    ibe = 0;
    izle = 0;
    izbe = 0;
    is = *bBOM ? 3 : 0;
    it = (sizeUTF > UNICODE_CHECKLEN) ? UNICODE_CHECKLEN : sizeUTF;

    // trailing zeros?
    if ((pSource[it - 1] == 0) && (pSource[it - 2] == 0)) {
        ii = it - 2;
        while (pSource[ii] == 0) {
            if ((ii <= 0) || (it <= 0)) {
                break;
            }
            --it;
            --ii;
        }
        if (it <= is) {
            return false;
        }
    }
    //

    for (ii = is; ii < it; ii += 4) {
        cTa = pSource[ii];
        cTb = pSource[ii + 1];
        cTc = pSource[ii + 2];
        cTd = pSource[ii + 3];

        if ((cTa == 0x00) && (cTb == 0x00) && (cTc == 0x00)) {
            if ((cTd == 0x0a) || (cTd == 0x0d)) {
                ibe += 1;
            }
            izbe += 1;
        }
        else if ((cTd == 0x00) && (cTc == 0x00) && (cTb == 0x00)) {
            if ((cTa == 0x0a) || (cTa == 0x0d)) {
                ile += 1;
            }
            izle += 1;
        }

        if ((ibe > 0) && (ile > 0)) {
            return false;
        }
    }

    *bBigEndian = (ibe > 0);
    bUTF32 = (*bBigEndian || (ile > 0));

    if (bUTF32 == false) {
        if ((izle > 1) || (izbe > 1)) {
            if (izle > (izbe + 2)) {
                *bBigEndian = false;
                bUTF32 = true;
            }
            else if (izbe > (izle + 2)) {
                *bBigEndian = true;
                bUTF32 = true;
            }
        }
    }

    return bUTF32;
}

bool ScriptEdit::isUTF32BOM(uint8_t *pBufferUTF, size_t sizeUTF, bool *bBigEndian)
{
    if ((sizeUTF < 4) || (sizeUTF > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    if ((pBufferUTF[0] == ScriptEdit::UTF32_BOM_LE[0]) && (pBufferUTF[1] == ScriptEdit::UTF32_BOM_LE[1]) && (pBufferUTF[2] == ScriptEdit::UTF32_BOM_LE[2]) && (pBufferUTF[3] == ScriptEdit::UTF32_BOM_LE[3])) {
        *bBigEndian = false;
        return true;
    }
    else if ((pBufferUTF[0] == ScriptEdit::UTF32_BOM_BE[0]) && (pBufferUTF[1] == ScriptEdit::UTF32_BOM_BE[1]) && (pBufferUTF[2] == ScriptEdit::UTF32_BOM_BE[2]) && (pBufferUTF[3] == ScriptEdit::UTF32_BOM_BE[3])) {
        *bBigEndian = true;
        return true;
    }

    return false;
}

bool ScriptEdit::convertFromCP1252(uint8_t *pBufferUTF8, size_t sizeUTF8)
{
    size_t ii;
    bool bret = true;

    size_t sizeCP = (size_t)(this->GetTextLength());

    if ((sizeCP < 4) || (sizeCP > LF_SCRIPT_MAXCHARS) || (sizeUTF8 < 4) || (sizeUTF8 > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    size_t ll = 0;
    uint8_t cT;
    unsigned int iT;
    const unsigned int iMask = (unsigned int)(0x000000ff);
    bool bBigEndian = (lf_getEndian() == 0x20);

    for (ii = 0; ii < sizeCP; ii++) {

        cT = (uint8_t)(this->GetCharAt((int)ii));

        if (cT < 0x80) {
            pBufferUTF8[ll] = (char)cT;
            ll += 1;
        }
        else if (cT >= 0xa0) {
            pBufferUTF8[ll] = 0xc2 + (cT > 0xbf);
            ll += 1;
            pBufferUTF8[ll] = (cT & 0x3f) + 0x80;
            ll += 1;
        }
        else {
            iT = UTF8FROM_CP1252_TABLE[cT - 0x80];

            if (iT == 0xefbfbd) {
                pBufferUTF8[ll] = '?';
                ll += 1;
                continue;
            }

            if (bBigEndian == false) {
                if (iT > 0xe00000) {
                    pBufferUTF8[ll] = (char)((iT >> 16) & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)((iT >> 8) & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)(iT & iMask);
                    ll += 1;
                }
                else {
                    pBufferUTF8[ll] = (char)((iT >> 8) & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)(iT & iMask);
                    ll += 1;
                }
            }
            else {
                if (iT > 0xe00000) {
                    pBufferUTF8[ll] = (char)(iT & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)((iT >> 8) & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)((iT >> 16) & iMask);
                    ll += 1;
                }
                else {
                    pBufferUTF8[ll] = (char)(iT & iMask);
                    ll += 1;
                    pBufferUTF8[ll] = (char)((iT >> 8) & iMask);
                    ll += 1;
                }
            }
        }
    }

    if (ll < sizeUTF8) {
        pBufferUTF8[ll] = 0;
    }

    return bret;
}

bool ScriptEdit::convertFromUTF16(uint8_t *pBufferUTF8, uint8_t *pBufferUTF16, size_t sizeUTF8, size_t sizeUTF16, bool bBigEndian)
{
    size_t ii, jj;
    bool bret = true;

    uint32_t wT, wTs, wTa, wTb;
    uint16_t nbytes = 0;

    uint8_t *pSource = pBufferUTF16;
    uint8_t *pTarget = pBufferUTF8;

    if ((sizeUTF16 < 4) || (sizeUTF16 > LF_SCRIPT_MAXCHARS) || (sizeUTF8 < 4) || (sizeUTF8 > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    ii = 0;
    jj = 0;
    for (;;) {
        wTa = (uint32_t)(*pSource);
        pSource++;
        ii++;
        wTb = (uint32_t)(*pSource);
        if (ii < sizeUTF16) {
            pSource++;
            ii++;
        }

        if (bBigEndian == false) {
            wT = (wTa & 0x00FFUL) + ((wTb << 8) & 0xFF00UL);
        }
        else {
            wT = (wTb & 0x00FFUL) + ((wTa << 8) & 0xFF00UL);
        }

        if ((wT >= UNICODE_SURR_HIGHSTART) && (wT <= UNICODE_SURR_HIGHEND) && (ii < (sizeUTF16 - 1))) {
            wTa = (uint32_t)(*pSource);
            wTb = (uint32_t)(*(pSource + 1));
            if (bBigEndian == false) {
                wTs = (wTa & 0x00FFUL) + ((wTb << 8) & 0xFF00UL);
            }
            else {
                wTs = (wTb & 0x00FFUL) + ((wTa << 8) & 0xFF00UL);
            }
            if ((wTs >= UNICODE_SURR_LOWSTART) && (wTs <= UNICODE_SURR_LOWEND)) {
                wT = ((wT - UNICODE_SURR_HIGHSTART) << UNICODE_HALFSHIFT) + (wTs - UNICODE_SURR_LOWSTART) + UNICODE_HALFBASE;

                pSource++;
                ii++;
                if (ii < sizeUTF16) {
                    pSource++;
                    ii++;
                }
            }
            else {
                bret = false;
                break;
            }
        }
        else if ((wT >= UNICODE_SURR_LOWSTART) && (wT <= UNICODE_SURR_LOWEND)) {
            bret = false;
            break;
        }

        if (wT < (uint32_t)0x80) {
            nbytes = 1;
        }
        else if (wT < (uint32_t)0x800) {
            nbytes = 2;
        }
        else if (wT < (uint32_t)0x10000) {
            nbytes = 3;
        }
        else if (wT < (uint32_t)0x200000) {
            nbytes = 4;
        }
        else {
            nbytes = 3;
            wT = 0x0000FFFDUL;
        }

        if (jj > (sizeUTF8 - nbytes)) {
            break;
        }

        pTarget += nbytes;
        jj += nbytes;

        switch (nbytes) {
            case 4:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 3:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 2:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 1:
                *--pTarget = (uint8_t)(wT | UTF16_BYTEMARK[nbytes]);
                --jj;
        }

        pTarget += nbytes;
        jj += nbytes;

        if (ii >= sizeUTF16) {
            break;
        }
        if (jj >= sizeUTF8) {
            break;
        }
    }

    if (jj < sizeUTF8) {
        pBufferUTF8[jj] = 0x00;
    }

    return bret;
}

bool ScriptEdit::convertFromUTF32(uint8_t *pBufferUTF8, uint8_t *pBufferUTF32, size_t sizeUTF8, size_t sizeUTF32, bool bBigEndian)
{
    size_t ii, jj;
    bool bret = true;

    uint32_t wT, wTa, wTb, wTc, wTd;
    uint16_t nbytes = 0;

    uint8_t *pSource = pBufferUTF32;
    uint8_t *pTarget = pBufferUTF8;

    if ((sizeUTF32 < 4) || (sizeUTF32 > LF_SCRIPT_MAXCHARS) || (sizeUTF8 < 4) || (sizeUTF8 > LF_SCRIPT_MAXCHARS)) {
        return false;
    }

    ii = 0;
    jj = 0;
    for (;;) {
        wTa = (uint32_t)(*pSource);
        pSource++;
        ii++;
        wTb = (uint32_t)(*pSource);
        if (ii >= sizeUTF32) {
            break;
        }
        pSource++;
        ii++;
        wTc = (uint32_t)(*pSource);
        if (ii >= sizeUTF32) {
            break;
        }
        pSource++;
        ii++;
        wTd = (uint32_t)(*pSource);
        if (ii >= sizeUTF32) {
            break;
        }
        pSource++;
        ii++;

        if (bBigEndian == false) {
            wT = (wTa & 0x000000FFUL) + ((wTb << 8) & 0x0000FF00UL) + ((wTc << 16) & 0x00FF0000UL) + ((wTd << 24) & 0xFF000000UL);
        }
        else {
            wT = (wTd & 0x000000FFUL) + ((wTc << 8) & 0x0000FF00UL) + ((wTb << 16) & 0x00FF0000UL) + ((wTa << 24) & 0xFF000000UL);
        }

        if ((wT >= UNICODE_SURR_HIGHSTART) && (wT <= UNICODE_SURR_LOWEND)) {
            bret = false;
            break;
        }

        if (wT < (uint32_t)0x80) {
            nbytes = 1;
        }
        else if (wT < (uint32_t)0x800) {
            nbytes = 2;
        }
        else if (wT < (uint32_t)0x10000) {
            nbytes = 3;
        }
        else if (wT <= (uint32_t)0x0010FFFF) {
            nbytes = 4;
        }
        else {
            nbytes = 3;
            wT = 0x0000FFFD;
        }

        if (jj > (sizeUTF8 - nbytes)) {
            break;
        }

        pTarget += nbytes;
        jj += nbytes;

        switch (nbytes) {
            case 4:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 3:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 2:
                *--pTarget = (uint8_t)((wT | UNICODE_BYTEMARK) & UNICODE_BYTEMASK);
                wT >>= 6;
                --jj;
            case 1:
                *--pTarget = (uint8_t)(wT | UTF16_BYTEMARK[nbytes]);
                --jj;
        }

        pTarget += nbytes;
        jj += nbytes;

        if (ii >= sizeUTF32) {
            break;
        }
        if (jj >= sizeUTF8) {
            break;
        }
    }

    if (jj < sizeUTF8) {
        pBufferUTF8[jj] = 0x00;
    }

    return bret;
}

int ScriptEdit::doConvertFromWC(uint8_t *tUTF8, wchar_t tWC, size_t sizeUTF8)
{
    int tlen = 0;
    unsigned int iWC = (unsigned int)tWC;

    if (iWC < 0x80) {
        if (sizeUTF8 >= 1) {
            tUTF8[tlen++] = (char)iWC;
        }
    }
    else if (iWC < 0x800) {
        if (sizeUTF8 >= 2) {
            tUTF8[tlen++] = 0xc0 | (iWC >> 6);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x10000) {
        if (sizeUTF8 >= 3) {
            tUTF8[tlen++] = 0xe0 | (iWC >> 12);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x200000) {
        if (sizeUTF8 >= 4) {
            tUTF8[tlen++] = 0xf0 | (iWC >> 18);
            tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x4000000) {
        if (sizeUTF8 >= 5) {
            tUTF8[tlen++] = 0xf8 | (iWC >> 24);
            tUTF8[tlen++] = 0x80 | ((iWC >> 18) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
            tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
            tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
        }
    }
    else if (iWC < 0x80000000) {
        if (sizeUTF8 >= 6) {
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

bool ScriptEdit::convertFromWC(uint8_t *pBufferUTF8, wchar_t *pBufferWC, size_t sizeUTF8, size_t sizeWC, size_t *psizeRet, bool *pbCheck)
{
    uint8_t *pUTF8 = pBufferUTF8;
    int nn = -1;

    *pbCheck = true;
    *psizeRet = 0;

    while (*pBufferWC) {
        nn = doConvertFromWC(pUTF8, *pBufferWC++, sizeUTF8);
        if (nn <= 0) {
            break;
        }
        if (*pbCheck && (*(pBufferWC - 1) == uT('\n')) && ((char)(*pUTF8) != ('\n'))) {
            *pbCheck = false;
        }
        pUTF8 += nn;
        sizeUTF8 -= nn;
    }

    if (nn == 0) {
        while (sizeUTF8--) {
            *pUTF8++ = 0;
        }
    }
    else if (sizeUTF8) {
        *pUTF8 = 0;
    }

    if (nn < 0) {
        return false;
    }

    *psizeRet = (size_t)(pUTF8 - pBufferUTF8);

    return ((pUTF8 - pBufferUTF8) > 0);
}


// UTF8
bool ScriptEdit::DoUTF8Encode(int iFrom)
{
    if ((iFrom < 0) || (iFrom > UTF8FROM_LAST)) {
        return false;
    }

    CometFrame *pFrame = static_cast<CometFrame *>(wxGetApp().getMainFrame());
    if (NULL == pFrame) {
        // should never happen
        return false;
    }

    if (true == m_bUTF8done[iFrom]) {
        pFrame->OutputStatusbar(uT("Cannot encode document in UTF-8: already done"), SIGMAFRAME_TIMER_SHORT);
        return false;
    }

    const int iBufferLen = (int)this->GetLength();
    if (iBufferLen < 2) {
        return false;
    }

    bool bModified = false;

    m_bLoading = true;

    wxString strT;

    int ii = 0;

    wxBusyCursor waitC;

    if ((UTF8FROM_ISO8859L1 == iFrom) || (UTF8FROM_ISO8859L9 == iFrom)) {

        const int iBufferLenU8 = (iBufferLen << 2) + 1;

        unsigned char *pszBufferU8A = (unsigned char *)malloc(iBufferLenU8 * sizeof(unsigned char));
        if (pszBufferU8A == NULL) {
            pFrame->OutputStatusbar(uT("Cannot encode document in UTF-8: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
            m_bLoading = false;
            return false;
        }

        pszBufferU8A[iBufferLenU8 - 1] = 0;

        int ll = 0;
        unsigned char cT;
        for (ii = 0; ii < iBufferLen; ii++) {
            cT = this->GetCharAt(ii);
            if (cT < 128) {
                pszBufferU8A[ll] = (char)cT;
                ll += 1;
            }
            else if (UTF8FROM_ISO8859L9 == iFrom) {
                if (cT == 0xa4) {
                    pszBufferU8A[ll] = 0xe2;
                    ll += 1;
                    pszBufferU8A[ll] = 0x82;
                    ll += 1;
                    pszBufferU8A[ll] = 0xac;
                    ll += 1;
                }
                else if (cT == 0xa6) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0xa0;
                    ll += 1;
                }
                else if (cT == 0xa8) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0xa1;
                    ll += 1;
                }
                else if (cT == 0xb4) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0xbd;
                    ll += 1;
                }
                else if (cT == 0xb8) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0xbe;
                    ll += 1;
                }
                else if (cT == 0xbc) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0x92;
                    ll += 1;
                }
                else if (cT == 0xbd) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0x93;
                    ll += 1;
                }
                else if (cT == 0xbe) {
                    pszBufferU8A[ll] = 0xc5;
                    ll += 1;
                    pszBufferU8A[ll] = 0xb8;
                    ll += 1;
                }
                else {
                    pszBufferU8A[ll] = 0xc2 + (cT > 0xbf);
                    ll += 1;
                    pszBufferU8A[ll] = (cT & 0x3f) + 0x80;
                    ll += 1;
                }
            }
            else {
                pszBufferU8A[ll] = 0xc2 + (cT > 0xbf);
                ll += 1;
                pszBufferU8A[ll] = (cT & 0x3f) + 0x80;
                ll += 1;
            }
        }

        if (ll < iBufferLenU8) {
            pszBufferU8A[ll] = 0;
        }

        this->SetTextRaw((const char *)(pszBufferU8A));

        free(pszBufferU8A);
        pszBufferU8A = NULL;

        this->Refresh();

        m_bUTF8done[iFrom] = true;

        bModified = true;
    }

    else if (UTF8FROM_CP1252 == iFrom) {

        const int iBufferLenU8 = (iBufferLen << 2) + 1;

        unsigned char *pszBufferU8A = (unsigned char *)malloc(iBufferLenU8 * sizeof(unsigned char));
        if (pszBufferU8A == NULL) {
            pFrame->OutputStatusbar(uT("Cannot encode document in UTF-8: insufficient memory"), SIGMAFRAME_TIMER_SHORT);
            m_bLoading = false;
            return false;
        }

        pszBufferU8A[iBufferLenU8 - 1] = 0;

        bool bret = convertFromCP1252(pszBufferU8A, iBufferLenU8 - 1);

        if (bret) {
            this->SetTextRaw((const char *)(pszBufferU8A));
        }

        free(pszBufferU8A);
        pszBufferU8A = NULL;

        if (bret) {
            this->Refresh();
        }

        m_bUTF8done[iFrom] = bret;

        bModified = bret;

        if (bret == false) {
            pFrame->OutputStatusbar(uT("Cannot encode document in UTF-8: invalid characters found"), SIGMAFRAME_TIMER_SHORT);
        }
    }

    m_bLoading = false;

    if (bModified) {
        DoSetModified();
    }

    return true;
}
