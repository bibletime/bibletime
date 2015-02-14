#include "btglobal.h"

#include <QDataStream>


QDataStream &operator<<(QDataStream &out, const alignmentMode &mode) {
    out << (qint8) mode;
    return out;
}

QDataStream &operator>>(QDataStream &in, alignmentMode &mode) {
    qint8 i;
    in >> i;
    mode = (alignmentMode) i;
    return in;
}

FilterOptions::FilterOptions()
    : footnotes(0), strongNumbers(0), headings(0), morphTags(0),
      lemmas(0), hebrewPoints(0), hebrewCantillation(0), greekAccents(0),
      textualVariants(0), redLetterWords(0), scriptureReferences(0),
      morphSegmentation(0) {
}
