//
// C++ Implementation: btstringmgr
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "btstringmgr.h"

//System includes
#include <ctype.h>

char* BTStringMgr::upperUTF8(char* text, unsigned int maxlen) const {
	const int max = (maxlen>0) ? maxlen : strlen(text);

	if (isUtf8(text)) {
		strncpy(text, (const char*)QString::fromUtf8(text).upper().utf8(), max);

		return text;
	}
	else {
		char* ret = text;

		while (*text) {
			*text = toupper(*text);
			text++;
		}

		return ret;
	}

	return text;
}

char* BTStringMgr::upperLatin1(char* text, unsigned int max) const {
	char* ret = text;

	while (*text) {
		*text = toupper(*text);
		text++;
	}

	return ret;
}

bool BTStringMgr::supportsUnicode() const {
	return true;
}

const bool BTStringMgr::isUtf8(const char *buf) const {
	int i, n;
	register unsigned char c;
	bool gotone = false;

	#define F 0   /* character never appears in text */
	#define T 1   /* character appears in plain ASCII text */
	#define I 2   /* character appears in ISO-8859 text */
	#define X 3   /* character appears in non-ISO extended ASCII (Mac, IBM PC) */

	static const unsigned char text_chars[256] = {
				/*                  BEL BS HT LF    FF CR    */
				F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  /* 0x0X */
				/*                              ESC          */
				F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  /* 0x1X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x2X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x3X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x4X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x5X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x6X */
				T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  /* 0x7X */
				/*            NEL                            */
				X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  /* 0x8X */
				X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  /* 0x9X */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xaX */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xbX */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xcX */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xdX */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xeX */
				I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   /* 0xfX */
			};

	/* *ulen = 0; */

	for (i = 0; (c = buf[i]); i++) {
		if ((c & 0x80) == 0) {        /* 0xxxxxxx is plain ASCII */
			/*
			 * Even if the whole file is valid UTF-8 sequences,
			 * still reject it if it uses weird control characters.
			 */

			if (text_chars[c] != T)
				return false;

		}
		else if ((c & 0x40) == 0) { /* 10xxxxxx never 1st byte */
			return false;
		}
		else {                           /* 11xxxxxx begins UTF-8 */
			int following;

			if ((c & 0x20) == 0) {             /* 110xxxxx */
				following = 1;
			}
			else if ((c & 0x10) == 0) {      /* 1110xxxx */
				following = 2;
			}
			else if ((c & 0x08) == 0) {      /* 11110xxx */
				following = 3;
			}
			else if ((c & 0x04) == 0) {      /* 111110xx */
				following = 4;
			}
			else if ((c & 0x02) == 0) {      /* 1111110x */
				following = 5;
			}
			else
				return false;

			for (n = 0; n < following; n++) {
				i++;

				if (!(c = buf[i]))
					goto done;

				if ((c & 0x80) == 0 || (c & 0x40))
					return false;
			}

			gotone = true;
		}
	}

done:
	return gotone;   /* don't claim it's UTF-8 if it's all 7-bit */
}

#undef F
#undef T
#undef I
#undef X
