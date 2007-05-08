/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmoduleresultview.h"

#include "backend/cswordmoduleinfo.h"

#include "frontend/util/csortlistviewitem.h"
#include "frontend/cexportmanager.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//Qt includes

//KDE includes
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>


namespace Search {
	namespace Result {

/********************************************
************  ModuleResultList **************
********************************************/

CModuleResultView::CModuleResultView(QWidget* parent, const char* name) :
	KListView(parent, name) {
	initView();
	initConnections();
   strongsResults = 0;
};

CModuleResultView::~CModuleResultView() {}
;

/** Initializes this widget. */
void CModuleResultView::initView() {
	addColumn(i18n("Work"));
	addColumn(i18n("Hits"));
	setFullWidth(true);
	
	//  setFullWidth(true);
	setSorting(0, true);
	setSorting(1, true);
	setAllColumnsShowFocus(true);


	//setup the popup menu
	m_popup = new KPopupMenu(this);
	// m_popup->insertTitle(i18n("Bible window"));

	m_actions.copyMenu = new KActionMenu(i18n("Copy..."), CResMgr::searchdialog::result::moduleList::copyMenu::icon, m_popup);
	m_actions.copyMenu->setDelayed(false);
	m_actions.copy.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(copyResult()), this);
	m_actions.copyMenu->insert(m_actions.copy.result);
	m_actions.copy.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(copyResultWithText()), this);
	m_actions.copyMenu->insert(m_actions.copy.resultWithText);
	m_actions.copyMenu->plug(m_popup);

	m_actions.saveMenu = new KActionMenu(i18n("Save..."), CResMgr::searchdialog::result::moduleList::saveMenu::icon, m_popup);
	m_actions.saveMenu->setDelayed( false );
	m_actions.save.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(saveResult()), this);
	m_actions.saveMenu->insert(m_actions.save.result);
	m_actions.save.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(saveResultWithText()), this);
	m_actions.saveMenu->insert(m_actions.save.resultWithText);
	m_actions.saveMenu->plug(m_popup);

	m_actions.printMenu = new KActionMenu(i18n("Print..."), CResMgr::searchdialog::result::moduleList::printMenu::icon, m_popup);
	m_actions.printMenu->setDelayed(false);
	m_actions.print.result = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(printResult()), this);
	m_actions.printMenu->insert(m_actions.print.result);


	m_actions.printMenu->plug(m_popup);
}

/** Initializes the connections of this widget, */
void CModuleResultView::initConnections() {
	connect(this, SIGNAL(currentChanged(QListViewItem*)),
			this, SLOT(executed(QListViewItem*)));
	connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
			this, SLOT(showPopup(KListView*, QListViewItem*, const QPoint&)));
}

/** Setups the tree using the given list of modules. */
void CModuleResultView::setupTree( ListCSwordModuleInfo modules, const QString& searchedText ) {
	clear();
	
	util::CSortListViewItem* item = 0;
	util::CSortListViewItem* oldItem = 0;
	sword::ListKey result;

	if (strongsResults) {
		delete strongsResults;
		strongsResults = 0;
		}

	bool strongsAvailable = false;

	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		//   for (modules.first(); modules.current(); modules.next()) {
		result = (*it)->searchResult();

		item = new util::CSortListViewItem(this, (*it)->name(), QString::number(result.Count()) );
		item->setColumnSorting(1, util::CSortListViewItem::Number);
				
		item->setPixmap(0,CToolClass::getIconForModule(*it) );
		oldItem = item;
		//----------------------------------------------------------------------
		// we need to make a decision here.  Either don't show any Strong's
		// number translations, or show the first one in the search text, or
		// figure out how to show them all.
		// I choose option number 2 at this time.
		//----------------------------------------------------------------------
		int sstIndex, sTokenIndex; // strong search text index for finding "strong:"
		if ((sstIndex = searchedText.find("strong:", 0)) != -1) {
			QString sNumber;
			//--------------------------------------------------
			// get the strongs number from the search text
			//--------------------------------------------------
			// first find the first space after "strong:"
			//    this should indicate a change in search token
			sstIndex = sstIndex + 7;
			sTokenIndex = searchedText.find(" ", sstIndex);
			sNumber = searchedText.mid(sstIndex, sTokenIndex - sstIndex);

			setupStrongsResults((*it), item, sNumber);

			item->setOpen(true);
			strongsAvailable = true;
		}
	};

	//Allow to hide the module strongs if there are any available
	setRootIsDecorated( strongsAvailable );
	
	setSelected(currentItem(), true);
	executed(currentItem());
}

void CModuleResultView::setupStrongsResults(CSwordModuleInfo* module, QListViewItem* parent,
                                            const QString& sNumber) {
	QString lText;
	util::CSortListViewItem* item = 0;
	
	strongsResults = new StrongsResultClass(module, sNumber);
	
	for (int cnt = 0; cnt < strongsResults->Count(); ++cnt) {
		lText = strongsResults->keyText(cnt);
		
		item = new util::CSortListViewItem(parent, lText, QString::number(strongsResults->keyCount(cnt)));
		item->setColumnSorting(1, util::CSortListViewItem::Number);
	}
}


/** Is executed when an item was selected in the list. */
void CModuleResultView::executed( QListViewItem* i ) {
   QString itemText, lText;

	if (CSwordModuleInfo* m = CPointers::backend()->findModuleByName(i->text(0))) {
		emit moduleChanged();
		emit moduleSelected(m);
      return;
	}
	
	if (!strongsResults) {
      return;
	}

   itemText = i->text(0);
   for (int cnt = 0; cnt < strongsResults->Count(); cnt++) {
      lText = strongsResults->keyText(cnt);
      if (lText == itemText) {
         //clear the verses list
         	emit moduleChanged();
         	emit strongsSelected(activeModule(), strongsResults->getKeyList(cnt));
         	return;
		}
	}
}

/** Returns the currently active module. */
CSwordModuleInfo* const CModuleResultView::activeModule() {
	Q_ASSERT(currentItem());

	QListViewItem* item = currentItem();
	if (!item) {
		return 0;
	}

	// we need to find the parent most node because that is the node
	// that is the module name.
	while (item->parent()) {
		item = item->parent();
	}

	if (item) {
		return CPointers::backend()->findModuleByName(item->text(0));
	}

	return 0;
}

/** No descriptions */
void CModuleResultView::showPopup(KListView*, QListViewItem*, const QPoint& point) {
	//make sure that all entries have the correct status
	m_popup->exec(point);
}

/** Copies the whole search result into the clipboard. */
void CModuleResultView::copyResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Copies the whole search result with the text into the clipboard. */
void CModuleResultView::copyResultWithText() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Saves the search result keys. */
void CModuleResultView::saveResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Saves the search result with it's text. */
void CModuleResultView::saveResultWithText() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Appends the whole search result to the printer queue. */
void CModuleResultView::printResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Print search result..."), true, i18n("Printing search result"));
		mgr.printKeyList(&result,m,CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults());
	};
}

	} //end of namespace Search.Result
} //end of namespace Search
