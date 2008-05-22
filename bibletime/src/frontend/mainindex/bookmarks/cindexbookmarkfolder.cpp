/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cindexbookmarkfolder.h"
#include "cindexbookmarkitem.h"
#include "cindextreefolder.h"
#include "cindexfolderbase.h"
#include "cindexsubfolder.h"
#include "cindexoldbookmarksfolder.h"

#include "util/cpointers.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

#include "frontend/cprinter.h"

#include <QTextStream>
#include <QString>
#include <QFile>
#include <QMimeData>
#include <QDropEvent>
#include <QFileDialog>


CIndexBookmarkFolder::CIndexBookmarkFolder(CBookmarkIndex* bookmarkIndex, const Type type)
	: CIndexTreeFolder(bookmarkIndex, type)
{}

CIndexBookmarkFolder::CIndexBookmarkFolder(CIndexFolderBase* parentItem, const Type type)
	: CIndexTreeFolder(parentItem, type)
{}

CIndexBookmarkFolder::~CIndexBookmarkFolder() {}

void CIndexBookmarkFolder::initTree()
{
	addGroup(OldBookmarkFolder);

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

	if ((action == PrintBookmarks) && childCount()){
	
		Printing::CPrinter::KeyTree tree;
		Printing::CPrinter::KeyTreeItem::Settings settings;

		QList<QTreeWidgetItem*> items = getChildList();

		//create a tree of keytreeitems using the bookmark hierarchy.
		QListIterator<QTreeWidgetItem*> it(items);
		while(it.hasNext()) {
			CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(it.next());
			if (i) {
				tree.append( new Printing::CPrinter::KeyTreeItem( i->key(), i->module(), settings ) );
			}
		}
		QList<CSwordModuleInfo*> modules = tree.collectModules();
		return modules.count() > 0;
	}

	return false;
}


void CIndexBookmarkFolder::exportBookmarks() {
	QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
	QString fileName = QFileDialog::getSaveFileName(0, QObject::tr("Export Bookmarks"), "", filter);
	
	if (!fileName.isEmpty()) {
		saveBookmarks( fileName, false ); //false means we don't want to overwrite the file without asking the user
	};
}

void CIndexBookmarkFolder::importBookmarks() {
	QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
	QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Import bookmarks"), "", filter);
	if (!fileName.isEmpty()) {
		//we have to decide if we should load an old bookmark file from 1.2 or earlier or the new XML format of > 1.3
		if ( !loadBookmarks(fileName) ) { //if this failed try to load it as old bookmark file
			loadBookmarksFromXML( COldBookmarkImport::oldBookmarksXML( fileName ) );
		};
	};
}

bool CIndexBookmarkFolder::acceptDrop(QDropEvent* e) const
{
	qDebug("CIndexBookmarkFolder::acceptDrop");
	if (e->provides("BibleTime/Bookmark")) {
		e->acceptProposedAction();
		return true;
	} else {
		return false;
	}
}

void CIndexBookmarkFolder::droppedItem(QDropEvent* /*e*/, QTreeWidgetItem* /*after*/)
{
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
			QDomElement newElem = i->saveToXML( doc ); // the child creates it's own XML code
			if (!newElem.isNull()) {
				content.appendChild( newElem ); //append to this folder
			}
		}
		
	}
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
		t.setAutoDetectUnicode(false);
		t.setCodec(QTextCodec::codecForName("UTF-8"));
		t.setDevice(&file);
		xml = t.readAll();
		file.close();
	}

	return loadBookmarksFromXML( xml );
}
