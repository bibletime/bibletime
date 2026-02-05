/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cp1252.h"

#include <QStringDecoder>
#include <QStringEncoder>
#include "btassert.h"


namespace util {
namespace cp1252 {

QString toUnicode(QByteArray const & data) {
    QStringDecoder decoder("Windows-1252", QStringDecoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QString result = decoder(data);
    BT_ASSERT(!decoder.hasError());
    return result;
}

QByteArray fromUnicode(QString const & str) {
    QStringEncoder encoder("Windows-1252", QStringEncoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QByteArray result = encoder(str);
    BT_ASSERT(!encoder.hasError());
    return result;
}

} /* namespace cp1252 { */
} /* namespace util { */
