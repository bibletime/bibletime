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

#include "csearchresultview.h"

#include <QContextMenuEvent>
#include <QList>
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/keys/cswordkey.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../BtMimeData.h"
#include "../cexportmanager.h"


namespace Search {

CSearchResultView::CSearchResultView(QWidget* parent)
        : QTreeWidget(parent),
        m_module(nullptr) {
    initView();
    initConnections();
}

/** Initializes the view of this widget. */
void CSearchResultView::initView() {
    setToolTip(tr("Search result of the selected work"));
    setHeaderLabel(tr("Results"));
    setDragEnabled(true);
    setRootIsDecorated( false );
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    //setup the popup menu
    m_popup = new QMenu(this);

    m_actions.copyMenu = new QMenu(tr("Copy..."), m_popup);
    m_actions.copyMenu->setIcon(CResMgr::searchdialog::result::foundItems::copyMenu::icon());

    struct SelectedKeysList : QList<CSwordKey *> {
        SelectedKeysList(CSearchResultView & self) {
            auto const * const m = self.m_module;
            auto const selectedItems(self.selectedItems());
            reserve(selectedItems.size());
            try {
                for (auto const * const i : self.selectedItems()) {
                    append(m->createKey());
                    last()->setKey(i->text(0));
                }
            } catch (...) {
                qDeleteAll(*this);
            }
        }

        ~SelectedKeysList() noexcept { qDeleteAll(*this); }
    };

    m_actions.copy.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.copy.result, &QAction::triggered,
               [this]{
                   SelectedKeysList keys(*this);
                   CExportManager(true, tr("Copying search result"))
                           .copyKeyList(keys, CExportManager::Text, false);
               });
    m_actions.copyMenu->addAction(m_actions.copy.result);

    m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.copy.resultWithText, &QAction::triggered,
               [this]{
                   SelectedKeysList keys(*this);
                   CExportManager(true, tr("Copying search result"))
                           .copyKeyList(keys, CExportManager::Text, true);
               });
    m_actions.copyMenu->addAction(m_actions.copy.resultWithText);

    m_popup->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
    m_actions.saveMenu->setIcon(CResMgr::searchdialog::result::foundItems::saveMenu::icon());

    m_actions.save.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.save.result, &QAction::triggered,
               [this]{
                   SelectedKeysList keys(*this);
                   CExportManager(true, tr("Saving search result"))
                           .saveKeyList( keys, CExportManager::Text, false);
               });
    m_actions.saveMenu->addAction(m_actions.save.result);

    m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
    m_actions.saveMenu->addAction(m_actions.save.resultWithText);
    BT_CONNECT(m_actions.save.resultWithText, &QAction::triggered,
               [this]{
                   SelectedKeysList keys(*this);
                   CExportManager(true, tr("Saving search result"))
                           .saveKeyList(keys, CExportManager::Text, true);
               });
    m_popup->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), m_popup);
    m_actions.printMenu->setIcon(CResMgr::searchdialog::result::foundItems::printMenu::icon());

    m_actions.print.result = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.print.result, &QAction::triggered,
               [this]{
                   QStringList list;
                   for (auto const * const k : selectedItems())
                       list.append(k->text(0));
                   CExportManager(true, tr("Printing search result"))
                           .printKeyList(list,
                                         m_module,
                                         btConfig().getDisplayOptions(),
                                         btConfig().getFilterOptions());
               });
    m_actions.printMenu->addAction(m_actions.print.result);
    m_popup->addMenu(m_actions.printMenu);
}

/** No descriptions */
void CSearchResultView::initConnections() {
    /// \todo are these right after porting?
    //items: current, previous
    BT_CONNECT(this, &CSearchResultView::currentItemChanged,
               [this](QTreeWidgetItem * const current, QTreeWidgetItem *) {
                   if (current) {
                       Q_EMIT keySelected(current->text(0));
                   } else {
                       Q_EMIT keyDeselected();
                   }
               });
}

/** Setups the list with the given module. */
void CSearchResultView::setupTree(
        CSwordModuleInfo const * m,
        CSwordModuleSearch::ModuleResultList const & result)
{
    clear();

    if (!m) return;

    m_module = m;
    if (result.empty())
        return;

    setUpdatesEnabled(false);

    QTreeWidgetItem* oldItem = nullptr;
    QTreeWidgetItem* item = nullptr;
    for (auto const & keyPtr : result) {
        item = new QTreeWidgetItem(this, oldItem);
        item->setText(0, QString::fromUtf8(keyPtr->getText()));
        oldItem = item;
    }

    setUpdatesEnabled(true);
    //pre-select the first item
    this->setCurrentItem(this->topLevelItem(0), 0);
}

void CSearchResultView::setupStrongsTree(CSwordModuleInfo* m, const QStringList &vList) {
    clear();
    if (!m) return;

    m_module = m;

    if (vList.empty()) return;

    setUpdatesEnabled(false);

    QTreeWidgetItem* oldItem = nullptr;
    QTreeWidgetItem* item = nullptr;

    for (auto const & s : vList) {
        item = new QTreeWidgetItem(this, oldItem);
        item->setText(0, (s));
        oldItem = item;
    }

    setUpdatesEnabled(true);

    /// \todo select the first item
    //setSelected(firstChild(), true);
    //executed(currentItem());
}

/// \todo another function?
/** Reimplementation to show the popup menu. */
void CSearchResultView::contextMenuEvent(QContextMenuEvent* event) {
    m_popup->exec(event->globalPos());
}

/// \todo port this to the new d'n'd
// Q3DragObject* CSearchResultView::dragObject() {
//     //return a valid DragObject to make DnD possible!
//
//     /*
//     * First get all selected items and fill with them the dndItems list. The return the QDragObject we got from CDRagDropMgr
//     */
//     CDragDropMgr::ItemList dndItems;
//
//     Q3PtrList<Q3ListViewItem> items = selectedItems();
//     for (items.first(); items.current(); items.next()) {
//         dndItems.append( CDragDropMgr::Item(m_module->name(), items.current()->text(0), QString()) ); //no description
//     };
//
//     return CDragDropMgr::dragObject(dndItems, viewport());
// }


QMimeData *
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
CSearchResultView::mimeData(QList<QTreeWidgetItem *> const items) const {
#else
CSearchResultView::mimeData(QList<QTreeWidgetItem *> const & items) const {
#endif
    if (items.empty())
        return nullptr;
    BTMimeData::ItemList bookmarks;
    for (auto const * const i : items)
        bookmarks.append({m_module->name(), i->text(0), {}});
    return new BTMimeData(std::move(bookmarks));
}

QStringList CSearchResultView::mimeTypes() const
{ return QStringList(QStringLiteral("BibleTime/Bookmark")); }

} //end of namespace

