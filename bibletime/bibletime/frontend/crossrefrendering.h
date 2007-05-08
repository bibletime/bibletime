//
// C++ Interface: crossrefrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INFODISPLAYCROSSREFRENDERING_H
#define INFODISPLAYCROSSREFRENDERING_H

//Backend includes
#include "backend/chtmlexportrendering.h"

namespace InfoDisplay {

class CrossRefRendering : public Rendering::CHTMLExportRendering {
protected:
		friend class CInfoDisplay;

		CrossRefRendering(
			CSwordBackend::DisplayOptions displayOptions = CBTConfig::getDisplayOptionDefaults(),
			CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults()
		);

		virtual const QString entryLink( const KeyTreeItem& item, CSwordModuleInfo* module );
		virtual const QString finishText( const QString&, KeyTree& tree );
	};


};

#endif
