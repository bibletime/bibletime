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

#ifndef FILTERS_THMLTOHTML_H
#define FILTERS_THMLTOHTML_H

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swbuf.h>
#include <thmlhtml.h>
#pragma GCC diagnostic pop


namespace Filters {

/**
  \brief ThML to HTML conversion filter.
*/
class ThmlToHtml: public sword::ThMLHTML {
    protected: /* Types: */
        class UserData: public sword::ThMLHTML::MyUserData {
            public:
                UserData(sword::SWModule const * module,
                         sword::SWKey const * key)
                    : sword::ThMLHTML::MyUserData(module, key),
                      inscriptRef(false), inFootnoteTag(false),
                      swordFootnote(1) {}

                bool inscriptRef;
                bool inFootnoteTag;
                unsigned short int swordFootnote;
        };

    public: /* Methods: */
        ThmlToHtml();

        bool handleToken(sword::SWBuf &buf,
                         const char *token,
                         sword::BasicFilterUserData *userData) override;

        char processText(sword::SWBuf &buf,
                         const sword::SWKey *key,
                         const sword::SWModule *module = nullptr) override;

    protected: /* Methods: */

        sword::BasicFilterUserData * createUserData(
                sword::SWModule const * module,
                sword::SWKey const * key) override
        { return new UserData(module, key); }
};

} // namespace Filters

#endif
