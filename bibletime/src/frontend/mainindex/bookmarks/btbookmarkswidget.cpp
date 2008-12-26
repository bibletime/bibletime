/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "btbookmarkswidget.h"
#include "btbookmarkswidget.moc"

#include "newbtbookmarkitem.h"


#include "backend/managers/creferencemanager.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "backend/config/cbtconfig.h"
#include "frontend/cinfodisplay.h"

#include "frontend/cprinter.h"
#include "frontend/cdragdrop.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <boost/scoped_ptr.hpp>

//Qt includes
#include <QInputDialog>
#include <QRegExp>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QTimer>
#include <QToolTip>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCursor>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>
#include <QAction>



BtBookmarksWidget::BtBookmarksWidget(QWidget *parent)
	: QTreeWidget(parent),
	m_itemsMovable(false),
	m_autoOpenFolder(0),
	m_autoOpenTimer(this),
	m_magTimer(this),
	m_previousEventItem(0)
{
	setMouseTracking(true);
	m_magTimer.setSingleShot(true);
	m_magTimer.setInterval(CBTConfig::get(CBTConfig::magDelay));
	setContextMenuPolicy(Qt::CustomContextMenu);
	initView();
	initConnections();
	initTree();
}

BtBookmarksWidget::~BtBookmarksWidget() {
	saveBookmarks();
}

// /** Reimplementation. Adds the given group to the tree. */
// void BtBookmarksWidget::addGroup(const BtBookmarkItem::Type type, const QString /*language*/) {
// 	qDebug("BtBookmarksWidget::addGroup");
// 	CIndexTreeFolder *i = 0;
// 	switch (type) {
// 		case BtBookmarkItem::BookmarkFolder:
// 			qDebug("case BtBookmarkItem::BookmarkFolder");
// 			i = new CIndexBookmarkFolder(this);
// 			break;
// 		default: break;
// 	}
// 	
// 	if (i) {
// 		i->init();
// 	}
// }


/** Initializes the view. */
void BtBookmarksWidget::initView()
{	
	qDebug("BtBookmarksWidget::initView");

	header()->hide();

	setFocusPolicy(Qt::WheelFocus);
	setDragEnabled( true );
	setAcceptDrops( true );
	setDropIndicatorShown( true );
	setDragDropMode(QAbstractItemView::InternalMove);

	setItemsExpandable(true);
	viewport()->setAcceptDrops(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new QMenu(viewport());
	m_popup->setTitle(tr("Bookmarks"));

	m_actions.newFolder = newQAction(tr("New folder"), CResMgr::mainIndex::newFolder::icon, 0, this, SLOT(createNewFolder()), this);
	m_actions.changeFolder = newQAction(tr("Rename folder"),CResMgr::mainIndex::changeFolder::icon, 0, this, SLOT(changeFolder()), this);

	m_actions.changeBookmark = newQAction(tr("Change bookmark description..."),CResMgr::mainIndex::changeBookmark::icon, 0, this, SLOT(changeBookmark()), this);
	m_actions.importBookmarks = newQAction(tr("Import bookmarks..."),CResMgr::mainIndex::importBookmarks::icon, 0, this, SLOT(importBookmarks()), this);
	m_actions.exportBookmarks = newQAction(tr("Export bookmarks..."),CResMgr::mainIndex::exportBookmarks::icon, 0, this, SLOT(exportBookmarks()), this);
	m_actions.printBookmarks = newQAction(tr("Print bookmarks..."),CResMgr::mainIndex::printBookmarks::icon, 0, this, SLOT(printBookmarks()), this);

	m_actions.deleteEntries = newQAction(tr("Remove selected items..."),CResMgr::mainIndex::deleteItems::icon, 0, this, SLOT(deleteEntries()), this);


	//fill the popup menu itself
	m_popup->addAction(m_actions.newFolder);
	m_popup->addAction(m_actions.changeFolder);
	QAction* separator = new QAction(this);
	separator->setSeparator(true);
	m_popup->addAction(separator);
	m_popup->addAction(m_actions.changeBookmark);
	m_popup->addAction(m_actions.importBookmarks);
	m_popup->addAction(m_actions.exportBookmarks);
	m_popup->addAction(m_actions.printBookmarks);
	separator = new QAction(this);
	separator->setSeparator(true);
	m_popup->addAction(separator);
	m_popup->addAction(m_actions.deleteEntries);

	qDebug("BtBookmarksWidget::initView end");
}

/** Convenience function for creating a new QAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all QAction constructors.
*/
QAction* BtBookmarksWidget::newQAction(const QString& text, const QString& pix, const int /*shortcut*/, const QObject* receiver, const char* slot, QObject* parent)
{
	QAction* action = new QAction(util::filesystem::DirectoryUtil::getIcon(pix), text, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void BtBookmarksWidget::initConnections()
{
	qDebug("BtBookmarksWidget::initConnections");
	
	//Strangely itemActivated only didn't let open a tree by clicking even though the relevant
	//code in slotExecuted was executed. Therefore itemClicked is necessary.
	QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));
	QObject::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(droppedItem(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(droppedItem(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)));
 	
	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenu(const QPoint&)));
	QObject::connect(&m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpenTimeout()));
	QObject::connect(&m_magTimer, SIGNAL(timeout()), this, SLOT(magTimeout()));	

	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
	qDebug("BtBookmarksWidget::initConnections");

}


