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

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swkey.h>
#pragma GCC diagnostic pop


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

class CSwordLDKey final : public CSwordKey {

    public:

        CSwordLDKey & operator=(CSwordLDKey const &) = delete;

        /**
          \todo Document param
        */
        CSwordLDKey(const CSwordModuleInfo *module);

        CSwordLDKey(const CSwordLDKey &copy);

        /**
          \todo Document params
        */
        CSwordLDKey(const sword::SWKey *k, const CSwordModuleInfo *module);

        sword::SWKey const & asSwordKey() const noexcept final override;

        CSwordLDKey* copy() const final override;
        /**
        * Uses the parameter to returns the next entry afer this key.
        */
        CSwordLDKey* NextEntry();
        /**
        * Uses the parameter to returns the previous entry afer this key.
        */
        CSwordLDKey* PreviousEntry();

        void setModule(const CSwordModuleInfo *module) final override;

        QString key() const final override;

        bool setKey(const QString &newKey) final override;

        bool setKey(const char *key) final override;

    protected:

        const char* rawKey() const final override;

    private: // fields:

        sword::SWKey m_key;

};
