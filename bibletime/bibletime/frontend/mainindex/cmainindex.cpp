/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cmainindex.h"
#include "cindexitem.h"

#include "backend/creferencemanager.h"
#include "backend/cswordmoduleinfo.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/cbtconfig.h"
#include "frontend/cdragdropmgr.h"
#include "frontend/cinfodisplay.h"

#include "frontend/cprinter.h"

#include "util/cresmgr.h"
#include "util/scoped_resource.h"

//Qt includes
#include <qheader.h>
#include <qlistview.h>
#include <qdragobject.h>
#include <qinputdialog.h>
#include <qregexp.h>

//KDE includes
#include <klocale.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>

using namespace Printing;

CMainIndex::ToolTip::ToolTip(CMainIndex* parent) : QToolTip(parent->viewport()), m_mainIndex(parent) {}

void CMainIndex::ToolTip::maybeTip(const QPoint& p) {
	CItemBase* i = dynamic_cast<CItemBase*>(m_mainIndex->itemAt(p));
	Q_ASSERT(i);
	if ( !i ) {
		return;
	}

	QRect r = m_mainIndex->itemRect(i);
	if (!r.isValid()) {
		return;
	}

	//get type of item and display correct text
	const QString text = i->toolTip();
	if (!text.isEmpty()) {
		CBookmarkItem* bookmark = dynamic_cast<CBookmarkItem*>(i);
		if (bookmark) {
			(CPointers::infoDisplay())->setInfo(
				InfoDisplay::CInfoDisplay::CrossReference,
				bookmark->module()->name() + ":" + bookmark->key()
			);
		}
		else {
			CPointers::infoDisplay()->clearInfo();
		}

		tip(r, text);
	}
	else {
		CPointers::infoDisplay()->clearInfo();
	}
}

CMainIndex::CMainIndex(QWidget *parent) : KListView(parent),
m_searchDialog(0), m_toolTip(0), m_itemsMovable(false), m_autoOpenFolder(0), m_autoOpenTimer(this) {
	initView();
	initConnections();
	//initTree() is called in polish()
}

CMainIndex::~CMainIndex() {
	saveSettings();
	saveBookmarks();

	m_toolTip->remove(this);
	delete m_toolTip;
}

/** Reimplementation. Adds the given group to the tree. */
void CMainIndex::addGroup(const CItemBase::Type type, const QString language) {
	CTreeFolder *i = 0;
	switch (type) {
		case CItemBase::BookmarkFolder:
			i = new CBookmarkFolder(this);
			break;
		case CItemBase::GlossaryModuleFolder:
			//we have no second language
			i = new CGlossaryFolder(this, type, language, QString::null); 
			break;
		default:
			i = new CTreeFolder(this, type, language);
			break;
	}
	
	if (i) {
		i->init();
		if (i->childCount() == 0 && type != CItemBase::BookmarkFolder) {
			delete i;
		}
	}
}


