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

#include "cswordkey.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <treekeyidx.h>
#pragma GCC diagnostic pop


class CSwordModuleInfo;

/** BibleTime's implementation of Sword's TreeKeyIdx class.
 * @short CSwordKey implementation for Sword's TreeKey
 * @author The BibleTime team
 */

class CSwordTreeKey final : public CSwordKey, public sword::TreeKeyIdx {

    public:

        CSwordTreeKey & operator=(CSwordTreeKey const &) = delete;

        /**
          \param k The Sword tree key which belongs to this key
          \param module The module which belongs to this key
         */
        CSwordTreeKey(const sword::TreeKeyIdx *k,
                      const CSwordModuleInfo *module);

        CSwordTreeKey( const CSwordTreeKey& k );

        sword::SWKey const & asSwordKey() const noexcept final override;

        void setModule(const CSwordModuleInfo *newModule) final override;

        CSwordTreeKey* copy() const final override;

        /**
        * Returns the TreeKeyIdx::getLocalKey value in unicode.
        * Local key is the last part of the tree key, for example "Subsection1" from "/Section1/Subsection1".
        * Use this instead of getLocalKey() to avoid encoding problems.
        */
        QString getLocalNameUnicode();

        QString key() const final override;

        bool setKey(const QString &key) final override;

        bool setKey(const char *key) final override;

    protected:

        const char * rawKey() const final override;
};
