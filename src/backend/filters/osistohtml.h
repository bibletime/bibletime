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

#ifndef FILTERS_OSISTOHTML_H
#define FILTERS_OSISTOHTML_H

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <osishtmlhref.h>
#include <swbuf.h>
#include <swmodule.h>
#pragma GCC diagnostic pop


namespace Filters {

/**
  \brief OSIS to HTMl conversion filter.
*/
class OsisToHtml: public sword::OSISHTMLHREF {
    protected: /* Types: */
        class UserData: public sword::OSISHTMLHREF::MyUserData {
            public:
                UserData(sword::SWModule const * module,
                         sword::SWKey const * key)
                     : sword::OSISHTMLHREF::MyUserData(module, key),
                       swordFootnote(1), inCrossrefNote(false),
                       entryAttributes(module->getEntryAttributes()),
                       noteType(Unknown) {}

                unsigned short int swordFootnote;
                bool inCrossrefNote;
                sword::AttributeTypeList entryAttributes;

                enum NoteType {
                    Unknown,
                    Alternative,
                    CrossReference,
                    Footnote,
                    StrongsMarkup
                } noteType;

                struct {
                    sword::SWBuf who;
                } quote;
        };

    public: /* Methods: */
        OsisToHtml();

        /** Reimplemented from sword::OSISHTMLHREF. */
        bool handleToken(sword::SWBuf &buf,
                         const char *token,
                         sword::BasicFilterUserData *userData) override;

    protected: /* Methods: */
        /** Reimplemented from sword::OSISHTMLHREF. */
        sword::BasicFilterUserData * createUserData(
                sword::SWModule const * module,
                sword::SWKey const * key) override
        { return new UserData(module, key); }

    private: /* Methods: */
        void renderReference(const char *osisRef, sword::SWBuf &buf,
                             sword::SWModule *myModule, UserData *myUserData);
};

} // namespace Filters

#endif
