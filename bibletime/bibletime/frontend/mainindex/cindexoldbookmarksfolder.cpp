//
// C++ Implementation: cindexoldbookmarksfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

OldBookmarksFolder::OldBookmarksFolder(CTreeFolder* folder) : CBookmarkFolder(folder, OldBookmarkFolder) {}

	OldBookmarksFolder::~OldBookmarksFolder() {}

	/** Reimplementation to handle special bookmark tree. */
	void OldBookmarksFolder::initTree() {
		// Import the bookmarks of the previous BibleTime versions
		if (!CBTConfig::get
					( CBTConfig::readOldBookmarks )) { //if we havn't yet loaded the old bookmarks
				loadBookmarksFromXML( Bookmarks::OldBookmarkImport::oldBookmarksXML() );
			}
	}


	QDomElement OldBookmarksFolder::saveToXML( QDomDocument& doc ) {
		QDomElement elem = doc.createElement("Folder");
		elem.setAttribute("caption", text(0));

		// Append the XML nodes of all child items
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

		// Save to config, that we imported the old bookmarks and that we have them on disk
		CBTConfig::set
			( CBTConfig::readOldBookmarks, true );

		return elem;
	}

	void OldBookmarksFolder::loadFromXML( QDomElement& /*element*/ ) {
		//this function is empty because the folder imports the old 1.2 bookmarks from the bt-groupmanager config file
	}



const QString OldBookmarkImport::oldBookmarksXML( const QString& configFileName ) {
		QString fileName = (configFileName.isEmpty()) ? "bt-groupmanager" : configFileName;
		KConfig* config = new KSimpleConfig( fileName );

		KConfigGroupSaver groupSaver(config, configFileName.isEmpty() ? "Groupmanager" : "Bookmarks");

		QDomDocument doc("DOC");
		doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

		QDomElement content = doc.createElement("SwordBookmarks");
		content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
		doc.appendChild(content);

		//first create the bookmark groups in the XML document, then add the bookmarks to each parent
		QMap<int, QDomElement> parentMap; //maps parent ids to dom elements


		QStringList groupList = config->readListEntry("Groups");
		Q3ValueList<int> parentList = config->readIntListEntry("Group parents");

		QStringList::Iterator it_groups = groupList.begin();
		Q3ValueList<int>::Iterator it_parents = parentList.begin();

		int parentIDCounter = 0;
		while ( (it_groups != groupList.end()) && (it_parents != parentList.end()) ) {
			QDomElement parentElement = (*it_parents == -1) ? content : parentMap[*it_parents];
			if (parentElement.isNull()) {
				qWarning("EMPTY PARENT FOUND!");
				parentElement = content;
			};

			QDomElement elem = doc.createElement("Folder");
			elem.setAttribute("caption", (*it_groups));
			parentMap.insert(parentIDCounter, elem);

			parentElement.appendChild( elem );


			++it_parents;
			++it_groups;
			++parentIDCounter;
		}

		//groups are now read in, create now the bookmarks
		parentList  = config->readIntListEntry("Bookmark parents");
		QStringList bookmarkList        = config->readListEntry("Bookmarks");
		QStringList bookmarkModulesList    = config->readListEntry("Bookmark modules");
		QStringList bookmarkDescriptionsList  = config->readListEntry("Bookmark descriptions");

		it_parents  = parentList.begin();
		QStringList::Iterator it_bookmarks    = bookmarkList.begin();
		QStringList::Iterator it_modules     = bookmarkModulesList.begin();
		QStringList::Iterator it_descriptions = bookmarkDescriptionsList.begin();

		while ( it_bookmarks != bookmarkList.end()
				&& it_parents != parentList.end()
				&& it_modules != bookmarkModulesList.end()
			  ) {
			QDomElement parentElement = ((*it_parents) == -1) ? content : parentMap[(*it_parents)];
			if (parentElement.isNull()) {
				qWarning("EMPTY PARENT FOUND!");
				parentElement = content;
			};
			QDomElement elem = doc.createElement("Bookmark");

			elem.setAttribute("key", *it_bookmarks);
			elem.setAttribute("description", *it_descriptions);
			elem.setAttribute("modulename", *it_modules);

			CSwordModuleInfo* m = CPointers::backend()->findModuleByName( *it_modules );
			elem.setAttribute("moduledescription", m ? m->config(CSwordModuleInfo::Description) : QString::null);

			parentElement.appendChild( elem );


			++it_parents;
			++it_modules;
			++it_descriptions;
			++it_bookmarks;
		};

		return doc.toString();
	};

