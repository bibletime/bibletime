/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cmainindex.h"
#include "cmainindex.moc"

#include "cindexitembase.h"
#include "cindexbookmarkitem.h"
#include "cindexbookmarkfolder.h"
#include "cindexglossaryfolder.h"
#include "cindexmoduleitem.h"
#include "cindexsubfolder.h"

#include "backend/managers/creferencemanager.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinfodisplay.h"

#include "frontend/cprinter.h"
#include "frontend/cdragdrop.h"

#include "util/cresmgr.h"
#include "util/scoped_resource.h"

//Qt includes
#include <QInputDialog>
#include <QRegExp>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTreeWidget>
#include <QHeaderView>

//KDE includes
#include <klocale.h>
#include <kstandarddirs.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <kaction.h>
#include <kactionmenu.h>



//CMainIndex::ToolTip::ToolTip(CMainIndex* parent)
//{}

// void CMainIndex::ToolTip::maybeTip(const QPoint& p) {
// 	CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(m_mainIndex->itemAt(p));
// 	Q_ASSERT(i);
// 	if ( !i ) {
// 		return;
// 	}
// 
// 	//TODO: ???
// 	//QRect r = m_mainIndex->itemRect(i);
// 	//if (!r.isValid()) {
// 	//	return;
// 	//}
// 
// 	//get type of item and display correct text
// 	const QString text = i->toolTip();
// 	if (!text.isEmpty()) {
// 		CIndexBookmarkItem* bookmark = dynamic_cast<CIndexBookmarkItem*>(i);
// 		if (bookmark) {
// 			(CPointers::infoDisplay())->setInfo(
// 				InfoDisplay::CInfoDisplay::CrossReference,
// 				bookmark->module()->name() + ":" + bookmark->key()
// 			);
// 		}
// 		else {
// 			CPointers::infoDisplay()->clearInfo();
// 		}
// 
// 		//tip(r, text); //TODO
// 	}
// 	else {
// 		CPointers::infoDisplay()->clearInfo();
// 	}
// }

CMainIndex::CMainIndex(QWidget *parent)
	: QTreeWidget(parent),
	m_searchDialog(0),
	//m_toolTip(0),
	m_itemsMovable(false),
	m_autoOpenFolder(0),
	m_autoOpenTimer(this)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	initView();
	initConnections();
	//initTree() is called in polish()
}

CMainIndex::~CMainIndex() {
	saveSettings();
	saveBookmarks();

	//this->setToolTip("");
	//delete m_toolTip;
}

/** Reimplementation. Adds the given group to the tree. */
void CMainIndex::addGroup(const CIndexItemBase::Type type, const QString language) {
	qDebug("CMainIndex::addGroup");
	CIndexTreeFolder *i = 0;
	switch (type) {
		case CIndexItemBase::BookmarkFolder:
			qDebug("case CIndexItemBase::BookmarkFolder");
			i = new CIndexBookmarkFolder(this);
			break;
		case CIndexItemBase::GlossaryModuleFolder:
			qDebug("case CIndexItemBase::GlossaryModuleFolder");
			//we have no second language
			i = new CIndexGlossaryFolder(this, type, language, QString::null); 
			break;
		default:
			qDebug("case treefolder");
			i = new CIndexTreeFolder(this, type, language);
			break;
	}
	
	if (i) {
		i->init();
		if (i->childCount() == 0 && type != CIndexItemBase::BookmarkFolder) {
			delete i;
		}
	}
}


