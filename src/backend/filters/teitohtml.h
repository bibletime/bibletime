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
#include <teihtmlhref.h>
#include <swbuf.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


namespace Filters {

/**
  \brief TEI to HTML conversion filter.
*/
class TeiToHtml: public sword::TEIHTMLHREF {
    public: // methods:
        TeiToHtml();

        bool handleToken(sword::SWBuf &buf,
                         const char *token,
                         sword::BasicFilterUserData *userData) override;

    private: // methods:
        void renderReference(const char *osisRef, sword::SWBuf &buf,
                             sword::BasicFilterUserData *myUserData);
        void renderTargetReference(const char *osisRef, sword::SWBuf &buf,
                             sword::BasicFilterUserData *myUserData);
};

} // namespace Filters
