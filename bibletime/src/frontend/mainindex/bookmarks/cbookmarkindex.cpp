/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "cbookmarkindex.h"
#include "cbookmarkindex.moc"

#include "cindexitembase.h"
#include "cindexbookmarkitem.h"
#include "cindexbookmarkfolder.h"
#include "cindexsubfolder.h"

#include "backend/managers/creferencemanager.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/cbtconfig.h"
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

#include <QDebug>

//KDE includes
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kactionmenu.h>

CBookmarkIndex::CBookmarkIndex(QWidget *parent)
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

CBookmarkIndex::~CBookmarkIndex() {
	saveBookmarks();
}

/** Reimplementation. Adds the given group to the tree. */
void CBookmarkIndex::addGroup(const CIndexItemBase::Type type, const QString language) {
	qDebug("CBookmarkIndex::addGroup");
	CIndexTreeFolder *i = 0;
	switch (type) {
		case CIndexItemBase::BookmarkFolder:
			qDebug("case CIndexItemBase::BookmarkFolder");
			i = new CIndexBookmarkFolder(this);
			break;
	}
	
	if (i) {
		i->init();
	}
}


/** Initializes the view. */
void CBookmarkIndex::initView()
{	
	qDebug("CBookmarkIndex::initView");

	header()->hide();

	setFocusPolicy(Qt::WheelFocus);
	setDragEnabled( true );
	setAcceptDrops( true );
	setDropIndicatorShown( true );
	setDragDropMode(QAbstractItemView::DragDrop);

	setItemsExpandable(true);
	viewport()->setAcceptDrops(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new KMenu(viewport());
	m_popup->addTitle(i18n("Bookmarks"));

	m_actions.newFolder = newKAction(i18n("New folder"), CResMgr::mainIndex::newFolder::icon, 0, this, SLOT(createNewFolder()), this);
	m_actions.changeFolder = newKAction(i18n("Rename folder"),CResMgr::mainIndex::changeFolder::icon, 0, this, SLOT(changeFolder()), this);

	m_actions.changeBookmark = newKAction(i18n("Change bookmark description"),CResMgr::mainIndex::changeBookmark::icon, 0, this, SLOT(changeBookmark()), this);
	m_actions.importBookmarks = newKAction(i18n("Import bookmarks"),CResMgr::mainIndex::importBookmarks::icon, 0, this, SLOT(importBookmarks()), this);
	m_actions.exportBookmarks = newKAction(i18n("Export bookmarks"),CResMgr::mainIndex::exportBookmarks::icon, 0, this, SLOT(exportBookmarks()), this);
	m_actions.printBookmarks = newKAction(i18n("Print bookmarks"),CResMgr::mainIndex::printBookmarks::icon, 0, this, SLOT(printBookmarks()), this);

	m_actions.deleteEntries = newKAction(i18n("Remove selected item(s)"),CResMgr::mainIndex::deleteItems::icon, 0, this, SLOT(deleteEntries()), this);


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

	qDebug("CBookmarkIndex::initView end");
}

/** Convenience function for creating a new KAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all KAction constructors.
*/
KAction* CBookmarkIndex::newKAction(const QString& text, const QString& pix, const int shortcut, const QObject* receiver, const char* slot, QObject* parent)
{
	KAction* action = new KAction(KIcon(pix), text, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void CBookmarkIndex::initConnections()
{
	qDebug("CBookmarkIndex::initConnections");
	
	//Strangely itemActivated only didn't let open a tree by clicking even though the relevant
	//code in slotExecuted was executed. Therefore itemClicked is necessary.
	QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));
	QObject::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)));
 	
	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenu(const QPoint&)));
	QObject::connect(&m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpenTimeout()));
	QObject::connect(&m_magTimer, SIGNAL(timeout()), this, SLOT(magTimeout()));	

	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
	qDebug("CBookmarkIndex::initConnections");

}


/**
* Hack to get single click and selection working. See slotExecuted.
*/
void CBookmarkIndex::mouseReleaseEvent(QMouseEvent* event) {
	//qDebug("CBookmarkIndex::mouseReleaseEvent");
	//qDebug() << event->type() << event->modifiers();
	m_mouseReleaseEventModifiers = event->modifiers();
	QTreeWidget::mouseReleaseEvent(event);
}

