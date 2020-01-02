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

#ifndef RENDERINGCCHAPTERDISPLAY_H
#define RENDERINGCCHAPTERDISPLAY_H

#include "centrydisplay.h"


namespace Rendering {

/**
  \brief CEntryDisplay implementation  for whole chapters.

  A CEntryDisplay implementation made for Bibles to display whole chapters at
  once.
*/
class CChapterDisplay: public CEntryDisplay {

    public: /* Methods: */

        const QString text(const BtConstModuleList &modules,
                           const QString &key,
                           const DisplayOptions &displayOptions,
                           const FilterOptions &filterOptions) override;

}; /* class CChapterDisplay */

} /* namespace Rendering */

#endif
