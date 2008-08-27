/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cindexsubfolder.h"
#include "cindexfolderbase.h"
#include "cindexbookmarkfolder.h"
#include "cindexbookmarkitem.h"

#include <QtXml/qdom.h>


CIndexSubFolder::CIndexSubFolder(CIndexFolderBase* parentItem, const QString& caption)
	: CIndexBookmarkFolder(parentItem, BookmarkFolder)
{
	m_startupXML = QDomElement();
	setText( 0, caption );
}

CIndexSubFolder::CIndexSubFolder(CIndexFolderBase* parentItem, QDomElement& xml )
	: CIndexBookmarkFolder(parentItem, BookmarkFolder)
{
	m_startupXML = xml;
}

CIndexSubFolder::~CIndexSubFolder() {}

void CIndexSubFolder::init()
{
	CIndexFolderBase::init();
	if (!m_startupXML.isNull())
		loadFromXML(m_startupXML);
}

/** Reimplementation from  CItemBase. */
const bool CIndexSubFolder::enableAction(const MenuAction action) {
	if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
		return true;
	if (action == ExportBookmarks || action == ImportBookmarks )
		return true;
	if ((action == PrintBookmarks) && childCount())
		return true;
	return false;
}

/** Returns the XML code which represents the content of this folder. */
QDomElement CIndexSubFolder::saveToXML( QDomDocument& doc ) {
	/**
	* Save all subitems (bookmarks and folders) to the XML file.
	* We get the XML code for the items by calling their own saveToXML implementations.
	*/
	QDomElement elem = doc.createElement("Folder");
	elem.setAttribute("caption", text(0));
	
	for(int n = 0; n < childCount(); n++) {
		CIndexItemBase* i = dynamic_cast<CIndexItemBase*>( child(n) );
		if (i->parent() == this) { //only one level under this folder
			QDomElement newElem = i->saveToXML( doc ); // the child creates it's own XML code
			if (!newElem.isNull()) {
				elem.appendChild( newElem ); //append to this folder
			}
		}
		
	}

	return elem;
}

/** Loads the content of this folder from the XML code passed as argument to this function. */
void CIndexSubFolder::loadFromXML( QDomElement& elem )
{
	//get the caption and restore all child items!
	if (elem.hasAttribute("caption"))
		setText(0, elem.attribute("caption"));

	//restore all child items
	QDomElement child = elem.firstChild().toElement();
	CIndexItemBase* oldItem = 0;
	while ( !child.isNull() && child.parentNode() == elem ) {
		CIndexItemBase* i = 0;
		if (child.tagName() == "Folder") {
			i = new CIndexSubFolder(this, child);
		}
		else if (child.tagName() == "Bookmark") {
			i = new CIndexBookmarkItem(this, child);
		}
		i->init();
		if (oldItem)
			i->moveAfter(oldItem);
		oldItem = i;

		child = child.nextSibling().toElement();
	}
}
