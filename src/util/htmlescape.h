/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef UTIL_HTMLESCAPE_H
#define UTIL_HTMLESCAPE_H

#include <QString>
#if QT_VERSION < 0x050000
#include <QTextDocument>
#endif


namespace util {
namespace tool {

inline QString htmlEscape(const QString & str) {
#if QT_VERSION < 0x050000
    return Qt::escape(str);
#else
    return str.toHtmlEscaped();
#endif
}

} /* namespace tool { */
} /* namespace util { */

#endif /* UTIL_HTMLESCAPE_H */