/** Initializes the view. */
void CMainIndex::initView() {
	addColumn(QString::null);
	header()->hide();

	m_toolTip = new ToolTip(this);
	setTooltipColumn(-1);
	//to disable Qt's tooltips
	setShowToolTips(false);

	setBackgroundMode(PaletteBase);
	setFullWidth(true);
	setFocusPolicy(WheelFocus);

	setAcceptDrops( true );
	setDragEnabled( true );
	setDropVisualizer( true );
	setDropHighlighter( true );
	setAutoOpen(true);
	viewport()->setAcceptDrops(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setItemsMovable(false);
	setSelectionModeExt(Extended);

	//setup the popup menu
	m_popup = new KPopupMenu(viewport());
	m_popup->insertTitle(i18n("Bookshelf"));

	m_actions.newFolder = new KAction(i18n("Create a new folder"), CResMgr::mainIndex::newFolder::icon, 0, this, SLOT(createNewFolder()), this);
	m_actions.changeFolder = new KAction(i18n("Change this folder"),CResMgr::mainIndex::changeFolder::icon, 0, this, SLOT(changeFolder()), this);

	m_actions.changeBookmark = new KAction(i18n("Change bookmark description"),CResMgr::mainIndex::changeBookmark::icon, 0, this, SLOT(changeBookmark()), this);
	m_actions.importBookmarks = new KAction(i18n("Import bookmarks"),CResMgr::mainIndex::importBookmarks::icon, 0, this, SLOT(importBookmarks()), this);
	m_actions.exportBookmarks = new KAction(i18n("Export bookmarks"),CResMgr::mainIndex::exportBookmarks::icon, 0, this, SLOT(exportBookmarks()), this);
	m_actions.printBookmarks = new KAction(i18n("Print bookmarks"),CResMgr::mainIndex::printBookmarks::icon, 0, this, SLOT(printBookmarks()), this);

	m_actions.deleteEntries = new KAction(i18n("Remove selected item(s)"),CResMgr::mainIndex::deleteItems::icon, 0, this, SLOT(deleteEntries()), this);

	m_actions.editModuleMenu = new KActionMenu(i18n("Edit this work"),CResMgr::mainIndex::editModuleMenu::icon, this);
	m_actions.editModuleMenu->setDelayed(false);
	m_actions.editModulePlain = new KAction(i18n("Plain text"),CResMgr::mainIndex::editModulePlain::icon, 0, this, SLOT(editModulePlain()), this);
	m_actions.editModuleHTML = new KAction(i18n("HTML"),CResMgr::mainIndex::editModuleHTML::icon, 0, this, SLOT(editModuleHTML()), this);
	
	m_actions.searchInModules = new KAction(i18n("Search in selected work(s)"),CResMgr::mainIndex::search::icon, 0, this, SLOT(searchInModules()), this);
	m_actions.unlockModule = new KAction(i18n("Unlock this work"),CResMgr::mainIndex::unlockModule::icon, 0, this, SLOT(unlockModule()), this);
	m_actions.aboutModule = new KAction(i18n("About this work"),CResMgr::mainIndex::aboutModule::icon, 0, this, SLOT(aboutModule()), this);


	//fill the popup menu itself
	m_actions.newFolder->plug(m_popup);
	m_actions.changeFolder->plug(m_popup);
	(new KActionSeparator(this))->plug(m_popup);
	m_actions.changeBookmark->plug(m_popup);
	m_actions.importBookmarks->plug(m_popup);
	m_actions.exportBookmarks->plug(m_popup);
	m_actions.printBookmarks->plug(m_popup);
	(new KActionSeparator(this))->plug(m_popup);
	m_actions.deleteEntries->plug(m_popup);
	(new KActionSeparator(this))->plug(m_popup);
	m_actions.editModuleMenu->plug(m_popup);
	//sub item of edit module menu
	m_actions.editModuleMenu->insert(m_actions.editModulePlain); 
	//sub item of edit module menu
	m_actions.editModuleMenu->insert(m_actions.editModuleHTML);  

	m_actions.searchInModules->plug(m_popup);
	m_actions.unlockModule->plug(m_popup);
	m_actions.aboutModule->plug(m_popup);
}

/** Initialize the SIGNAL<->SLOT connections */
void CMainIndex::initConnections() {
	connect(this, SIGNAL(executed(QListViewItem*)),
			SLOT(slotExecuted(QListViewItem*)));
	connect(this, SIGNAL(dropped(QDropEvent*, QListViewItem*, QListViewItem*)),
			SLOT(dropped(QDropEvent*, QListViewItem*, QListViewItem*)));
	connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
			SLOT(contextMenu(KListView*, QListViewItem*, const QPoint&)));
	connect(&m_autoOpenTimer, SIGNAL(timeout()),
			this, SLOT(autoOpenTimeout()));
}

