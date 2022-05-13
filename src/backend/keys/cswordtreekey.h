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

#include <QString>
#include <type_traits>

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <treekeyidx.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


class CSwordModuleInfo;

/** BibleTime's implementation of Sword's TreeKeyIdx class.
 * @short CSwordKey implementation for Sword's TreeKey
 * @author The BibleTime team
 */

class CSwordTreeKey final : public CSwordKey {

    public: // types:

        using Offset =
                decltype(std::declval<sword::TreeKeyIdx const &>().getOffset());

    public:

        #define BibleTime_CSwordTreeKey_DEFINE_COMP(op) \
            friend bool operator op(CSwordTreeKey const & lhs, \
                                    CSwordTreeKey const & rhs) \
            { return lhs.offset() op rhs.offset(); }
        #if __cpp_impl_three_way_comparison >= 201907L
        BibleTime_CSwordTreeKey_DEFINE_COMP(<=>)
        #else
        BibleTime_CSwordTreeKey_DEFINE_COMP(<)
        BibleTime_CSwordTreeKey_DEFINE_COMP(<=)
        BibleTime_CSwordTreeKey_DEFINE_COMP(==)
        BibleTime_CSwordTreeKey_DEFINE_COMP(!=)
        BibleTime_CSwordTreeKey_DEFINE_COMP(>=)
        BibleTime_CSwordTreeKey_DEFINE_COMP(>)
        #endif
        #undef BibleTime_CSwordTreeKey_DEFINE_COMP

        CSwordTreeKey & operator=(CSwordTreeKey const &) = delete;

        /**
          \param k The Sword tree key which belongs to this key
          \param module The module which belongs to this key
         */
        CSwordTreeKey(const sword::TreeKeyIdx *k,
                      const CSwordModuleInfo *module);

        CSwordTreeKey( const CSwordTreeKey& k );

        sword::TreeKeyIdx const & asSwordKey() const noexcept final override;

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

        bool hasChildren() { return m_key.hasChildren(); }
        void positionToRoot() { m_key.root(); }
        bool positionToParent() { return m_key.parent(); }
        bool positionToFirstChild() { return m_key.firstChild(); }
        bool positionToNextSibling() { return m_key.nextSibling(); }
        Offset offset() const { return m_key.getOffset(); }
        void setOffset(Offset value) { m_key.setOffset(value); }

    protected:

        const char * rawKey() const final override;

    private: // fields:

        sword::TreeKeyIdx m_key;

};
