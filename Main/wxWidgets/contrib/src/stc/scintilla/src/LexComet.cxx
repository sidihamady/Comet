// Scintilla source code edit control
/** @file LexUser.cxx
 ** User-defined lexer.
 ** written by Pr. Sidi HAMADY
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

// [:COMET:]:151118: custom lexer

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"


static bool IsCometCommentChar(int c) {
	return (c == '#') ;
}

static bool IsCometComment(Accessor &styler, int pos, int len) {
	return len > 0 && IsCometCommentChar(styler[pos]) ;
}

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && (isalnum(ch) || ch == '_');
}

static void ColouriseCometDoc(
            unsigned int startPos, int length, int initStyle,
            WordList *keywordlists[], Accessor &styler) {

	WordList &keywords = *keywordlists[0];

	styler.StartAt(startPos);

	bool transpose = false;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.state == SCE_COMET_OPERATOR) {
			if (sc.chPrev == '.') {
				if (sc.ch == '*' || sc.ch == '/' || sc.ch == '\\' || sc.ch == '^') {
					sc.ForwardSetState(SCE_COMET_DEFAULT);
					transpose = false;
				} else if (sc.ch == '\'') {
					sc.ForwardSetState(SCE_COMET_DEFAULT);
					transpose = true;
				} else {
					sc.SetState(SCE_COMET_DEFAULT);
				}
			} else {
				sc.SetState(SCE_COMET_DEFAULT);
			}
		} else if (sc.state == SCE_COMET_KEYWORD) {
			if (!isalnum(sc.ch) && sc.ch != '_') {
				char s[100];
				sc.GetCurrentLowered(s, sizeof(s));
				if (keywords.InList(s)) {
					sc.SetState(SCE_COMET_DEFAULT);
					transpose = false;
				} else {
					sc.ChangeState(SCE_COMET_IDENTIFIER);
					sc.SetState(SCE_COMET_DEFAULT);
					transpose = true;
				}
			}
		} else if (sc.state == SCE_COMET_NUMBER) {
			if (!isdigit(sc.ch) && sc.ch != '.'
			        && !(sc.ch == 'e' || sc.ch == 'E')
			        && !((sc.ch == '+' || sc.ch == '-') && (sc.chPrev == 'e' || sc.chPrev == 'E'))) {
				sc.SetState(SCE_COMET_DEFAULT);
				transpose = true;
			}
		} else if (sc.state == SCE_COMET_STRING) {
			if (sc.ch == '\'' && sc.chPrev != '\\') {
				sc.ForwardSetState(SCE_COMET_DEFAULT);
			}
		} else if (sc.state == SCE_COMET_DOUBLEQUOTESTRING) {
			if (sc.ch == '"' && sc.chPrev != '\\') {
				sc.ForwardSetState(SCE_COMET_DEFAULT);
			}
		} else if (sc.state == SCE_COMET_COMMENT || sc.state == SCE_COMET_COMMAND) {
			if (sc.atLineEnd) {
				sc.SetState(SCE_COMET_DEFAULT);
				transpose = false;
			}
		}

		if (sc.state == SCE_COMET_DEFAULT) {
			if (IsCometCommentChar(sc.ch)) {
				sc.SetState(SCE_COMET_COMMENT);
			} else if (sc.ch == '!') {
				sc.SetState(SCE_COMET_COMMAND);
			} else if (sc.ch == '\'') {
				if (transpose) {
					sc.SetState(SCE_COMET_OPERATOR);
				} else {
					sc.SetState(SCE_COMET_STRING);
				}
			} else if (sc.ch == '"') {
				sc.SetState(SCE_COMET_DOUBLEQUOTESTRING);
			} else if (isdigit(sc.ch) || (sc.ch == '.' && isdigit(sc.chNext))) {
				sc.SetState(SCE_COMET_NUMBER);
			} else if (isalpha(sc.ch)) {
				sc.SetState(SCE_COMET_KEYWORD);
			} else if (isoperator(static_cast<char>(sc.ch)) || sc.ch == '@' || sc.ch == '\\') {
				if (sc.ch == ')' || sc.ch == ']') {
					transpose = true;
				} else {
					transpose = false;
				}
				sc.SetState(SCE_COMET_OPERATOR);
			} else {
				transpose = false;
			}
		}
	}
	sc.Complete();
}

static void FoldCometDoc(unsigned int startPos, int length, int,
                          WordList *[], Accessor &styler) {

	int endPos = startPos + length;

	// Backtrack to previous line in case need to fix its fold status
	int lineCurrent = styler.GetLine(startPos);
	if (startPos > 0) {
		if (lineCurrent > 0) {
			lineCurrent--;
			startPos = styler.LineStart(lineCurrent);
		}
	}
	int spaceFlags = 0;
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, IsCometComment);
	char chNext = styler[startPos];
	for (int i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n') || (i == endPos)) {
			int lev = indentCurrent;
			int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags, IsCometComment);
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				} else if (indentNext & SC_FOLDLEVELWHITEFLAG) {
					// Line after is blank so check the next - maybe should continue further?
					int spaceFlags2 = 0;
					int indentNext2 = styler.IndentAmount(lineCurrent + 2, &spaceFlags2, IsCometComment);
					if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext2 & SC_FOLDLEVELNUMBERMASK)) {
						lev |= SC_FOLDLEVELHEADERFLAG;
					}
				}
			}
			indentCurrent = indentNext;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
		}
	}
}

static const char * const cometWordListDesc[] = {
	"Keywords",
	0
};

LexerModule lmComet(SCLEX_COMET, ColouriseCometDoc, "comet", FoldCometDoc, cometWordListDesc);
