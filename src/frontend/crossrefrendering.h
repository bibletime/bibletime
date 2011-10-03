/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef INFODISPLAYCROSSREFRENDERING_H
#define INFODISPLAYCROSSREFRENDERING_H

#include "backend/rendering/chtmlexportrendering.h"


namespace InfoDisplay {

class CrossRefRendering : public Rendering::CHTMLExportRendering {
    protected:
        friend class CInfoDisplay;

        CrossRefRendering(
            const DisplayOptions &displayOptions = CBTConfig::getDisplayOptionDefaults(),
            const FilterOptions &filterOptions = CBTConfig::getFilterOptionDefaults()
        );

        virtual QString entryLink(const KeyTreeItem &item,
                                  const CSwordModuleInfo *module);

        virtual QString finishText(const QString &text, const KeyTree &tree);
};


}

#endif
