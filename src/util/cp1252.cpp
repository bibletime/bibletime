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

#include "cp1252.h"

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringDecoder>
#include <QStringEncoder>
#else
#include <QTextCodec>
#endif
#include "btassert.h"


namespace util {
namespace cp1252 {

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
namespace {

QTextCodec const & codec() {
    static auto const * const codec =
            QTextCodec::codecForName(QByteArrayLiteral("Windows-1252"));
    BT_ASSERT(codec);
    return *codec;
}

} // anonymous namespace
#endif

QString toUnicode(QByteArray const & data) {
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    return codec().toUnicode(data);
    #else
    QStringDecoder decoder("Windows-1252", QStringDecoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QString result = decoder(data);
    BT_ASSERT(!decoder.hasError());
    return result;
    #endif
}

QByteArray fromUnicode(QString const & str) {
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    return codec().fromUnicode(str);
    #else
    QStringEncoder encoder("Windows-1252", QStringEncoder::Flag::Stateless);
    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
    QByteArray result = encoder(str);
    BT_ASSERT(!encoder.hasError());
    return result;
    #endif
}

} /* namespace cp1252 { */
} /* namespace util { */
