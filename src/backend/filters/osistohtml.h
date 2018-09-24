/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef FILTERS_OSISTOHTML_H
#define FILTERS_OSISTOHTML_H

#include <swordxx/filters/osishtmlhref.h>
#include <swordxx/swmodule.h>


namespace Filters {

/**
  \brief OSIS to HTMl conversion filter.
*/
class OsisToHtml: public swordxx::OSISHTMLHREF {
    protected: /* Types: */
        class UserData: public swordxx::OSISHTMLHREF::MyUserData {
            public:
                inline UserData(const swordxx::SWModule *module,
                                const swordxx::SWKey *key)
                     : swordxx::OSISHTMLHREF::MyUserData(module, key),
                       swordFootnote(1), inCrossrefNote(false),
                       entryAttributes(module->getEntryAttributes()),
                       noteType(Unknown) {}

                unsigned short int swordFootnote;
                bool inCrossrefNote;
                swordxx::SWModule::AttributeTypeList entryAttributes;

                enum NoteType {
                    Unknown,
                    Alternative,
                    CrossReference,
                    Footnote,
                    StrongsMarkup
                } noteType;

                struct {
                    std::string who;
                } quote;
        };

    public: /* Methods: */
        OsisToHtml();

        /** Reimplemented from swordxx::OSISHTMLHREF. */
        bool handleToken(std::string &buf,
                         const char *token,
                         swordxx::BasicFilterUserData *userData) override;

    protected: /* Methods: */
        /** Reimplemented from swordxx::OSISHTMLHREF. */
        inline std::unique_ptr<swordxx::BasicFilterUserData> createUserData(
                const swordxx::SWModule *module,
                const swordxx::SWKey *key) override
        { return std::make_unique<UserData>(module, key); }

    private: /* Methods: */
        void renderReference(const char *osisRef, std::string &buf,
                             swordxx::SWModule *myModule, UserData *myUserData);
};

} // namespace Filters

#endif