/** Called when an item is clicked with mouse or activated with keyboard. */
void CBookmarkIndex::slotExecuted( QTreeWidgetItem* i )
{
	qDebug("CBookmarkIndex::slotExecuted");

	//HACK: checking the modifier keys from the last mouseReleaseEvent
	//depends on executing order: mouseReleaseEvent first, then itemClicked signal
	int modifiers = m_mouseReleaseEventModifiers;
	m_mouseReleaseEventModifiers = Qt::NoModifier;
	if (modifiers != Qt::NoModifier) {
		return;
	}

	qDebug(QString().setNum((int)i).toLatin1().data());
	CIndexItemBase* ci = dynamic_cast<CIndexItemBase*>(i);
	if (!ci) {
		qDebug("if (!ci)");
		return;
	}

	if (ci->isFolder()) {
		//qDebug() << "folder was activated";
		//qDebug() << "item was:" << i->isExpanded() << "and will be:" << !i->isExpanded();
		i->setExpanded( !i->isExpanded() );
	}
	else if (CIndexBookmarkItem* b = dynamic_cast<CIndexBookmarkItem*>(i) ) { //clicked on a bookmark
		if (CSwordModuleInfo* mod = b->module()) {
			ListCSwordModuleInfo modules;
			modules.append(mod);
			emit createReadDisplayWindow(modules, b->key());
		}
	}
}

/** Reimplementation. Returns the drag object for the current selection. */
QMimeData* CBookmarkIndex::dragObject()
{
	if (!m_itemsMovable) {
		return false;
	}

	BTMimeData::ItemList dndItems;

// 	QList<QTreeWidgetItem> items = selectedItems();
// 	for (items.first(); items.current(); items.next()) {
// 		if (CItemBase* i = dynamic_cast<CItemBase*>(items.current())) {
// 			//we can move this item!
// 			if (!i->isMovable()) { //we can only drag items which allow us to do it, e.g. which are movable
// 				continue;
// 			}
// 
// 			if (CBookmarkItem* bookmark = dynamic_cast<CBookmarkItem*>( items.current() )) {
// 				//take care of bookmarks which have no valid module any more, e.g. if it was uninstalled
// 				const QString moduleName = bookmark->module() ? bookmark->module()->name() : QString::null;
// 				dndItems.append( CDragDropMgr::Item(moduleName, bookmark->key(), bookmark->description()) );
// 			}
// 		}
// 	}
// 
// 	return CDragDropMgr::dragObject( dndItems, viewport() );
	
	BTMimeData* mimeData = new BTMimeData;
	
	foreach( QTreeWidgetItem* widgetItem, selectedItems() ) {
		if (!widgetItem)
			break;
		if (CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(widgetItem)) {
			//we can only drag items which allow us to do it, e.g. which are movable
			if (!i->isMovable()) { 
				continue;
			}	

			if (CIndexBookmarkItem* bookmark = dynamic_cast<CIndexBookmarkItem*>( widgetItem )) {
				//take care of bookmarks which have no valid module any more, e.g. if it was uninstalled
				const QString moduleName = bookmark->module() ? bookmark->module()->name() : QString::null;
				mimeData->appendBookmark(moduleName, bookmark->key(), bookmark->description());
			}

		}
	}
	return mimeData;

}

void CBookmarkIndex::dragEnterEvent( QDragEnterEvent* event )
{
	qDebug("CBookmarkIndex::dragEnterEvent");
	event->acceptProposedAction();
}


void CBookmarkIndex::dragMoveEvent( QDragMoveEvent* event )
{
	qDebug("CBookmarkIndex::dragMoveEvent");
	const QPoint pos = event->pos();
	if (itemAt(pos)) {
		CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(itemAt(pos));
		if (i) {
			i->acceptDrop(event);
		}
	} else {
		if (event->provides("BibleTime/Bookmark")) {
			event->acceptProposedAction();
		}
	}
}

