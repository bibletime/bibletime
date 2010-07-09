/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDLDKEY_H
#define CSWORDLDKEY_H

#include "backend/keys/cswordkey.h"

#include <QString>

// Sword includes:
#include <swkey.h>


class CSwordModuleInfo;

/**
 * This class is the implementation of CSwordKey used for dictionaries and lexicons.
 *
 * CSwordLDKey is the implementation of CKey for Lexicons and dictionaries.
 * It provides a simple interface to set the current key,
 * to get the text for the key and functions to get the next and previous items
 * of the used module in comparision to the current key.<br/>
 * Here's an example how to use this class:<br/>
 * @code
 *  CSwordLexiconModuleInfo* m_module = backend()->findModuleByName("ISBE");
 *  CSwordLDKey ldKey(m_module);
 *  ldKey.key("Adam");
 *  ldKey.nextEntry();
 *  qDebug( QString("The current key is: %1").arg(ldKey.key()));
 * @endcode
 *
 * Please not, that the result will be invalid if use the operator const char*
 * on the adress of the object, use something like this
  *
 * @code
 *  CSwordLDKey* key = new CSwordLDKey( lexicon_module );
 *  const QString keyname = key->getKey();
 * @endcode
 *
  * @author The BibleTime team
  * @version $Id: cswordldkey.h,v 1.24 2006/02/25 11:38:15 joachim Exp $
  */

class CSwordLDKey : public CSwordKey, public sword::SWKey {

    public:
        /**
          \todo Document param
        */
        CSwordLDKey(CSwordModuleInfo *module);

        CSwordLDKey(const CSwordLDKey &copy);

        /**
          \todo Document params
        */
        CSwordLDKey(const sword::SWKey *k, CSwordModuleInfo *module);

        /**
          Reimplementation of CSwordKey::copy()
        */
        virtual CSwordLDKey* copy() const;
        /**
        * Uses the parameter to returns the next entry afer this key.
        */
        CSwordLDKey* NextEntry( void );
        /**
        * Uses the parameter to returns the previous entry afer this key.
        */
        CSwordLDKey* PreviousEntry( void );
        /**
        * Sets the module of this key.
        */
        virtual CSwordModuleInfo* module( CSwordModuleInfo* const module = 0 );
        /**
        * Returns the current key as a QString
        */
        virtual QString key() const;

        /**
          Reimplemented from CSwordKey::setKey(const QString &key).
        */
        virtual bool setKey(const QString &newKey);

        /**
          Reimplemented from CSwordKey::setKey(const char *key).
        */
        virtual bool setKey(const char *key);

    protected:
        /**
         * Returns the raw key appropriate for use directly with Sword.
         */
        virtual const char* rawKey() const;

    private:
        /**
        * Disable assignment operator
        */
        CSwordLDKey& operator= (const CSwordLDKey& );

};


#endif

