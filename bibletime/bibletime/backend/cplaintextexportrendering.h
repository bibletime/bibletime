//
// C++ Interface: cplaintextexportrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERINGCPLAINTEXTEXPORTRENDERING_H
#define RENDERINGCPLAINTEXTEXPORTRENDERING_H

#include "chtmlexportrendering.h"

namespace Rendering {

	/**
	 * This implementation can be used to export content as plain text.
	 * @short Text rendering as plain text.
	 * @author The BibleTime team
	 */

class CPlainTextExportRendering : public CHTMLExportRendering {

public:
		CPlainTextExportRendering(
			const Settings& settings,
			CSwordBackend::DisplayOptions displayOptions = CBTConfig::getDisplayOptionDefaults(),
			CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults()
		);
		virtual ~CPlainTextExportRendering();

protected:
		virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 );
		virtual const QString finishText( const QString&, KeyTree& tree );
	};

};

#endif
