/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKMARKINDEX_H
#define CBOOKMARKINDEX_H

//BibleTime includes
#include "cindexitembase.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/displaywindow/cdisplaywindow.h"

#include "util/cpointers.h"

//Qt includes
#include <QTimer>
#include <QToolTip>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

//KDE includes
#include <kaction.h>

class CSearchDialog;
class CMainIndex;
class QWidget;
class QDropEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class BTMimeData;
class KActionMenu;
class KMenu;

class QMouseEvent; //testing

/** The class which manages all bookmarks and modules. The modules are put into own, fixed subfolders sorted by language.
  * @author The BibleTime team
  */
class CMainIndex : public QTreeWidget {
	Q_OBJECT

public:
	CMainIndex(QWidget *parent);
	virtual ~CMainIndex();
	
	void initTree();
	
	/**
	* Saves the bookmarks to disk
	*/
	void saveBookmarks();

signals:
	/**
	* Is emitted when a module should be opened,
	*/
	void createReadDisplayWindow( ListCSwordModuleInfo, const QString& );

protected: // Protected methods

	//testing
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
	virtual void dragEnterEvent( QDragEnterEvent* event ) const;
	virtual void dragMoveEvent( QDragMoveEvent* event ) const;
	virtual void dropEvent( QDropEvent* event ) const;
	/**
	* Returns the correct KAction object for the given type of action.
	*/
	KAction* const action( const CIndexItemBase::MenuAction type ) const;
	/**
	* Reimplementation from QAbstractItemView. Takes care of movable items.
	*/
	virtual void startDrag(Qt::DropActions supportedActions);
	/**
	* TODO: qt4 Reimplementation to support the items dragEnter and dragLeave functions.
	*/
	virtual void contentsDragMoveEvent( QDragMoveEvent* event );
	/**
	 * Reimplementation.
	 */
	virtual void contentsDragLeaveEvent( QDragLeaveEvent* e );


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

	void autoOpenTimeout();
	/**
	* Is called when items should be moved.
	*/
	void moved( QList<QTreeWidgetItem>& items, QList<QTreeWidgetItem>& afterFirst, QList<QTreeWidgetItem>& afterNow);

private:
	bool m_itemsMovable;
	QTreeWidgetItem* m_autoOpenFolder;
	QTimer m_autoOpenTimer;

	int m_mouseReleaseEventModifiers;

	/**
	* Initializes the view.
	*/
	void initView();
	/** Convenience function for creating a new KAction. */
	KAction* newKAction(const QString& text, const QString& pix, int shortcut, const QObject* receiver, const char* slot, QObject* parent);

	/**
	* Returns true if more than one netry is supported by this action type.
	* Returns false for actions which support only one entry, e.g. about module etc.
	*/
	const bool isMultiAction( const CIndexItemBase::MenuAction type ) const;

	struct Actions {
		KAction* newFolder;
		KAction* changeFolder;

		KAction* changeBookmark;
		KAction* importBookmarks;
		KAction* exportBookmarks;
		KAction* printBookmarks;

		KAction* deleteEntries;
	}
	m_actions;
	KMenu* m_popup;

};

#endif