/**
* Hack to get single click and selection working. See slotExecuted.
*/
void BtBookmarksWidget::mouseReleaseEvent(QMouseEvent* event) {
	//qDebug("BtBookmarksWidget::mouseReleaseEvent");
	//qDebug() << event->type() << event->modifiers();
	m_mouseReleaseEventModifiers = event->modifiers();
	QTreeWidget::mouseReleaseEvent(event);
}

/** Called when an item is clicked with mouse or activated with keyboard. */
void BtBookmarksWidget::slotExecuted( QTreeWidgetItem* i )
{
	qDebug("BtBookmarksWidget::slotExecuted");

	//HACK: checking the modifier keys from the last mouseReleaseEvent
	//depends on executing order: mouseReleaseEvent first, then itemClicked signal
	int modifiers = m_mouseReleaseEventModifiers;
	m_mouseReleaseEventModifiers = Qt::NoModifier;
	if (modifiers != Qt::NoModifier) {
		return;
	}

	if (NewBtBookmarkFolder* folder = dynamic_cast<NewBtBookmarkFolder*>(i)) {
		//qDebug() << "folder was activated";
		//qDebug() << "item was:" << i->isExpanded() << "and will be:" << !i->isExpanded();
		folder->setExpanded( !folder->isExpanded() );
	}
	else if (NewBtBookmarkProper* b = dynamic_cast<NewBtBookmarkProper*>(i) ) { //clicked on a bookmark
		if (CSwordModuleInfo* mod = b->module()) {
			QList<CSwordModuleInfo*> modules;
			modules.append(mod);
			emit createReadDisplayWindow(modules, b->key());
		}
	}
	else {
		qDebug("item wasn't bookmark or folder - impossible!?");
	}
}

/** Reimplementation. Returns the drag object for the current selection. */
QMimeData* BtBookmarksWidget::dragObject()
{
	qDebug("BtBookmarksWidget::dragObject");

	if (!m_itemsMovable) {
		return false;
	}

	BTMimeData::ItemList dndItems;
	
	BTMimeData* mimeData = new BTMimeData;
	
	foreach( QTreeWidgetItem* widgetItem, selectedItems() ) {
		if (!widgetItem)
			break;
		if (NewBtBookmarkItem* i = dynamic_cast<NewBtBookmarkItem*>(widgetItem)) {
			if (NewBtBookmarkProper* bookmark = dynamic_cast<NewBtBookmarkProper*>( widgetItem )) {
				//take care of bookmarks which have no valid module any more, e.g. if it was uninstalled
				const QString moduleName = bookmark->module() ? bookmark->module()->name() : QString::null;
				mimeData->appendBookmark(moduleName, bookmark->key(), bookmark->description());
			}

		}
	}
	return mimeData;

}

void BtBookmarksWidget::dragEnterEvent( QDragEnterEvent* event )
{
	qDebug("BtBookmarksWidget::dragEnterEvent");
	event->acceptProposedAction();
}