void CBookmarkIndex::dropEvent( QDropEvent* event )
{
	qDebug("CBookmarkIndex::dropEvent");

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
			CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(itemAt(pos));
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

void CBookmarkIndex::createBookmarkFromDrop(QDropEvent* event, CIndexItemBase* droppedIntoItem)
{
	qDebug("CBookmarkIndex::createBookmarkFromDrop");
	//take the bookmark data from the mime source
	const BTMimeData* mdata = dynamic_cast<const BTMimeData*>(event->mimeData());
	if (mdata) {
		//create the new bookmark
		QString moduleName = mdata->bookmark().module();
		QString keyText = mdata->bookmark().key();
		QString description = mdata->bookmark().description();
		CSwordModuleInfo* minfo = CPointers::backend()->findModuleByName(moduleName);
		CIndexFolderBase* parentItem =
		dynamic_cast<CIndexFolderBase*>( droppedIntoItem->isFolder()?droppedIntoItem:droppedIntoItem->parent());
		
		CIndexBookmarkItem* newItem = new CIndexBookmarkItem(parentItem, minfo, keyText, description);
	}			
}

/** No descriptions */
void CBookmarkIndex::initTree() {
	qDebug("CBookmarkIndex::initTree");
	addGroup(CIndexItemBase::BookmarkFolder, QString("*"));
}

/** Use this from dropEvent or move the code somewhere else */
void CBookmarkIndex::dropped( QDropEvent* e, QTreeWidgetItem* parent, QTreeWidgetItem* after) {
// 	Q_ASSERT(after);
// 	Q_ASSERT(parent);
// 
// 	//the drop was started in this main index widget
// 	if (m_itemsMovable && ((e->source()) == (viewport()))) {
// 		/*
// 		* If the drag was started from the main index and should move items and if the destination is the bookmark
// 		* folder or one of its subfolders
// 		* we remove the current items because the new ones will be inserted soon.
// 		*/
// 		if (dynamic_cast<CIndexBookmarkFolder*>(parent) || dynamic_cast<CIndexSubFolder*>(parent)) { 
// 			// we drop onto the bookmark folder or one of it's subfolders
// 			//       QPtrList<QListViewItem> items = selectedItems();
// 			//       items.setAutoDelete(true);
// 			//       items.clear(); //delete the selected items we dragged
// 		}
// 	}
// 
// 	//finally do the drop, either with external drop data or with the moved items' data
// 	CIndexItemBase* const parentItem = dynamic_cast<CIndexItemBase*>(parent);
// 	CIndexItemBase* const afterItem  = dynamic_cast<CIndexItemBase*>(after);
// 
// 	bool removeSelectedItems = true;
// 	bool moveSelectedItems = false;
// 
// 	if (afterItem && afterItem->isFolder()) {
// 		moveSelectedItems = false;
// 		removeSelectedItems = false; //why TRUE?
// 
// 		afterItem->setExpanded(true);
// 		afterItem->dropped(e); //inserts new items, moving only works on the same level
// 	}
// 	else if (afterItem && !afterItem->isFolder() && parentItem) {
// 		const bool justMoveSelected =
// 			(e->source() == viewport())
// 			&& m_itemsMovable
// 			&& parentItem->acceptDrop(e)
// 			&& !afterItem->acceptDrop(e);
// 
// 		if (justMoveSelected) {
// 			moveSelectedItems = true;
// 			removeSelectedItems = false;
// 		}
// 		else {
// 			moveSelectedItems = false;
// 			removeSelectedItems = false;
// 
// 			if (afterItem->acceptDrop(e)) {
// 				afterItem->dropped(e, after);
// 			}
// 			else { //insert in the parent folder and then move the inserted items
// 				parentItem->dropped(e, after);
// 			}
// 		}
// 
// 		parentItem->setExpanded(true);
// 	}
// 	else if (parentItem) { //no after item present, but a parent is there
// 		moveSelectedItems = false;
// 		removeSelectedItems = false;
// 
// 		parentItem->setExpanded(true);
// 		parentItem->dropped(e);
// 	}
// 
// 	if (moveSelectedItems) {
// 		//move all selected items after the afterItem
// 		if (m_itemsMovable) {
// 			QList<QTreeWidgetItem *> items = selectedItems();
// 			QListIterator<QTreeWidgetItem *> it(items);
// 			QTreeWidgetItem* i = it.next();
// 			QTreeWidgetItem* after = afterItem;
// 			while (i && afterItem) {
// 				i->moveItem(after);
// 				after = i;
// 
// 				i = it.next();
// 			}
// 		}
// 	}
// 
// 	if (removeSelectedItems) {
// 		QList<QTreeWidgetItem *> items = selectedItems();
// 		qDeleteAll(items);
// 		items.clear(); //delete the selected items we dragged
// 	}
}



/** Returns the correct KAction object for the given type of action. */
KAction* const CBookmarkIndex::action( const CIndexItemBase::MenuAction type ) const {
	switch (type) {
	case CIndexItemBase::NewFolder:
		return m_actions.newFolder;
	case CIndexItemBase::ChangeFolder:
		return m_actions.changeFolder;

	case CIndexItemBase::ChangeBookmark:
		return m_actions.changeBookmark;
	case CIndexItemBase::ImportBookmarks:
		return m_actions.importBookmarks;
	case CIndexItemBase::ExportBookmarks:
		return m_actions.exportBookmarks;
	case CIndexItemBase::PrintBookmarks:
		return m_actions.printBookmarks;

	case CIndexItemBase::DeleteEntries:
		return m_actions.deleteEntries;

	default:
		return 0;
	}
}

/** Shows the context menu at the given position. */
void CBookmarkIndex::contextMenu(const QPoint& p)
{
	qDebug("CBookmarkIndex::contextMenu");
	//setup menu entries depending on current selection
	QTreeWidgetItem* i = itemAt(p);
	QList<QTreeWidgetItem *> items = selectedItems();
	//The item which was clicked may not be selected
	if (i && !items.contains(i))
		items.append(i);
	qDebug("CBookmarkIndex::contextMenu 1");
	if (items.count() == 0) { 
		qDebug("CBookmarkIndex::contextMenu 2");
		return;
		//special handling for no selection
	}
	else if (items.count() == 1) { 
		qDebug("CBookmarkIndex::contextMenu 4");
		//special handling for one selected item
		
		CIndexItemBase* item = dynamic_cast<CIndexItemBase*>(items.at(0));
		CIndexItemBase::MenuAction actionType;
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			qDebug("CBookmarkIndex::contextMenu 6");
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled( item->enableAction(actionType) );
		}
		qDebug("CBookmarkIndex::contextMenu 7");
	}
	
	else {
		qDebug("CBookmarkIndex::contextMenu 8");
		//first disable all actions
		CIndexItemBase::MenuAction actionType;
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled(false);
		}
		qDebug("CBookmarkIndex::contextMenu 9");
		//enable the menu items depending on the types of the selected items.
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			qDebug("CBookmarkIndex::contextMenu 10");
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			bool enableAction = isMultiAction(actionType);
			QListIterator<QTreeWidgetItem *> it(items);
			while(it.hasNext()) {
				qDebug("CBookmarkIndex::contextMenu 11");
				CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(it.next());
				enableAction = enableAction && i->enableAction(actionType);
			}
			qDebug("CBookmarkIndex::contextMenu 12");
			if (enableAction) {
				KAction* a = action(actionType) ;
				if (i && a)
					a->setEnabled(enableAction);
			}
		}
	}
	qDebug("CBookmarkIndex::contextMenu13");
	//finally, open the popup
	m_popup->exec(mapToGlobal(p));
	qDebug("CBookmarkIndex::contextMenu end");
}

