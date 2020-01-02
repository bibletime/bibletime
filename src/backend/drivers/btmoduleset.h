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

#ifndef BTMODULESET_H
#define BTMODULESET_H

#include <QSet>


class CSwordModuleInfo;

class BtModuleSet: public QSet<CSwordModuleInfo *> {

public: /* Methods: */

    inline BtModuleSet() {}

    inline BtModuleSet(QSet<CSwordModuleInfo *> const & copy)
        : QSet<CSwordModuleInfo *>(copy)
    {}

    inline bool contains(CSwordModuleInfo const * const m) const {
        return this->QSet<CSwordModuleInfo *>::contains(
                const_cast<CSwordModuleInfo *>(m));
    }

};

#endif /* BTMODULESET_H */
