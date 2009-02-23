/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "cbookmarkindex.h"
#include "cbookmarkindex.moc"

#include "btbookmarkitembase.h"
#include "btbookmarkitem.h"
#include "btbookmarkfolder.h"
#include "btbookmarkloader.h"

#include "backend/managers/creferencemanager.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "backend/config/cbtconfig.h"
#include "frontend/cinfodisplay.h"

#include "frontend/cprinter.h"
#include "frontend/cdragdrop.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "util/ctoolclass.h"

#include <boost/scoped_ptr.hpp>

//Qt includes
#include <QInputDialog>
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
#include <QMenu>
#include <QAction>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
#include <QDrag>

#include <QDebug>

CBookmarkIndex::CBookmarkIndex(QWidget *parent)
	: QTreeWidget(parent),
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

CBookmarkIndex::~CBookmarkIndex()
{
	saveBookmarks();
}


/** Initializes the view. */
void CBookmarkIndex::initView()
{
	//qDebug("CBookmarkIndex::initView");

	header()->hide();

	setFocusPolicy(Qt::WheelFocus);

	//d'n'd related settings
	setDragEnabled( true );
	setAcceptDrops( true );
	setDragDropMode(QAbstractItemView::DragDrop);
	viewport()->setAcceptDrops(true);
	setAutoScroll(true);
	setAutoExpandDelay(800);

	setItemsExpandable(true);
	setRootIsDecorated(true);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new QMenu(viewport());
	m_popup->setTitle(tr("Bookmarks"));

	m_actions.newFolder = newQAction(tr("New folder"), CResMgr::mainIndex::newFolder::icon, 0, this, SLOT(createNewFolder()), this);
	m_actions.changeFolder = newQAction(tr("Rename folder"),CResMgr::mainIndex::changeFolder::icon, 0, this, SLOT(changeFolder()), this);

	m_actions.changeBookmark = newQAction(tr("Change bookmark description..."), CResMgr::mainIndex::changeBookmark::icon, 0, this, SLOT(changeBookmark()), this);
	m_actions.importBookmarks = newQAction(tr("Import to folder..."), CResMgr::mainIndex::importBookmarks::icon, 0, this, SLOT(importBookmarks()), this);
	m_actions.exportBookmarks = newQAction(tr("Export from folder..."), CResMgr::mainIndex::exportBookmarks::icon, 0, this, SLOT(exportBookmarks()), this);
	m_actions.printBookmarks = newQAction(tr("Print bookmarks..."), CResMgr::mainIndex::printBookmarks::icon, 0, this, SLOT(printBookmarks()), this);

	m_actions.deleteEntries = newQAction(tr("Remove selected items..."), CResMgr::mainIndex::deleteItems::icon, 0, this, SLOT(deleteEntries()), this);


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

	//qDebug("CBookmarkIndex::initView end");
}

