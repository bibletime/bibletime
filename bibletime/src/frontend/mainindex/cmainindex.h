/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMAININDEX_H
#define CMAININDEX_H

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

/** The class which manages all bookmarks and modules. The modules are put into own, fixed subfolders sorted by language.
  * @author The BibleTime team
  */
class CMainIndex : public QTreeWidget {
	Q_OBJECT

	
	// Must be removed, qtooltip cannot be subclassed (it's not even a class really!)
	// Maybe items should just have their own tooltips (do they)?
	// Tooltip event can be handled and data shown in tooltip or in mag view.
// 	class ToolTip : public QToolTip {
// 	public:
// 		ToolTip(CMainIndex* parent);
// 		virtual ~ToolTip() {}
// 		/**
// 		* Displays a tooltip for position p using the getToolTip() function of CGroupManagerItem
// 		*/
// 		virtual void maybeTip( const QPoint &pos);
// 
// 	private:
// 		CMainIndex* m_mainIndex;
// 	};

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
	QRect drawItemHighlighter(QPainter* painter, QTreeWidgetItem * item );
	/** Read settings like open groups or scrollbar position and restore them
	*/
	void readSettings();
	/** Save settings like roups close/open status to the settings file.
	*/
	void saveSettings();
	/** Reimplementation.
	 */
	//virtual void ensurePolished();

	/** Catch the QEvent::Polish to call ensurePolished. */ 
	virtual bool event(QEvent* event);

protected slots: // Protected slots
	/**
	* Is called when an item was clicked/double clicked.
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
	void moved( QList<QTreeWidgetItem>& items, QList<QTreeWidgetItem>& afterFirst, QList<QTreeWidgetItem>& afterNow);
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
	//ToolTip* m_toolTip;
	bool m_itemsMovable;
	QTreeWidgetItem* m_autoOpenFolder;
	QTimer m_autoOpenTimer;

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

		KActionMenu* editModuleMenu;
		KAction* editModulePlain;
		KAction* editModuleHTML;

		KAction* searchInModules;
		KAction* unlockModule;
		KAction* aboutModule;
	}
	m_actions;
	KMenu* m_popup;

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
