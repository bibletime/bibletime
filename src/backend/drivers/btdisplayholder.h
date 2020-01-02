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

#ifndef BTDISPLAYHOLDER_H
#define BTDISPLAYHOLDER_H

#include "../rendering/centrydisplay.h"


template <typename CRTP>
class BtDisplayHolder {

public: /* Methods: */

    BtDisplayHolder(Rendering::CEntryDisplay * display = nullptr) noexcept
        : m_display(display)
    {}

    virtual ~BtDisplayHolder() noexcept {}

    void setDisplay(Rendering::CEntryDisplay * display = nullptr) noexcept
    { m_display = display; }

    Rendering::CEntryDisplay * getDisplay() const noexcept { return m_display; }

    void display() noexcept {
        if (auto * display = m_display)
            display->display(static_cast<CRTP &>(*this));
    }

private: /* Fields: */

    Rendering::CEntryDisplay * m_display = nullptr;

};

#endif /* BTDISPLAYHOLDER_H */
