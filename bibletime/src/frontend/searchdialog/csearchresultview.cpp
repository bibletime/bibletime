/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchresultview.h"
#include "csearchresultview.moc"

#include "backend/keys/cswordversekey.h"

//#include "frontend/cdragdropmgr.h"
#include "frontend/cexportmanager.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QWidget>
#include <QMenu>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>

//KDE includes


namespace Search {


CSearchResultView::CSearchResultView(QWidget* parent)
	: QTreeWidget(parent),
	 m_module(0)
{
	initView();
	initConnections();
}

CSearchResultView::~CSearchResultView() {}

/** Initializes the view of this widget. */
void CSearchResultView::initView()
{
	setToolTip(tr("Search result of the selected work"));
	setHeaderLabel(tr("Results"));
	setDragEnabled(true);
	setRootIsDecorated( false );
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	//setup the popup menu
	m_popup = new QMenu(this);

	m_actions.copyMenu = new QMenu(tr("Copy..."), m_popup);
	m_actions.copyMenu->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::result::foundItems::copyMenu::icon));
	
	m_actions.copy.result = new QAction(tr("Reference only"), this);
	QObject::connect(m_actions.copy.result, SIGNAL(triggered()), this, SLOT(copyItems()) );
	m_actions.copyMenu->addAction(m_actions.copy.result);
	
	m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this);
	QObject::connect(m_actions.copy.resultWithText, SIGNAL(triggered()),
		this, SLOT(copyItemsWithText()));
	m_actions.copyMenu->addAction(m_actions.copy.resultWithText);

	m_popup->addMenu(m_actions.copyMenu);

	m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
	m_actions.saveMenu->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::result::foundItems::saveMenu::icon));

	m_actions.save.result = new QAction(tr("Reference only"), this);
	QObject::connect(m_actions.save.result, SIGNAL(triggered()), this, SLOT(saveItems()) );
	m_actions.saveMenu->addAction(m_actions.save.result);

	m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
	m_actions.saveMenu->addAction(m_actions.save.resultWithText);
	QObject::connect(m_actions.save.resultWithText, SIGNAL(triggered()), this, SLOT(saveItemsWithText()));
	m_popup->addMenu(m_actions.saveMenu);

	m_actions.printMenu = new QMenu(tr("Print..."), m_popup);
	m_actions.printMenu->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::searchdialog::result::foundItems::printMenu::icon));

	m_actions.print.result = new QAction(tr("Reference with text"), this);
	QObject::connect(m_actions.print.result, SIGNAL(triggered()), this, SLOT(printItems()) );
	m_actions.printMenu->addAction(m_actions.print.result);
	m_popup->addMenu(m_actions.printMenu);
}

/** No descriptions */
void CSearchResultView::initConnections() {
	//  connect(this, SIGNAL(executed(QListViewItem*)),
	//   this, SLOT(executed(QListViewItem*)));
	//TODO: are these right after porting?
	//items: current, previous
	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
			this, SLOT(executed(QTreeWidgetItem*, QTreeWidgetItem*)));
}

/** Setups the list with the given module. */
void CSearchResultView::setupTree(CSwordModuleInfo* m) {
	clear();
	
	if (!m) return;

	m_module = m;
	sword::ListKey& result = m->searchResult();
	const int count = result.Count();
	if (!count) return;

	setUpdatesEnabled(false);

	QTreeWidgetItem* oldItem = 0;
	QTreeWidgetItem* item = 0;
	for (int index = 0; index < count; index++) {
		item = new QTreeWidgetItem(this, oldItem);
		item->setText(0, QString::fromUtf8(result.GetElement(index)->getText()));
		oldItem = item;
	}

	setUpdatesEnabled(true);
	//pre-select the first item
	this->setCurrentItem(this->topLevelItem(0), 0);
}

void CSearchResultView::setupStrongsTree(CSwordModuleInfo* m, QStringList* vList)
{
	clear();
	if (!m) {
		return;
	}

	m_module = m;

	if (vList->count() <= 0) {
		return;
	}

	setUpdatesEnabled(false);

	QTreeWidgetItem* oldItem = 0;
	QTreeWidgetItem* item = 0;

	foreach (QString s, *vList) {
		item = new QTreeWidgetItem(this, oldItem);
		item->setText(0, (s));
		oldItem = item;
	}

	setUpdatesEnabled(true);

	//TODO:select the first item
	//setSelected(firstChild(), true);
	//executed(currentItem());
}

//TODO: is this still valid?
/** Is connected to the signal executed, which is emitted when a mew item was chosen. */
void CSearchResultView::executed(QTreeWidgetItem* current, QTreeWidgetItem*) {
	if (current){
		emit keySelected(current->text(0));
	}
	else{
		emit keyDeselected();
	}
}

//TODO: another function?
/** Reimplementation to show the popup menu. */
void CSearchResultView::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug("CSearchResultView::showPopup");
	m_popup->exec(event->globalPos());
}

void CSearchResultView::printItems() {
	QList<QTreeWidgetItem*> items = selectedItems();
	CExportManager mgr(tr("Print search result..."), true, tr("Printing search result"));

	QStringList list;
	foreach (QTreeWidgetItem* k, items) {
		list.append( k->text(0) );
	}
	mgr.printKeyList( list, module(), CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() );
}

void CSearchResultView::saveItems() {
	CExportManager mgr(tr("Save search result..."), true, tr("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	}
	mgr.saveKeyList( keys, CExportManager::Text, false);

	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

void CSearchResultView::saveItemsWithText() {
	CExportManager mgr(tr("Save search result..."), true, tr("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.saveKeyList( keys, CExportManager::Text, true);

	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

void CSearchResultView::copyItems() {
	CExportManager mgr(tr("Copy search result..."), true, tr("Copying search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.copyKeyList( keys, CExportManager::Text, false);

	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

void CSearchResultView::copyItemsWithText() {
	CExportManager mgr(tr("Copy search result..."), true, tr("Copying search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.copyKeyList( keys, CExportManager::Text, true);

	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

CSwordModuleInfo* const CSearchResultView::module() {
	return m_module;
}

//TODO: port this to the new d'n'd
// Q3DragObject* CSearchResultView::dragObject() {
// 	//return a valid DragObject to make DnD possible!
// 
// 	/*
// 	* First get all selected items and fill with them the dndItems list. The return the QDragObject we got from CDRagDropMgr
// 	*/
// 	CDragDropMgr::ItemList dndItems;
// 
// 	Q3PtrList<Q3ListViewItem> items = selectedItems();
// 	for (items.first(); items.current(); items.next()) {
// 		dndItems.append( CDragDropMgr::Item(m_module->name(), items.current()->text(0), QString::null) ); //no description
// 	};
// 
// 	return CDragDropMgr::dragObject(dndItems, viewport());
// }


} //end of namespace

