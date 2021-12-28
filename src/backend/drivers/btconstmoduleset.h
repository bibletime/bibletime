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

class BtConstModuleSet: public QSet<CSwordModuleInfo const *> {

public: // methods:

    BtConstModuleSet(QSet<CSwordModuleInfo const *> const & copy)
        : QSet<CSwordModuleInfo const *>(copy)
    {}

    BtConstModuleSet(QSet<CSwordModuleInfo *> other) {
        for (auto const * const item : other)
            insert(item);
    }

};
