/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cmoduleresultview.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QStringList>
#include <QTreeWidget>
#include <QtAlgorithms>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bibletimeapp.h"
#include "../cexportmanager.h"


namespace Search {

/********************************************
************  ModuleResultList **************
********************************************/

CModuleResultView::CModuleResultView(QWidget* parent)
        : QTreeWidget(parent) {
    initView();
    initConnections();
}

CModuleResultView::~CModuleResultView() {
    qDeleteAll(m_strongsResults);
}


/** Initializes this widget. */
void CModuleResultView::initView() {
    // see also csearchresultview.cpp
    setToolTip(tr("Works chosen for the search and the number of the hits in each work"));
    setHeaderLabels( QStringList(tr("Work")) << tr("Hits") );

    setColumnWidth(0, util::tool::mWidth(this, 8));
    setColumnWidth(1, util::tool::mWidth(this, 4));
    QSize sz(util::tool::mWidth(this, 13), util::tool::mWidth(this, 5));
    //setMinimumSize(sz);
    m_size = sz;
    /// \todo sorting
    //setSorting(0, true);
    //setSorting(1, true);

    //setup the popup menu
    m_popup = new QMenu(this);

    m_actions.copyMenu = new QMenu(tr("Copy..."), m_popup);
    m_actions.copyMenu->setIcon(CResMgr::searchdialog::result::moduleList::copyMenu::icon());
    m_actions.copy.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.copy.result, SIGNAL(triggered()),
               this,                  SLOT(copyResult()));
    m_actions.copyMenu->addAction(m_actions.copy.result);
    m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.copy.resultWithText, SIGNAL(triggered()),
               this,                           SLOT(copyResultWithText()));
    m_actions.copyMenu->addAction(m_actions.copy.resultWithText);
    m_popup->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
    m_actions.saveMenu->setIcon(CResMgr::searchdialog::result::moduleList::saveMenu::icon());
    m_actions.save.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.save.result, SIGNAL(triggered()),
               this,                  SLOT(saveResult()));
    m_actions.saveMenu->addAction(m_actions.save.result);
    m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.save.resultWithText, SIGNAL(triggered()),
               this,                          SLOT(saveResultWithText()));
    m_actions.saveMenu->addAction(m_actions.save.resultWithText);
    m_popup->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), m_popup);
    m_actions.printMenu->setIcon(CResMgr::searchdialog::result::moduleList::printMenu::icon());
    m_actions.print.result = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.print.result, SIGNAL(triggered()),
               this,                   SLOT(printResult()));
    m_actions.printMenu->addAction(m_actions.print.result);
    m_popup->addMenu(m_actions.printMenu);
}

/** Initializes the connections of this widget, */
void CModuleResultView::initConnections() {
    /// \todo
    BT_CONNECT(this,
               SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
               this,
               SLOT(executed(QTreeWidgetItem *, QTreeWidgetItem *)));
}

void CModuleResultView::setupTree(const CSwordModuleSearch::Results & results,
                                  const QString & searchedText)
{
    /// \todo implement sorting in this method.

    clear();

    m_results = results;

    qDeleteAll(m_strongsResults);
    m_strongsResults.clear();

    bool strongsAvailable = false;

    Q_FOREACH(const CSwordModuleInfo * m, results.keys()) {
        QTreeWidgetItem * const item =
                new QTreeWidgetItem(this,
                                    QStringList(m->name())
                                    << QString::number(
                                            results.value(m).getCount()));

        item->setIcon(0, util::tool::getIconForModule(m));
        /*
          We need to make a decision here.  Either don't show any Strong's
          number translations, or show the first one in the search text, or
          figure out how to show them all. I choose option number 2 at this time.
        */

        // strong search text index for finding "strong:"
        int sstIndex = searchedText.indexOf("strong:", 0);
        if (sstIndex != -1) {
            /*
              Get the strongs number from the search text. First find the first
              space after "strong:". This should indicate a change in search
              token
            */
            sstIndex += 7;
            const int sTokenIndex = searchedText.indexOf(" ", sstIndex);
            const QString sNumber(searchedText.mid(sstIndex, sTokenIndex - sstIndex));

            setupStrongsResults(m, results[m], item, sNumber);

            /// \todo item->setOpen(true);
            strongsAvailable = true;
        }
    };

    // Allow to hide the module strongs if there are any available
    setRootIsDecorated( strongsAvailable );
}