void BtBookmarksWidget::dragMoveEvent( QDragMoveEvent* event )
{
	qDebug("BtBookmarksWidget::dragMoveEvent");
	const QPoint pos = event->pos();
	if (itemAt(pos)) {
		NewBtBookmarkItem* i = dynamic_cast<NewBtBookmarkItem*>(itemAt(pos));
		if (i) {
			i->acceptDrop(event);
		}
	} else {
		if (event->provides("BibleTime/Bookmark")) {
			event->acceptProposedAction();
		}
	}
}

void BtBookmarksWidget::dropEvent( QDropEvent* event )
{
	qDebug("BtBookmarksWidget::dropEvent");

	if ( event->source() == this ) {
		qDebug("dropping internal drag, not implemented");
		// handle the internal drag, see the Qt source code for QTreeWidget::dropEvent for an example
		//Create a new subtree from the selected items (by copying?)
		//If the parent folder of the item is also selected, put the item under that parent
		//If the parent was not selected, find a grandparent which was selected
		//If no grandparent, put it to top level
		//Or should we move the subitems of a folder automatically?
		// Copy and move could be handled differently?

		// Ask whether to copy or move (with a popup menu)
	} else {
		const QPoint pos = event->pos();
		if (itemAt(pos)) {
			NewBtBookmarkItem* i = dynamic_cast<NewBtBookmarkItem*>(itemAt(pos));
			if (i) {
				if (i->acceptDrop(event)) {
					createBookmarkFromDrop(event, i);
				}
			}
		} else {
			//drop the new bookmark into top level?
		}
	}
}

void BtBookmarksWidget::createBookmarkFromDrop(QDropEvent* event, NewBtBookmarkItem* droppedIntoItem)
{
	qDebug("BtBookmarksWidget::createBookmarkFromDrop");
	//take the bookmark data from the mime source
	const BTMimeData* mdata = dynamic_cast<const BTMimeData*>(event->mimeData());
	if (mdata) {
		//create the new bookmark
		QString moduleName = mdata->bookmark().module();
		QString keyText = mdata->bookmark().key();
		QString description = mdata->bookmark().description();
		CSwordModuleInfo* minfo = CPointers::backend()->findModuleByName(moduleName);
		NewBtBookmarkFolder* parentItem =
		dynamic_cast<NewBtBookmarkFolder*>( droppedIntoItem->isFolder()?droppedIntoItem:droppedIntoItem->parent());
		
		new NewBtBookmarkProper(parentItem, minfo, keyText, description);
	}
}

/** No descriptions */
void BtBookmarksWidget::initTree()
{
	qDebug("BtBookmarksWidget::initTree");



}

/** Use this from dropEvent or move the code somewhere else */
void BtBookmarksWidget::dropped( QDropEvent* /*e*/, QTreeWidgetItem* /*parent*/, QTreeWidgetItem* /*after*/)
{
	qDebug("BtBookmarksWidget::dropped");
}


/** Returns the correct QAction object for the given type of action. */
QAction* const BtBookmarksWidget::action( const BtBookmarkItem::MenuAction type ) const {
	switch (type) {
	case BtBookmarkItem::NewFolder:
		return m_actions.newFolder;
	case BtBookmarkItem::ChangeFolder:
		return m_actions.changeFolder;

	case BtBookmarkItem::ChangeBookmark:
		return m_actions.changeBookmark;
	case BtBookmarkItem::ImportBookmarks:
		return m_actions.importBookmarks;
	case BtBookmarkItem::ExportBookmarks:
		return m_actions.exportBookmarks;
	case BtBookmarkItem::PrintBookmarks:
		return m_actions.printBookmarks;

	case BtBookmarkItem::DeleteEntries:
		return m_actions.deleteEntries;

	default:
		return 0;
	}
}

