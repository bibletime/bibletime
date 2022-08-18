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

#include <QString>
#include "../drivers/btmodulelist.h"
#include "ctextrendering.h"


struct DisplayOptions;
struct FilterOptions;

namespace Rendering {

class CEntryDisplay {

    public: // methods:

        virtual ~CEntryDisplay() {}

        /**
          \returns the rendered text using the modules in the list and using the
                   key parameter.
        */
        virtual const QString text(const BtConstModuleList &modules,
                                   const QString &key,
                                   const DisplayOptions &displayOptions,
                                   const FilterOptions &filterOptions);

    const QString textKeyRendering(const BtConstModuleList &modules,
                               const QString &key,
                               const DisplayOptions &displayOptions,
                               const FilterOptions &filterOptions,
                               CTextRendering::KeyTreeItem::Settings::KeyRenderingFace keyRendering);

}; /* class CEntryDisplay */

} /* namespace Rendering */
