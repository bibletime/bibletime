/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "creaddisplay.h"

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbiblemoduleinfo.h"
#include "backend/cswordlexiconmoduleinfo.h"
#include "backend/cswordbookmoduleinfo.h"
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/cswordtreekey.h"
#include "backend/cswordldkey.h"

#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"

#include "frontend/cexportmanager.h"

#include "util/scoped_resource.h"

//Qt includes
#include <qpopupmenu.h>

//KDE includes
#include <klocale.h>

CReadDisplay::CReadDisplay(CReadWindow* readWindow) :
CDisplay(readWindow),
m_activeAnchor(QString::null),
m_useMouseTracking(true) {}

CReadDisplay::~CReadDisplay() {}

/** Returns the current active anchor. */
const QString& CReadDisplay::activeAnchor() {
	return m_activeAnchor;
}

/** Sets the current anchor to the parameter. */
void CReadDisplay::setActiveAnchor( const QString& anchor ) {
	m_activeAnchor = anchor;
}


/** Returns true if the display has an active anchor. */
const bool CReadDisplay::hasActiveAnchor() {
	return !activeAnchor().isEmpty();
}


void CReadDisplay::print(const CDisplay::TextPart type, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
	CDisplayWindow* window = parentWindow();
	CSwordKey* const key = window->key();
	CSwordModuleInfo* module = key->module();


	CExportManager mgr(i18n("Print keys"),false, QString::null, parentWindow()->filterOptions(), parentWindow()->displayOptions());

	switch (type) {
		case Document: {
			if (module->type() == CSwordModuleInfo::Bible) {
				CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>(key);

				CSwordVerseKey startKey(*vk);
				startKey.Verse(1);

				CSwordVerseKey stopKey(*vk);

				CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module);
				if (bible) {
					stopKey.Verse( bible->verseCount( bible->bookNumber(startKey.book()), startKey.Chapter() ) );
				}

				mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
			}
			else if (module->type() == CSwordModuleInfo::Lexicon || module->type() == CSwordModuleInfo::Commentary ) {
				mgr.printKey(module, key->key(), key->key(), displayOptions, filterOptions);
			}
			else if (module->type() == CSwordModuleInfo::GenericBook) {
				CSwordTreeKey* tree = dynamic_cast<CSwordTreeKey*>(key);

				CSwordTreeKey startKey(*tree);
				//        while (startKey.previousSibling()) { // go to first sibling on this level!
				//        }

				CSwordTreeKey stopKey(*tree);
				//    if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(module)) {
				//          while ( stopKey.nextSibling() ) { //go to last displayed sibling!
				//          }
				//        }
				mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
			}
		}

		case AnchorWithText: {
			if (hasActiveAnchor()) {
				mgr.printByHyperlink( activeAnchor(), displayOptions, filterOptions );
			}
		}

		default:
		break;
	}
}

