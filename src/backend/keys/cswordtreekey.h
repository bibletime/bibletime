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

#ifndef CSWORDTREEKEYIDX_H
#define CSWORDTREEKEYIDX_H

#include "cswordkey.h"

// Sword includes:
#include <treekeyidx.h>


class CSwordModuleInfo;

/** BibleTime's implementation of Sword's TreeKeyIdx class.
 * @short CSwordKey implementation for Sword's TreeKey
 * @author The BibleTime team
 */

class CSwordTreeKey : public CSwordKey, public sword::TreeKeyIdx {

    public:

        CSwordTreeKey & operator=(CSwordTreeKey const &) = delete;

        /**
          \param k The Sword tree key which belongs to this key
          \param module The module which belongs to this key
         */
        CSwordTreeKey(const sword::TreeKeyIdx *k,
                      const CSwordModuleInfo *module);

        CSwordTreeKey( const CSwordTreeKey& k );

        void setModule(const CSwordModuleInfo *newModule) override;

        CSwordTreeKey* copy() const override;

        /**
        * Returns the TreeKeyIdx::getLocalKey value in unicode.
        * Local key is the last part of the tree key, for example "Subsection1" from "/Section1/Subsection1".
        * Use this instead of getLocalKey() to avoid encoding problems.
        */
        QString getLocalNameUnicode();

        QString key() const override;

        bool setKey(const QString &key) override;

        bool setKey(const char *key) override;

    protected:

        const char * rawKey() const override;
};

#endif
