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

#ifndef FILTERS_TEITOHTML_H
#define FILTERS_TEITOHTML_H

#include <swordxx/filters/teihtmlhref.h>


namespace Filters {

/**
  \brief TEI to HTML conversion filter.
*/
class TeiToHtml: public swordxx::TEIHTMLHREF {
    public: /* Methods: */
        TeiToHtml();

        bool handleToken(std::string &buf,
                         const char *token,
                         swordxx::BasicFilterUserData *userData) override;

    private: /* Methods: */
        void renderReference(std::string const & osisRef,
                             std::string & buf,
                             swordxx::BasicFilterUserData * myUserData);
};

} // namespace Filters

#endif
