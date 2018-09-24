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

#ifndef FILTERS_THMLTOHTML_H
#define FILTERS_THMLTOHTML_H

#include <swordxx/filters/thmlhtml.h>


namespace Filters {

/**
  \brief ThML to HTML conversion filter.
*/
class ThmlToHtml: public swordxx::ThMLHTML {
    protected: /* Types: */
        class UserData: public swordxx::ThMLHTML::MyUserData {
            public:
                inline UserData(const swordxx::SWModule *module,
                                const swordxx::SWKey *key)
                    : swordxx::ThMLHTML::MyUserData(module, key),
                      inscriptRef(false), inFootnoteTag(false),
                      swordFootnote(1) {}

                bool inscriptRef;
                bool inFootnoteTag;
                unsigned short int swordFootnote;
        };

    public: /* Methods: */
        ThmlToHtml();

        bool handleToken(std::string &buf,
                         const char *token,
                         swordxx::BasicFilterUserData *userData) override;

        char processText(std::string &buf,
                         const swordxx::SWKey *key,
                         const swordxx::SWModule *module = nullptr) override;

    protected: /* Methods: */

        inline std::unique_ptr<swordxx::BasicFilterUserData> createUserData(
                const swordxx::SWModule *module, const swordxx::SWKey *key) override
        { return std::make_unique<UserData>(module, key); }
};

} // namespace Filters

#endif
