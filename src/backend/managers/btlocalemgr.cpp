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

#include "../../util/btassert.h"


namespace {

BtLocaleMgr & btLocaleMgrInstance() {
    auto * btLocaleMgr =
            dynamic_cast<BtLocaleMgr *>(sword::LocaleMgr::getSystemLocaleMgr());
    if (!btLocaleMgr) {
        /* Beware that sword::StringMgr::setSystemStringMgr() also replaces the
           Sword system locale manager with new sword::LocaleMgr(). */
        btLocaleMgr = new BtLocaleMgr();
        sword::LocaleMgr::setSystemLocaleMgr(btLocaleMgr);
    }
    return *btLocaleMgr;
}

} // anonymous namespace

BtLocaleMgr::BtLocaleMgr() = default;
BtLocaleMgr::~BtLocaleMgr() = default;

sword::LocaleMap const & BtLocaleMgr::internalSwordLocales() {
    auto const & btLocaleMgr = btLocaleMgrInstance();
    BT_ASSERT(btLocaleMgr.locales);
    return *btLocaleMgr.locales;
}

sword::SWLocale * BtLocaleMgr::localeTranslator()
{ return btLocaleMgrInstance().getLocale("locales"); }
