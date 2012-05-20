/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCHTMLEXPORTRENDERING_H
#define RENDERINGCHTMLEXPORTRENDERING_H

#include "backend/rendering/ctextrendering.h"

#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "btglobal.h"


namespace Rendering {

/**
 * This CTextRenerding implementation
 * creates HTML specially made for export as HTML files.
 * @short HTML rendering for export.
 * @author The BibleTime team
 */
class CHTMLExportRendering: public CTextRendering {

    public: /* Methods: */

        CHTMLExportRendering(
            bool addText,
            const DisplayOptions &displayOptions = CBTConfig::getDisplayOptionDefaults(),
            const FilterOptions &filterOptions = CBTConfig::getFilterOptionDefaults());

        ~CHTMLExportRendering() {};

    protected: /* Methods: */

        virtual QString renderEntry(const KeyTreeItem &item, CSwordKey * key = 0);
        virtual QString finishText(const QString &text, const KeyTree &tree);
        virtual QString entryLink(const KeyTreeItem &item,
                                  const CSwordModuleInfo *module);
        virtual void initRendering();

    protected: /* Fields: */

        DisplayOptions m_displayOptions;
        FilterOptions m_filterOptions;
        bool m_addText;

}; /* class CHTMLExportRendering */

} /* namespace Rendering */

#endif
