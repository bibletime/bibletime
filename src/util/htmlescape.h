/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_HTMLESCAPE_H
#define UTIL_HTMLESCAPE_H

#include <QString>


namespace util {

inline QString htmlEscape(const QString & str) {
    return str.toHtmlEscaped();
}

} /* namespace util { */

#endif /* UTIL_HTMLESCAPE_H */
