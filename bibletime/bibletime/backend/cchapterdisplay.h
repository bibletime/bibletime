//
// C++ Interface: cchapterdisplay
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERINGCCHAPTERDISPLAY_H
#define RENDERINGCCHAPTERDISPLAY_H

#include "centrydisplay.h"

namespace Rendering {

/** Chapter rendering.
* A CEntryDisplay implementation mde for Bibles to display whole chapters
* at once.
* @author The BibleTime team
*/

class CChapterDisplay : public CEntryDisplay {

public: // Public methods
	virtual ~CChapterDisplay() {}

	/**
	* Returns the rendered text using the modules in the list and using the key parameter.
	* The displayoptions and filter options are used, too.
	*/
	virtual const QString text( const ListCSwordModuleInfo& modules, const QString& key, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions);
};

};

#endif
