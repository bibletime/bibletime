/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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
        /**
          \param k The Sword tree key which belongs to this key
          \param module The module which belongs to this key
         */
        CSwordTreeKey(const sword::TreeKeyIdx *k,
                      const CSwordModuleInfo *module);

        CSwordTreeKey( const CSwordTreeKey& k );

        /**
          Reimplemented from CSwordKey.
        */
        virtual void setModule(const CSwordModuleInfo *newModule) override;

        /** Copy method.
        * @return A new copy of this object.
        */
        virtual CSwordTreeKey* copy() const override;

        /**
        * Returns the TreeKeyIdx::getLocalKey value in unicode.
        * Local key is the last part of the tree key, for example "Subsection1" from "/Section1/Subsection1".
        * Use this instead of getLocalKey() to avoid encoding problems.
        */
        QString getLocalNameUnicode();
        /**
        * Returns the current key as unicode decoded QString.
        */
        virtual QString key() const override;

        /**
          Reimplemented from CSwordKey::setKey(const QString &key).
        */
        virtual bool setKey(const QString &key) override;

        /**
          Reimplemented from CSwordKey::setKey(const char *key).
        */
        virtual bool setKey(const char *key) override;

    protected:
        /**
         * Returns the raw key appropriate for use directly with Sword.
         */
        virtual const char * rawKey() const override;

    private:
        /** Disable assignment operator */
        CSwordTreeKey& operator= (const CSwordTreeKey&);
        /** Disable from base class to prevent compiler warnings */
        inline virtual CSwordTreeKey& operator= (const sword::TreeKeyIdx&) override {
            return (*this);
        };
};

#endif