/** Shows the context menu at the given position. */
void BtBookmarksWidget::contextMenu(const QPoint& p)
{
	qDebug("BtBookmarksWidget::contextMenu");
	//setup menu entries depending on current selection
	QTreeWidgetItem* i = itemAt(p);
	QList<QTreeWidgetItem *> items = selectedItems();
	//The item which was clicked may not be selected
	if (i && !items.contains(i))
		items.append(i);
	
	if (items.count() == 0) {
		//special handling for no selection
		BtBookmarkItem::MenuAction actionType;
		for (int index = BtBookmarkItem::ActionBegin; index <= BtBookmarkItem::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItem::MenuAction>(index);
			if (QAction* a = action(actionType)) {
				switch (index) {
				case BtBookmarkItem::ExportBookmarks:
				case BtBookmarkItem::ImportBookmarks:
				case BtBookmarkItem::NewFolder:
				case BtBookmarkItem::PrintBookmarks:
					a->setEnabled(true);
					break;
				default:
					a->setEnabled(false);
				}
			}
		}
	}
	else if (items.count() == 1) {
		//special handling for one selected item
		
		BtBookmarkItem* item = dynamic_cast<BtBookmarkItem*>(items.at(0));
		BtBookmarkItem::MenuAction actionType;
		for (int index = BtBookmarkItem::ActionBegin; index <= BtBookmarkItem::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItem::MenuAction>(index);
			if (QAction* a = action(actionType))
				a->setEnabled( item->enableAction(actionType) );
		}
	}
	else {
		//first disable all actions
		BtBookmarkItem::MenuAction actionType;
		for (int index = BtBookmarkItem::ActionBegin; index <= BtBookmarkItem::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItem::MenuAction>(index);
			if (QAction* a = action(actionType))
				a->setEnabled(false);
		}
		//enable the menu items depending on the types of the selected items.
		for (int index = BtBookmarkItem::ActionBegin; index <= BtBookmarkItem::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItem::MenuAction>(index);
			bool enableAction = isMultiAction(actionType);
			QListIterator<QTreeWidgetItem *> it(items);
			while(it.hasNext()) {
				BtBookmarkItem* i = dynamic_cast<BtBookmarkItem*>(it.next());
				enableAction = enableAction && i->enableAction(actionType);
			}
			if (enableAction) {
				QAction* a = action(actionType) ;
				if (i && a)
					a->setEnabled(enableAction);
			}
		}
	}
	//finally, open the popup
	m_popup->exec(mapToGlobal(p));
	qDebug("BtBookmarksWidget::contextMenu end");
}

/** Adds a new subfolder to the current item.*/
void BtBookmarksWidget::createNewFolder() {
	//TODO:  Need to remove/change: we won't have a top level folder.
	qDebug("BtBookmarksWidget::createNewFolder");
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->newSubFolder();
	}
}

/** Opens a dialog to change the current folder name. */
void BtBookmarksWidget::renameFolder() {
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	if (i) {
		i->rename();
	}
}

/** Changes the current bookmark. */
void BtBookmarksWidget::changeBookmark() {
	CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->rename();
	}
}

/** Exports the bookmarks being in the selected folder. */
void BtBookmarksWidget::exportBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->exportBookmarks();
	}
}

/** Import bookmarks from a file and add them to the selected folder. */
void BtBookmarksWidget::importBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->importBookmarks();
	}
}

/** Prints the selected bookmarks. */
void BtBookmarksWidget::printBookmarks() {
	Printing::CPrinter::KeyTree tree;
	Printing::CPrinter::KeyTreeItem::Settings settings;
	settings.keyRenderingFace = Printing::CPrinter::KeyTreeItem::Settings::CompleteShort;

	QList<QTreeWidgetItem*> items;
	CIndexBookmarkFolder* bf = dynamic_cast<CIndexBookmarkFolder*>(currentItem());

	if (bf) {
		items = bf->getChildList();
	}
	else {
		items = selectedItems();
	}

	//create a tree of keytreeitems using the bookmark hierarchy.
	QListIterator<QTreeWidgetItem*> it(items);
	while(it.hasNext()) {
		CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(it.next());
		if (i) {
			tree.append( new Printing::CPrinter::KeyTreeItem( i->key(), i->module(), settings ) );
		}
	}

	boost::scoped_ptr<Printing::CPrinter> printer( 
		new Printing::CPrinter( this, CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() ) 
	);
	printer->printKeyTree(tree);
}

