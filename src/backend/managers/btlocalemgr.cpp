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

#include "btlocalemgr.h"

#include <QByteArray>
#include "../../util/btassert.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <localemgr.h>
#pragma GCC diagnostic pop

namespace {

struct BtLocaleMgrImpl final: public sword::LocaleMgr {

    friend sword::LocaleMap const & ::BtLocaleMgr::internalSwordLocales();

};

BtLocaleMgrImpl & btLocaleMgrInstance() {
    auto * btLocaleMgr =
            dynamic_cast<BtLocaleMgrImpl *>(
                sword::LocaleMgr::getSystemLocaleMgr());
    if (!btLocaleMgr) {
        /* Beware that sword::StringMgr::setSystemStringMgr() also replaces the
           Sword system locale manager with new sword::LocaleMgr(). */
        btLocaleMgr = new BtLocaleMgrImpl();
        sword::LocaleMgr::setSystemLocaleMgr(btLocaleMgr);
    }
    return *btLocaleMgr;
}

} // anonymous namespace

auto BtLocaleMgr::internalSwordLocales() -> sword::LocaleMap const & {
    auto const & btLocaleMgr = btLocaleMgrInstance();
    BT_ASSERT(btLocaleMgr.locales);
    return *btLocaleMgr.locales;
}

sword::SWLocale * BtLocaleMgr::localeTranslator()
{ return btLocaleMgrInstance().getLocale("locales"); }

QString BtLocaleMgr::defaultLocaleName()
{ return btLocaleMgrInstance().getDefaultLocaleName(); }

void BtLocaleMgr::setDefaultLocaleName(QString const & localeName) {
    static_cast<sword::LocaleMgr &>(btLocaleMgrInstance()).setDefaultLocaleName(
                localeName.toUtf8().constData());
}
