/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btstringmgr.h"

#include <cstring>
#include <QString>
#include "../../util/btassert.h"
#include "../../util/macros.h"


namespace {

/** CODE TAKEN FROM KDELIBS 3.2, which is licensed under the LGPL 2.
*
* This code was taken from KStringHandler, which is part of the KDE libraries.
*
* This function checks whether a string is utf8 or not.
* It was taken from kdelibs so we do not depend on KDE 3.2.
*/
bool isUtf8(const char * buf) {
    int i, n;
    unsigned char c;
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
    #undef F
    #undef I
    #undef X

    /* *ulen = 0; */

    for (i = 0; (c = buf[i]); i++) {
        if ((c & 0x80) == 0) {        /* 0xxxxxxx is plain ASCII */
            /*
             * Even if the whole file is valid UTF-8 sequences,
             * still reject it if it uses weird control characters.
             */

            if (text_chars[c] != T)
                return false;
    #undef T

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

} // anonymous namespace

char * BtStringMgr::upperUTF8(char * text, unsigned int maxlen) const {
    size_t max = (maxlen > 0u) ? maxlen : strlen(text)+1;

    if (LIKELY(max > 1u)) {
        max--;
        if (isUtf8(text)) {
            strncpy(text, QString::fromUtf8(text).toUpper().toUtf8().constData(), max);
        }
        else {
            strncpy(text, QString::fromLatin1(text).toUpper().toLatin1().constData(), max);
        }
        text[max] = '\0';
    } else if (max == 1u) {
        text[0u] = '\0';
    } else {
        BT_ASSERT(max == 0u);
    }

    return text;
}

char * BtStringMgr::upperLatin1(char * text, unsigned int maxlen) const {
    size_t max = (maxlen > 0u) ? maxlen : strlen(text);

    if (LIKELY(max > 1u)) {
        max--;
        strncpy(text, QString::fromLatin1(text).toUpper().toLatin1().constData(), max);
        text[max] = '\0';
    } else if (max == 1u) {
        text[0u] = '\0';
    } else {
        BT_ASSERT(max == 0u);
    }

    return text;
}

bool BtStringMgr::supportsUnicode() const {
    return true;
}
