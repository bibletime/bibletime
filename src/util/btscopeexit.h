/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTSCOPEEXIT_H
#define BTSCOPEEXIT_H

#include <type_traits>
#include <utility>
#include "btstringjoin.h"


template <typename F>
class BtScopeExit {

public: /* Methods: */

    inline BtScopeExit(F f) : m_f{f} {}
    inline ~BtScopeExit() noexcept { m_f(); }

private: /* Fields: */

    F m_f;

};

template <typename F>
BtScopeExit<typename std::remove_reference<F>::type> createBtScopeExit(F f)
{ return f; }

#define BT_SCOPE_EXIT(...) \
    auto BT_STRING_JOIN(bt_scope_exit_, __LINE__) = \
            createBtScopeExit([=](){__VA_ARGS__})

#endif /* BTSCOPEEXIT_H */