/** Convenience function for creating a new QAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all QAction constructors.
*/
QAction* CBookmarkIndex::newQAction(const QString& text, const QString& pix, const int /*shortcut*/, const QObject* receiver, const char* slot, QObject* parent)
{
	QAction* action = new QAction(util::filesystem::DirectoryUtil::getIcon(pix), text, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void CBookmarkIndex::initConnections()
{
	//qDebug("CBookmarkIndex::initConnections");

	// See also the bookshelf index.
	QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(droppedItem(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(droppedItem(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenu(const QPoint&)));
	QObject::connect(&m_magTimer, SIGNAL(timeout()), this, SLOT(magTimeout()));

	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotItemChanged(QTreeWidgetItem*, int)) );

	QObject::connect(this, SIGNAL(itemEntered(QTreeWidgetItem*, int)), this, SLOT(slotItemEntered(QTreeWidgetItem*, int)) );
}


/**
* Hack to get single click and selection working. See slotExecuted.
*/
void CBookmarkIndex::mouseReleaseEvent(QMouseEvent* event)
{
	//qDebug("CBookmarkIndex::mouseReleaseEvent");
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

	BtBookmarkItemBase* btItem = dynamic_cast<BtBookmarkItemBase*>(i);
	if (!btItem) {
		return;
	}

	BtBookmarkFolder* folderItem = 0;
	BtBookmarkItem* bookmarkItem = 0;
	if ((folderItem = dynamic_cast<BtBookmarkFolder*>(btItem))) {
		i->setExpanded( !i->isExpanded() );
	}
	else if (( bookmarkItem = dynamic_cast<BtBookmarkItem*>(btItem) )) { //clicked on a bookmark
		if (CSwordModuleInfo* mod = bookmarkItem->module()) {
			QList<CSwordModuleInfo*> modules;
			modules.append(mod);
			emit createReadDisplayWindow(modules, bookmarkItem->key());
		}
	}
}

/** Creates a drag mime data object for the current selection. */
QMimeData* CBookmarkIndex::dragObject()
{
	BTMimeData::ItemList dndItems;
	BTMimeData* mimeData = new BTMimeData;

	foreach( QTreeWidgetItem* widgetItem, selectedItems() ) {
		if (!widgetItem)
			break;
		if (dynamic_cast<BtBookmarkItemBase*>(widgetItem)) {
			if (BtBookmarkItem* bookmark = dynamic_cast<BtBookmarkItem*>( widgetItem )) {
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
	//qDebug("CBookmarkIndex::dragEnterEvent");
	setState(QAbstractItemView::DraggingState);
	QTreeWidget::dragEnterEvent(event);
	if (event->source() == this || event->mimeData()->hasFormat("BibleTime/Bookmark")) {
		event->acceptProposedAction();
	}
}


void CBookmarkIndex::dragMoveEvent( QDragMoveEvent* event )
{
	//qDebug("CBookmarkIndex::dragMoveEvent");

	// do this first, otherwise the event may be ignored
	QTreeWidget::dragMoveEvent(event);

	event->acceptProposedAction();
	event->accept();

	// do this to paint the arrow
	m_dragMovementPosition = event->pos();
	viewport()->update();
	
}

void CBookmarkIndex::dragLeaveEvent( QDragLeaveEvent* )
{
	qDebug("CBookmarkIndex::dragLeaveEvent");
	setState(QAbstractItemView::NoState); // not dragging anymore
	viewport()->update(); // clear the arrow
}


void CBookmarkIndex::paintEvent(QPaintEvent* event)
{
	static QPixmap pix;
	static int halfPixHeight;
	static bool arrowInitialized = false;

	// Initialize the static variables, including the arrow pixmap
	if (!arrowInitialized) {
		arrowInitialized = true;
		int arrowSize = CToolClass::mWidth(this, 1);
		QString fileName;
		if (util::filesystem::DirectoryUtil::getIconDir().exists("pointing_arrow.svg")) {
			fileName = util::filesystem::DirectoryUtil::getIconDir().filePath("pointing_arrow.svg");	
		} else {
			if (util::filesystem::DirectoryUtil::getIconDir().exists("pointing_arrow.png")) {
				fileName = util::filesystem::DirectoryUtil::getIconDir().filePath("pointing_arrow.png");
			} else {
				qWarning() << "Picture file pointing_arrow.svg or .png not found!";
			}
		}
		
		pix = QPixmap(fileName);
		pix = pix.scaled(arrowSize, arrowSize, Qt::KeepAspectRatioByExpanding);
		halfPixHeight = pix.height()/2;
	}

	// Do the normal painting first
	QTreeWidget::paintEvent(event);

	// Paint the arrow if the drag is going on
	if (QAbstractItemView::DraggingState == state()) {
		bool rtol = QApplication::isRightToLeft();
		
		QPainter painter(this->viewport());
		QTreeWidgetItem* item = itemAt(m_dragMovementPosition);
		bool isFolder = dynamic_cast<BtBookmarkFolder*>(item);
		bool isBookmark = dynamic_cast<BtBookmarkItem*>(item);

		// Find the place for the arrow
		QRect rect = visualItemRect(item);
		int xCoord = rtol ? rect.right() : rect.left();
		int yCoord;
		if (isFolder) {
			if (m_dragMovementPosition.y() > rect.bottom() - (2* rect.height()/3) ) {
				yCoord = rect.bottom() - halfPixHeight; // bottom
				xCoord = rtol ? (xCoord - indentation()) : (xCoord + indentation());
			} else {
				yCoord = rect.top() - halfPixHeight - 1; // top
			}
			
		} else {
			if (isBookmark) {
				if (m_dragMovementPosition.y() > rect.bottom() - rect.height()/2) {
					yCoord = rect.bottom() - halfPixHeight; // bottom
				} else {
					yCoord = rect.top() - halfPixHeight - 1; // top
				}
			} else {
				if (item) { // the extra item
					yCoord = rect.top() - halfPixHeight - 1;
				} else { // empty area
					rect = visualItemRect(m_extraItem);
					yCoord = rect.top() - halfPixHeight - 1;
					xCoord = rtol ? rect.right() : rect.left();
				}
			}
		}
		
		painter.drawPixmap(xCoord, yCoord, pix);
	}
}


void CBookmarkIndex::dropEvent( QDropEvent* event )
{
	qDebug("CBookmarkIndex::dropEvent");

	//setState(QAbstractItemView::NoState);
	// Try to prevent annoying timed autocollapsing. Remember to disconnect before return.
	QObject::connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(expandAutoCollapsedItem(QTreeWidgetItem*)));
	QTreeWidgetItem* item = itemAt(event->pos());
	QTreeWidgetItem* parentItem = 0;
	int indexUnderParent = 0;

	// Find the place where the drag is dropped
	if (item) {
		qDebug()<<"there was item";

		QRect rect = visualItemRect(item);
		bool isFolder = dynamic_cast<BtBookmarkFolder*>(item);
		bool isBookmark = dynamic_cast<BtBookmarkItem*>(item);

		if (isFolder) { // item is a folder
			qDebug()<<"item was folder";
			if (event->pos().y() > rect.bottom() - (2* rect.height()/3) ) {
				parentItem = item;
			} else {
				parentItem = item->parent();
				if (!parentItem) {
					parentItem = invisibleRootItem();
				}
				qDebug()<<"item:" << item << "parent:" << parentItem;
				indexUnderParent = parentItem->indexOfChild(item); // before the current folder
			}
		} else {
			if (isBookmark) { // item is a bookmark
				qDebug()<<"item was bookmark";
				parentItem = item->parent();
				if (!parentItem) {
					parentItem = invisibleRootItem();
				}
				indexUnderParent = parentItem->indexOfChild(item); // before the current bookmark
				if (event->pos().y() > rect.bottom() - rect.height()/2) {
					indexUnderParent++; // after the current bookmark
				}
			} else { // item is the extra item
				parentItem = item->parent();
				if (!parentItem) {
					parentItem = invisibleRootItem();
				}
				indexUnderParent = parentItem->indexOfChild(item); // before the current bookmark
			}
		}

	} else { // no item under event point: drop to the end
		qDebug()<<"there was no item";
		parentItem = invisibleRootItem();
		indexUnderParent = parentItem->childCount()-1;
	}


	if ( event->source() == this ) {
		qDebug("dropping internal drag");
		event->accept();

		bool bookmarksOnly = true;
		bool targetIncluded = false;
		bool moreThanOneFolder = false;

		QList<QTreeWidgetItem*> newItems = addItemsToDropTree(parentItem, bookmarksOnly, targetIncluded, moreThanOneFolder);

		if (moreThanOneFolder) {
			QToolTip::showText(QCursor::pos(), tr("Can drop only bookmarks or one folder"));
			QObject::disconnect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(expandAutoCollapsedItem(QTreeWidgetItem*)));
			return;
		}
		if (targetIncluded) {
			QToolTip::showText(QCursor::pos(), tr("Can't drop folder into the folder itself or into its subfolder"));
			QObject::disconnect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(expandAutoCollapsedItem(QTreeWidgetItem*)));
			return;
		}
		// Ask whether to copy or move with a popup menu
		
		QMenu* dropPopupMenu = new QMenu(this);
		QAction* copy = dropPopupMenu->addAction(tr("Copy"));
		QAction* move = dropPopupMenu->addAction(tr("Move"));
		QAction* dropAction = dropPopupMenu->exec(QCursor::pos());
		if (dropAction == copy) {
			qDebug() << "copy";
			parentItem->insertChildren(indexUnderParent, newItems);
		} else {
			if (dropAction == move) {
				qDebug() << "move";
				parentItem->insertChildren(indexUnderParent, newItems);
				deleteEntries(false);
			} else {
				QObject::disconnect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(expandAutoCollapsedItem(QTreeWidgetItem*)));
				return; // user canceled
			}
		}

	} else {
		qDebug()<<"the source was outside this";
		createBookmarkFromDrop(event, parentItem, indexUnderParent);
	}
	QObject::disconnect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(expandAutoCollapsedItem(QTreeWidgetItem*)));
	setState(QAbstractItemView::NoState);
}


