/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "csearchresultview.h"
#include "csearchresultview.moc"

#include "backend/keys/cswordversekey.h"

#include "frontend/cdragdropmgr.h"
#include "frontend/cexportmanager.h"

#include "util/cresmgr.h"

//Qt includes
#include <QWidget>
#include <QMenu>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

//KDE includes
#include <klocale.h>
//#include <kaction.h>
//#include <kactionmenu.h>
//#include <kpopupmenu.h>


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
	//addColumn(i18n("Results"));
	setHeaderLabel(i18n("Results"));
	//TODO: if needed: setFullWidth(true);
	
	//TODO: if sorting is wrong: setSorting(-1);
	setDragEnabled(true);
	//setSelectionModeExt(KListView::Extended);

	//setup the popup menu
	m_popup = new QMenu(this);

	//TODO: convert to QMenu?
	m_actions.copyMenu = new QMenu(i18n("Copy..."), m_popup);
	// CResMgr::searchdialog::result::foundItems::copyMenu::icon, m_popup);
	m_actions.copyMenu->setIcon(QIcon(CResMgr::searchdialog::result::foundItems::copyMenu::icon));
	//m_actions.copyMenu->setDelayed(false);
	
	m_actions.copy.result = new QAction(i18n("Reference only"), this);
	QObject::connect(m_actions.copy.result, SIGNAL(triggered()), this, SLOT(copyItems()) );
	m_actions.copyMenu->addAction(m_actions.copy.result);
	
	m_actions.copy.resultWithText = new QAction(i18n("Reference with text"), this);
	QObject::connect(m_actions.copy.resultWithText, SIGNAL(triggered()),
		this, SLOT(copyItemsWithText()));
	m_actions.copyMenu->addAction(m_actions.copy.resultWithText);
	//m_actions.copyMenu->plug(m_popup);
	m_popup->addMenu(m_actions.copyMenu);

	m_actions.saveMenu = new QMenu(i18n("Save..."), m_popup);
	m_actions.saveMenu->setIcon(QIcon(CResMgr::searchdialog::result::foundItems::saveMenu::icon));
	//m_actions.saveMenu->setDelayed( false );

	m_actions.save.result = new QAction(i18n("Reference only"), this);
	QObject::connect(m_actions.save.result, SIGNAL(triggered()), this, SLOT(saveItems()) );
	m_actions.saveMenu->addAction(m_actions.save.result);
	

	m_actions.save.resultWithText = new QAction(i18n("Reference with text"), this);
	//KShortcut(0), this, SLOT(saveItemsWithText()), this);
	m_actions.saveMenu->addAction(m_actions.save.resultWithText);
	QObject::connect(m_actions.save.resultWithText, SIGNAL(triggered()), this, SLOT(saveItemsWithText()));
	//m_actions.saveMenu->plug(m_popup);
	m_popup->addMenu(m_actions.saveMenu);

	m_actions.printMenu = new QMenu(i18n("Print..."), m_popup);
	//CResMgr::searchdialog::result::foundItems::printMenu::icon, m_popup);
	m_actions.printMenu->setIcon(QIcon(CResMgr::searchdialog::result::foundItems::printMenu::icon));
	//m_actions.printMenu->setDelayed(false);

	m_actions.print.result = new QAction(i18n("Reference with text"), this);
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

	//connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	//		this, SLOT(showPopup(const QPoint&)));
}

/** Setups the list with the given module. */
void CSearchResultView::setupTree(CSwordModuleInfo* m) {
	clear();
	
	if (!m) {
		return;
	}

	m_module = m;

	sword::ListKey& result = m->searchResult();
	const int count = result.Count();
	if (!count) {
		return;
	}

	setUpdatesEnabled(false);

	QTreeWidgetItem* oldItem = 0;
	QTreeWidgetItem* item = 0;
	for (int index = 0; index < count; index++) {
		item = new QTreeWidgetItem(this, oldItem);
		item->setText(0, QString::fromUtf8(result.GetElement(index)->getText()));
		oldItem = item;
	}

	setUpdatesEnabled(true);

	//TODO: select the first item
	//setSelected(firstChild(), true);
	//executed(currentItem());
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

	//for ( QStringList::Iterator it = vList->begin(); it != vList->end(); ++it ) {
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
	//  Q_ASSERT(item);
	//  qWarning("executed");
	emit keySelected(current->text(0));
}

//TODO: another function?
/** Reimplementation to show the popup menu. */
void CSearchResultView::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug("CSearchResultView::showPopup");
	m_popup->exec(event->globalPos());
}

/** No descriptions */
void CSearchResultView::printItems() {
	QList<QTreeWidgetItem*> items = selectedItems();
	CExportManager mgr(i18n("Print search result..."), true, i18n("Printing search result"));

	QStringList list;
	//for (Q3ListViewItem* k = items.first(); k; k = items.next()) {
	foreach (QTreeWidgetItem* k, items) {
		list.append( k->text(0) );
	};
	mgr.printKeyList( list, module(), CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() );
}

/** No descriptions */
void CSearchResultView::saveItems() {
	CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	//for (Q3ListViewItem* i = items.first(); i; i = items.next()) {
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.saveKeyList( keys, CExportManager::Text, false);

	//keys.setAutoDelete(true);
	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::saveItemsWithText() {
	CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	//for (QTreeWidgetItem* i = items.first(); i; i = items.next()) {
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.saveKeyList( keys, CExportManager::Text, true);

	//keys.setAutoDelete(true);
	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::copyItems() {
	CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QList<QTreeWidgetItem*> items = selectedItems();
	QList<CSwordKey*> keys;
	//for (QTreeWidgetItem* i = items.first(); i; i = items.next()) {
	foreach (QTreeWidgetItem* i, items) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.copyKeyList( keys, CExportManager::Text, false);

	//keys.setAutoDelete(true);
	qDeleteAll(keys);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::copyItemsWithText() {
	CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));

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

/** Returns the module which is currently used. */
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

