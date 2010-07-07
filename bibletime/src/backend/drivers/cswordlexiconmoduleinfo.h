/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDLEXICONMODULEINFO_H
#define CSWORDLEXICONMODULEINFO_H

#include "backend/drivers/cswordmoduleinfo.h"

#include <QStringList>


/**
  The implementation of CModuleInfo for the Sword lexiccons and citionaries.
*/
class CSwordLexiconModuleInfo: public CSwordModuleInfo {
        Q_OBJECT

    public: /* Methods: */
        CSwordLexiconModuleInfo(sword::SWModule *module,
                                CSwordBackend * const backend)
                : CSwordModuleInfo(module, backend, Lexicon), m_entryList(0) {}

        CSwordLexiconModuleInfo(const CSwordLexiconModuleInfo &copy);

        /* Reimplementation of CSwordModuleInfo::clone(). */
        virtual inline CSwordModuleInfo *clone() {
            return new CSwordLexiconModuleInfo(*this);
        }

        inline ~CSwordLexiconModuleInfo() {
            delete m_entryList;
        }

        /**
          This method returns the entries of the modules represented by this
          object. If this function is called for the first time the list is load
          from disk and stored in a list which cahes it. If the function is
          called again, the cached list is returned so we have a major speed
          improvement.
          \returns the list of lexicon entries in the module.
        */
        QStringList *entries();

        /**
          Jumps to the closest entry in the module.
        */
        inline bool snap() {
            return module()->getRawEntry();
        }

    private: /* Fields: */
        /**
        * This is the list which caches the entres of the module.
        */
        QStringList *m_entryList;
};

#endif
