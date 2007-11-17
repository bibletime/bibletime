//
// C++ Interface: cindexitembase
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXITEMBASE_H
#define CINDEXITEMBASE_H

#include <QTreeWidgetItem>
#include <QtXml/qdom.h>

class CBookmarkIndex;
class QDropEvent;
class QDragMoveEvent;
class QMimeData;

class CIndexItemBase : public QTreeWidgetItem/*, public CPointers */ {
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
		Module,
		ImageModuleFolder
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

	CIndexItemBase(CBookmarkIndex* bookmarkIndex, const Type type = Unknown);
	CIndexItemBase(CIndexItemBase* parentItem, const Type type = Unknown);
	virtual ~CIndexItemBase();

	virtual const QString toolTip();
	virtual CBookmarkIndex* treeWidget() const;

	/**
	* Returns if the implementation of this class is a folder item or not.
	* Reimplement this function to return the correct value.
	*/
	virtual const bool isFolder();

	const Type& type() const;
	virtual void init();
	virtual void update();
	void moveAfter( CIndexItemBase* const item );

	/**
	* Returns true if the given action should be enabled in the popup menu.
	*/
	virtual const bool enableAction( const MenuAction action );

	virtual const bool isMovable();

	/**
	* Returns the XML code which represents the content of this folder.
	*/
	virtual QDomElement saveToXML( QDomDocument& /*document*/ );
	
	/**
	* Loads the content of this folder from the XML code passed as argument to this function.
	*/
	virtual void loadFromXML( QDomElement& /*element*/ );
	
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
	virtual void sortChildItems( int col, Qt::SortOrder order );
	/**
	* Reimplementation which takes care of the our isSortingEnabled() setting.
	*/
	virtual void sort();

	virtual QList<QTreeWidgetItem*> getChildList();

	virtual bool acceptDrop(QDropEvent* event) const;
	
protected:
	friend class CMainIndex;

	/** Reimplementation which uses our extended version of dropped (see below).
	*/
	virtual void dropped( QDropEvent* e);

	/** Our extended version of the dropped method to include a item above the point we dropped the stuff.
	*/
	virtual void dropped( QDropEvent* e, QTreeWidgetItem* after);
	
	/**
	* Reimplementation. Returns true if the auto opening of this folder is allowd
	* The default return value is "false"
	*/
	virtual const bool allowAutoOpen( const QMimeData* data ) const;

private:
	Type m_type;
	bool m_sortingEnabled;
};

#endif
