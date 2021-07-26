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

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <localemgr.h>
#pragma GCC diagnostic pop


/**
  A replacement for sword::LocaleMgr which provides direct access to the list of
  locales.
*/
class BtLocaleMgr final: public sword::LocaleMgr {

public: /* Methods: */

    BtLocaleMgr();
    BtLocaleMgr(BtLocaleMgr &&) = delete;
    BtLocaleMgr(BtLocaleMgr const &) = delete;

    ~BtLocaleMgr() final override;

    BtLocaleMgr & operator=(BtLocaleMgr &&) = delete;
    BtLocaleMgr & operator=(BtLocaleMgr const &) = delete;

    static sword::LocaleMap const & internalSwordLocales();
    static sword::SWLocale * localeTranslator();

};
