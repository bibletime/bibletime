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
