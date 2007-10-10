//
// C++ Implementation: cindexbookmarkfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//



#include "cindexbookmarkfolder.h"
#include "cindexbookmarkitem.h"
#include "cindextreefolder.h"
#include "cindexfolderbase.h"
#include "cindexsubfolder.h"
#include "cindexoldbookmarksfolder.h"

#include "util/cpointers.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

#include <QTextStream>
#include <QString>
#include <QFile>
#include <QMimeData>

#include <kfiledialog.h>
#include <klocale.h>



CIndexBookmarkFolder::CIndexBookmarkFolder(CMainIndex* mainIndex, const Type type) : CIndexTreeFolder(mainIndex, type, "*") {
	setSortingEnabled(false);
}

CIndexBookmarkFolder::CIndexBookmarkFolder(CIndexFolderBase* parentItem, const Type type) : CIndexTreeFolder(parentItem, type, "*") {
	setSortingEnabled(false);
}

CIndexBookmarkFolder::~CIndexBookmarkFolder() {}

void CIndexBookmarkFolder::initTree() {
	addGroup(OldBookmarkFolder, "*");

	const QString path = util::filesystem::DirectoryUtil::getUserBaseDir().absolutePath() + "/";
	if (!path.isEmpty()) {
		loadBookmarks(path + "bookmarks.xml");
	}
}

/** Reimplementation. */
const bool CIndexBookmarkFolder::enableAction(const MenuAction action) {
	if ((action == NewFolder) || (action == ImportBookmarks))
		return true;

	if ((action == ExportBookmarks) && childCount())
		return true;

	if ((action == PrintBookmarks) && childCount())
		return true;

	return false;
}


void CIndexBookmarkFolder::exportBookmarks() {
	QString fileName = KFileDialog::getSaveFileName(KUrl(QString::null), i18n("*.btb | BibleTime bookmark files (*.btb)\n*.* | All files (*.*)"), 0, i18n("BibleTime - Export bookmarks"));
	if (!fileName.isEmpty()) {
		saveBookmarks( fileName, false ); //false means we don't want to overwrite the file without asking the user
	};
}


void CIndexBookmarkFolder::importBookmarks() {
	QString fileName = KFileDialog::getOpenFileName(KUrl(QString::null), i18n("*.btb | BibleTime bookmark files (*.btb)\n*.* | All files (*.*)"), 0, i18n("BibleTime - Import bookmarks"));
	if (!fileName.isEmpty()) {
		//we have to decide if we should load an old bookmark file from 1.2 or earlier or the new XML format of > 1.3
		if ( !loadBookmarks(fileName) ) { //if this failed try to load it as old bookmark file
			loadBookmarksFromXML( COldBookmarkImport::oldBookmarksXML( fileName ) );
		};
	};
}

bool CIndexBookmarkFolder::acceptDrop(const QMimeData * data) const {
// 	//   qWarning("bool CIndexBookmarkFolder::acceptDrop(const QMimeSource * src): return%ii", (CDragDropMgr::canDecode(src) && (CDragDropMgr::dndType(src) == CDragDropMgr::Item::Bookmark)));
// 
// 	return CDragDropMgr::canDecode(src)
// 		   && (CDragDropMgr::dndType(src) == CDragDropMgr::Item::Bookmark);
	qDebug("CIndexBookmarkFolder::acceptDrop");
	if (data->hasFormat("BibleTime/Bookmark")) {
		return true;
	} else {
		return false;
	}
}

void CIndexBookmarkFolder::dropped(QDropEvent *e, QTreeWidgetItem* after) {
// 	if (acceptDrop(e)) {
// 		CDragDropMgr::ItemList dndItems = CDragDropMgr::decode(e);
// 		CDragDropMgr::ItemList::Iterator it;
// 		CItemBase* previousItem = dynamic_cast<CItemBase*>(after);
// 
// 		for( it = dndItems.begin(); it != dndItems.end(); ++it) {
// 			CSwordModuleInfo* module = CPointers::backend()->findModuleByName(
// 										   (*it).bookmarkModule()
// 									   );
// 
// 			CBookmarkItem* i = new CBookmarkItem(
// 								   this,
// 								   module,
// 								   (*it).bookmarkKey(),
// 								   (*it).bookmarkDescription()
// 							   );
// 
// 			if (previousItem) {
// 				i->moveAfter( previousItem );
// 			}
// 
// 			i->init();
// 			previousItem = i;
// 		};
// 	};
}

/** Saves the bookmarks in a file. */
const bool CIndexBookmarkFolder::saveBookmarks( const QString& filename, const bool& forceOverwrite ) {
	QDomDocument doc("DOC");
	doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

	QDomElement content = doc.createElement("SwordBookmarks");
	content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
	doc.appendChild(content);

	//append the XML nodes of all child items
	
	for(int n = 0; n < childCount(); n++) {
		CIndexItemBase* i = dynamic_cast<CIndexItemBase*>( child(n) );
		if (i->parent() == this) { //only one level under this folder
			QDomElement newElem = i->saveToXML( doc ); // the cild creates it's own XML code
			if (!newElem.isNull()) {
				content.appendChild( newElem ); //append to this folder
			}
		}
		
	}
	//QTextCodec& fileCodec
	return CToolClass::savePlainFile(filename, doc.toString(), forceOverwrite, QTextCodec::codecForName("UTF-8"));
}

const bool CIndexBookmarkFolder::loadBookmarksFromXML( const QString& xml ) {
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement document = doc.documentElement();
	if( document.tagName() != "SwordBookmarks" ) {
		qWarning("Not a BibleTime Bookmark XML file");
		return false;
	}

	CIndexItemBase* oldItem = 0;
	//restore all child items
	QDomElement child = document.firstChild().toElement();
	while ( !child.isNull() && child.parentNode() == document) {
		CIndexItemBase* i = 0;
		if (child.tagName() == "Folder") {
			i = new CIndexSubFolder(this, child);
		}
		else if (child.tagName() == "Bookmark") {
			i = new CIndexBookmarkItem(this, child);
		}
		if (!i) {
			break;
		}

		i->init();
		if (oldItem) {
			i->moveAfter(oldItem);
		}
		oldItem = i;

		if (!child.nextSibling().isNull()) {
			child = child.nextSibling().toElement();
		}
		else {
			break;
		}
	}
	return true;
}

/** Loads bookmarks from a file. */
const bool CIndexBookmarkFolder::loadBookmarks( const QString& filename ) {
	QFile file(filename);
	if (!file.exists())
		return false;

	QString xml;
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream t;
		//t.setEncoding(QTextStream::UnicodeUTF8); //set encoding before file is used for input!
		t.setAutoDetectUnicode(false);
		t.setCodec(QTextCodec::codecForName("UTF-8"));
		t.setDevice(&file);
		xml = t.readAll();
		file.close();
	}

	return loadBookmarksFromXML( xml );
}
