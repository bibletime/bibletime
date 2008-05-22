/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cindexoldbookmarksfolder.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "util/cpointers.h"
#include "backend/config/cbtconfig.h"

#include <QtXml/qdom.h>
#include <QString>

//please use QSettings instead of KConfig

CIndexOldBookmarksFolder::CIndexOldBookmarksFolder(CIndexTreeFolder* folder) : CIndexBookmarkFolder(folder, OldBookmarkFolder) {}

	CIndexOldBookmarksFolder::~CIndexOldBookmarksFolder() {}

	/** Reimplementation to handle special bookmark tree. */
	void CIndexOldBookmarksFolder::initTree() {
		// Import the bookmarks of the previous BibleTime versions
		if (!CBTConfig::get
					( CBTConfig::readOldBookmarks )) { //if we havn't yet loaded the old bookmarks
				loadBookmarksFromXML( COldBookmarkImport::oldBookmarksXML() );
			}
	}


QDomElement CIndexOldBookmarksFolder::saveToXML( QDomDocument& doc )
{
	QDomElement elem = doc.createElement("Folder");
	elem.setAttribute("caption", text(0));

		// Append the XML nodes of all child items
// 		CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(firstChild());
// 		while( i ) {
// 			if (i->parent() == this) {
// 				QDomElement newElem = i->saveToXML( doc );
// 				if (!newElem.isNull()) {
// 					elem.appendChild( newElem ); //append to this folder
// 				}
// 			}
// 			i = dynamic_cast<CIndexItemBase*>( i->nextSibling() );
// 		}
		
	for(int n = 0; n < childCount(); n++) {
		CIndexItemBase* i = dynamic_cast<CIndexItemBase*>( child(n) );
		if (i->parent() == this) { //only one level under this folder
			QDomElement newElem = i->saveToXML( doc ); // the cild creates it's own XML code
			if (!newElem.isNull()) {
				elem.appendChild( newElem ); //append to this folder
			}
		}
	
	}

		
	// Save to config, that we imported the old bookmarks and that we have them on disk
	CBTConfig::set( CBTConfig::readOldBookmarks, true );

	return elem;
}

void CIndexOldBookmarksFolder::loadFromXML( QDomElement& /*element*/ ) {
	//this function is empty because the folder imports the old 1.2 bookmarks from the bt-groupmanager config file
}



const QString COldBookmarkImport::oldBookmarksXML( const QString& /*configFileName*/ )
{
// 	QString fileName = (configFileName.isEmpty()) ? "bt-groupmanager" : configFileName;
// 	KConfig* conf = new KConfig( fileName );
// 
// 	//KConfigGroupSaver groupSaver(config, configFileName.isEmpty() ? "Groupmanager" : "Bookmarks");
// 	KConfigGroup* config = &(conf->group(configFileName.isEmpty() ? "Groupmanager" : "Bookmarks"));
// 
// 	QDomDocument doc("DOC");
// 	doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
// 
// 	QDomElement content = doc.createElement("SwordBookmarks");
// 	content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
// 	doc.appendChild(content);
// 
// 	//first create the bookmark groups in the XML document, then add the bookmarks to each parent
// 	QMap<int, QDomElement> parentMap; //maps parent ids to dom elements
// 
// 
// 	QStringList groupList;
// 	config->readEntry("Groups", groupList);
// 	QList<int> parentList;
// 	config->readEntry("Group parents", parentList);
// 
// 	QStringList::Iterator it_groups = groupList.begin();
// 	QList<int>::Iterator it_parents = parentList.begin();
// 
// 	int parentIDCounter = 0;
// 	while ( (it_groups != groupList.end()) && (it_parents != parentList.end()) ) {
// 		QDomElement parentElement = (*it_parents == -1) ? content : parentMap[*it_parents];
// 		if (parentElement.isNull()) {
// 			qWarning("EMPTY PARENT FOUND!");
// 			parentElement = content;
// 		}
// 
// 		QDomElement elem = doc.createElement("Folder");
// 		elem.setAttribute("caption", (*it_groups));
// 		parentMap.insert(parentIDCounter, elem);
// 
// 		parentElement.appendChild( elem );
// 
// 
// 		++it_parents;
// 		++it_groups;
// 		++parentIDCounter;
// 	}
// 
// 	//groups are now read in, create now the bookmarks
// 	config->readEntry("Bookmark parents", parentList);
// 	QStringList bookmarkList;
// 	config->readEntry("Bookmarks", bookmarkList);
// 	QStringList bookmarkModulesList;
// 	config->readEntry("Bookmark modules", bookmarkModulesList);
// 	QStringList bookmarkDescriptionsList;
// 	config->readEntry("Bookmark descriptions", bookmarkDescriptionsList);
// 
// 	it_parents  = parentList.begin();
// 	QStringList::Iterator it_bookmarks    = bookmarkList.begin();
// 	QStringList::Iterator it_modules     = bookmarkModulesList.begin();
// 	QStringList::Iterator it_descriptions = bookmarkDescriptionsList.begin();
// 
// 	while ( it_bookmarks != bookmarkList.end() && it_parents != parentList.end() && it_modules != bookmarkModulesList.end() ) {
// 			QDomElement parentElement = ((*it_parents) == -1) ? content : parentMap[(*it_parents)];
// 			if (parentElement.isNull()) {
// 				qWarning("EMPTY PARENT FOUND!");
// 				parentElement = content;
// 			};
// 			QDomElement elem = doc.createElement("Bookmark");
// 
// 			elem.setAttribute("key", *it_bookmarks);
// 			elem.setAttribute("description", *it_descriptions);
// 			elem.setAttribute("modulename", *it_modules);
// 
// 			CSwordModuleInfo* m = CPointers::backend()->findModuleByName( *it_modules );
// 			elem.setAttribute("moduledescription", m ? m->config(CSwordModuleInfo::Description) : QString::null);
// 
// 			parentElement.appendChild( elem );
// 
// 
// 			++it_parents;
// 			++it_modules;
// 			++it_descriptions;
// 			++it_bookmarks;
// 		};
// 
// 		return doc.toString();
	return QString::null;
}

