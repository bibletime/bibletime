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

#include "ctextrendering.h"

#include "../config/btconfig.h"


namespace Rendering {

class CrossRefRendering : public Rendering::CTextRendering {

public: // methods:

    CrossRefRendering(
        const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
        const FilterOptions &filterOptions = btConfig().getFilterOptions()
    );

protected: // methods:

    QString entryLink(KeyTreeItem const & item,
                      CSwordModuleInfo const & module) const override;

    QString finishText(QString const & text, KeyTree const & tree)
            const override;

}; /* class CrossRefRendering */

} /* namespace Rendering */
