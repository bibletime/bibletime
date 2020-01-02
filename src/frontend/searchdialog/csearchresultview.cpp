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

#include "csearchresultview.h"

#include <QContextMenuEvent>
#include <QList>
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletimeapp.h"
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

    m_actions.copy.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.copy.result, SIGNAL(triggered()),
               this,                  SLOT(copyItems()));
    m_actions.copyMenu->addAction(m_actions.copy.result);

    m_actions.copy.resultWithText = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.copy.resultWithText, SIGNAL(triggered()),
               this,                          SLOT(copyItemsWithText()));
    m_actions.copyMenu->addAction(m_actions.copy.resultWithText);

    m_popup->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), m_popup);
    m_actions.saveMenu->setIcon(CResMgr::searchdialog::result::foundItems::saveMenu::icon());

    m_actions.save.result = new QAction(tr("Reference only"), this);
    BT_CONNECT(m_actions.save.result, SIGNAL(triggered()),
               this,                  SLOT(saveItems()) );
    m_actions.saveMenu->addAction(m_actions.save.result);

    m_actions.save.resultWithText = new QAction(tr("Reference with text"), this);
    m_actions.saveMenu->addAction(m_actions.save.resultWithText);
    BT_CONNECT(m_actions.save.resultWithText, SIGNAL(triggered()),
               this,                          SLOT(saveItemsWithText()));
    m_popup->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), m_popup);
    m_actions.printMenu->setIcon(CResMgr::searchdialog::result::foundItems::printMenu::icon());

    m_actions.print.result = new QAction(tr("Reference with text"), this);
    BT_CONNECT(m_actions.print.result, SIGNAL(triggered()),
               this,                   SLOT(printItems()));
    m_actions.printMenu->addAction(m_actions.print.result);
    m_popup->addMenu(m_actions.printMenu);
}

/** No descriptions */
void CSearchResultView::initConnections() {
    //  BT_CONNECT(this, SIGNAL(executed(QListViewItem *)),
    //             this, SLOT(executed(QListViewItem *)));
    /// \todo are these right after porting?
    //items: current, previous
    BT_CONNECT(this,
               SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
               this, SLOT(executed(QTreeWidgetItem *, QTreeWidgetItem *)));
}

/** Setups the list with the given module. */
void CSearchResultView::setupTree(const CSwordModuleInfo *m,
                                  const sword::ListKey & result)
{
    clear();

    if (!m) return;

    m_module = m;
    const int count = result.getCount();
    if (!count) return;

    setUpdatesEnabled(false);

    QTreeWidgetItem* oldItem = nullptr;
    QTreeWidgetItem* item = nullptr;
    for (int index = 0; index < count; index++) {
        item = new QTreeWidgetItem(this, oldItem);
        item->setText(0, QString::fromUtf8(result.getElement(index)->getText()));
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

    Q_FOREACH(QString const & s, vList) {
        item = new QTreeWidgetItem(this, oldItem);
        item->setText(0, (s));
        oldItem = item;
    }

    setUpdatesEnabled(true);

    /// \todo select the first item
    //setSelected(firstChild(), true);
    //executed(currentItem());
}

/// \todo is this still valid?
/** Is connected to the signal executed, which is emitted when a mew item was chosen. */
void CSearchResultView::executed(QTreeWidgetItem* current, QTreeWidgetItem*) {
    if (current) {
        emit keySelected(current->text(0));
    }
    else {
        emit keyDeselected();
    }
}

/// \todo another function?
/** Reimplementation to show the popup menu. */
void CSearchResultView::contextMenuEvent(QContextMenuEvent* event) {
    m_popup->exec(event->globalPos());
}

void CSearchResultView::printItems() {
    QList<QTreeWidgetItem*> items = selectedItems();
    CExportManager mgr(true, tr("Printing search result"));

    QStringList list;
    Q_FOREACH(QTreeWidgetItem const * const k, items)
        list.append( k->text(0) );
    mgr.printKeyList( list, module(), btConfig().getDisplayOptions(), btConfig().getFilterOptions() );
}

void CSearchResultView::saveItems() {
    CExportManager mgr(true, tr("Saving search result"));

    const CSwordModuleInfo *m = module();
    CSwordKey* k = nullptr;
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<CSwordKey*> keys;
    Q_FOREACH(QTreeWidgetItem const * const i, items) {
        k = CSwordKey::createInstance( m );
        k->setKey(i->text(0));
        keys.append( k );
    }
    mgr.saveKeyList( keys, CExportManager::Text, false);

    qDeleteAll(keys);
    keys.clear(); //delete all the keys we created
}

void CSearchResultView::saveItemsWithText() {
    CExportManager mgr(true, tr("Saving search result"));

    const CSwordModuleInfo *m = module();
    CSwordKey* k = nullptr;
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<CSwordKey*> keys;
    Q_FOREACH(QTreeWidgetItem const * const i, items) {
        k = CSwordKey::createInstance( m );
        k->setKey(i->text(0));
        keys.append( k );
    };
    mgr.saveKeyList( keys, CExportManager::Text, true);

    qDeleteAll(keys);
    keys.clear(); //delete all the keys we created
}

void CSearchResultView::copyItems() {
    CExportManager mgr(true, tr("Copying search result"));

    const CSwordModuleInfo *m = module();
    CSwordKey* k = nullptr;
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<CSwordKey*> keys;
    Q_FOREACH(QTreeWidgetItem const * const i, items) {
        k = CSwordKey::createInstance( m );
        k->setKey(i->text(0));
        keys.append( k );
    };
    mgr.copyKeyList( keys, CExportManager::Text, false);

    qDeleteAll(keys);
    keys.clear(); //delete all the keys we created
}

void CSearchResultView::copyItemsWithText() {
    CExportManager mgr(true, tr("Copying search result"));

    const CSwordModuleInfo *m = module();
    CSwordKey* k = nullptr;
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<CSwordKey*> keys;
    Q_FOREACH(QTreeWidgetItem const * const i, items) {
        k = CSwordKey::createInstance( m );
        k->setKey(i->text(0));
        keys.append( k );
    };
    mgr.copyKeyList( keys, CExportManager::Text, true);

    qDeleteAll(keys);
    keys.clear(); //delete all the keys we created
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


QMimeData * CSearchResultView::mimeData ( const QList<QTreeWidgetItem *> items ) const {
    BTMimeData* mdata = new BTMimeData(m_module->name(), items.first()->text(0), QString());
    Q_FOREACH(QTreeWidgetItem const * const i, items)
        mdata->appendBookmark(m_module->name(), i->text(0), QString());
    return mdata;
}

QStringList CSearchResultView::mimeTypes () const {
    return QStringList("BibleTime/Bookmark");
}

} //end of namespace

