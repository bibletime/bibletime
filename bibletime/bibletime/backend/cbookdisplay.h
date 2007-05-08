//
// C++ Interface: cbookdisplay
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RENDERINGCBOOKDISPLAY_H
#define RENDERINGCBOOKDISPLAY_H

#include "centrydisplay.h"

class CSwordTreeKey;

namespace Rendering {

	class CTextRendering::KeyTree;

	/**
	* A CEntryDisplay implementation which works on tree-based GenBook modules
	* of Sword.
	* @short CEntryDisplay implementation for GenBook modules,
	* @author The BibleTime team
	*/

class CBookDisplay : public CEntryDisplay {
public: // Public methods
		virtual ~CBookDisplay() {}

		/**
		* Returns the rendered text using the modules in the list and using the key parameter.
		* The displayoptions and filter options are used, too.
		*/
		virtual const QString text( const ListCSwordModuleInfo& modules, const QString& key, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions);

protected:
		void setupRenderTree(CSwordTreeKey* swordTree, CTextRendering::KeyTree* renderTree, const QString& highlightKey);
	};
};

#endif
