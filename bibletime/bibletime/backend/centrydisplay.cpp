/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "centrydisplay.h"

#include "cswordkey.h"
#include "cswordversekey.h"
#include "cswordbookmoduleinfo.h"
#include "creferencemanager.h"
#include "cdisplaytemplatemgr.h"
#include "cdisplayrendering.h"

#include "frontend/cbtconfig.h"

#include "util/scoped_resource.h"

//Qt includes
#include <qapplication.h>
#include <qregexp.h>

using namespace Rendering;

/** Returns the rendered text using the modules in the list and using the key parameter.
 * The displayoptions and filter options are used, too. 
 */
const QString CEntryDisplay::text( const ListCSwordModuleInfo& modules, const QString& keyName, const CSwordBackend::DisplayOptions displayOptions, const CSwordBackend::FilterOptions filterOptions ) {
	CDisplayRendering render(displayOptions, filterOptions);

	//no highlighted key and no extra key link in the text
	CTextRendering::KeyTreeItem::Settings normal_settings(false, CTextRendering::KeyTreeItem::Settings::CompleteShort);
	CSwordModuleInfo* module = modules.first();
	QString result;

	//in Bibles and Commentaries we need to check if 0:0 and X:0 contain something
	if (module->type() == CSwordModuleInfo::Bible || module->type() == CSwordModuleInfo::Commentary) {
		((VerseKey*)(module->module()->getKey()))->Headings(1); //HACK: enable headings for VerseKeys

		CSwordVerseKey k1(module);
		k1.Headings(1);
		k1.key(keyName);
		
		// don't print the key
		CTextRendering::KeyTreeItem::Settings preverse_settings(false, CTextRendering::KeyTreeItem::Settings::NoKey);

		if (k1.Verse() == 1){ //X:1, prepend X:0
			if (k1.Chapter() == 1){ //1:1, also prepend 0:0 before that
				k1.Chapter(0);
				k1.Verse(0);
				if ( k1.rawText().length() > 0 ) result.append( render.renderSingleKey(k1.key(), modules, preverse_settings ) );
				k1.Chapter(1);
			}
			k1.Verse(0);
			if ( k1.rawText().length() > 0 ) result.append( render.renderSingleKey(k1.key(), modules, preverse_settings ) );
		}
	}
	return result.append( render.renderSingleKey(keyName, modules, normal_settings) );
}
