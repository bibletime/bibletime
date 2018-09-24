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

#ifndef FILTERS_GBFTOHTML_H
#define FILTERS_GBFTOHTML_H

#include <swordxx/filters/gbfhtml.h>


namespace Filters {

/**
  \brief GBF to HTML conversion filter.
*/
class GbfToHtml: public swordxx::GBFHTML {
    protected: /* Types: */
        class UserData: public swordxx::GBFHTML::MyUserData {
            public:
                inline UserData(const swordxx::SWModule *module,
                                 const swordxx::SWKey *key)
                    : swordxx::GBFHTML::MyUserData(module, key),
                      swordFootnote(1)
                {
                    hasFootnotePreTag = false;
                }

                short unsigned int swordFootnote;
        };

    public: /* Methods: */
        GbfToHtml();

        /** Reimplemented from swordxx::OSISHTMLHREF. */
        bool handleToken(std::string &buf,
                         const char *token,
                         swordxx::BasicFilterUserData *userData) override;

        /** Reimplemented from swordxx::SWFilter. */
        char processText(std::string &buf,
                         const swordxx::SWKey *key,
                         const swordxx::SWModule *module = nullptr) override;

    protected: /* Methods: */
        /** Reimplemented from swordxx::OSISHTMLHREF. */
        inline std::unique_ptr<swordxx::BasicFilterUserData> createUserData(
                const swordxx::SWModule *module, const swordxx::SWKey *key) override
        { return std::make_unique<UserData>(module, key); }
};

} // namespace Filters

#endif
