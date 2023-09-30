/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cmoduleresultview.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QStringList>
#include <QtAlgorithms>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <vector>
#include "../../backend/config/btconfig.h"
#include "../../backend/cswordmodulesearch.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../cexportmanager.h"
#include "btsearchresultarea.h"


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

    setColumnWidth(0, util::tool::mWidth(*this, 8));
    setColumnWidth(1, util::tool::mWidth(*this, 4));
    QSize sz(util::tool::mWidth(*this, 13), util::tool::mWidth(*this, 5));
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
    BT_CONNECT(m_actions.copy.result, &QAction::triggered,
               [this]{
                   if (auto * const m = activeModule())
                       CExportManager(true, tr("Copying search result"))
                               .copyKeyList(m_results[m],
                                            m,
                                            CExportManager::Text,
                                            false);
               });
    m_actions.copyMenu->addAction(m_actions.copy.result);
    m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.copy.resultWithText, &QAction::triggered,
               [this]{
                   if (auto * const m = activeModule())
                       CExportManager(true, tr("Copying search result"))
                               .copyKeyList(m_results[m],
                                            m,
                                            CExportManager::Text,
                                            true);
               });
    m_actions.copyMenu->addAction(m_actions.copy.resultWithText);
    m_popup->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
    m_actions.saveMenu->setIcon(CResMgr::searchdialog::result::moduleList::saveMenu::icon());
    m_actions.save.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.save.result, &QAction::triggered,
               [this]{
                   if (auto * const m = activeModule())
                       CExportManager(true, tr("Saving search result"))
                               .saveKeyList(m_results[m],
                                            m,
                                            CExportManager::Text,
                                            false);
               });
    m_actions.saveMenu->addAction(m_actions.save.result);
    m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.save.resultWithText, &QAction::triggered,
               [this]{
                   if (auto * const m = activeModule())
                       CExportManager(true, tr("Saving search result"))
                               .saveKeyList(m_results[m],
                                            m,
                                            CExportManager::Text,
                                            true);
               });
    m_actions.saveMenu->addAction(m_actions.save.resultWithText);
    m_popup->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), m_popup);
    m_actions.printMenu->setIcon(CResMgr::searchdialog::result::moduleList::printMenu::icon());
    m_actions.print.result = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.print.result, &QAction::triggered,
               [this]{
                   if (auto * const m = activeModule())
                       CExportManager(true, tr("Printing search result"))
                               .printKeyList(m_results[m],
                                             m,
                                             btConfig().getDisplayOptions(),
                                             btConfig().getFilterOptions());
               });
    m_actions.printMenu->addAction(m_actions.print.result);
    m_popup->addMenu(m_actions.printMenu);
}

/** Initializes the connections of this widget, */
void CModuleResultView::initConnections() {
    /// \todo
    BT_CONNECT(this, &CModuleResultView::currentItemChanged,
               this, &CModuleResultView::executed);
}

void CModuleResultView::setupTree(const CSwordModuleSearch::Results & results,
                                  const QString & searchedText)
{
    /// \todo implement sorting in this method.

    clear();
    m_results.clear();
    qDeleteAll(m_strongsResults);
    m_strongsResults.clear();

    bool strongsAvailable = false;

    for (auto const & result : results) {
        auto const * const m = result.module;
        BT_ASSERT(!m_results.contains(m));
        m_results.insert(m, result.results);
        QTreeWidgetItem * const item =
                new QTreeWidgetItem(this,
                                    QStringList(m->name())
                                    << QString::number(result.results.size()));

        item->setIcon(0, util::tool::getIconForModule(m));
        /*
          We need to make a decision here.  Either don't show any Strong's
          number translations, or show the first one in the search text, or
          figure out how to show them all. I choose option number 2 at this time.
        */

        // strong search text index for finding "strong:"
        int sstIndex = searchedText.indexOf(QStringLiteral("strong:"), 0);
        if (sstIndex != -1) {
            /*
              Get the strongs number from the search text. First find the first
              space after "strong:". This should indicate a change in search
              token
            */
            sstIndex += 7;
            const int sTokenIndex = searchedText.indexOf(' ', sstIndex);
            const QString sNumber(searchedText.mid(sstIndex, sTokenIndex - sstIndex));

            setupStrongsResults(m, result.results, item, sNumber);

            /// \todo item->setOpen(true);
            strongsAvailable = true;
        }
    }

    // Allow to hide the module strongs if there are any available
    setRootIsDecorated( strongsAvailable );
}

void CModuleResultView::setupStrongsResults(
        CSwordModuleInfo const * module,
        CSwordModuleSearch::ModuleResultList const & results,
        QTreeWidgetItem * parent,
        QString const & sNumber)
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
    if (!i) {
        //Clear list
        Q_EMIT moduleChanged();
        return;
    }

    auto const & itemText = i->text(0);
    if (auto * const m = CSwordBackend::instance().findModuleByName(itemText)) {
        Q_EMIT moduleChanged();
        Q_EMIT moduleSelected(m, m_results.value(m));
        return;
    }

    StrongsResultList *strongsResult = m_strongsResults[activeModule()];

    if (!strongsResult) {
        return;
    }

    for (int cnt = 0; cnt < strongsResult->count(); cnt++) {
        if (strongsResult->at(cnt).keyText() == itemText) {
            //clear the verses list
            Q_EMIT moduleChanged();
            Q_EMIT strongsSelected(activeModule(),
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

    return CSwordBackend::instance().findModuleByName(item->text(0));
}

/** Reimplementation from QWidget. */
void CModuleResultView::contextMenuEvent( QContextMenuEvent * event ) {
    //make sure that all entries have the correct status
    m_popup->exec(event->globalPos());
}

} //end of namespace Search
