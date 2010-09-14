/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCHTMLEXPORTRENDERING_H
#define RENDERINGCHTMLEXPORTRENDERING_H

#include "backend/managers/cswordbackend.h"
#include "ctextrendering.h"

#include "backend/config/cbtconfig.h"

namespace Rendering {

	/**
	 * This CTextRenerding implementation
	 * creates HTML specially made for export as HTML files.
	 * @short HTML rendering for export.
	 * @author The BibleTime team
	 */

class CHTMLExportRendering : public CTextRendering {

public:
	struct Settings {
		Settings(const bool text = true) {
			addText = text;
		};

		bool addText;
	};

	CHTMLExportRendering(
		const Settings& settings,
		CSwordBackend::DisplayOptions displayOptions = CBTConfig::getDisplayOptionDefaults(),
		CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults()
	);
	virtual ~CHTMLExportRendering();

protected:
	virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 );
	virtual const QString finishText( const QString&, KeyTree& tree );
	virtual const QString entryLink( const KeyTreeItem& item, CSwordModuleInfo* module );
	virtual void initRendering();

	CSwordBackend::DisplayOptions m_displayOptions;
	CSwordBackend::FilterOptions m_filterOptions;
	Settings m_settings;
};

}

#endif
