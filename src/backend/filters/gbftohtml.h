/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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
        virtual bool handleToken(sword::SWBuf &buf,
                                 const char *token,
                                 sword::BasicFilterUserData *userData);

        /** Reimplemented from sword::SWFilter. */
        virtual char processText(sword::SWBuf &buf,
                                 const sword::SWKey *key,
                                 const sword::SWModule *module = 0);

    protected: /* Methods: */
        /** Reimplemented from sword::OSISHTMLHREF. */
        virtual inline sword::BasicFilterUserData *createUserData(
                const sword::SWModule *module, const sword::SWKey *key)
        {
            return new UserData(module, key);
        }
};

} // namespace Filters

#endif
