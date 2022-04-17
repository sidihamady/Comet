// Scintilla source code edit control
/** @file LexOthers.cxx
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

static void ColouriseMakeLine(
	char *lineBuffer,
	unsigned int lengthLine,
	unsigned int startLine,
	unsigned int endPos,
	Accessor &styler) {

	unsigned int i = 0;
	int lastNonSpace = -1;
	unsigned int state = SCE_MAKE_DEFAULT;
	bool bSpecial = false;
	// Skip initial spaces
	while ((i < lengthLine) && isspacechar(lineBuffer[i])) {
		i++;
	}
	if (lineBuffer[i] == '#') {	// Comment
		styler.ColourTo(endPos, SCE_MAKE_COMMENT);
		return;
	}
	if (lineBuffer[i] == '!') {	// Special directive
		styler.ColourTo(endPos, SCE_MAKE_PREPROCESSOR);
		return;
	}
	while (i < lengthLine) {
		if (lineBuffer[i] == '$' && lineBuffer[i + 1] == '(') {
			styler.ColourTo(startLine + i - 1, state);
			state = SCE_MAKE_IDEOL;
		}
		else if (state == SCE_MAKE_IDEOL && lineBuffer[i] == ')') {
			styler.ColourTo(startLine + i, state);
			state = SCE_MAKE_DEFAULT;
		}
		if (!bSpecial) {
			if (lineBuffer[i] == ':') {
				// We should check that no colouring was made since the beginning of the line,
				// to avoid colouring stuff like /OUT:file
				if (lastNonSpace >= 0)
					styler.ColourTo(startLine + lastNonSpace, SCE_MAKE_TARGET);
				styler.ColourTo(startLine + i - 1, SCE_MAKE_DEFAULT);
				styler.ColourTo(startLine + i, SCE_MAKE_OPERATOR);
				bSpecial = true;	// Only react to the first ':' of the line
				state = SCE_MAKE_DEFAULT;
			}
			else if (lineBuffer[i] == '=') {
				if (lastNonSpace >= 0)
					styler.ColourTo(startLine + lastNonSpace, SCE_MAKE_IDENTIFIER);
				styler.ColourTo(startLine + i - 1, SCE_MAKE_DEFAULT);
				styler.ColourTo(startLine + i, SCE_MAKE_OPERATOR);
				bSpecial = true;	// Only react to the first '=' of the line
				state = SCE_MAKE_DEFAULT;
			}
		}
		if (!isspacechar(lineBuffer[i])) {
			lastNonSpace = i;
		}
		i++;
	}
	if (state == SCE_MAKE_IDENTIFIER) {
		styler.ColourTo(endPos, SCE_MAKE_IDEOL);	// Error, variable reference not ended
	}
	else {
		styler.ColourTo(endPos, SCE_MAKE_DEFAULT);
	}
}

static void ColouriseMakeDoc(unsigned int startPos, int length, int, WordList *[], Accessor &styler) {
	char lineBuffer[1024];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int linePos = 0;
	unsigned int startLine = startPos;

	for (unsigned int i = startPos; i < startPos + length; i++) {
		lineBuffer[linePos++] = styler[i];
		if (AtEOL(styler, i) || (linePos >= sizeof(lineBuffer) - 1)) {
			// End of line (or of line buffer) met, colourise it
			lineBuffer[linePos] = '\0';
			ColouriseMakeLine(lineBuffer, linePos, startLine, i, styler);
			linePos = 0;
			startLine = i + 1;
		}
	}
	if (linePos > 0) {	// Last line does not have ending characters
		ColouriseMakeLine(lineBuffer, linePos, startLine, startPos + length - 1, styler);
	}
}

static const char * const emptyWordListDesc[] = {
	0
};

LexerModule lmMake(SCLEX_MAKEFILE, ColouriseMakeDoc, "makefile", 0, emptyWordListDesc);
