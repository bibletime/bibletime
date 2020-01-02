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

#ifndef CSWORDLEXICONMODULEINFO_H
#define CSWORDLEXICONMODULEINFO_H

#include "cswordmoduleinfo.h"

#include <QStringList>


/**
  The implementation of CModuleInfo for the Sword lexiccons and citionaries.
*/
class CSwordLexiconModuleInfo: public CSwordModuleInfo {
        Q_OBJECT

    public: /* Methods: */
        inline CSwordLexiconModuleInfo(sword::SWModule & module,
                                       CSwordBackend & backend)
                : CSwordModuleInfo(module, backend, Lexicon),
                m_hasStrongsKeys(false),
                m_hasLeadingStrongsLetter(false),
                m_strongsDigitsLength(0) {
            testForStrongsKeys();
        }

        /**
          Return true if this lexicon has Strong's keys
        */
        bool hasStrongsKeys() const;

        /**
          Takes a Strong's key string and formats it consistent
          with the key strings of this module.
        */
        QString normalizeStrongsKey(const QString& key) const;

        /**
          This method returns the entries of the modules represented by this
          object. If this function is called for the first time the list is load
          from disk and stored in a list which cahes it. If the function is
          called again, the cached list is returned so we have a major speed
          improvement.
          \returns the list of lexicon entries in the module.
        */
        const QStringList &entries() const;

        /**
          Jumps to the closest entry in the module.
        */
        inline bool snap() const override {
            return module().getRawEntry();
        }

    private: /* Fields: */

        /**
          See if module keys are consistent with Strong's references
          and determine if keys start with "G" or "H" and the number
          of digits in the keys.
         */
        void testForStrongsKeys();

        bool m_hasStrongsKeys;
        bool m_hasLeadingStrongsLetter;
        int m_strongsDigitsLength;

        /**
          This is the list which caches the entres of the module.
        */
        mutable QStringList m_entries;
};

#endif
