//
// C++ Implementation: cindexsubfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
SubFolder::SubFolder(CFolderBase* parentItem, const QString& caption) : CBookmarkFolder(parentItem, BookmarkFolder) {
		m_startupXML = QDomElement();
		setText( 0, caption );
	}

SubFolder::SubFolder(CFolderBase* parentItem, QDomElement& xml ) : CBookmarkFolder(parentItem, BookmarkFolder) {
		m_startupXML = xml;
	}

	SubFolder::~SubFolder() {}

	void SubFolder::init() {
		CFolderBase::init();
		if (!m_startupXML.isNull())
			loadFromXML(m_startupXML);

		setDropEnabled(true);
		setRenameEnabled(0,true);
	}

	/** Reimplementation from  CItemBase. */
	const bool SubFolder::enableAction(const MenuAction action) {
		if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
			return true;

		if (action == ExportBookmarks || action == ImportBookmarks )
			return true; //not yet implemented

		if ((action == PrintBookmarks) && childCount())
			return true;

		return false;
	}

	/** Returns the XML code which represents the content of this folder. */
	QDomElement SubFolder::saveToXML( QDomDocument& doc ) {
		/**
		* Save all subitems (bookmarks and folders) to the XML file.
		* We get the XML code for the items by calling their own saveToXML implementations.
		*/
		QDomElement elem = doc.createElement("Folder");
		elem.setAttribute("caption", text(0));

		//append the XML nodes of all child items
		CItemBase* i = dynamic_cast<CItemBase*>(firstChild());
		while( i ) {
			if (i->parent() == this) {
				QDomElement newElem = i->saveToXML( doc );
				if (!newElem.isNull()) {
					elem.appendChild( newElem ); //append to this folder
				}
			}
			i = dynamic_cast<CItemBase*>( i->nextSibling() );
		}
		return elem;
	}

	/** Loads the content of this folder from the XML code passed as argument to this function. */
	void SubFolder::loadFromXML( QDomElement& elem ) {
		//get the caption and restore all child items!
		if (elem.hasAttribute("caption"))
			setText(0, elem.attribute("caption"));

		//restore all child items
		QDomElement child = elem.firstChild().toElement();
		CIndexItemBase* oldItem = 0;
		while ( !child.isNull() && child.parentNode() == elem ) {
			CItemBase* i = 0;
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
