/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btglobal.h"

#include <QDataStream>
#include <QtGlobal>


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

bool FilterOptions::filterOptionsAreEqual(FilterOptions const & opts)
    const noexcept
{
    return opts.footnotes == footnotes &&
            opts.strongNumbers == strongNumbers &&
            opts.headings == headings &&
            opts.morphTags == morphTags &&
            opts.lemmas == lemmas &&
            opts.hebrewPoints == hebrewPoints &&
            opts.hebrewCantillation == hebrewCantillation &&
            opts.greekAccents == greekAccents &&
            opts.textualVariants == textualVariants &&
            opts.redLetterWords == redLetterWords &&
            opts.scriptureReferences == scriptureReferences &&
            opts.morphSegmentation == morphSegmentation;
}

bool DisplayOptions::displayOptionsAreEqual(DisplayOptions const & opts)
    const noexcept
{
    return opts.lineBreaks == lineBreaks &&
            opts.verseNumbers == verseNumbers;
}
