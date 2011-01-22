/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CENTRYDISPLAY_H
#define CENTRYDISPLAY_H

#include <QString>

// Sword includes:
#include <swdisp.h>


class CSwordModuleInfo;
struct DisplayOptions;
struct FilterOptions;

namespace Rendering {

class CEntryDisplay: public sword::SWDisplay {
    public:
        /**
          \returns the rendered text using the modules in the list and using the
                   key parameter.
        */
        virtual const QString text(const QList<const CSwordModuleInfo*> &modules,
                                   const QString &key,
                                   const DisplayOptions &displayOptions,
                                   const FilterOptions &filterOptions);
};


}

#endif
