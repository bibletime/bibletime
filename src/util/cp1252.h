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

#pragma once

#include <QByteArray>
#include <QString>


namespace util {
namespace cp1252 {

QString toUnicode(QByteArray const & data);
QByteArray fromUnicode(QString const & str);

} /* namespace cp1252 { */
} /* namespace util { */