/** Initializes the view. */
void CMainIndex::initView()
{	
	qDebug("CMainIndex::initView");

	header()->hide();
	//m_toolTip = new ToolTip(this);
	//??? setTooltipColumn(-1);
	//to disable Qt's tooltips
	//setShowToolTips(false);
	//setBackgroundMode(PaletteBase);
	//setFullWidth(true);

	setFocusPolicy(Qt::WheelFocus);
	setAcceptDrops( true );
	setDragEnabled( true );
	setDropIndicatorShown( true );

	setItemsExpandable(true);
	viewport()->setAcceptDrops(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new KMenu(viewport());
	m_popup->addTitle(i18n("Bookshelf"));

	m_actions.newFolder = newKAction(i18n("Create a new folder"), CResMgr::mainIndex::newFolder::icon, 0, this, SLOT(createNewFolder()), this);
	m_actions.changeFolder = newKAction(i18n("Change this folder"),CResMgr::mainIndex::changeFolder::icon, 0, this, SLOT(changeFolder()), this);

	m_actions.changeBookmark = newKAction(i18n("Change bookmark description"),CResMgr::mainIndex::changeBookmark::icon, 0, this, SLOT(changeBookmark()), this);
	m_actions.importBookmarks = newKAction(i18n("Import bookmarks"),CResMgr::mainIndex::importBookmarks::icon, 0, this, SLOT(importBookmarks()), this);
	m_actions.exportBookmarks = newKAction(i18n("Export bookmarks"),CResMgr::mainIndex::exportBookmarks::icon, 0, this, SLOT(exportBookmarks()), this);
	m_actions.printBookmarks = newKAction(i18n("Print bookmarks"),CResMgr::mainIndex::printBookmarks::icon, 0, this, SLOT(printBookmarks()), this);

	m_actions.deleteEntries = newKAction(i18n("Remove selected item(s)"),CResMgr::mainIndex::deleteItems::icon, 0, this, SLOT(deleteEntries()), this);

	m_actions.editModuleMenu = new KActionMenu(KIcon(CResMgr::mainIndex::editModuleMenu::icon), i18n("Edit this work"), this);
	m_actions.editModuleMenu->setDelayed(false);

	m_actions.editModulePlain = newKAction(i18n("Plain text"),CResMgr::mainIndex::editModulePlain::icon, 0, this, SLOT(editModulePlain()), this);
	m_actions.editModuleHTML = newKAction(i18n("HTML"),CResMgr::mainIndex::editModuleHTML::icon, 0, this, SLOT(editModuleHTML()), this);
	
	m_actions.searchInModules = newKAction(i18n("Search in selected work(s)"),CResMgr::mainIndex::search::icon, 0, this, SLOT(searchInModules()), this);
	m_actions.unlockModule = newKAction(i18n("Unlock this work"),CResMgr::mainIndex::unlockModule::icon, 0, this, SLOT(unlockModule()), this);
	m_actions.aboutModule = newKAction(i18n("About this work"),CResMgr::mainIndex::aboutModule::icon, 0, this, SLOT(aboutModule()), this);


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
	separator = new QAction(this);
	separator->setSeparator(true);	
	m_popup->addAction(separator);
	m_popup->addAction(m_actions.editModuleMenu);

	//sub item of edit module menu
	m_actions.editModuleMenu->addAction(m_actions.editModulePlain);
	//sub item of edit module menu
	m_actions.editModuleMenu->addAction(m_actions.editModuleHTML);
	m_popup->addAction(m_actions.searchInModules);
	m_popup->addAction(m_actions.unlockModule);
	m_popup->addAction(m_actions.aboutModule);
	qDebug("CMainIndex::initView end");
}

/** Convenience function for creating a new KAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all KAction constructors.
*/
KAction* CMainIndex::newKAction(const QString& text, const QString& pix, const int shortcut, const QObject* receiver, const char* slot, QObject* parent)
{
	KAction* action = new KAction(KIcon(pix), text, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, slot);
	return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void CMainIndex::initConnections()
{
	qDebug("CMainIndex::initConnections");
	
	//Strangely itemActivated only didn't let open a tree by clicking even though the relevant
	//code in slotExecuted was executed. Therefore itemClicked is necessary.
	QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));
	QObject::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotExecuted(QTreeWidgetItem*)));

	QObject::connect(this, SIGNAL(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)),
		SLOT(dropped(QDropEvent*, QTreeWidgetItem*, QTreeWidgetItem*)));
 	
	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(contextMenu(const QPoint&)));
	QObject::connect(&m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpenTimeout()));
	qDebug("CMainIndex::initConnections");
}

