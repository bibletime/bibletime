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

#include "backend/config/btconfig.h"
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

    public: /* Types: */

        struct Settings {

            inline Settings(const bool text = true)
                : addText(text) {}

            bool addText;

        }; /* struct Settings */

    public: /* Methods: */

        CHTMLExportRendering(
            const Settings &settings,
            const DisplayOptions &displayOptions = getBtConfig().getDisplayOptions(),
            const FilterOptions &filterOptions = getBtConfig().getFilterOptions());

    protected: /* Methods: */

        virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 );
        virtual const QString finishText( const QString&, KeyTree& tree );
        virtual const QString entryLink(const KeyTreeItem &item,
                                        const CSwordModuleInfo *module);
        virtual void initRendering();

    protected: /* Fields: */

        DisplayOptions m_displayOptions;
        FilterOptions m_filterOptions;
        Settings m_settings;

}; /* class CHTMLExportRendering */

} /* namespace Rendering */

#endif