void CBookmarkIndex::createBookmarkFromDrop(QDropEvent* event, QTreeWidgetItem* parentItem, int indexInParent)
{
	//qDebug("CBookmarkIndex::createBookmarkFromDrop");
	//take the bookmark data from the mime source
	const BTMimeData* mdata = dynamic_cast<const BTMimeData*>(event->mimeData());
	if (mdata) {
		//create the new bookmark
		QString moduleName = mdata->bookmark().module();
		QString keyText = mdata->bookmark().key();
		QString description = mdata->bookmark().description();
		CSwordModuleInfo* minfo = CPointers::backend()->findModuleByName(moduleName);

		QTreeWidgetItem* newItem = new BtBookmarkItem(minfo, keyText, description);
		parentItem->insertChild(indexInParent, newItem);
	}
}


/** Load the tree from file */
void CBookmarkIndex::initTree() {
	qDebug("CBookmarkIndex::initTree");
	BtBookmarkLoader loader;
	addTopLevelItems(loader.loadTree());

	// add the invisible extra item at the end
	m_extraItem = new QTreeWidgetItem();
	m_extraItem->setFlags(Qt::ItemIsDropEnabled);
	addTopLevelItem(m_extraItem);
}

void CBookmarkIndex::slotItemEntered(QTreeWidgetItem* item, int)
{
	qDebug() << "CBookmarkIndex::slotItemEntered";
	if (item == m_extraItem) {
		m_extraItem->setText(0, tr("Drag references from text views to this view"));
	}
	else {
		m_extraItem->setText(0, QString::null);
	}
}


