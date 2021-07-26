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

#include <map>
#include <QString>

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swbuf.h>
#pragma GCC diagnostic pop


namespace sword { class SWLocale; }


/** An interface replacement for sword::LocaleMgr. */
namespace BtLocaleMgr {

std::map<sword::SWBuf, sword::SWLocale *> const & internalSwordLocales();
sword::SWLocale * localeTranslator();
QString defaultLocaleName();
void setDefaultLocaleName(QString const & localeName);

};