/** Is called when an item was clicked/double clicked. */
void CMainIndex::slotExecuted( QListViewItem* i ) {
	CItemBase* ci = dynamic_cast<CItemBase*>(i);
	if (!ci) {
		return;
	}

	if (ci->isFolder()) {
		i->setOpen(!i->isOpen());
	}
	else if (CModuleItem* m = dynamic_cast<CModuleItem*>(i))  { //clicked on a module
		CSwordModuleInfo* mod = m->module();
		ListCSwordModuleInfo modules;
		modules.append(mod);
		emit createReadDisplayWindow(modules, QString::null);
	}
	else if (CBookmarkItem* b = dynamic_cast<CBookmarkItem*>(i) ) { //clicked on a bookmark
		if (CSwordModuleInfo* mod = b->module()) {
			ListCSwordModuleInfo modules;
			modules.append(mod);
			emit createReadDisplayWindow(modules, b->key());
		}
	}
}

/** Reimplementation. Returns the drag object for the current selection. */
QDragObject* CMainIndex::dragObject() {
	if (!m_itemsMovable) {
		return false;
	}

	CDragDropMgr::ItemList dndItems;

	QPtrList<QListViewItem> items = selectedItems();
	for (items.first(); items.current(); items.next()) {
		if (CItemBase* i = dynamic_cast<CItemBase*>(items.current())) {
			//we can move this item!
			if (!i->isMovable()) { //we can only drag items which allow us to do it, e.g. which are movable
				continue;
			}

			if (CBookmarkItem* bookmark = dynamic_cast<CBookmarkItem*>( items.current() )) {
				//take care of bookmarks which have no valid module any more, e.g. if it was uninstalled
				const QString moduleName = bookmark->module() ? bookmark->module()->name() : QString::null;
				dndItems.append( CDragDropMgr::Item(moduleName, bookmark->key(), bookmark->description()) );
			}
		}
	}

	return CDragDropMgr::dragObject( dndItems, viewport() );
}

/** Reimplementation from KListView. Returns true if the drag is acceptable for the listview. */
bool CMainIndex::acceptDrag( QDropEvent* event ) const {
	const QPoint pos = contentsToViewport(event->pos());

	CItemBase* i = dynamic_cast<CItemBase*>(itemAt(pos));
	return i ? (i->acceptDrop(event) || i->isMovable()) : false;
}

/** No descriptions */
void CMainIndex::initTree() {
	addGroup(CItemBase::BookmarkFolder, QString("*"));
	addGroup(CItemBase::BibleModuleFolder, QString("*"));
	addGroup(CItemBase::BookModuleFolder, QString("*"));
	addGroup(CItemBase::CommentaryModuleFolder, QString("*"));
	addGroup(CItemBase::DevotionalModuleFolder, QString("*"));
	addGroup(CItemBase::GlossaryModuleFolder, QString("*"));
	addGroup(CItemBase::LexiconModuleFolder, QString("*"));
}

