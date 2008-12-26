/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btbookmarkloader.h"

#include <QTreeWidgetItem>
#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QIODevice>
#include <QTextCodec>

QList<QTreeWidgetItem*> BtBookmarkLoader::loadTree()
{
	QList<QTreeWidgetItem*> itemList();
	
	QDomDocument doc;
	doc.setContent(loadXmlFromFile());
	
	//bookmarkfolder::loadBookmarksFromXML()
	
	QDomElement document = doc.documentElement();
	if( document.tagName() != "SwordBookmarks" ) {
		qWarning("Not a BibleTime Bookmark XML file");
		return QList<QTreeWidgetItem*>();
	}

	QDomElement child = document.firstChild().toElement();

	while ( !child.isNull() && child.parentNode() == document) {
		QTreeWidgetItem* i = handleXmlElement(child, 0);
		itemList.add(i);
		if (!child.nextSibling().isNull()) {
			child = child.nextSibling().toElement();
		}
		
	}

	return itemList;
}

QTreeWidgetItem* BtBookmarkLoader::handleXmlElement(QDomElement& element, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* newItem = 0;
	if (element.tagName() == "Folder") {
		newItem = new BtBookmarkFolder(parent);
		if (elem.hasAttribute("caption")) {
			newItem->setText(0, elem.attribute("caption"));
		}
		foreach (QDomNode node, element.childNodes()) {
			newItem->addChild(handleXmlElement(node.toElement(), newItem));
		}
	}
	else if (element.tagName() == "Bookmark") {
		newItem = new BtBookmarkItem(parent);
		if (element.hasAttribute("modulename")) {
			//we use the name in all cases, even if the module isn't installed anymore
			newItem->m_moduleName = element.attribute("modulename");
		}
		if (element.hasAttribute("key")) {
			newItem->m_key = element.attribute("key");
		}
		if (element.hasAttribute("description")) {
			newItem->m_description = element.attribute("description");
		}
	}
	
	return newItem;
}


QString BtBookmarkLoader::loadXmlFromFile()
{
	QFile file(FILENAME);
	if (!file.exists())
		return QString();

	QString xml;
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream t;
		t.setAutoDetectUnicode(false);
		t.setCodec(QTextCodec::codecForName("UTF-8"));
		t.setDevice(&file);
		xml = t.readAll();
		file.close();
	}
	return xml;
}

BtBookmarkLoader::saveTreeFromRootItem(QTreeWidgetItem* rootItem)
{
	const QString path = util::filesystem::DirectoryUtil::getUserBaseDir().absolutePath() + "/";
	if (!path.isEmpty()) {
		//save the bookmarks to the right file
		if (CIndexBookmarkFolder* f = dynamic_cast<CIndexBookmarkFolder*>(i)) {
			f->saveBookmarks( path + "bookmarks.xml" );
		}
	}

	//f::saveBookmarks
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
