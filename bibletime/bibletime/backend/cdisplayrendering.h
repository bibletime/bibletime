//
// C++ Interface: cdisplayrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERINGCDISPLAYRENDERING_H
#define RENDERINGCDISPLAYRENDERING_H

#include "chtmlexportrendering.h"

namespace Rendering {

/** HTML rendering for the text display widgets.
 * @short Rendering for the html display widget.
 * @author The BibleTime team
 */

class CDisplayRendering : public CHTMLExportRendering {
public:
	static const QString keyToHTMLAnchor(const QString& key);

	CDisplayRendering(
		CSwordBackend::DisplayOptions displayOptions = CBTConfig::getDisplayOptionDefaults(),
		CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults()
	);

protected:
	virtual const QString entryLink( const KeyTreeItem& item, CSwordModuleInfo* const module );
	virtual const QString finishText( const QString&, KeyTree& tree );
};

}

#endif
