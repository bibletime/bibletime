/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmoduleresultview.h"
#include "cmoduleresultview.moc"

#include "backend/drivers/cswordmoduleinfo.h"

//#include "frontend/util/csortlistviewitem.h"
#include "frontend/cexportmanager.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//Qt includes
#include <QTreeWidget>
#include <QAction>
#include <QStringList>
#include <QMenu>
#include <QContextMenuEvent>

//KDE includes



namespace Search {


/********************************************
************  ModuleResultList **************
********************************************/

CModuleResultView::CModuleResultView(QWidget* parent) 
	: QTreeWidget(parent),
	strongsResults(0)
{
	initView();
	initConnections();
};

CModuleResultView::~CModuleResultView() {}
;

/** Initializes this widget. */
void CModuleResultView::initView()
{
	// see also csearchresultview.cpp
	
	setHeaderLabels( QStringList(tr("Work")) << QString(tr("Hits")) );
	//setFullWidth(true);
	
	//  setFullWidth(true);
	//TODO: sorting
	//setSorting(0, true);
	//setSorting(1, true);

	//setup the popup menu
	m_popup = new QMenu(this);

	m_actions.copyMenu = new QMenu(tr("Copy..."), m_popup);
	m_actions.copyMenu->setIcon(QIcon( CResMgr::searchdialog::result::moduleList::copyMenu::icon) );
	m_actions.copy.result = new QAction(tr("Reference only"), this);
	QObject::connect(m_actions.copy.result, SIGNAL(triggered()), this, SLOT(copyResult()) );
	m_actions.copyMenu->addAction(m_actions.copy.result);
	m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this); 
	QObject::connect(m_actions.copy.resultWithText, SIGNAL(triggered()), this, SLOT(copyResultWithText()) );
	m_actions.copyMenu->addAction(m_actions.copy.resultWithText);
	m_popup->addMenu(m_actions.copyMenu);

	m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
	m_actions.saveMenu->setIcon(QIcon( CResMgr::searchdialog::result::moduleList::saveMenu::icon) );
	m_actions.save.result = new QAction(tr("Reference only"), this);
	QObject::connect(m_actions.save.result, SIGNAL(triggered()), this, SLOT(saveResult()) );
	m_actions.saveMenu->addAction(m_actions.save.result);
	m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
	QObject::connect(m_actions.save.resultWithText, SIGNAL(triggered()), this, SLOT(saveResultWithText()) );
	m_actions.saveMenu->addAction(m_actions.save.resultWithText);
	m_popup->addMenu(m_actions.saveMenu);

	m_actions.printMenu = new QMenu(tr("Print..."), m_popup); 
	m_actions.printMenu->setIcon(QIcon(CResMgr::searchdialog::result::moduleList::printMenu::icon));
	m_actions.print.result = new QAction(tr("Reference with text"), this);
	QObject::connect(m_actions.print.result, SIGNAL(triggered()), this, SLOT(printResult()) );
	m_actions.printMenu->addAction(m_actions.print.result);
	m_popup->addMenu(m_actions.printMenu);
}

/** Initializes the connections of this widget, */
void CModuleResultView::initConnections()
{
	//TODO:
	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
			this, SLOT(executed(QTreeWidgetItem*, QTreeWidgetItem*)));
}

/** Setups the tree using the given list of modules. */
void CModuleResultView::setupTree( ListCSwordModuleInfo modules, const QString& searchedText )
{
	clear();
	//TODO: this class is for sorting
	//util::CSortListViewItem* item = 0;
	//util::CSortListViewItem* oldItem = 0;
	QTreeWidgetItem* item = 0;
	QTreeWidgetItem* oldItem = 0;

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

		item = new QTreeWidgetItem(this, QStringList((*it)->name()) << QString::number(result.Count()) );
		//TODO: item->setColumnSorting(1, util::CSortListViewItem::Number);

		item->setIcon(0,CToolClass::getIconForModule(*it) );
		oldItem = item;
		//----------------------------------------------------------------------
		// we need to make a decision here.  Either don't show any Strong's
		// number translations, or show the first one in the search text, or
		// figure out how to show them all.
		// I choose option number 2 at this time.
		//----------------------------------------------------------------------
		int sstIndex, sTokenIndex; // strong search text index for finding "strong:"
		if ((sstIndex = searchedText.indexOf("strong:", 0)) != -1) {
			QString sNumber;
			//--------------------------------------------------
			// get the strongs number from the search text
			//--------------------------------------------------
			// first find the first space after "strong:"
			//    this should indicate a change in search token
			sstIndex = sstIndex + 7;
			sTokenIndex = searchedText.indexOf(" ", sstIndex);
			sNumber = searchedText.mid(sstIndex, sTokenIndex - sstIndex);

			setupStrongsResults((*it), item, sNumber);

			//TODO: item->setOpen(true);
			strongsAvailable = true;
		}
	};

	//Allow to hide the module strongs if there are any available
	setRootIsDecorated( strongsAvailable );
	
	//TODO: setSelected(currentItem(), true);
	//TODO: executed(currentItem());
}

void CModuleResultView::setupStrongsResults(CSwordModuleInfo* module, QTreeWidgetItem* parent,
                                            const QString& sNumber)
{
	QString lText;
	//TODO: 
	//util::CSortListViewItem* item = 0;
	QTreeWidgetItem* item = 0;
	
	strongsResults = new StrongsResultClass(module, sNumber);
	
	for (int cnt = 0; cnt < strongsResults->Count(); ++cnt) {
		lText = strongsResults->keyText(cnt);
		
		item = new QTreeWidgetItem(parent, QStringList(lText) << QString::number(strongsResults->keyCount(cnt)));
		//TODO:
		//item->setColumnSorting(1, util::CSortListViewItem::Number);
	}
}

//TODO:
/** Is executed when an item was selected in the list. */
void CModuleResultView::executed( QTreeWidgetItem* i, QTreeWidgetItem*)
{
    QString itemText, lText;
 
 	if (!i){
		//Clear list
		emit moduleChanged();
		return;
	}
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
CSwordModuleInfo* const CModuleResultView::activeModule()
{
	Q_ASSERT(currentItem());

	QTreeWidgetItem* item = currentItem();
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

/** Reimplementation from QWidget. */
void CModuleResultView::contextMenuEvent( QContextMenuEvent * event )
{
	qDebug("CModuleResultView::showPopup");
	//make sure that all entries have the correct status
	m_popup->exec(event->globalPos());
}

/** Copies the whole search result into the clipboard. */
void CModuleResultView::copyResult()
{
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(tr("Copy search result..."), true, tr("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Copies the whole search result with the text into the clipboard. */
void CModuleResultView::copyResultWithText()
{
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(tr("Copy search result..."), true, tr("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Saves the search result keys. */
void CModuleResultView::saveResult()
{
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(tr("Save search result..."), true, tr("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Saves the search result with it's text. */
void CModuleResultView::saveResultWithText()
{
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(tr("Save search result..."), true, tr("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Appends the whole search result to the printer queue. */
void CModuleResultView::printResult()
{
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(tr("Print search result..."), true, tr("Printing search result"));
		mgr.printKeyList(&result,m,CBTConfig::getDisplayOptionDefaults(), CBTConfig::getFilterOptionDefaults());
	};
}



} //end of namespace Search