/** Is called when an item was clicked/double clicked. */
void CMainIndex::slotExecuted( QTreeWidgetItem* i )
{
	qDebug("CMainIndex::slotExecuted");
	qDebug(QString().setNum((int)i).toLatin1().data());
	CIndexItemBase* ci = dynamic_cast<CIndexItemBase*>(i);
	if (!ci) {
		qDebug("if (!ci)");
		return;
	}

	if (ci->isFolder()) {
		qDebug() << "folder was activated";
		qDebug() << "item was:" << i->isExpanded() << "and will be:" << !i->isExpanded();
		i->setExpanded( !i->isExpanded() );
	}
	else if (CIndexModuleItem* m = dynamic_cast<CIndexModuleItem*>(i))  { //clicked on a module
		CSwordModuleInfo* mod = m->module();
		ListCSwordModuleInfo modules;
		modules.append(mod);
		qDebug("will emit createReadDisplayWindow");
		emit createReadDisplayWindow(modules, QString::null);
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
QMimeData* CMainIndex::dragObject()
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


/** Reimplementation from QTreeWidget. Returns true if the drag is acceptable for the widget. */
void CMainIndex::dragEnterEvent( QDragEnterEvent* event ) const
{
	event->acceptProposedAction();
}

void CMainIndex::dragMoveEvent( QDragMoveEvent* event ) const
{
	const QPoint pos = event->pos();
 
	CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(itemAt(pos));
	//TODO: CIndexItemBase needs acceptDrop
	//if (i->acceptDrop(event) || i->isMovable()) {
	//	event->acceptProposedAction();
	//} else {
	//
	//}
	
}

void CMainIndex::dropEvent( QDropEvent* event ) const
{

}

/** No descriptions */
void CMainIndex::initTree() {
	qDebug("CMainIndex::initTree");
	addGroup(CIndexItemBase::BookmarkFolder, QString("*"));
	addGroup(CIndexItemBase::BibleModuleFolder, QString("*"));
	addGroup(CIndexItemBase::BookModuleFolder, QString("*"));
	addGroup(CIndexItemBase::CommentaryModuleFolder, QString("*"));
	addGroup(CIndexItemBase::DevotionalModuleFolder, QString("*"));
	addGroup(CIndexItemBase::GlossaryModuleFolder, QString("*"));
	addGroup(CIndexItemBase::LexiconModuleFolder, QString("*"));
}

/** No descriptions */
void CMainIndex::dropped( QDropEvent* e, QTreeWidgetItem* parent, QTreeWidgetItem* after) {
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
// 		//items.setAutoDelete(true);
// 		qDeleteAll(items);
// 		items.clear(); //delete the selected items we dragged
// 	}
}

/** No descriptions */
void CMainIndex::emitModulesChosen( ListCSwordModuleInfo modules, QString key ) {
	emit createReadDisplayWindow(modules, key);
}

/** Returns the correct KAction object for the given type of action. */
KAction* const CMainIndex::action( const CIndexItemBase::MenuAction type ) const {
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

	case CIndexItemBase::EditModule:
		return m_actions.editModuleMenu;
	case CIndexItemBase::SearchInModules:
		return m_actions.searchInModules;
	case CIndexItemBase::UnlockModule:
		return m_actions.unlockModule;
	case CIndexItemBase::AboutModule:
		return m_actions.aboutModule;
	default:
		return 0;
	}
}

/** Shows the context menu at the given position. */
void CMainIndex::contextMenu(const QPoint& p) {
	//setup menu entries depending on current selection
	QTreeWidgetItem* i = itemAt(p);
	QList<QTreeWidgetItem *> items = selectedItems();

	if (items.count() == 0) { 
		//special handling for no selection
	}
	else if (items.count() == 1) { 
		//special handling for one selected item
		
		CIndexItemBase* item = dynamic_cast<CIndexItemBase*>(items.at(0));
		CIndexItemBase::MenuAction actionType;
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled( item->enableAction(actionType) );
		}
	}
	else {
		//first disable all actions
		CIndexItemBase::MenuAction actionType;
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled(false);
		}

		//enable the menu items depending on the types of the selected items.
		for (int index = CIndexItemBase::ActionBegin; index <= CIndexItemBase::ActionEnd; ++index) {
			actionType = static_cast<CIndexItemBase::MenuAction>(index);
			bool enableAction = isMultiAction(actionType);
			QListIterator<QTreeWidgetItem *> it(items);
			while(it.hasNext()) {
				CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(it.next());
				enableAction = enableAction && i->enableAction(actionType);
			}
			if (enableAction) {
				KAction* a = action(actionType) ;
				if (i && a)
					a->setEnabled(enableAction);
			}
		}
	}
	
	//finally, open the popup
	m_popup->exec(mapToGlobal(p));
}

