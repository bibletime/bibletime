/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btbookmarkloader.h"

#include "btbookmarkitem.h"
#include "btbookmarkfolder.h"

#include "util/ctoolclass.h"

#include <QTreeWidgetItem>
#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QIODevice>
#include <QTextCodec>

#define CURRENT_SYNTAX_VERSION 1

QList<QTreeWidgetItem*> BtBookmarkLoader::loadTree()
{
	QList<QTreeWidgetItem*> itemList;
	
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
		itemList.append(i);
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
		BtBookmarkFolder* newFolder = new BtBookmarkFolder(parent);
		if (element.hasAttribute("caption")) {
			newFolder->setText(0, element.attribute("caption"));
		}
		foreach (QDomNode node, element.childNodes()) {
			QDomElement newElement = node.toElement();
			QTreeWidgetItem* newChildItem = handleXmlElement(newElement, newFolder);
			newFolder->addChild(newChildItem);
		}
		newItem = newFolder;
	}
	else if (element.tagName() == "Bookmark") {
		BtBookmarkItem* newBookmarkItem = new BtBookmarkItem(parent);
		if (element.hasAttribute("modulename")) {
			//we use the name in all cases, even if the module isn't installed anymore
			newBookmarkItem->m_moduleName = element.attribute("modulename");
		}
		if (element.hasAttribute("key")) {
			newBookmarkItem->m_key = element.attribute("key");
		}
		if (element.hasAttribute("description")) {
			newBookmarkItem->m_description = element.attribute("description");
		}
		newItem = newBookmarkItem;
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

void BtBookmarkLoader::saveTreeFromRootItem(QTreeWidgetItem* rootItem)
{
	const QString path = util::filesystem::DirectoryUtil::getUserBaseDir().absolutePath() + "/";
	if (!path.isEmpty()) {

		QDomDocument doc("DOC");
		doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
	
		QDomElement content = doc.createElement("SwordBookmarks");
		content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
		doc.appendChild(content);
	
		//append the XML nodes of all child items
	
		foreach(QTreeWidgetItem* childItem, rootItem->children()) {
			saveItem(childItem, content);
		}
		return CToolClass::savePlainFile(filename, doc.toString(), forceOverwrite, QTextCodec::codecForName("UTF-8"));
	}
}

void BtBookmarkLoader::saveItem(QTreeWidgetItem* item, QDomElement& parentElement)
{
	BtBookmarkFolder* folderItem = 0;
	BtBookmarkItem* bookmarkItem = 0;

	if (folderItem = dynamic_cast<BtBookmarkFolder*>(item)) {
		QDomElement elem = doc.createElement("Folder");
		elem.setAttribute("caption", text(0));

		parentElement.appendChild(elem);

		foreach (QTreeWidgetItem* subItem, folderItem->children()) {
			saveItem(subItem, elem);
		}
	}
	else if (bookmarkItem = dynamic_cast<BtBookmarkItem*>(item)) {
		QDomElement elem = doc.createElement("Bookmark");

		elem.setAttribute("key", englishKey());
		elem.setAttribute("description", description());
		elem.setAttribute("modulename", m_moduleName);
		elem.setAttribute("moduledescription", module() ? module()->config(CSwordModuleInfo::Description) : QString::null);

		parentElement.appendChild(elem);
	}
}