/** Adds a new subfolder to the current item. */
void CBookmarkIndex::createNewFolder() {
	qDebug("CBookmarkIndex::createNewFolder");
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->newSubFolder();
	}
}

/** Opens a dialog to change the current folder. */
void CBookmarkIndex::changeFolder() {
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	if (i) {
		i->rename();
	}
}

/** Changes the current bookmark. */
void CBookmarkIndex::changeBookmark() {
	CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->rename();
	}
}

/** Exports the bookmarks being in the selected folder. */
void CBookmarkIndex::exportBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->exportBookmarks();
	}
}

/** Import bookmarks from a file and add them to the selected folder. */
void CBookmarkIndex::importBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->importBookmarks();
	}
}

/** Prints the selected bookmarks. */
void CBookmarkIndex::printBookmarks() {
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
void CBookmarkIndex::deleteEntries()
{
	if (!selectedItems().count()) {
		CIndexItemBase* f = dynamic_cast<CIndexItemBase*>(currentItem());
		if (f) {
			currentItem()->setSelected(true);
		} else {
			return;
		}
	}

	if (KMessageBox::warningYesNo(this, i18n("Do you really want to delete the selected items and child-items?"), i18n("Delete Items")) != KMessageBox::Yes) {
		return;
	}

	while (selectedItems().size() > 0) {
		delete selectedItems().at(0); // deleting all does not work because it may cause double deletion
	}

}




/** Reimplementation. Takes care of movable items. */
//void CBookmarkIndex::startDrag(Qt::DropActions supportedActions) {
// 	QList<QTreeWidgetItem *> items = selectedItems();
// 	QListIterator<QTreeWidgetItem *> it(items);
// 	m_itemsMovable = true;
// 	
// 	while(it.hasNext() && m_itemsMovable) {
// 		if (CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(it.next())) {
// 			m_itemsMovable = (m_itemsMovable && i->isMovable());
// 		}
// 		else {
// 			m_itemsMovable = false;
// 		}
// 	}
// 
// 	QTreeWidget::startDrag();
//}



void CBookmarkIndex::autoOpenTimeout() {
	m_autoOpenTimer.stop();
	if (m_autoOpenFolder && !m_autoOpenFolder->isExpanded() && m_autoOpenFolder->childCount()) {
		m_autoOpenFolder->setExpanded(true);
	}
}


/** Returns true if more than one netry is supported by this action type. Returns false for actions which support only one entry, e.g. about module etc. */
const bool CBookmarkIndex::isMultiAction( const CIndexItemBase::MenuAction type ) const {
	switch (type) {
	case CIndexItemBase::NewFolder:
		return false;
	case CIndexItemBase::ChangeFolder:
		return false;

	case CIndexItemBase::ChangeBookmark:
		return false;
	case CIndexItemBase::ImportBookmarks:
		return false;
	case CIndexItemBase::ExportBookmarks:
		return false;
	case CIndexItemBase::PrintBookmarks:
		return true;

	case CIndexItemBase::DeleteEntries:
		return true;
	}
	
	return false;
}

/** Is called when items should be moved. */
void CBookmarkIndex::moved( QList<QTreeWidgetItem>& /*items*/, QList<QTreeWidgetItem>& /*afterFirst*/, QList<QTreeWidgetItem>& /*afterNow*/) {
	qDebug("move items");
}

void CBookmarkIndex::slotItemChanged(QTreeWidgetItem* item, int index)
{
	CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(item);
	if (i){
		//i->setDescription(item->text(index)); no no no!!! why this was here???
	}
}

/** Saves the bookmarks to disk */
void CBookmarkIndex::saveBookmarks() {
	qDebug("CBookmarkIndex::saveBookmarks");
	//find the bookmark folder
	CIndexItemBase* i = 0;

	QTreeWidgetItemIterator it( this );
	while ( *it ) {
		i = dynamic_cast<CIndexItemBase*>( *it );

		if (i && (i->type() == CIndexItemBase::BookmarkFolder)) { 
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
	qDebug("CBookmarkIndex::saveBookmarks end");
}

void CBookmarkIndex::mouseMoveEvent(QMouseEvent* event)
{
	//qDebug("CBookmarkIndex::mouseMoveEvent");
	// Restart timer if we have moved to another item and shift was not pressed
	QTreeWidgetItem* itemUnderPointer = itemAt(event->pos());
	if (itemUnderPointer && (itemUnderPointer != m_previousEventItem) ) {
		//qDebug("CBookmarkIndex::mouseMoveEvent, moved onto another item");
		if ( !(event->modifiers() & Qt::ShiftModifier)) {
			m_magTimer.start(); // see the ctor for the timer properties
		}
	}
	m_previousEventItem = itemUnderPointer;
}

void CBookmarkIndex::magTimeout()
{
	qDebug("CBookmarkIndex::magTimeout");

	QTreeWidgetItem* itemUnderPointer = 0;
	if (underMouse()) {
		itemUnderPointer = itemAt(mapFromGlobal(QCursor::pos()));
	}
	// if the mouse pointer have been over the same item since the timer was started
	if (itemUnderPointer && (m_previousEventItem == itemUnderPointer)) {
		CIndexBookmarkItem* bitem = dynamic_cast<CIndexBookmarkItem*>(itemUnderPointer);
		if (bitem) {
			//qDebug("CBookmarkIndex::timerEvent: update the infodisplay");
			// Update the mag
			(CPointers::infoDisplay())->setInfo(
				InfoDisplay::CInfoDisplay::CrossReference,
				bitem->module()->name() + ":" + bitem->key()
			);
		}
	}
}
