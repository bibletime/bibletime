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

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <gbfhtml.h>
#include <swbasicfilter.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


namespace sword {
class SWBuf;
class SWKey;
class SWModule;
} // namespace sword

namespace Filters {

/**
  \brief GBF to HTML conversion filter.
*/
class GbfToHtml: public sword::GBFHTML {
    protected: // types:
        class UserData: public sword::GBFHTML::MyUserData {
            public:
                UserData(sword::SWModule const * module,
                         sword::SWKey const * key)
                    : sword::GBFHTML::MyUserData(module, key),
                      swordFootnote(1)
                {
                    hasFootnotePreTag = false;
                }

                short unsigned int swordFootnote;
        };

    public: // methods:
        GbfToHtml();

        /** Reimplemented from sword::OSISHTMLHREF. */
        bool handleToken(sword::SWBuf &buf,
                         const char *token,
                         sword::BasicFilterUserData *userData) override;

        /** Reimplemented from sword::SWFilter. */
        char processText(sword::SWBuf &buf,
                         const sword::SWKey *key,
                         const sword::SWModule *module = nullptr) override;

    protected: // methods:
        /** Reimplemented from sword::OSISHTMLHREF. */
        sword::BasicFilterUserData * createUserData(
                sword::SWModule const * module,
                sword::SWKey const * key) override
        { return new UserData(module, key); }
};

} // namespace Filters