/** Deletes the selected entries. */
void BtBookmarksWidget::deleteEntries()
{
	if (!selectedItems().count()) {
		BtBookmarkItem* f = dynamic_cast<BtBookmarkItem*>(currentItem());
		if (f) {
			currentItem()->setSelected(true);
		} else {
			return;
		}
	}

	if (QMessageBox::question(this, tr("Delete Items"), tr("Do you really want to delete the selected items and child-items?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No ) != QMessageBox::Yes) {
		return;
	}

	while (selectedItems().size() > 0) {
		delete selectedItems().at(0); // deleting all does not work because it may cause double deletion
	}

}


void BtBookmarksWidget::autoOpenTimeout() {
	m_autoOpenTimer.stop();
	if (m_autoOpenFolder && !m_autoOpenFolder->isExpanded() && m_autoOpenFolder->childCount()) {
		m_autoOpenFolder->setExpanded(true);
	}
}


/** Returns true if more than one netry is supported by this action type. Returns false for actions which support only one entry, e.g. about module etc. */
const bool BtBookmarksWidget::isMultiAction( const BtBookmarkItem::MenuAction type ) const {
	switch (type) {
	case BtBookmarkItem::NewFolder:
		return false;
	case BtBookmarkItem::ChangeFolder:
		return false;

	case BtBookmarkItem::ChangeBookmark:
		return false;
	case BtBookmarkItem::ImportBookmarks:
		return false;
	case BtBookmarkItem::ExportBookmarks:
		return false;
	case BtBookmarkItem::PrintBookmarks:
		return true;

	case BtBookmarkItem::DeleteEntries:
		return true;
	}
	
	return false;
}

/** Is called when items should be moved. */
void BtBookmarksWidget::moved( QList<QTreeWidgetItem>& /*items*/, QList<QTreeWidgetItem>& /*afterFirst*/, QList<QTreeWidgetItem>& /*afterNow*/) {
	qDebug("move items");
}


/** Saves the bookmarks to disk */
void BtBookmarksWidget::saveBookmarks() {
	qDebug("BtBookmarksWidget::saveBookmarks");
	//find the bookmark folder
	BtBookmarkItem* i = 0;

	QTreeWidgetItemIterator it( this );
	while ( *it ) {
		i = dynamic_cast<BtBookmarkItem*>( *it );

		if (i && (i->type() == BtBookmarkItem::BookmarkFolder)) { 
			//found the bookmark folder

			const QString path = util::filesystem::DirectoryUtil::getUserBaseDir().absolutePath() + "/";
			if (!path.isEmpty()) {
				//save the bookmarks to the right file
				if (CIndexBookmarkFolder* f = dynamic_cast<CIndexBookmarkFolder*>(i)) {
					f->saveBookmarks( path + "bookmarks.xml" );
				}
			}
			break;
		}

		++it;
	}
	qDebug("BtBookmarksWidget::saveBookmarks end");
}

void BtBookmarksWidget::mouseMoveEvent(QMouseEvent* event)
{
	//qDebug("BtBookmarksWidget::mouseMoveEvent");
	// Restart timer if we have moved to another item and shift was not pressed
	QTreeWidgetItem* itemUnderPointer = itemAt(event->pos());
	if (itemUnderPointer && (itemUnderPointer != m_previousEventItem) ) {
		//qDebug("BtBookmarksWidget::mouseMoveEvent, moved onto another item");
		if ( !(event->modifiers() & Qt::ShiftModifier)) {
			m_magTimer.start(); // see the ctor for the timer properties
		}
	}
	m_previousEventItem = itemUnderPointer;
}

void BtBookmarksWidget::magTimeout()
{
	qDebug("BtBookmarksWidget::magTimeout");

	QTreeWidgetItem* itemUnderPointer = 0;
	if (underMouse()) {
		itemUnderPointer = itemAt(mapFromGlobal(QCursor::pos()));
	}
	// if the mouse pointer have been over the same item since the timer was started
	if (itemUnderPointer && (m_previousEventItem == itemUnderPointer)) {
		CIndexBookmarkItem* bitem = dynamic_cast<CIndexBookmarkItem*>(itemUnderPointer);
		if (bitem) {
			//qDebug("BtBookmarksWidget::timerEvent: update the infodisplay");
			// Update the mag
			if (bitem->module()) {
				(CPointers::infoDisplay())->setInfo(
					InfoDisplay::CInfoDisplay::CrossReference,
					bitem->module()->name() + ":" + bitem->key()
				);
			} else {
				(CPointers::infoDisplay())->setInfo(InfoDisplay::CInfoDisplay::Text, tr("The work to which the bookmark points to is not installed."));
			}

		}
	}
}
