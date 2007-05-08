/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CINDEXITEM_H
#define CINDEXITEM_H

//BibleTime includes
#include "util/cpointers.h"

#include "backend/cswordmoduleinfo.h"

//Qt includes
#include <qdom.h>

//KDE includes
#include <klistview.h>

class KConfig;
class CFolderBase;
class CTreeFolder;
class CMainIndex;

class CItemBase : public KListViewItem/*, public CPointers */ {
public:
	enum Type {
		Unknown = 0,
		BookmarkFolder,
		Bookmark,
		OldBookmarkFolder, /* Bookmarks in the old format from BibleTime 1.1.x and 1.2.x */
		BibleModuleFolder,
		CommentaryModuleFolder,
		LexiconModuleFolder,
		BookModuleFolder,
		DevotionalModuleFolder,
		GlossaryModuleFolder,
		ModuleLanguageFolder,
		Module
	};
	enum MenuAction {
		NewFolder = 0,
		ChangeFolder,

		ChangeBookmark,
		ImportBookmarks,
		ExportBookmarks,
		PrintBookmarks,

		DeleteEntries,

		EditModule,
		SearchInModules,
		UnlockModule,
		AboutModule,

		ActionBegin = NewFolder,
		ActionEnd = AboutModule
	};

	CItemBase(CMainIndex* mainIndex, const Type type = Unknown);
	CItemBase(CItemBase* item, const Type type = Unknown);
	virtual ~CItemBase();

	virtual const QString toolTip();
	virtual CMainIndex* listView() const;
	/**
	* Returns if the implementation of this class is a folder item or not.
	* Reimplement this function to return the correct value.
	*/
	virtual const bool isFolder() {
		return false;
	};
	const Type& type() const;

	virtual void init();
	virtual void update();
	void moveAfter( CItemBase* const item );
	/**
	* Returns true if the given action should be enabled in the popup menu.
	*/
	virtual const bool enableAction( const MenuAction action );
	virtual const bool isMovable();

	/**
	* Returns the XML code which represents the content of this folder.
	*/
	virtual QDomElement saveToXML( QDomDocument& /*document*/ ) {
		return QDomElement();
	};
	/**
	* Loads the content of this folder from the XML code passed as argument to this function.
	*/
	virtual void loadFromXML( QDomElement& /*element*/ ) {}
	;
	/**
	* Returns true whether the sorting is enabled or not.
	*/
	const bool isSortingEnabled();
	/**
	* This function engables or disables sorting depending on the parameter.
	*/
	void setSortingEnabled( const bool& enableSorting );
	/**
	* Reimplementation which takes care of the our isSortingEnabled() setting.
	*/
	virtual void sortChildItems( int col, bool asc );
	/**
	* Reimplementation which takes care of the our isSortingEnabled() setting.
	*/
	virtual void sort();

protected:
	friend class CMainIndex;

	/** Reimplementation which uses our extended version of dropped (see below).
	*/
	virtual void dropped( QDropEvent* e) {
		dropped(e,0);
	};
	/** Our extended version of the dropped method to include a item above the point we dropped the stuff.
	*/
	virtual void dropped( QDropEvent* /*e*/, QListViewItem* /*after*/) {}
	;
	/**
	* Reimplementation. Returns true if the auto opening of this folder is allowd
	* The default return value is "false"
	*/
	virtual const bool allowAutoOpen( const QMimeSource* src ) const;

private:
	Type m_type;
	bool m_sortingEnabled;
};

class CModuleItem : public CItemBase {
public:
	CModuleItem(CTreeFolder* item, CSwordModuleInfo* module);
	virtual ~CModuleItem();
	virtual CSwordModuleInfo* const module() const;
	virtual const QString toolTip();

	/**
	* Reimplementation from  CItemBase.
	*/
	virtual const bool enableAction( const MenuAction action );
	virtual void update();
	virtual void init();

protected: // Protected methods
	/**
	* Reimplementation to handle text drops on a module.
	* In this case open the searchdialog. In the case of a referebnce open the module at the given position.
	*/
	virtual bool acceptDrop( const QMimeSource* src ) const;
	virtual void dropped( QDropEvent* e, QListViewItem* after );

private:
	CSwordModuleInfo* m_module;
};

class CBookmarkItem : public CItemBase {
public:
	CBookmarkItem(CFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description);
	CBookmarkItem(CFolderBase* parentItem, QDomElement& xml);
	virtual ~CBookmarkItem();
	CSwordModuleInfo* const module();
	const QString key();
	const QString& description();
	virtual const QString toolTip();

	//virtual int compare( QListViewItem * i, int col, bool ascending ) const;

	virtual void update();
	virtual void init();
	virtual const bool isMovable();
	/**
	* Reimplementation to handle  the menu entries of the main index.
	*/
	virtual const bool enableAction(const MenuAction action);
	/**
	* Prints this bookmark.
	*/
	//   void print();
	/**
	* Changes this bookmark.
	*/
	void rename();
	/**
	* Reimplementation of CItemBase::saveToXML.
	*/
	virtual QDomElement saveToXML( QDomDocument& document );
	/**
	* Loads the content of this folder from the XML code passed as argument to this function.
	*/
	virtual void loadFromXML( QDomElement& element );

private:
	QString m_key;
	QString m_description;
	QString m_moduleName;