/** No descriptions */
void CMainIndex::dropped( QDropEvent* e, QListViewItem* parent, QListViewItem* after) {
	Q_ASSERT(after);
	Q_ASSERT(parent);

	//the drop was started in this main index widget
	if (m_itemsMovable && ((e->source()) == (viewport()))) {
		/*
		* If the drag was started from the main index and should move items and if the destination is the bookmark
		* folder or one of its subfolders
		* we remove the current items because the new ones will be inserted soon.
		*/
		if (dynamic_cast<CBookmarkFolder*>(parent) || dynamic_cast<Bookmarks::SubFolder*>(parent)) { 
			// we drop onto the bookmark folder or one of it's subfolders
			//       QPtrList<QListViewItem> items = selectedItems();
			//       items.setAutoDelete(true);
			//       items.clear(); //delete the selected items we dragged
		}
	}

	//finally do the drop, either with external drop data or with the moved items' data
	CItemBase* const parentItem = dynamic_cast<CItemBase*>(parent);
	CItemBase* const afterItem  = dynamic_cast<CItemBase*>(after);

	bool removeSelectedItems = true;
	bool moveSelectedItems = false;

	if (afterItem && afterItem->isFolder()) {
		moveSelectedItems = false;
		removeSelectedItems = false; //why TRUE?

		afterItem->setOpen(true);
		afterItem->dropped(e); //inserts new items, moving only works on the same level
	}
	else if (afterItem && !afterItem->isFolder() && parentItem) {
		const bool justMoveSelected =
			(e->source() == viewport())
			&& m_itemsMovable
			&& parentItem->acceptDrop(e)
			&& !afterItem->acceptDrop(e);

		if (justMoveSelected) {
			moveSelectedItems = true;
			removeSelectedItems = false;
		}
		else {
			moveSelectedItems = false;
			removeSelectedItems = false;

			if (afterItem->acceptDrop(e)) {
				afterItem->dropped(e, after);
			}
			else { //insert in the parent folder and then move the inserted items
				parentItem->dropped(e, after);
			}
		}

		parentItem->setOpen(true);
	}
	else if (parentItem) { //no after item present, but a parent is there
		moveSelectedItems = false;
		removeSelectedItems = false;

		parentItem->setOpen(true);
		parentItem->dropped(e);
	}

	if (moveSelectedItems) {
		//move all selected items after the afterItem
		if (m_itemsMovable) {
			QPtrList<QListViewItem> items = selectedItems();
			QListViewItem* i = items.first();
			QListViewItem* after = afterItem;
			while (i && afterItem) {
				i->moveItem(after);
				after = i;

				i = items.next();
			}
		}
	}

	if (removeSelectedItems) {
		QPtrList<QListViewItem> items = selectedItems();
		items.setAutoDelete(true);
		items.clear(); //delete the selected items we dragged
	}
}

/** No descriptions */
void CMainIndex::emitModulesChosen( ListCSwordModuleInfo modules, QString key ) {
	emit createReadDisplayWindow(modules, key);
}

/** Returns the correct KAction object for the given type of action. */
KAction* const CMainIndex::action( const CItemBase::MenuAction type ) const {
	switch (type) {
	case CItemBase::NewFolder:
		return m_actions.newFolder;
	case CItemBase::ChangeFolder:
		return m_actions.changeFolder;

	case CItemBase::ChangeBookmark:
		return m_actions.changeBookmark;
	case CItemBase::ImportBookmarks:
		return m_actions.importBookmarks;
	case CItemBase::ExportBookmarks:
		return m_actions.exportBookmarks;
	case CItemBase::PrintBookmarks:
		return m_actions.printBookmarks;

	case CItemBase::DeleteEntries:
		return m_actions.deleteEntries;

	case CItemBase::EditModule:
		return m_actions.editModuleMenu;
	case CItemBase::SearchInModules:
		return m_actions.searchInModules;
	case CItemBase::UnlockModule:
		return m_actions.unlockModule;
	case CItemBase::AboutModule:
		return m_actions.aboutModule;
	default:
		return 0;
	}
}

/** Shows the context menu at the given position. */
void CMainIndex::contextMenu(KListView* /*list*/, QListViewItem* i, const QPoint& p) {
	//setup menu entries depending on current selection
	QPtrList<QListViewItem> items = selectedItems();

	if (items.count() == 0) { 
		//special handling for no selection
	}
	else if (items.count() == 1) { 
		//special handling for one selected item
		
		CItemBase* item = dynamic_cast<CItemBase*>(i);
		CItemBase::MenuAction actionType;
		for (int index = CItemBase::ActionBegin; index <= CItemBase::ActionEnd; ++index) {
			actionType = static_cast<CItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled( item->enableAction(actionType) );
		}
	}
	else {
		//first disable all actions
		CItemBase::MenuAction actionType;
		for (int index = CItemBase::ActionBegin; index <= CItemBase::ActionEnd; ++index) {
			actionType = static_cast<CItemBase::MenuAction>(index);
			if (KAction* a = action(actionType))
				a->setEnabled(false);
		}

		//enable the menu items depending on the types of the selected items.
		for (int index = CItemBase::ActionBegin; index <= CItemBase::ActionEnd; ++index) {
			actionType = static_cast<CItemBase::MenuAction>(index);
			bool enableAction = isMultiAction(actionType);
			for (items.first(); items.current(); items.next()) {
				CItemBase* i = dynamic_cast<CItemBase*>(items.current());
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
	m_popup->exec(p);
}

/** Adds a new subfolder to the current item. */
void CMainIndex::createNewFolder() {
	CFolderBase* i = dynamic_cast<CFolderBase*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->newSubFolder();
	}
}

/** Opens a dialog to change the current folder. */
void CMainIndex::changeFolder() {
	CFolderBase* i = dynamic_cast<CFolderBase*>(currentItem());
	Q_ASSERT(i);
	if (i) {
		i->rename();
	}
}

/** Changes the current bookmark. */
void CMainIndex::changeBookmark() {
	CBookmarkItem* i = dynamic_cast<CBookmarkItem*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->rename();
	}
}

/** Exports the bookmarks being in the selected folder. */
void CMainIndex::exportBookmarks() {
	CBookmarkFolder* i = dynamic_cast<CBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->exportBookmarks();
	}
}

