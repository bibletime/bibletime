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

#include <QSet>


class CSwordModuleInfo;

class BtModuleSet: public QSet<CSwordModuleInfo *> {

public: // methods:

    BtModuleSet() = default;

    BtModuleSet(QSet<CSwordModuleInfo *> const & copy)
        : QSet<CSwordModuleInfo *>(copy)
    {}

    bool contains(CSwordModuleInfo const * const m) const {
        return this->QSet<CSwordModuleInfo *>::contains(
                const_cast<CSwordModuleInfo *>(m));
    }

};
