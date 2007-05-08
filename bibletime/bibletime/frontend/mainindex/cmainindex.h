/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMAININDEX_H
#define CMAININDEX_H

//BibleTime includes
#include "cindexitem.h"

#include "backend/cswordmoduleinfo.h"
#include "frontend/displaywindow/cdisplaywindow.h"

#include "util/cpointers.h"

//Qt includes
#include <qwidget.h>
#include <qtimer.h>
#include <qtooltip.h>

//KDE includes
#include <kaction.h>
#include <klistview.h>

class CSearchDialog;

/** The class which manages all bookmarks and modules. The modules are put into own, fixed subfolders sorted by language.
  * @author The BibleTime team
  */
class CMainIndex : public KListView {
	Q_OBJECT

	class ToolTip : public QToolTip {
	public:
		ToolTip(CMainIndex* parent);
		virtual ~ToolTip() {}
		/**
		* Displays a tooltip for position p using the getToolTip() function of CGroupManagerItem
		*/
		virtual void maybeTip( const QPoint &pos);

	private:
		CMainIndex* m_mainIndex;
	};

public:
	CMainIndex(QWidget *parent);
	virtual ~CMainIndex();
	
	void initTree();
	/**
	* Opens the searchdialog using the given modules using the given search text.
	*/
	void emitModulesChosen( ListCSwordModuleInfo modules, QString key );
	/**
	* Saves the bookmarks to disk
	*/
	void saveBookmarks();
	/**
	* Reloads the main index's Sword dependend things like modules
	*/
	void reloadSword();

public slots:
	/**
	 * Opens the searchdialog for the selected modules.
	 */
	void searchInModules();

protected: // Protected methods
	/**
	* Reimplementation. Adds the given group to the tree.
	*/
	virtual void addGroup( const CItemBase::Type type, const QString language);
	/**
	* Initialize the SIGNAL<->SLOT connections
	*/
	void initConnections();
	/**
	* Reimplementation. Returns the drag object for the current selection.
	*/
	virtual QDragObject* dragObject();
	/**
	* Reimplementation from KListView. Returns true if the drag is acceptable for the listview.
	*/
	virtual bool acceptDrag( QDropEvent* event ) const;
	/**
	* Returns the correct KAction object for the given type of action.
	*/
	KAction* const action( const CItemBase::MenuAction type ) const;
	/**
	* Reimplementation. Takes care of movable items.
	*/
	virtual void startDrag();
	/**
	* Reimplementation to support the items dragEnter and dragLeave functions.
	*/
	virtual void contentsDragMoveEvent( QDragMoveEvent* event );
	/**
	 * Reimplementation.
	 */
	virtual void contentsDragLeaveEvent( QDragLeaveEvent* e );
	QRect drawItemHighlighter(QPainter* painter, QListViewItem * item );
	/** Read settings like open groups or scrollbar position and restore them
	*/
	void readSettings();
	/** Save settings like roups close/open status to the settings file.
	*/
	void saveSettings();
	/** Reimplementation.
	 */
	virtual void polish();

protected slots: // Protected slots
	/**
	* Is called when an item was clicked/double clicked.
	*/
	void slotExecuted( QListViewItem* );
	void dropped( QDropEvent*, QListViewItem*, QListViewItem*);
	/**
	* Shows the context menu at the given position.
	*/
	void contextMenu(KListView*, QListViewItem*, const QPoint&);
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
	/**
	* Shows information about the current module.
	*/
	void aboutModule();
	/**
	* Unlocks the current module.
	*/
	void unlockModule();
	void autoOpenTimeout();
	/**
	* Is called when items should be moved.
	*/
	void moved( QPtrList<QListViewItem>& items, QPtrList<QListViewItem>& afterFirst, QPtrList<QListViewItem>& afterNow);
	/**
	* Opens a plain text editor window to edit the modules content.
	*/
	void editModulePlain();
	/**
	* Opens an HTML editor window to edit the modules content.
	*/
	void editModuleHTML();

private:
	CSearchDialog* m_searchDialog;
	ToolTip* m_toolTip;
	bool m_itemsMovable;
	QListViewItem* m_autoOpenFolder;
	QTimer m_autoOpenTimer;

	/**
	* Initializes the view.
	*/
	void initView();
	/**
	* Returns true if more than one netry is supported by this action type.
	* Returns false for actions which support only one entry, e.g. about module etc.
	*/
	const bool isMultiAction( const CItemBase::MenuAction type ) const;

	struct Actions {
		KAction* newFolder;
		KAction* changeFolder;

		KAction* changeBookmark;
		KAction* importBookmarks;
		KAction* exportBookmarks;
		KAction* printBookmarks;

		KAction* deleteEntries;

		KActionMenu* editModuleMenu;
		KAction* editModulePlain;
		KAction* editModuleHTML;

		KAction* searchInModules;
		KAction* unlockModule;
		KAction* aboutModule;
	}
	m_actions;
	KPopupMenu* m_popup;

signals:
	/**
	* Is emitted when a module should be opened,
	*/
	void createReadDisplayWindow( ListCSwordModuleInfo, const QString& );
	/**
	 * Is emitted when a write window should be created.
	 */
	void createWriteDisplayWindow( CSwordModuleInfo*, const QString&, const CDisplayWindow::WriteWindowType& );
	void signalSwordSetupChanged();
};

#endif
