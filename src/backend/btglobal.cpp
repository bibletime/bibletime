/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btglobal.h"

#include <QDataStream>


QDataStream &operator<<(QDataStream &out, const alignmentMode &mode) {
    out << static_cast<qint8>(mode);
    return out;
}

QDataStream &operator>>(QDataStream &in, alignmentMode &mode) {
    qint8 i;
    in >> i;
    mode = static_cast<alignmentMode>(i);
    return in;
}

FilterOptions::FilterOptions()
    : footnotes(0), strongNumbers(0), headings(0), morphTags(0),
      lemmas(0), hebrewPoints(0), hebrewCantillation(0), greekAccents(0),
      textualVariants(0), redLetterWords(0), scriptureReferences(0),
      morphSegmentation(0) {
}