/** Import bookmarks from a file and add them to the selected folder. */
void CMainIndex::importBookmarks() {
	CBookmarkFolder* i = dynamic_cast<CBookmarkFolder*>(currentItem());
	Q_ASSERT(i);
	
	if (i) {
		i->importBookmarks();
	}
}

/** Prints the selected bookmarks. */
void CMainIndex::printBookmarks() {
	CPrinter::KeyTree tree;
	CPrinter::KeyTreeItem::Settings settings;
	settings.keyRenderingFace = CPrinter::KeyTreeItem::Settings::CompleteShort;

	QPtrList<QListViewItem> items;
	CBookmarkFolder* bf = dynamic_cast<CBookmarkFolder*>(currentItem());

	if (bf) {
		items = bf->getChildList();
	}
	else {
		items = selectedItems();
	}

	//create a tree of keytreeitems using the bookmark hierarchy.
	for (items.first(); items.current(); items.next()) {
		CBookmarkItem* i = dynamic_cast<CBookmarkItem*>(items.current());
		if (i) {
			tree.append( new CPrinter::KeyTreeItem( i->key(), i->module(), settings ) );
		}
	}

	util::scoped_ptr<CPrinter> printer( 
		new CPrinter( this, CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() ) 
	);
	printer->printKeyTree(tree);
}

/** Deletes the selected entries. */
void CMainIndex::deleteEntries() {
	QPtrList<QListViewItem> items = selectedItems();
	if (!items.count())
		return;

	if (KMessageBox::warningYesNo(this, i18n("Do you really want to delete the selected items and child-items?"), i18n("Delete Items")) != KMessageBox::Yes) {
		return;
	}

	// We have to go backwards because otherwise deleting folders would delete their childs => crash before we delete those
	for (items.last(); items.current(); items.prev()) {
		if (CItemBase* i = dynamic_cast<CItemBase*>(items.current())) {
			if (i->enableAction(CItemBase::DeleteEntries)) {
				delete i;
			}
		}
	}
}

