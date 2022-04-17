// Scintilla source code edit control
/** @file LexLaTeX.cxx
 ** Lexer for LaTeX.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
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

static inline bool AtEOL(Accessor &styler, unsigned int i) {
	return (styler[i] == '\n') ||
	       ((styler[i] == '\r') && (styler.SafeGetCharAt(i + 1) != '\n'));
}

static int isSpecial(char s) {
	return (s == '\\') || (s == ',') || (s == ';') || (s == '\'') || (s == ' ') ||
	       (s == '\"') || (s == '`') || (s == '^') || (s == '~');
}

static int isTag(int start, Accessor &styler) {
	char s[6];
	unsigned int i = 0, e = 1;
	while (i < 5 && e) {
		s[i] = styler[start + i];
		i++;
		e = styler[start + i] != '{';
	}
	s[i] = '\0';
	return (strcmp(s, "begin") == 0) || (strcmp(s, "end") == 0);
}

static void ColouriseLatexDoc(unsigned int startPos, int length, int initStyle,
                              WordList *[], Accessor &styler) {

	styler.StartAt(startPos);

	int state = initStyle;
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	int lengthDoc = startPos + length;

	for (int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			i++;
			continue;
		}
		switch (state) {
		case SCE_L_DEFAULT :
			switch (ch) {
			case '\\' :
			case '@' :
				styler.ColourTo(i - 1, state);
				if (isSpecial(styler[i + 1])) {
					styler.ColourTo(i + 1, SCE_L_COMMAND);
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					if (isTag(i + 1, styler))
						state = SCE_L_TAG;
					else
						state = SCE_L_COMMAND;
				}
				break;
			case '$' :
				styler.ColourTo(i - 1, state);
				state = SCE_L_MATH;
				if (chNext == '$') {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				break;
			case '%' :
				styler.ColourTo(i - 1, state);
				state = SCE_L_COMMENT;
				break;
			}
			break;
		case SCE_L_COMMAND :
			if (chNext == '[' || chNext == '{' || chNext == '}' ||
			        chNext == ' ' || chNext == '\r' || chNext == '\n') {
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
			}
			break;
		case SCE_L_TAG :
			if (ch == '}') {
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
			}
			break;
		case SCE_L_MATH :
			if (ch == '$') {
				if (chNext == '$') {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				styler.ColourTo(i, state);
				state = SCE_L_DEFAULT;
			}
			break;
		case SCE_L_COMMENT :
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_L_DEFAULT;
			}
		}
	}
	styler.ColourTo(lengthDoc-1, state);
}

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmLatex(SCLEX_LATEX, ColouriseLatexDoc, "latex", 0, emptyWordListDesc);