void CModuleResultView::setupStrongsResults(const CSwordModuleInfo *module,
                                            const sword::ListKey &results,
                                            QTreeWidgetItem *parent,
                                            const QString &sNumber)
{
    StrongsResultList *m = new StrongsResultList(module, results, sNumber);
    m_strongsResults[module] = m;

    for (int cnt = 0; cnt < m->count(); ++cnt) {
        QStringList columns(m->at(cnt).keyText());
        columns.append(QString::number(m->at(cnt).keyCount()));
        new QTreeWidgetItem(parent, columns);
    }
}

/// \todo
/** Is executed when an item was selected in the list. */
void CModuleResultView::executed( QTreeWidgetItem* i, QTreeWidgetItem*) {
    QString itemText;

    if (!i) {
        //Clear list
        emit moduleChanged();
        return;
    }
    if (CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(i->text(0))) {
        emit moduleChanged();
        emit moduleSelected(m, m_results.value(m));
        return;
    }

    StrongsResultList *strongsResult = m_strongsResults[activeModule()];

    if (!strongsResult) {
        return;
    }

    itemText = i->text(0);
    for (int cnt = 0; cnt < strongsResult->count(); cnt++) {
        if (strongsResult->at(cnt).keyText() == itemText) {
            //clear the verses list
            emit moduleChanged();
            emit strongsSelected(activeModule(),
                                 strongsResult->at(cnt).getKeyList());
            return;
        }
    }
}

/** Returns the currently active module. */
CSwordModuleInfo* CModuleResultView::activeModule() {
    QTreeWidgetItem * item = currentItem();
    BT_ASSERT(item);

    // we need to find the parent most node because that is the node
    // that is the module name.
    while (item->parent())
        item = item->parent();

    return CSwordBackend::instance()->findModuleByName(item->text(0));
}

/** Reimplementation from QWidget. */
void CModuleResultView::contextMenuEvent( QContextMenuEvent * event ) {
    //make sure that all entries have the correct status
    m_popup->exec(event->globalPos());
}

/** Copies the whole search result into the clipboard. */
void CModuleResultView::copyResult() {
    CSwordModuleInfo *m = activeModule();
    if (m != nullptr) {
        CExportManager mgr(true, tr("Copying search result"));

        mgr.copyKeyList(m_results[m], m, CExportManager::Text, false);
    };
}

/** Copies the whole search result with the text into the clipboard. */
void CModuleResultView::copyResultWithText() {
    CSwordModuleInfo *m = activeModule();
    if (m != nullptr) {
        CExportManager mgr(true, tr("Copying search result"));
        mgr.copyKeyList(m_results[m], m, CExportManager::Text, true);
    };
}

/** Saves the search result keys. */
void CModuleResultView::saveResult() {
    CSwordModuleInfo *m = activeModule();
    if (m != nullptr) {
        CExportManager mgr(true, tr("Saving search result"));
        mgr.saveKeyList(m_results[m], m, CExportManager::Text, false);
    };
}

/** Saves the search result with it's text. */
void CModuleResultView::saveResultWithText() {
    CSwordModuleInfo *m = activeModule();
    if (m != nullptr) {
        CExportManager mgr(true, tr("Saving search result"));
        mgr.saveKeyList(m_results[m], m, CExportManager::Text, true);
    };
}

/** Appends the whole search result to the printer queue. */
void CModuleResultView::printResult() {
    CSwordModuleInfo *m = activeModule();
    if (m != nullptr) {
        CExportManager mgr(true, tr("Printing search result"));
        mgr.printKeyList(m_results[m], m, btConfig().getDisplayOptions(),
                         btConfig().getFilterOptions());
    };
}



} //end of namespace Search