/** Returns the correct QAction object for the given type of action. */
QAction* CBookmarkIndex::action( BtBookmarkItemBase::MenuAction type ) const {
	switch (type) {
	case BtBookmarkItemBase::NewFolder:
		return m_actions.newFolder;
	case BtBookmarkItemBase::ChangeFolder:
		return m_actions.changeFolder;

	case BtBookmarkItemBase::ChangeBookmark:
		return m_actions.changeBookmark;
	case BtBookmarkItemBase::ImportBookmarks:
		return m_actions.importBookmarks;
	case BtBookmarkItemBase::ExportBookmarks:
		return m_actions.exportBookmarks;
	case BtBookmarkItemBase::PrintBookmarks:
		return m_actions.printBookmarks;

	case BtBookmarkItemBase::DeleteEntries:
		return m_actions.deleteEntries;

	default:
		return 0;
	}
}

/** Shows the context menu at the given position. */
void CBookmarkIndex::contextMenu(const QPoint& p)
{
	//qDebug("CBookmarkIndex::contextMenu");
	//setup menu entries depending on current selection
	QTreeWidgetItem* i = itemAt(p);
	QList<QTreeWidgetItem *> items = selectedItems();
	//The item which was clicked may not be selected
	if (i && !items.contains(i) && i != m_extraItem)
		items.append(i);

	if (items.count() == 0) {
		//special handling for no selection
		BtBookmarkItemBase::MenuAction actionType;
		for (int index = BtBookmarkItemBase::ActionBegin; index <= BtBookmarkItemBase::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItemBase::MenuAction>(index);
			if (QAction* a = action(actionType)) {
				switch (index) {
				//case BtBookmarkItemBase::ExportBookmarks:
				//case BtBookmarkItemBase::ImportBookmarks:
				case BtBookmarkItemBase::NewFolder:
				//case BtBookmarkItemBase::PrintBookmarks:
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

		BtBookmarkItemBase* item = dynamic_cast<BtBookmarkItemBase*>(items.at(0));
		BtBookmarkItemBase::MenuAction actionType;
		for (int index = BtBookmarkItemBase::ActionBegin; index <= BtBookmarkItemBase::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItemBase::MenuAction>(index);
			if (QAction* a = action(actionType))
				a->setEnabled( item->enableAction(actionType) );
		}
	}
	else {
		//first disable all actions
		BtBookmarkItemBase::MenuAction actionType;
		for (int index = BtBookmarkItemBase::ActionBegin; index <= BtBookmarkItemBase::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItemBase::MenuAction>(index);
			if (QAction* a = action(actionType))
				a->setEnabled(false);
		}
		//enable the menu items depending on the types of the selected items.
		for (int index = BtBookmarkItemBase::ActionBegin; index <= BtBookmarkItemBase::ActionEnd; ++index) {
			actionType = static_cast<BtBookmarkItemBase::MenuAction>(index);
			bool enableAction = isMultiAction(actionType);
			QListIterator<QTreeWidgetItem *> it(items);
			while(it.hasNext()) {
				BtBookmarkItemBase* i = dynamic_cast<BtBookmarkItemBase*>(it.next());
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
	//qDebug("CBookmarkIndex::contextMenu end");
}

/** Adds a new subfolder to the current item. */
void CBookmarkIndex::createNewFolder() {
	//qDebug("CBookmarkIndex::createNewFolder");
	QList<QTreeWidgetItem*> selected = selectedItems();
	if (selected.count() > 0) {
		BtBookmarkFolder* i = dynamic_cast<BtBookmarkFolder*>(currentItem());
		if (i) {
			i->newSubFolder();
		}
	}
	else {
		// create a top level folder
		BtBookmarkFolder* newFolder = new BtBookmarkFolder(0, QObject::tr("New folder"));
		//parentFolder->addChild(newFolder);
		insertTopLevelItem(topLevelItemCount()-1, newFolder);
		newFolder->update();
		newFolder->rename();
	}
}

/** Opens a dialog to change the current folder. */
void CBookmarkIndex::changeFolder() {
	BtBookmarkFolder* i = dynamic_cast<BtBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	if (i) {
		i->rename();
	}
}

/** Changes the current bookmark. */
void CBookmarkIndex::changeBookmark() {
	BtBookmarkItem* i = dynamic_cast<BtBookmarkItem*>(currentItem());
	Q_ASSERT(i);

	if (i) {
		i->rename();
	}
}

/** Exports the bookmarks being in the selected folder. */
void CBookmarkIndex::exportBookmarks() {
	BtBookmarkFolder* i = dynamic_cast<BtBookmarkFolder*>(currentItem());
	Q_ASSERT(i);

	if (i) {
		i->exportBookmarks();
	}
}

/** Import bookmarks from a file and add them to the selected folder. */
void CBookmarkIndex::importBookmarks() {
	BtBookmarkFolder* i = dynamic_cast<BtBookmarkFolder*>(currentItem());
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
	BtBookmarkFolder* bf = dynamic_cast<BtBookmarkFolder*>(currentItem());

	if (bf) {
		items = bf->getChildList();
	}
	else {
		items = selectedItems();
	}

	//create a tree of keytreeitems using the bookmark hierarchy.
	QListIterator<QTreeWidgetItem*> it(items);
	while(it.hasNext()) {
		BtBookmarkItem* i = dynamic_cast<BtBookmarkItem*>(it.next());
		if (i) {
			qDebug() << "printBookmarks: add to list" << i->key();
			tree.append( new Printing::CPrinter::KeyTreeItem( i->key(), i->module(), settings ) );
		}
	}

	if (items.count() == 0) {
		qWarning("Tried to print empty bookmark list.");
		return;
	}
	boost::scoped_ptr<Printing::CPrinter> printer(
		new Printing::CPrinter( this, CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() )
	);
	printer->printKeyTree(tree);
}

/** Deletes the selected entries. */
void CBookmarkIndex::deleteEntries(bool confirm)
{
	if (confirm) {
		if (!selectedItems().count()) {
			BtBookmarkItemBase* f = dynamic_cast<BtBookmarkItemBase*>(currentItem());
			if (f) {
				currentItem()->setSelected(true);
			} else {
				return;
			}
		}
	
		if (QMessageBox::question(this, tr("Delete Items"), tr("Do you really want to delete the selected items and child-items?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No ) != QMessageBox::Yes) {
			return;
		}
	}

	while (selectedItems().size() > 0) {
		delete selectedItems().at(0); // deleting all does not work because it may cause double deletion
	}

}


/*
Reimplementation from QAbstractItemView/QTreeWidget. Takes care of movable items.
It's easier to use this than to start drag with mouse event handlers.
The default implementation would drag items, but we don't call it. Instead we create
a BibleTime mimedata object. It can be dragged and dropped to a text view or somewhere else.
The internal drag is handled differently, it doesn't use the mimedata (see dropEvent()).
*/
void CBookmarkIndex::startDrag(Qt::DropActions)
{
	qDebug("CBookmarkIndex::startDrag");

	QMimeData* mData = dragObject(); // create the data which can be used in other widgets
	QDrag* drag = new QDrag(this);
	drag->setMimeData(mData);
	drag->exec();

	viewport()->update(); // because of the arrow	
}






/* Returns true if more than one entry is supported by this action type. Returns false for actions which support only one entry, e.g. about module etc. */
bool CBookmarkIndex::isMultiAction( const BtBookmarkItemBase::MenuAction type ) const {
	switch (type) {
	case BtBookmarkItemBase::NewFolder:
		return false;
	case BtBookmarkItemBase::ChangeFolder:
		return false;

	case BtBookmarkItemBase::ChangeBookmark:
		return false;
	case BtBookmarkItemBase::ImportBookmarks:
		return false;
	case BtBookmarkItemBase::ExportBookmarks:
		return false;
	case BtBookmarkItemBase::PrintBookmarks:
		return true;

	case BtBookmarkItemBase::DeleteEntries:
		return true;
	}

	return false;
}

/* Saves the bookmarks to the default bookmarks file. */
void CBookmarkIndex::saveBookmarks() {

	qDebug("CBookmarkIndex::saveBookmarks()");
	BtBookmarkLoader loader;
	loader.saveTreeFromRootItem(invisibleRootItem());
	//qDebug("CBookmarkIndex::saveBookmarks end");
}

void CBookmarkIndex::mouseMoveEvent(QMouseEvent* event)
{
	//qDebug() << "CBookmarkIndex::mouseMoveEvent";

	// Restart the mag timer if we have moved to another item and shift was not pressed.
	QTreeWidgetItem* itemUnderPointer = itemAt(event->pos());
	if (itemUnderPointer && (itemUnderPointer != m_previousEventItem) ) {
		//qDebug("CBookmarkIndex::mouseMoveEvent, moved onto another item");
		if ( !(event->modifiers() & Qt::ShiftModifier)) {
			m_magTimer.start(); // see the ctor for the timer properties
		}
	}
	m_previousEventItem = itemUnderPointer;

	// Clear the extra item text unless we are on top of it
	if ( (itemUnderPointer != m_extraItem) && !m_extraItem->text(0).isNull()) {
		m_extraItem->setText(0, QString::null);
	}

	QTreeWidget::mouseMoveEvent(event);
}

void CBookmarkIndex::magTimeout()
{
	//qDebug("CBookmarkIndex::magTimeout");

	QTreeWidgetItem* itemUnderPointer = 0;
	if (underMouse()) {
		itemUnderPointer = itemAt(mapFromGlobal(QCursor::pos()));
	}
	// if the mouse pointer have been over the same item since the timer was started
	if (itemUnderPointer && (m_previousEventItem == itemUnderPointer)) {
		BtBookmarkItem* bitem = dynamic_cast<BtBookmarkItem*>(itemUnderPointer);
		if (bitem) {
			//qDebug("CBookmarkIndex::timerEvent: update the infodisplay");
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

/*
Creates a list of new items based on the current selection.
If there are only bookmarks in the selection they are all included.
If there is one folder it's included as a deep copy.
Sets bookmarksOnly=false if it finds a folder.
Sets targetIncluded=true if the target is in the list.
Sets moreThanOneFolder=true if selection includes one folder and something more.
If moreThanOneFolder or targetIncluded is detected the creation of list is stopped
and the list is incomplete.
*/
QList<QTreeWidgetItem*> CBookmarkIndex::addItemsToDropTree(
	QTreeWidgetItem* target, bool& bookmarksOnly, bool& targetIncluded, bool& moreThanOneFolder)
{
	qDebug() << "CBookmarkIndex::addItemsToDropTree";
	QList<QTreeWidgetItem*> selectedList = selectedItems();
	QList<QTreeWidgetItem*> newList;

	foreach(QTreeWidgetItem* item, selectedList) {
		qDebug() << "go through all items:" << item;
		if ( BtBookmarkFolder* folder = dynamic_cast<BtBookmarkFolder*>(item)) {
			bookmarksOnly = false;
			if (selectedList.count() > 1) { // only one item allowed if a folder is selected
				qDebug() << "one folder and something else is selected";
				moreThanOneFolder = true;
				break;
			}
			if (folder->hasDescendant(target)) { // dropping to self or descendand not allowed
				qDebug() << "addItemsToDropTree: target is included";
				targetIncluded = true;
				break;
			}
		} else {
			qDebug() << "append new QTreeWidget item (should be BtBookmarkItem?)";
			newList.append(new BtBookmarkItem( *(dynamic_cast<BtBookmarkItem*>(item)) ));
		}
	}
	if (!bookmarksOnly && selectedList.count() == 1) {
		qDebug() << "exactly one folder";
		BtBookmarkFolder* folder = dynamic_cast<BtBookmarkFolder*>(selectedList.value(0));
		BtBookmarkFolder* copy = folder->deepCopy();
		newList.append(copy);
	}
	if (!bookmarksOnly && selectedList.count() > 1) {
		// wrong amount of items
		qDebug() << "one folder and something else is selected 2";
		moreThanOneFolder = true;
	}
	qDebug() << "return the new list" << newList;
	return newList;
}