/** Adds a new subfolder to the current item. */
void CMainIndex::createNewFolder() {
	qDebug("CMainIndex::createNewFolder");
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->newSubFolder();
	}
}

/** Opens a dialog to change the current folder. */
void CMainIndex::changeFolder() {
	CIndexFolderBase* i = dynamic_cast<CIndexFolderBase*>(currentItem());
	Q_ASSERT(i);
	if (i) {
		i->rename();
	}
}

/** Changes the current bookmark. */
void CMainIndex::changeBookmark() {
	CIndexBookmarkItem* i = dynamic_cast<CIndexBookmarkItem*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->rename();
	}
}

/** Exports the bookmarks being in the selected folder. */
void CMainIndex::exportBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->exportBookmarks();
	}
}

/** Import bookmarks from a file and add them to the selected folder. */
void CMainIndex::importBookmarks() {
	CIndexBookmarkFolder* i = dynamic_cast<CIndexBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->importBookmarks();
	}
}

/** Prints the selected bookmarks. */
void CMainIndex::printBookmarks() {
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

	util::scoped_ptr<Printing::CPrinter> printer( 
		new Printing::CPrinter( this, CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() ) 
	);
	printer->printKeyTree(tree);
}

/** Deletes the selected entries. */
void CMainIndex::deleteEntries() {
	QList<QTreeWidgetItem *> items = selectedItems();
	if (!items.count())
		return;

	if (KMessageBox::warningYesNo(this, i18n("Do you really want to delete the selected items and child-items?"), i18n("Delete Items")) != KMessageBox::Yes) {
		return;
	}

	// We have to go backwards because otherwise deleting folders would delete their childs => crash before we delete those
	QListIterator<QTreeWidgetItem *> it(items);
	it.toBack();
	while(it.hasPrevious()) {
		if (CIndexItemBase* i = dynamic_cast<CIndexItemBase*>(it.previous())) {
			if (i->enableAction(CIndexItemBase::DeleteEntries)) {
				delete i;
			}
		}
	}
}

/** Opens the searchdialog for the selected modules. */
void CMainIndex::searchInModules() {
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	ListCSwordModuleInfo modules;
	while(it.hasNext()) {
		if (CIndexModuleItem* i = dynamic_cast<CIndexModuleItem*>(it.next())) {
			if (i->module()) {
				modules.append(i->module());
			}
		}
	}

	if (modules.isEmpty()) { //get a list of useful default modules for the search if no modules were selected
		CSwordModuleInfo* m = CBTConfig::get(CBTConfig::standardBible);
		if (m) {
			modules.append(m);
		}
	}

	Search::CSearchDialog::openDialog(modules, QString::null);
}

/** Unlocks the current module. */
void CMainIndex::unlockModule() {
	if (CIndexModuleItem* i = dynamic_cast<CIndexModuleItem*>(currentItem())) {
		bool ok = false;
		const QString unlockKey =
			QInputDialog::getText(
				this,
				i18n("BibleTime - Unlock work"),
				i18n("Enter the unlock key for this work."),
				QLineEdit::Normal,
				i->module()->config(CSwordModuleInfo::CipherKey),
				&ok
			);
		
		if (ok) {
			i->module()->unlock( unlockKey );
			emit signalSwordSetupChanged();
		}
	}
}

/** Shows information about the current module. */
void CMainIndex::aboutModule() {
	if (CIndexModuleItem* i = dynamic_cast<CIndexModuleItem*>(currentItem())) {
		KMessageBox::about(this, i->module()->aboutText(), i->module()->config(CSwordModuleInfo::Description), false);
	}
}

/** Reimplementation. Takes care of movable items. */
void CMainIndex::startDrag(Qt::DropActions supportedActions) {
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
}

/** Reimplementation to support the items dragEnter and dragLeave functions. */
void CMainIndex::contentsDragMoveEvent( QDragMoveEvent* event ) {
// 	//  qWarning("void CMainIndex:: drag move event ( QDragLeaveEvent* e )");
// 	CIndexItemBase* i = dynamic_cast<CIndexItemBase*>( itemAt( contentsToViewport(event->pos())) );
// 	if (i) {
// 		if (i->allowAutoOpen(event) || (i->acceptDrop(event) && i->isFolder() && i->allowAutoOpen(event) && !i->isOpen() && autoOpen()) ) {
// 			if (m_autoOpenFolder != i)  {
// 				m_autoOpenTimer.stop();
// 			}
// 			
// 			m_autoOpenFolder = i;
// 			m_autoOpenTimer.start( 400 );
// 		}
// 		else {
// 			m_autoOpenFolder = 0;
// 		}
// 	}
// 	else {
// 		m_autoOpenFolder = 0;
// 	}
// 
// 	QTreeWidget::contentsDragMoveEvent(event);
}

