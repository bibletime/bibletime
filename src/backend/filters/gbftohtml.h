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

#ifndef FILTERS_GBFTOHTML_H
#define FILTERS_GBFTOHTML_H

// Sword includes:
#include <gbfhtml.h>


namespace Filters {

/**
  \brief GBF to HTML conversion filter.
*/
class GbfToHtml: public sword::GBFHTML {
    protected: /* Types: */
        class UserData: public sword::GBFHTML::MyUserData {
            public:
                inline UserData(const sword::SWModule *module,
                                 const sword::SWKey *key)
                    : sword::GBFHTML::MyUserData(module, key),
                      swordFootnote(1)
                {
                    hasFootnotePreTag = false;
                }

                short unsigned int swordFootnote;
        };

    public: /* Methods: */
        GbfToHtml();

        /** Reimplemented from sword::OSISHTMLHREF. */
        bool handleToken(sword::SWBuf &buf,
                         const char *token,
                         sword::BasicFilterUserData *userData) override;

        /** Reimplemented from sword::SWFilter. */
        char processText(sword::SWBuf &buf,
                         const sword::SWKey *key,
                         const sword::SWModule *module = nullptr) override;

    protected: /* Methods: */
        /** Reimplemented from sword::OSISHTMLHREF. */
        inline sword::BasicFilterUserData *createUserData(
                const sword::SWModule *module, const sword::SWKey *key) override
        {
            return new UserData(module, key);
        }
};

} // namespace Filters

#endif
