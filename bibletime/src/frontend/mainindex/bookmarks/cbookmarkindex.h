/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKMARKINDEX_H
#define CBOOKMARKINDEX_H

//BibleTime includes
#include "cindexitembase.h"

class CSwordModuleInfo;
#include "frontend/displaywindow/cdisplaywindow.h"

#include "util/cpointers.h"

//Qt includes
#include <QTimer>
#include <QToolTip>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class CSearchDialog;
class CMainIndex;
class QWidget;
class QDropEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class BTMimeData;
class QMenu;
class QAction;

class QMouseEvent;

/**
* The widget which manages all bookmarks.
* @author The BibleTime team
*/
class CBookmarkIndex : public QTreeWidget {
	Q_OBJECT

public:
	CBookmarkIndex(QWidget *parent);
	virtual ~CBookmarkIndex();

	void initTree();

	/**
	* Saves the bookmarks to disk
	*/
	void saveBookmarks();

signals:
	/**
	* Is emitted when a module should be opened,
	*/
	void createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& );

protected: // Protected methods

	// A hack to get the modifiers
	virtual void mouseReleaseEvent(QMouseEvent* event);

	/**
	* Reimplementation. Adds the given group to the tree.
	*/
	virtual void addGroup( const CIndexItemBase::Type type, const QString language);
	/**
	* Initialize the SIGNAL<->SLOT connections
	*/
	void initConnections();
	/**
	* Reimplementation. Returns the drag object for the current selection.
	*/
	virtual QMimeData* dragObject();
	/**
	* Reimplementation from QTreeWidget. Returns true if the drag is acceptable for the listview.
	*/
	virtual void dragEnterEvent( QDragEnterEvent* event );
	virtual void dragMoveEvent( QDragMoveEvent* event );
	virtual void dropEvent( QDropEvent* event );
	/**
	* Returns the correct action object for the given type of action.
	*/
	QAction* action( const CIndexItemBase::MenuAction type ) const;
	/**
	* Reimplementation from QAbstractItemView. Takes care of movable items.
	*/
	//virtual void startDrag(Qt::DropActions supportedActions);


	/**
	* Handle mouse moving (mag updates, d'n'd?)
	*/
	virtual void mouseMoveEvent(QMouseEvent* event);


protected slots: // Protected slots
	/**
	* Is called when an item was clicked or activated.
	*/
	void slotExecuted( QTreeWidgetItem* );
	void dropped( QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*);
	/**
	* Shows the context menu at the given position.
	*/
	void contextMenu(const QPoint&);
	/**
	* Adds a new subfolder to the current item.
	*/
	void createNewFolder();
	/**
	* Opens a dialog to change the current folder.
	*/
	void changeFolder();
	/**
	* Exports the bookmarks being in the selected folder.
	*/
	void exportBookmarks();
	/**
	* Changes the current bookmark.
	*/
	void changeBookmark();
	void slotItemChanged(QTreeWidgetItem*, int);
	/**
	* Import bookmarks from a file and add them to the selected folder.
	*/
	void importBookmarks();
	/**
	* Deletes the selected entries.
	*/
	void deleteEntries();
	/**
	* Prints the selected bookmarks.
	*/
	void printBookmarks();

	/**
	* Slot for the folder auto-open timer (when dragging into a folder).
	*/
	void autoOpenTimeout();

	/**
	* Slot for the mag update timer.
	*/
	void magTimeout();

	/**
	* Is called when items should be moved.
	*/
	void moved( QList<QTreeWidgetItem>& items, QList<QTreeWidgetItem>& afterFirst, QList<QTreeWidgetItem>& afterNow);


private:
	bool m_itemsMovable;
	QTreeWidgetItem* m_autoOpenFolder;
	QTimer m_autoOpenTimer;
	QTimer m_magTimer;
	int m_mouseReleaseEventModifiers;
	QTreeWidgetItem* m_previousEventItem;

	/**
	* Initializes the view.
	*/
	void initView();
	/** Convenience function for creating a new action. */
	QAction* newQAction(const QString& text, const QString& pix, int shortcut, const QObject* receiver, const char* slot, QObject* parent);

	/**
	* Returns true if more than one netry is supported by this action type.
	* Returns false for actions which support only one entry.
	*/
	bool isMultiAction( const CIndexItemBase::MenuAction type ) const;

	/**
	* A helper function for d'n'd which creates a new bookmark item when drop happens.
	*/
	void createBookmarkFromDrop(QDropEvent* event, CIndexItemBase* droppedIntoItem);

	struct Actions {
		QAction* newFolder;
		QAction* changeFolder;

		QAction* changeBookmark;
		QAction* importBookmarks;
		QAction* exportBookmarks;
		QAction* printBookmarks;

		QAction* deleteEntries;
	}
	m_actions;
	QMenu* m_popup;

};

#endif
