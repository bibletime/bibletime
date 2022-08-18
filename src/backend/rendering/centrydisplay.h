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

namespace CEntryDisplay {

/**
  \returns the rendered text using the modules in the list and using the
           key parameter.
*/
QString text(
        BtConstModuleList const & modules,
        QString const & key,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions,
        CTextRendering::KeyTreeItem::Settings::KeyRenderingFace keyRendering =
                CTextRendering::KeyTreeItem::Settings::CompleteShort);

}; /* namespace CEntryDisplay */

} /* namespace Rendering */
