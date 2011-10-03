/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCDISPLAYRENDERING_H
#define RENDERINGCDISPLAYRENDERING_H

#include "backend/rendering/chtmlexportrendering.h"


namespace Rendering {

/** HTML rendering for the text display widgets.
 * @short Rendering for the html display widget.
 * @author The BibleTime team
 */

class CDisplayRendering : public CHTMLExportRendering {

    public: /* Methods: */

        static QString keyToHTMLAnchor(const QString &key);

        CDisplayRendering(
            const DisplayOptions &displayOptions = CBTConfig::getDisplayOptionDefaults(),
            const FilterOptions &filterOptions = CBTConfig::getFilterOptionDefaults());

    protected: /* Methods: */

        virtual QString entryLink(const KeyTreeItem &item,
                                  const CSwordModuleInfo * module);

        virtual QString finishText(const QString &text, const KeyTree &tree);

}; /* class CDisplayRendering */

} /* namespace Rendering */

#endif