QRect CMainIndex::drawItemHighlighter(QPainter* painter, QTreeWidgetItem* item) {
// 	CIndexBookmarkItem* bookmark = dynamic_cast<CIndexBookmarkItem*>(item);
// 	if (bookmark) { 
// 		//no drops on bookmarks allowed, just moving items after it
// 		return QRect();
// 	}
// 
// 	return QTreeWidget::drawItemHighlighter(painter, item);
}


void CMainIndex::autoOpenTimeout() {
	m_autoOpenTimer.stop();
	if (m_autoOpenFolder && !m_autoOpenFolder->isExpanded() && m_autoOpenFolder->childCount()) {
		m_autoOpenFolder->setExpanded(true);
	}
}

/** No descriptions */
void CMainIndex::contentsDragLeaveEvent( QDragLeaveEvent* e ) {
// 	m_autoOpenTimer.stop();
// 	QTreeWidget::contentsDragLeaveEvent(e);
}

/** Returns true if more than one netry is supported by this action type. Returns false for actions which support only one entry, e.g. about module etc. */
const bool CMainIndex::isMultiAction( const CIndexItemBase::MenuAction type ) const {
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

	case CIndexItemBase::EditModule:
		return false;
	case CIndexItemBase::SearchInModules:
		return true;
	case CIndexItemBase::UnlockModule:
		return false;
	case CIndexItemBase::AboutModule:
		return false;
	}
	
	return false;
}

/** Is called when items should be moved. */
void CMainIndex::moved( QList<QTreeWidgetItem>& /*items*/, QList<QTreeWidgetItem>& /*afterFirst*/, QList<QTreeWidgetItem>& /*afterNow*/) {
	qDebug("move items");
}

/** Opens an editor window to edit the modules content. */
void CMainIndex::editModulePlain() {
	ListCSwordModuleInfo modules;
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	//loop through items
	while(it.hasNext()) {
		if (CIndexModuleItem* i = dynamic_cast<CIndexModuleItem*>(it.next())) {
			modules.append(i->module());
		}
	}
	if (modules.count() == 1) {
		emit createWriteDisplayWindow(modules.first(), QString::null, CDisplayWindow::PlainTextWindow);
	};
}

/** Opens an editor window to edit the modules content. */
void CMainIndex::editModuleHTML() {
	ListCSwordModuleInfo modules;
	QList<QTreeWidgetItem *> items = selectedItems();
	QListIterator<QTreeWidgetItem *> it(items);
	while(it.hasNext()) {
		if (CIndexModuleItem* i = dynamic_cast<CIndexModuleItem*>(it.next())) {
			modules.append(i->module());
		}
	}
	
	if (modules.count() == 1) {
		emit createWriteDisplayWindow(modules.first(), QString::null, CDisplayWindow::HTMLWindow);
	}
}

/** Reloads the main index's Sword dependend things like modules */
void CMainIndex::reloadSword() {
	//reload the modules, save the open groups before removing the items
	saveSettings();
	clear();
	initTree();	
	readSettings();
}

