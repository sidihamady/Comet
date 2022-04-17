// Scintilla source code edit control
/** @file LexBash.cxx
 ** Lexer for Bash.
 **/
// Copyright 2004-2005 by Neil Hodgson <neilh@scintilla.org>
// Adapted from LexPerl by Kein-Hong Man <mkh@pl.jaring.my> 2004
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"


static inline bool IsANumberChar(int ch)
{
    // Not exactly following number definition (several dots are seen as OK, etc.)
    // but probably enough in most cases.
    return (ch < 0x80) &&
        (isdigit(ch) || toupper(ch) == 'E' ||
        ch == '.' || ch == '-' || ch == '+' ||
        ch == 'o' || ch == 'b' || ch == 'x' ||
        (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
}

static inline bool isEOLChar(char ch) {
    return (ch == '\r') || (ch == '\n');
}

static bool isSingleCharOp(char ch) {
    char strCharSet[2];
    strCharSet[0] = ch;
    strCharSet[1] = '\0';
    return (NULL != strstr("rwxoRWXOezsfdlpSbctugkTBMACahGLNn", strCharSet));
}

// Tests for SOLIS Operators
static bool IsSolisOperator(char ch)
{
    return (ch == '=') || (ch == '+') || (ch == '>') || (ch == '<') ||
        (ch == '|') || (ch == '?') || (ch == '*');
}

static bool IsSolisDelim(char ch)
{
    return (ch == '[') || (ch == ']');
}

static inline bool IsSolisSeparator(char ch)
{
    if (ch == '^' || ch == '&' || ch == '\\' || ch == '%' ||
        ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
        ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
        ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
        ch == '>' || ch == ',' || ch == '/' || ch == '<' ||
        ch == '?' || ch == '!' || ch == '.' || ch == '~' ||
        ch == '@')
        return true;
    return false;
}

static int classifyWordSolis(unsigned int start, unsigned int end, WordList &keywords, WordList &keywords2, WordList &keywords3, Accessor &styler)
{
    char s[100];
    for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
        s[i] = styler[start + i];
        s[i + 1] = '\0';
    }
    char chAttr = SCE_SOLIS_IDENTIFIER;
    if (keywords.InList(s)) {
        chAttr = SCE_SOLIS_KEYWORD;
    }
    else if (keywords2.InList(s)) {
        chAttr = SCE_SOLIS_KEYWORD2;
    }
    else if (keywords3.InList(s)) {
        chAttr = SCE_SOLIS_COMMAND;
    }
    styler.ColourTo(end, chAttr);
    return chAttr;
}

static inline bool isEndVar(char ch) {
    return !isalnum(ch) && ch != '$' && ch != '_';
}

static inline bool isNonQuote(char ch) {
    return isalnum(ch) || ch == '_';
}

static char opposite(char ch) {
    if (ch == '(')
        return ')';
    if (ch == '[')
        return ']';
    if (ch == '{')
        return '}';
    if (ch == '<')
        return '>';
    return ch;
}

inline bool IsAlphaNumeric(int ch)
{
    return
        ((ch >= '0') && (ch <= '9')) ||
        ((ch >= 'a') && (ch <= 'z')) ||
        ((ch >= 'A') && (ch <= 'Z'));
}

inline bool isSolisWordchar(int ch)
{
    return IsAlphaNumeric(ch) || ch == '.' || ch == '_';
}
inline bool isSolisWordstart(int ch)
{
    return IsAlphaNumeric(ch) || ch == '_' || ch == '@' || ch == '$';
}

static void ColouriseSolisDoc(unsigned int startPos, int length, int initStyle,
                             WordList *keywordlists[], Accessor &styler) {

    // Lexer for bash often has to backtrack to start of current style to determine
    // which characters are being used as quotes, how deeply nested is the
    // start position and what the termination string is for here documents

    WordList &keywords = *keywordlists[0];
    WordList &keywords2 = *keywordlists[1];
    WordList &keywords3 = *keywordlists[2];

    class HereDocCls {
    public:
        int State;        // 0: '<<' encountered
        // 1: collect the delimiter
        // 2: here doc text (lines after the delimiter)
        char Quote;        // the char after '<<'
        bool Quoted;        // true if Quote in ('\'','"','`')
        bool Indent;        // indented delimiter (for <<-)
        int DelimiterLength;    // strlen(Delimiter)
        char *Delimiter;    // the Delimiter, 256: sizeof PL_tokenbuf
        HereDocCls() {
            State = 0;
            Quote = 0;
            Quoted = false;
            Indent = 0;
            DelimiterLength = 0;
            Delimiter = new char[256];
            Delimiter[0] = '\0';
        }
        ~HereDocCls() {
            delete []Delimiter;
        }
    };
    HereDocCls HereDoc;

    class QuoteCls {
        public:
        int  Rep;
        int  Count;
        char Up;
        char Down;
        QuoteCls() {
            this->New(1);
        }
        void New(int r) {
            Rep   = r;
            Count = 0;
            Up    = '\0';
            Down  = '\0';
        }
        void Open(char u) {
            Count++;
            Up    = u;
            Down  = opposite(Up);
        }
    };
    QuoteCls Quote;

    int state = initStyle;
    unsigned int lengthDoc = startPos + length;

    if (state == SCE_SOLIS_STRING
     || state == SCE_SOLIS_NUMBER
     || state == SCE_SOLIS_IDENTIFIER
    ) {
        while ((startPos > 1) && (styler.StyleAt(startPos - 1) == state)) {
            startPos--;
        }
        state = SCE_SOLIS_DEFAULT;
    }

    styler.StartAt(startPos);
    char chPrev = styler.SafeGetCharAt(startPos - 1);
    if (startPos == 0)
        chPrev = '\n';
    char chNext = styler[startPos];
    styler.StartSegment(startPos);

    for (unsigned int i = startPos; i < lengthDoc; i++) {
        char ch = chNext;
        // if the current character is not consumed due to the completion of an
        // earlier style, lexing can be restarted via a simple goto
    restartLexer:
        chNext = styler.SafeGetCharAt(i + 1);
        char chNext2 = styler.SafeGetCharAt(i + 2);
        char chNext3 = styler.SafeGetCharAt(i + 3);
        char chNext4 = styler.SafeGetCharAt(i + 4);
        char chNext5 = styler.SafeGetCharAt(i + 5);
        char chNext6 = styler.SafeGetCharAt(i + 6);

        if (styler.IsLeadByte(ch)) {
            chNext = styler.SafeGetCharAt(i + 2);
            chPrev = ' ';
            i += 1;
            continue;
        }

        if ((chPrev == '\r' && ch == '\n')) {    // skip on DOS/Windows
            styler.ColourTo(i, state);
            chPrev = ch;
            continue;
        }

        if ((chPrev == '\n') && (ch == '[') && (chNext == 'E') && (chNext2 == 'R') && (chNext3 == 'R') && (chNext4 == 'O') && (chNext5 == 'R') && (chNext6 == ']')) {
            state = SCE_SOLIS_ERROR;
        }

        if (state == SCE_SOLIS_DEFAULT) {
            /*if (ch == '\\') {    // escaped character
                if (i < lengthDoc - 1)
                    i++;
                ch = chNext;
                chNext = chNext2;
                styler.ColourTo(i, SCE_SOLIS_IDENTIFIER);
                } else*/
                if (isdigit(ch) && (chNext != '/' || !isalpha(chNext2))) {
                state = SCE_SOLIS_NUMBER;
                }
                else if (isSolisWordstart(ch) && (chPrev != '$')) {
                state = SCE_SOLIS_KEYWORD;
                if (!isSolisWordchar(chNext) && chNext != '+' && chNext != '-') {
                    // We need that if length of word == 1!
                    // This test is copied from the SCE_SOLIS_WORD handler.
                    classifyWordSolis(styler.GetStartSegment(), i, keywords, keywords2, keywords3, styler);
                    state = SCE_SOLIS_DEFAULT;
                }
            } else if ((ch == '#') || (ch == ';')) {
                state = SCE_SOLIS_COMMENT;
            } else if (ch == '\"') {
                state = SCE_SOLIS_STRING;
                Quote.New(1);
                Quote.Open(ch);
            } else if (ch == '\'') {
                state = SCE_SOLIS_STRING;
                Quote.New(1);
                Quote.Open(ch);
            } else if (ch == '*') {
                if (chNext == '*') {    // exponentiation
                    i++;
                    ch = chNext;
                    chNext = chNext2;
                }
                styler.ColourTo(i, SCE_SOLIS_OPERATOR);
            } else if (ch == '-'    // file test operators
                       && isSingleCharOp(chNext)
                       && !isalnum((chNext2 = styler.SafeGetCharAt(i+2)))) {
                styler.ColourTo(i + 1, SCE_SOLIS_KEYWORD);
                state = SCE_SOLIS_DEFAULT;
                i++;
                ch = chNext;
                chNext = chNext2;
            }
            else if (IsSolisOperator(ch)) {
                styler.ColourTo(i, SCE_SOLIS_OPERATOR);
            }
            else if (IsSolisDelim(ch)) {
                styler.ColourTo(i, SCE_SOLIS_COMMENTLINE);
            }
            else {
                // keep colouring defaults to make restart easier
                styler.ColourTo(i, SCE_SOLIS_DEFAULT);
            }
        } else if (state == SCE_SOLIS_NUMBER) {

            // [:COMET:]:160207:MOD: handle operator inside number
            if (!IsANumberChar(ch)) {
                styler.ColourTo(i - 1, state);
                state = SCE_SOLIS_DEFAULT;
                goto restartLexer;
            }
        } else if (state == SCE_SOLIS_KEYWORD) {
            if (!isSolisWordchar(chNext) && chNext != '+' && chNext != '-') {
                // "." never used in variable names
                // but used in file names
                classifyWordSolis(styler.GetStartSegment(), i, keywords, keywords2, keywords3, styler);
                state = SCE_SOLIS_DEFAULT;
                ch = ' ';
            }
        } else if (state == SCE_SOLIS_IDENTIFIER) {
            if (!isSolisWordchar(chNext) && chNext != '+' && chNext != '-') {
                styler.ColourTo(i, SCE_SOLIS_IDENTIFIER);
                state = SCE_SOLIS_DEFAULT;
                ch = ' ';
            }
        } else {
            if (state == SCE_SOLIS_COMMENT) {
                if (ch == '\\' && isEOLChar(chNext)) {
                    // comment continuation
                    if (chNext == '\r' && chNext2 == '\n') {
                        i += 2;
                        ch = styler.SafeGetCharAt(i);
                        chNext = styler.SafeGetCharAt(i + 1);
                    } else {
                        i++;
                        ch = chNext;
                        chNext = chNext2;
                    }
                } else if (isEOLChar(ch)) {
                    styler.ColourTo(i - 1, state);
                    state = SCE_SOLIS_DEFAULT;
                    goto restartLexer;
                } else if (isEOLChar(chNext)) {
                    styler.ColourTo(i, state);
                    state = SCE_SOLIS_DEFAULT;
                }
            } else if (state == SCE_SOLIS_STRING) {
                if (!Quote.Down && !isspacechar(ch)) {
                    Quote.Open(ch);
                }
                /* do not escape "    then a quoted string cannot contain quote: not a problem for Solis */
                /*else if (ch == '\\' && Quote.Up != '\\') {
                    i++;
                    ch = chNext;
                    chNext = styler.SafeGetCharAt(i + 1);
                }*/
                else if (ch == Quote.Down) {
                    Quote.Count--;
                    if (Quote.Count == 0) {
                        Quote.Rep--;
                        if (Quote.Rep <= 0) {
                            styler.ColourTo(i, state);
                            state = SCE_SOLIS_DEFAULT;
                            ch = ' ';
                        }
                        if (Quote.Up == Quote.Down) {
                            Quote.Count++;
                        }
                    }
                } else if (ch == Quote.Up) {
                    Quote.Count++;
                }
            }
            else if (state == SCE_SOLIS_ERROR) {
                if (isEOLChar(ch)) {
                    styler.ColourTo(i - 1, state);
                    state = SCE_SOLIS_DEFAULT;
                    goto restartLexer;
                }
                else if (isEOLChar(chNext)) {
                    styler.ColourTo(i, state);
                    state = SCE_SOLIS_DEFAULT;
                }
            }
        }
        chPrev = ch;
    }
    styler.ColourTo(lengthDoc - 1, state);
}

static void FoldSolisDoc(unsigned int startPos, int length, int, WordList *[],
                            Accessor &styler) {
    const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

    const int endPos = startPos + length;
    int visibleChars = 0;
    int lineCurrent = styler.GetLine(startPos);

    char chNext = styler[startPos];
    int styleNext = styler.StyleAt(startPos);
    bool headerPoint = false;
    int lev;

    for (int i = startPos; i < endPos; i++) {
        const char ch = chNext;
        chNext = styler[i + 1];

        const int style = styleNext;
        styleNext = styler.StyleAt(i + 1);
        const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

        if (style == SCE_SOLIS_COMMENTLINE) {
            headerPoint = true;
        }

        if (atEOL) {
            lev = SC_FOLDLEVELBASE;

            if (lineCurrent > 0) {
                const int levelPrevious = styler.LevelAt(lineCurrent - 1);

                if (levelPrevious & SC_FOLDLEVELHEADERFLAG) {
                    lev = SC_FOLDLEVELBASE + 1;
                }
                else {
                    lev = levelPrevious & SC_FOLDLEVELNUMBERMASK;
                }
            }

            if (headerPoint) {
                lev = SC_FOLDLEVELBASE;
            }
            if (visibleChars == 0 && foldCompact)
                lev |= SC_FOLDLEVELWHITEFLAG;

            if (headerPoint) {
                lev |= SC_FOLDLEVELHEADERFLAG;
            }
            if (lev != styler.LevelAt(lineCurrent)) {
                styler.SetLevel(lineCurrent, lev);
            }

            lineCurrent++;
            visibleChars = 0;
            headerPoint = false;
        }
        if (!isspacechar(ch))
            visibleChars++;
    }

    if (lineCurrent > 0) {
        const int levelPrevious = styler.LevelAt(lineCurrent - 1);
        if (levelPrevious & SC_FOLDLEVELHEADERFLAG) {
            lev = SC_FOLDLEVELBASE + 1;
        }
        else {
            lev = levelPrevious & SC_FOLDLEVELNUMBERMASK;
        }
    }
    else {
        lev = SC_FOLDLEVELBASE;
    }
    int flagsNext = styler.LevelAt(lineCurrent);
    styler.SetLevel(lineCurrent, lev | (flagsNext & ~SC_FOLDLEVELNUMBERMASK));
}

static const char * const SolisWordListDesc[] = {
    "Sections",
    "Records",
    "Commands",
    0
};

LexerModule lmSolis(SCLEX_SOLIS, ColouriseSolisDoc, "solis", FoldSolisDoc, SolisWordListDesc);
