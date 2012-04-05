/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookmarks/btbookmarkloader.h"

#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QFile>
#include <QIODevice>
#include <QTextCodec>
#include <QTextStream>
#include <QTreeWidgetItem>
#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/bookmarks/btbookmarkitem.h"
#include "frontend/bookmarks/btbookmarkfolder.h"
#include "util/tool.h"


#define CURRENT_SYNTAX_VERSION 1

QList<QTreeWidgetItem*> BtBookmarkLoader::loadTree(QString fileName) {
    QList<QTreeWidgetItem*> itemList;

    QDomDocument doc;
    doc.setContent(loadXmlFromFile(fileName));

    //bookmarkfolder::loadBookmarksFromXML()

    QDomElement document = doc.documentElement();
    if ( document.tagName() != "SwordBookmarks" ) {
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
        else {
            child = QDomElement(); //null
        }

    }

    return itemList;
}

QTreeWidgetItem* BtBookmarkLoader::handleXmlElement(QDomElement& element, QTreeWidgetItem* parent) {
    QTreeWidgetItem* newItem = 0;
    if (element.tagName() == "Folder") {
        BtBookmarkFolder* newFolder = new BtBookmarkFolder(QString::null, parent);
        if (element.hasAttribute("caption")) {
            newFolder->setText(0, element.attribute("caption"));
        }
        QDomNodeList childList = element.childNodes();
#if QT_VERSION < 0x050000
        for (unsigned int i = 0; i < childList.length(); i++) {
#else
        for (int i = 0; i < childList.length(); i++) {
#endif
            QDomElement newElement = childList.at(i).toElement();
            QTreeWidgetItem* newChildItem = handleXmlElement(newElement, newFolder);
            newFolder->addChild(newChildItem);
        }
        newFolder->update();
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
        if (element.hasAttribute("title")) {
            newBookmarkItem->m_title = element.attribute("title");
        }
        newBookmarkItem->update();
        newItem = newBookmarkItem;
    }
    return newItem;
}


QString BtBookmarkLoader::loadXmlFromFile(QString fileName) {
    namespace DU = util::directory;

    if (fileName.isNull()) {
        fileName = DU::getUserBaseDir().absolutePath() + "/bookmarks.xml";
    }
    QFile file(fileName);
    if (!file.exists())
        return QString::null;

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

void BtBookmarkLoader::saveTreeFromRootItem(QTreeWidgetItem* rootItem, QString fileName, bool forceOverwrite) {
    namespace DU = util::directory;

    Q_ASSERT(rootItem);
    if (fileName.isNull()) {
        fileName = DU::getUserBaseDir().absolutePath() + "/bookmarks.xml";
    }

    QDomDocument doc("DOC");
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = doc.createElement("SwordBookmarks");
    content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
    doc.appendChild(content);

    //append the XML nodes of all child items

    for (int i = 0; i < rootItem->childCount(); i++) {
        saveItem(rootItem->child(i), content);
    }
    util::tool::savePlainFile(fileName, doc.toString(), forceOverwrite, QTextCodec::codecForName("UTF-8"));

}

void BtBookmarkLoader::saveItem(QTreeWidgetItem* item, QDomElement& parentElement) {
    BtBookmarkFolder* folderItem = 0;
    BtBookmarkItem* bookmarkItem = 0;

    if ((folderItem = dynamic_cast<BtBookmarkFolder*>(item))) {
        QDomElement elem = parentElement.ownerDocument().createElement("Folder");
        elem.setAttribute("caption", folderItem->text(0));

        parentElement.appendChild(elem);

        for (int i = 0; i < folderItem->childCount(); i++) {
            saveItem(folderItem->child(i), elem);
        }
    }
    else if ((bookmarkItem = dynamic_cast<BtBookmarkItem*>(item))) {
        QDomElement elem = parentElement.ownerDocument().createElement("Bookmark");

        elem.setAttribute("key", bookmarkItem->englishKey());
        elem.setAttribute("description", bookmarkItem->description());
        elem.setAttribute("modulename", bookmarkItem->m_moduleName);
        elem.setAttribute("moduledescription", bookmarkItem->module() ? bookmarkItem->module()->config(CSwordModuleInfo::Description) : QString::null);
    if ( ! bookmarkItem->title().isEmpty()) {
      elem.setAttribute("title", bookmarkItem->m_title);
    }
        parentElement.appendChild(elem);
    }
}
