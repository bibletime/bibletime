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

#include "cswordmoduleinfo.h"

#include <QObject>
#include <QString>
#include <QStringList>


class CSwordBackend;
namespace sword { class SWModule; }

/**
  The implementation of CModuleInfo for the Sword lexiccons and citionaries.
*/
class CSwordLexiconModuleInfo final: public CSwordModuleInfo {
        Q_OBJECT

    public: // methods:
        CSwordLexiconModuleInfo(sword::SWModule & module,
                                CSwordBackend & backend);

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

        /** Jumps to the closest entry in the module. */
        bool snap() const final override;

        CSwordKey * createKey() const final override;

    private: // fields:

        bool m_hasStrongsKeys = false;
        bool m_hasLeadingStrongsLetter = false;
        int m_strongsDigitsLength = 0;

        /**
          This is the list which caches the entres of the module.
        */
        mutable QStringList m_entries;
};