/** Saves the bookmarks to disk */
void CMainIndex::saveBookmarks() {
	//find the bookmark folder
	CIndexItemBase* i = 0;

	QTreeWidgetItemIterator it( this );
	while ( *it ) {
		i = dynamic_cast<CIndexItemBase*>( *it );

		if (i && (i->type() == CIndexItemBase::BookmarkFolder)) { 
			//found the bookmark folder
			KStandardDirs stdDirs;

			const QString path = stdDirs.saveLocation("data", "bibletime/");
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
}

void CMainIndex::readSettings() {
//  	qDebug("CMainIndex::readSettings");
// 	
// 	//Open those groups which are marked open in configuration
// 	QStringList openGroups = CBTConfig::get(CBTConfig::bookshelfOpenGroups);
// 	for (QStringList::Iterator it( openGroups.begin() ); it != openGroups.end(); ++it) {
// 		/*
// 		For each item in openGroups, find the QTreeWidgetItem specified and open it.
// 		Better way to do this?
// 		*/
// 		QStringList path = QString((*it)).split("/"); //e.g. with items parent, child
// 		QTreeWidgetItem* item = topLevelItem(0); //get first child in this QTreeWidget
// 				//orig: firstChild() begin on the top for each item
// 		Q_ASSERT(item);
// 		unsigned int index = 1;
// 
// 		for (QStringList::Iterator p_it( path.begin() ); p_it != path.end(); ++p_it) {
// 			QString itemName = (*p_it).replace("\\/", "/");
// 
// 			while (item && (item->text(0) != itemName)) {
// 				//loop through QTreeWidgetItems, looking for one with the same name as from openGroups
// 				item = item->nextSibling();
// 			}
// 
// 			if (item && (item->text(0) == itemName)) {
// 				if (index < path.count()) { //don't call firstChild on the right, i.e. last item of the list
// 					item = item->firstChild();
// 				}
// 
// 				++index;
// 			}
// 		}
// 
// 		if (item) {
// 			item->setExpanded(true);
// 		}
// 	}
// 
// 	//restore the content position
// // 	setContentsPos(
// // 		CBTConfig::get(CBTConfig::bookshelfContentsX),
// // 		CBTConfig::get(CBTConfig::bookshelfContentsY)
// // 	);
// // 	horizontalScrollBar()->setValue(CBTConfig::get(CBTConfig::bookshelfContentsX));
// // 	verticalScrollBar()->setValue(CBTConfig::get(CBTConfig::bookshelfContentsY));
// 
// 
// 	//restore the selected item
// 	QStringList path = QString(CBTConfig::get(CBTConfig::bookshelfCurrentItem)).split("/");
// 	QTreeWidgetItem* item = firstChild();
// 	Q_ASSERT(item);
// 	unsigned int index = 1;
// 	for (QStringList::iterator it( path.begin() ); it != path.end(); ++it) {
// 		//search for the current caption and go down to it's childs
// 		while (item && (item->text(0) != (*it)) ) {
// 			item = item->nextSibling();
// 		}
// 
// 		if (item && ((*it) == item->text(0))) {
// 			if (index == path.count()) { //last item reached
// 				setCurrentItem( item );
// 				//setSelected( item, true );
// 				break;//for loop
// 			}
// 			else {
// 				item = item->firstChild();
// 			}
// 
// 			index++;
// 		}
// 	}
}

void CMainIndex::saveSettings() {
	//save the complete names of all open groups to the settings file (e.g. Bibles/German/,Bookmarks/Jeuss Christ
	QStringList openGroups;

	QTreeWidgetItemIterator it( this );
	while ( *it ) {
		//porting: checking for a group (has children), and it's open: expanded?
		if ( ((*it)->childCount() != 0) && ((*it)->isExpanded()) ) { //orig: (*it)->isOpen()is a group and open
			//it.current()'s full name needs to be added to the list
			QTreeWidgetItem* i = (*it);
			QString fullName = i->text(0);
			while (i->parent()) {
				i = i->parent();
				fullName.prepend("/").prepend( i->text(0).replace("/", "\\/")); // parent / child
			}
			openGroups << fullName;
		}

		++it;
	}

	CBTConfig::set(CBTConfig::bookshelfOpenGroups, openGroups);

	//now save the position of the scrollbars
// 	CBTConfig::set(CBTConfig::bookshelfContentsX,
// 		horizontalScrollBar() ? horizontalScrollBar()->value() : 0);
// 	CBTConfig::set(CBTConfig::bookshelfContentsY,
// 		verticalScrollBar() ? verticalScrollBar()->value() : 0);

	//save the currently selected item
	QTreeWidgetItem* item = currentItem();
	QString path;
	while (item) {
		path.prepend( item->text(0) + "/" );
		item = item->parent();
	}
	CBTConfig::set(CBTConfig::bookshelfCurrentItem, path);
}


bool CMainIndex::event(QEvent* event)
{
	if (event->type() == QEvent::Polish) {
		initTree();
		readSettings();
	}
	return QTreeWidget::event(event);
}