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

#include <QTextCodec>
#include "btassert.h"


namespace util {
namespace cp1252 {

namespace {

QTextCodec const & codec() {
    static auto const * const codec =
            QTextCodec::codecForName(QByteArrayLiteral("Windows-1252"));
    BT_ASSERT(codec);
    return *codec;
}

} // anonymous namespace

QString toUnicode(QByteArray const & data) { return codec().toUnicode(data); }

QByteArray fromUnicode(QString const & str)
{ return codec().fromUnicode(str); }

} /* namespace cp1252 { */
} /* namespace util { */