/** Opens the searchdialog for the selected modules. */
void CMainIndex::searchInModules() {
	QPtrList<QListViewItem> items = selectedItems();
	ListCSwordModuleInfo modules;
	for (items.first(); items.current(); items.next()) {
		if (CModuleItem* i = dynamic_cast<CModuleItem*>(items.current())) {
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
	if (CModuleItem* i = dynamic_cast<CModuleItem*>(currentItem())) {
		bool ok = false;
		const QString unlockKey = QInputDialog::getText(i18n("BibleTime - Unlock work"),
			i18n("Enter the unlock key for this work."),
			QLineEdit::Normal, i->module()->config(CSwordModuleInfo::CipherKey), &ok);
		
		if (ok) {
			i->module()->unlock( unlockKey );
			emit signalSwordSetupChanged();
		}
	}
}

/** Shows information about the current module. */
void CMainIndex::aboutModule() {
	if (CModuleItem* i = dynamic_cast<CModuleItem*>(currentItem())) {
		KMessageBox::about(this, i->module()->aboutText(), i->module()->config(CSwordModuleInfo::Description), false);
	}
}

/** Reimplementation. Takes care of movable items. */
void CMainIndex::startDrag() {
	QPtrList<QListViewItem> items = selectedItems();
	m_itemsMovable = true;

	for (items.first(); items.current() && m_itemsMovable; items.next()) {
		if (CItemBase* i = dynamic_cast<CItemBase*>(items.current())) {
			m_itemsMovable = (m_itemsMovable && i->isMovable());
		}
		else {
			m_itemsMovable = false;
		}
	}

	KListView::startDrag();
}

/** Reimplementation to support the items dragEnter and dragLeave functions. */
void CMainIndex::contentsDragMoveEvent( QDragMoveEvent* event ) {
	//  qWarning("void CMainIndex:: drag move event ( QDragLeaveEvent* e )");
	CItemBase* i = dynamic_cast<CItemBase*>( itemAt( contentsToViewport(event->pos())) );
	if (i) {
		if (i->allowAutoOpen(event) || (i->acceptDrop(event) && i->isFolder() && i->allowAutoOpen(event) && !i->isOpen() && autoOpen()) ) {
			if (m_autoOpenFolder != i)  {
				m_autoOpenTimer.stop();
			}
			
			m_autoOpenFolder = i;
			m_autoOpenTimer.start( 400, true );
		}
		else {
			m_autoOpenFolder = 0;
		}
	}
	else {
		m_autoOpenFolder = 0;
	}

	KListView::contentsDragMoveEvent(event);
}

QRect CMainIndex::drawItemHighlighter(QPainter* painter, QListViewItem* item) {
	CBookmarkItem* bookmark = dynamic_cast<CBookmarkItem*>(item);
	if (bookmark) { 
		//no drops on bookmarks allowed, just moving items after it
		return QRect();
	}

	return KListView::drawItemHighlighter(painter, item);
}


void CMainIndex::autoOpenTimeout() {
	m_autoOpenTimer.stop();
	if (m_autoOpenFolder && !m_autoOpenFolder->isOpen() && m_autoOpenFolder->childCount()) {
		m_autoOpenFolder->setOpen(true);
	}
}

/** No descriptions */
void CMainIndex::contentsDragLeaveEvent( QDragLeaveEvent* e ) {
	m_autoOpenTimer.stop();
	KListView::contentsDragLeaveEvent(e);
}

/** Returns true if more than one netry is supported by this action type. Returns false for actions which support only one entry, e.g. about module etc. */
const bool CMainIndex::isMultiAction( const CItemBase::MenuAction type ) const {
	switch (type) {
	case CItemBase::NewFolder:
		return false;
	case CItemBase::ChangeFolder:
		return false;

	case CItemBase::ChangeBookmark:
		return false;
	case CItemBase::ImportBookmarks:
		return false;
	case CItemBase::ExportBookmarks:
		return false;
	case CItemBase::PrintBookmarks:
		return true;

	case CItemBase::DeleteEntries:
		return true;

	case CItemBase::EditModule:
		return false;
	case CItemBase::SearchInModules:
		return true;
	case CItemBase::UnlockModule:
		return false;
	case CItemBase::AboutModule:
		return false;
	}
	
	return false;
}

/** Is called when items should be moved. */
void CMainIndex::moved( QPtrList<QListViewItem>& /*items*/, QPtrList<QListViewItem>& /*afterFirst*/, QPtrList<QListViewItem>& /*afterNow*/) {
	qDebug("move items");
}

/** Opens an editor window to edit the modules content. */
void CMainIndex::editModulePlain() {
	QPtrList<QListViewItem> items = selectedItems();
	ListCSwordModuleInfo modules;
	for (items.first(); items.current(); items.next()) {
		if (CModuleItem* i = dynamic_cast<CModuleItem*>(items.current())) {
			modules.append(i->module());
		}
	}
	if (modules.count() == 1) {
		emit createWriteDisplayWindow(modules.first(), QString::null, CDisplayWindow::PlainTextWindow);
	};
}

/** Opens an editor window to edit the modules content. */
void CMainIndex::editModuleHTML() {
	QPtrList<QListViewItem> items = selectedItems();
	ListCSwordModuleInfo modules;
	for (items.first(); items.current(); items.next()) {
		if (CModuleItem* i = dynamic_cast<CModuleItem*>(items.current())) {
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
	CItemBase* i = 0;

	QListViewItemIterator it( this );
	while ( it.current() ) {
		i = dynamic_cast<CItemBase*>( it.current() );

		if (i && (i->type() == CItemBase::BookmarkFolder)) { 
			//found the bookmark folder
			KStandardDirs stdDirs;

			const QString path = stdDirs.saveLocation("data", "bibletime/");
			if (!path.isEmpty()) {
				//save the bookmarks to the right file
				if (CBookmarkFolder* f = dynamic_cast<CBookmarkFolder*>(i)) {
					f->saveBookmarks( path + "bookmarks.xml" );
				}
			}
			break;
		}

		++it;
	}
}

void CMainIndex::readSettings() {
 	qDebug("CMainIndex::readSettings");
	
	QStringList openGroups = CBTConfig::get(CBTConfig::bookshelfOpenGroups);
	for (QStringList::Iterator it( openGroups.begin() ); it != openGroups.end(); ++it) {
		QStringList path = QStringList::split("/", (*it)); //e.g. with items parent, child
		QListViewItem* item = firstChild(); //begin on the top for each item
		Q_ASSERT(item);
		unsigned int index = 1;

		for (QStringList::Iterator p_it( path.begin() ); p_it != path.end(); ++p_it) {
			QString itemName = (*p_it).replace("\\/", "/");

			while (item && (item->text(0) != itemName)) {
				item = item->nextSibling();
			}

			if (item && (item->text(0) == itemName)) {
				if (index < path.count()) { //don't call firstChild on the right, i.e. last item of the list
					item = item->firstChild();
				}

				++index;
			}
		}

		if (item) {
			item->setOpen(true);
		}
	}

	//restore the content position
// 	setContentsPos(
// 		CBTConfig::get(CBTConfig::bookshelfContentsX),
// 		CBTConfig::get(CBTConfig::bookshelfContentsY)
// 	);
// 	horizontalScrollBar()->setValue(CBTConfig::get(CBTConfig::bookshelfContentsX));
// 	verticalScrollBar()->setValue(CBTConfig::get(CBTConfig::bookshelfContentsY));


	//restore the selected item
	QStringList path = QStringList::split("/", CBTConfig::get(CBTConfig::bookshelfCurrentItem));
	QListViewItem* item = firstChild();
	Q_ASSERT(item);
	unsigned int index = 1;
	for (QStringList::iterator it( path.begin() ); it != path.end(); ++it) {
		//search for the current caption and go down to it's childs
		while (item && (item->text(0) != (*it)) ) {
			item = item->nextSibling();
		}

		if (item && ((*it) == item->text(0))) {
			if (index == path.count()) { //last item reached
				setCurrentItem( item );
				setSelected( item, true );
				break;//for loop
			}
			else {
				item = item->firstChild();
			}

			index++;
		}
	}
}

void CMainIndex::saveSettings() {
	//save the complete names of all open groups to the settings file (e.g. Bibles/German/,Bookmarks/Jeuss Christ
	QStringList openGroups;

	QListViewItemIterator it( this );
	while ( it.current() ) {
		if ( it.current()->isOpen() ) { //is a group and open
			//it.current()'s full name needs to be added to the list
			QListViewItem* i = it.current();
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
	QListViewItem* item = currentItem();
	QString path;
	while (item) {
		path.prepend( item->text(0) + "/" );
		item = item->parent();
	}
	CBTConfig::set(CBTConfig::bookshelfCurrentItem, path);
}

void CMainIndex::polish()
{
	KListView::polish();
	initTree();
	readSettings();
}
