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

#ifndef RENDERINGCROSSREFRENDERING_H
#define RENDERINGCROSSREFRENDERING_H

#include "chtmlexportrendering.h"

#include "../config/btconfig.h"


namespace Rendering {

class CrossRefRendering : public Rendering::CHTMLExportRendering {

public: /* Methods: */

    CrossRefRendering(
        const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
        const FilterOptions &filterOptions = btConfig().getFilterOptions()
    );

protected: /* Methods: */

    QString entryLink(const KeyTreeItem &item,
                      const CSwordModuleInfo *module) override;

    QString finishText(const QString &text, const KeyTree &tree) override;

}; /* class CrossRefRendering */

} /* namespace Rendering */

#endif /* RENDERINGCROSSREFRENDERING_H */
