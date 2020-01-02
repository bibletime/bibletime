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

#ifndef INFODISPLAYCROSSREFRENDERING_H
#define INFODISPLAYCROSSREFRENDERING_H

#include "../backend/rendering/chtmlexportrendering.h"
#include "../backend/config/btconfig.h"


namespace InfoDisplay {

class CrossRefRendering : public Rendering::CHTMLExportRendering {
    protected:
        friend class CInfoDisplay;

        CrossRefRendering(
            const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
            const FilterOptions &filterOptions = btConfig().getFilterOptions()
        );

        QString entryLink(const KeyTreeItem &item,
                          const CSwordModuleInfo *module) override;

        QString finishText(const QString &text, const KeyTree &tree) override;
};


}

#endif