	QDomElement m_startupXML;

protected: // Protected methods
	/**
	* Reimplementation. Returns false everytime
	* because a bookmarks 
	* has not possible drops.
	*/
	virtual bool acceptDrop(const QMimeSource * src) const;

private: // Private methods
	/**
	* Returns the english key.
	* Only used internal of this class implementation.
	*/
	const QString& englishKey() const;
};


class CFolderBase : public CItemBase {
public:
	CFolderBase(CMainIndex* mainIndex, const Type type);
	CFolderBase(CFolderBase* parentFolder, const Type type);
	CFolderBase(CFolderBase* parentFolder, const QString& caption);
	virtual ~CFolderBase();

	virtual const bool isFolder();

	virtual void update();
	virtual void init();
	virtual void setOpen( bool open );
	/**
	* The function which renames this folder.
	*/
	void rename();
	virtual void newSubFolder();

	QPtrList<QListViewItem> getChildList();

protected:
	/**
	* Reimplementation. Returns true if the auto opening of this folder is allowd
	*/
	virtual const bool allowAutoOpen( const QMimeSource* src ) const;
	/**
	* Reimplementation. Returns false because folders have no use for drops
	* (except for the bookmark folders) 
	*/
	bool acceptDrop(const QMimeSource * src) const;
};

/** The base class for all items in the tree. Subclasses for module folders, modules and bookmarks exist.
  * @author The BibleTime team
  */
class CTreeFolder : public CFolderBase {
public:
	CTreeFolder(CMainIndex* mainIndex, const Type type, const QString& language );
	CTreeFolder(CFolderBase* parentFolder, const Type type, const QString& language );
	virtual ~CTreeFolder();

	virtual void addGroup(const Type type, const QString language);
	virtual void addModule(CSwordModuleInfo* const);
	virtual void addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description);

	virtual void initTree();

	virtual void update();
	virtual void init();

	virtual const QString& language() const;

private:
	QString m_language;
};

class CGlossaryFolder : public CTreeFolder {
public:
	CGlossaryFolder(CMainIndex* mainIndex, const Type type, const QString& fromLanguage, const QString& toLanguage );
	CGlossaryFolder(CFolderBase* parentFolder, const Type type, const QString& fromLanguage, const QString& toLanguage );
	virtual ~CGlossaryFolder();

	virtual void initTree();
	virtual void init();
	virtual void addGroup(const Type /*type*/, const QString& /*fromLanguage*/) {}
	; //standard reimpl to overload the old one right
	virtual void addGroup(const Type type, const QString& fromLanguage, const QString& toLanguage);
	/**
	* Returns the language this glossary folder maps from.
	*/
	const QString& fromLanguage() const;
	/**
	* Returns the language this glossary folder maps from.
	*/
	const QString& toLanguage() const;

private:
	QString m_fromLanguage;
	QString m_toLanguage;
};

class CBookmarkFolder : public CTreeFolder {
public:
	CBookmarkFolder(CMainIndex* mainIndex, const Type type = BookmarkFolder);
	CBookmarkFolder(CFolderBase* parentItem, const Type type = BookmarkFolder);
	virtual ~CBookmarkFolder();
	virtual const bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();
	virtual bool acceptDrop(const QMimeSource * src) const;
	virtual void dropped(QDropEvent *e, QListViewItem* after);

	/**
	* Loads bookmarks from XML content
	*/
	const bool loadBookmarksFromXML( const QString& xml );
	/**
	* Loads bookmarks from a file.
	*/
	const bool loadBookmarks( const QString& );
	/**
	* Saves the bookmarks in a file.
	*/
	const bool saveBookmarks( const QString& filename, const bool& forceOverwrite = true );

protected: // Protected methods
	virtual void initTree();
};


namespace Bookmarks {
class OldBookmarksFolder : public CBookmarkFolder {
public:
		OldBookmarksFolder(CTreeFolder* item);
		virtual ~OldBookmarksFolder();
		virtual void initTree();
		/**
		* Returns the XML code which represents the content of this folder.
		*/
		virtual QDomElement saveToXML( QDomDocument& document );
		/**
		* Loads the content of this folder from the XML code passed as argument to this function.
		*/
		virtual void loadFromXML( QDomElement& element );
	};

	class OldBookmarkImport {
public:
		/**
		* This function converts the old config based bookmarks into a valid 1.3 XML file, so importing is easy
		*/
		static const QString oldBookmarksXML( const QString& configFileName = QString::null );
private:
		// made provate because we offer one static functions which doesn't need constructor and destructor
		OldBookmarkImport();
		virtual ~OldBookmarkImport();
	};

class SubFolder : public CBookmarkFolder {
public:
		SubFolder(CFolderBase* parentItem, const QString& caption);
		SubFolder(CFolderBase* parentItem, QDomElement& xml);
		virtual ~SubFolder();
		virtual void init();
		/**
		* Reimplementation from  CItemBase.
		*/
		const bool enableAction(const MenuAction action);
		/**
		* Returns the XML code which represents the content of this folder.
		*/
		virtual QDomElement saveToXML( QDomDocument& document );
		/**
		* Loads the content of this folder from the XML code passed as argument to this function.
		*/
		virtual void loadFromXML( QDomElement& element );

private:
		QDomElement m_startupXML;
	};
} //end of namespace Bookmarks

#endif
