/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "csearchresultview.h"

#include "backend/cswordversekey.h"

#include "frontend/cdragdropmgr.h"
#include "frontend/cexportmanager.h"

#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>

namespace Search {
	namespace Result {

CSearchResultView::CSearchResultView(QWidget* parent, const char* name) :
	KListView(parent, name), m_module(0) {
	initView();
	initConnections();
}

CSearchResultView::~CSearchResultView() {}

/** Initializes the view of this widget. */
void CSearchResultView::initView() {
	addColumn(i18n("Results"));
	setFullWidth(true);
	
	setSorting(-1);
	setDragEnabled(true);
	setSelectionModeExt(KListView::Extended);

	//setup the popup menu
	m_popup = new KPopupMenu(this);

	m_actions.copyMenu = new KActionMenu(i18n("Copy..."), CResMgr::searchdialog::result::foundItems::copyMenu::icon, m_popup);
	m_actions.copyMenu->setDelayed(false);
	m_actions.copy.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(copyItems()), this);
	m_actions.copyMenu->insert(m_actions.copy.result);
	m_actions.copy.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(copyItemsWithText()), this);
	m_actions.copyMenu->insert(m_actions.copy.resultWithText);
	m_actions.copyMenu->plug(m_popup);

	m_actions.saveMenu = new KActionMenu(i18n("Save..."),CResMgr::searchdialog::result::foundItems::saveMenu::icon, m_popup);
	m_actions.saveMenu->setDelayed( false );
	m_actions.save.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(saveItems()), this);
	m_actions.saveMenu->insert(m_actions.save.result);
	m_actions.save.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(saveItemsWithText()), this);
	m_actions.saveMenu->insert(m_actions.save.resultWithText);
	m_actions.saveMenu->plug(m_popup);

	m_actions.printMenu = new KActionMenu(i18n("Print..."),CResMgr::searchdialog::result::foundItems::printMenu::icon, m_popup);
	m_actions.printMenu->setDelayed(false);
	m_actions.print.result = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(printItems()), this);
	m_actions.printMenu->insert(m_actions.print.result);
	m_actions.printMenu->plug(m_popup);
}

/** No descriptions */
void CSearchResultView::initConnections() {
	//  connect(this, SIGNAL(executed(QListViewItem*)),
	//   this, SLOT(executed(QListViewItem*)));
	connect(this, SIGNAL(currentChanged(QListViewItem*)),
			this, SLOT(executed(QListViewItem*)));

	connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
			this, SLOT(showPopup(KListView*, QListViewItem*, const QPoint&)));
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

	QListViewItem* oldItem = 0;
	KListViewItem* item = 0;
	for (int index = 0; index < count; index++) {
		item = new KListViewItem(this, oldItem);
		item->setText(0, QString::fromUtf8(result.GetElement(index)->getText()));
		
		oldItem = item;
	}

	setUpdatesEnabled(true);

	setSelected(firstChild(), true);
	executed(currentItem());
}

void CSearchResultView::setupStrongsTree(CSwordModuleInfo* m, QStringList* vList) {
   clear();
   if (!m) {
      return;
   }

   m_module = m;

   if (vList->count() <= 0) {
      return;
   }

   setUpdatesEnabled(false);

   KListViewItem* oldItem = 0;
   KListViewItem* item = 0;

   for ( QStringList::Iterator it = vList->begin(); it != vList->end(); ++it ) {
      item = new KListViewItem(this, oldItem);
      item->setText(0, (*it));
	  
      oldItem = item;
   }
   
   setUpdatesEnabled(true);

   setSelected(firstChild(), true);
   executed(currentItem());
}

/** Is connected to the signal executed, which is emitted when a mew item was chosen. */
void CSearchResultView::executed(QListViewItem* item) {
	//  Q_ASSERT(item);
	//  qWarning("executed");
	emit keySelected(item->text(0));
}

/** Reimplementation to show the popup menu. */
void CSearchResultView::showPopup(KListView*, QListViewItem*, const QPoint& point) {
	m_popup->exec(point);
}

/** No descriptions */
void CSearchResultView::printItems() {
	QPtrList<QListViewItem> items = selectedItems();
	CExportManager mgr(i18n("Print search result..."), true, i18n("Printing search result"));

	QStringList list;
	for (QListViewItem* k = items.first(); k; k = items.next()) {
		list.append( k->text(0) );
	};
	mgr.printKeyList( list, module(), CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults() );
}

/** No descriptions */
void CSearchResultView::saveItems() {
	CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QPtrList<QListViewItem> items = selectedItems();
	QPtrList<CSwordKey> keys;
	for (QListViewItem* i = items.first(); i; i = items.next()) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.saveKeyList( keys, CExportManager::Text, false);

	keys.setAutoDelete(true);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::saveItemsWithText() {
	CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QPtrList<QListViewItem> items = selectedItems();
	QPtrList<CSwordKey> keys;
	for (QListViewItem* i = items.first(); i; i = items.next()) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.saveKeyList( keys, CExportManager::Text, true);

	keys.setAutoDelete(true);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::copyItems() {
	CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QPtrList<QListViewItem> items = selectedItems();
	QPtrList<CSwordKey> keys;
	for (QListViewItem* i = items.first(); i; i = items.next()) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.copyKeyList( keys, CExportManager::Text, false);

	keys.setAutoDelete(true);
	keys.clear(); //delete all the keys we created
}

/** No descriptions */
void CSearchResultView::copyItemsWithText() {
	CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));

	CSwordModuleInfo* m = module();
	CSwordKey* k = 0;
	QPtrList<QListViewItem> items = selectedItems();
	QPtrList<CSwordKey> keys;
	for (QListViewItem* i = items.first(); i; i = items.next()) {
		k = CSwordKey::createInstance( m );
		k->key(i->text(0));
		keys.append( k );
	};
	mgr.copyKeyList( keys, CExportManager::Text, true);

	keys.setAutoDelete(true);
	keys.clear(); //delete all the keys we created
}

/** Returns the module which is currently used. */
CSwordModuleInfo* const CSearchResultView::module() {
	return m_module;
}

QDragObject* CSearchResultView::dragObject() {
	//return a valid DragObject to make DnD possible!

	/*
	* First get all selected items and fill with them the dndItems list. The return the QDragObject we got from CDRagDropMgr
	*/
	CDragDropMgr::ItemList dndItems;

	QPtrList<QListViewItem> items = selectedItems();
	for (items.first(); items.current(); items.next()) {
		dndItems.append( CDragDropMgr::Item(m_module->name(), items.current()->text(0), QString::null) ); //no description
	};

	return CDragDropMgr::dragObject(dndItems, viewport());
}

	} //end of namespace Search::Result
} //end of namespace

