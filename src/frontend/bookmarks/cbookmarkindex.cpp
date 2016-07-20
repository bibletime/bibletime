/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <memory>
#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDrag>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QToolTip>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/btbookmarksmodel.h"
#include "backend/managers/referencemanager.h"
#include "bibletime.h"
#include "bibletimeapp.h"
#include "frontend/btprinter.h"
#include "frontend/cdragdrop.h"
#include "frontend/cinfodisplay.h"
#include "frontend/messagedialog.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/bookmarks/bteditbookmarkdialog.h"
#include "frontend/bookmarks/cbookmarkindex.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/bticons.h"
#include "util/btscopeexit.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"


CBookmarkIndex::CBookmarkIndex(QWidget * const parent)
        : QTreeView{parent}
        , m_magTimer{this}
        , m_bookmarksModel{nullptr}
{
    setMouseTracking(true);
    m_magTimer.setSingleShot(true);
    m_magTimer.setInterval(btConfig().value<int>("GUI/magDelay", 400));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setHeaderHidden(true);

    //--------------------------------------------------------------------------
    // Initialize view:

    setHeaderHidden(true);

    setFocusPolicy(Qt::WheelFocus);

    //d'n'd related settings
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    viewport()->setAcceptDrops(true);
    setAutoScroll(true);
    setAutoExpandDelay(800);

    setItemsExpandable(true);
    setRootIsDecorated(true);
    setAllColumnsShowFocus(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    //setExpandsOnDoubleClick(true);
    setEditTriggers(editTriggers() ^ QAbstractItemView::DoubleClicked);

    //setup the popup menu
    m_popup = new QMenu{viewport()};
    m_popup->setTitle(tr("Bookmarks"));

    auto const newQAction = [this](QString const & text,
                                   QIcon const & pix,
                                   char const * const slot)
    {
        QAction * const action = new QAction{pix, text, this};
        BT_CONNECT(action, SIGNAL(triggered()), this, slot);
        return action;
    };

    namespace MI = CResMgr::mainIndex;
    m_actions[NewFolder] = newQAction(tr("New folder"),
                                      MI::newFolder::icon(),
                                      SLOT(createNewFolder()));
    m_actions[ChangeFolder] = newQAction(tr("Rename folder"),
                                         MI::changeFolder::icon(),
                                         SLOT(changeFolder()));
    m_actions[EditBookmark] = newQAction(tr("Edit bookmark..."),
                                         MI::editBookmark::icon(),
                                         SLOT(editBookmark()));
    /// \todo Add icons for sorting bookmarks
    m_actions[SortFolderBookmarks] = newQAction(tr("Sort folder bookmarks..."),
                                                MI::sortFolderBookmarks::icon(),
                                                SLOT(sortFolderBookmarks()));
    m_actions[SortAllBookmarks] = newQAction(tr("Sort all bookmarks..."),
                                             MI::sortAllBookmarks::icon(),
                                             SLOT(sortAllBookmarks()));
    m_actions[ImportBookmarks] = newQAction(tr("Import to folder..."),
                                            MI::importBookmarks::icon(),
                                            SLOT(importBookmarks()));
    m_actions[ExportBookmarks] = newQAction(tr("Export from folder..."),
                                            MI::exportBookmarks::icon(),
                                            SLOT(exportBookmarks()));
    m_actions[PrintBookmarks] = newQAction(tr("Print bookmarks..."),
                                           MI::printBookmarks::icon(),
                                           SLOT(printBookmarks()));

    m_actions[DeleteEntries] = newQAction(tr("Remove selected items..."),
                                          MI::deleteItems::icon(),
                                          SLOT(confirmDeleteEntries()));

    // Fill the popup menu:
    auto const createSeparator = [this]{
        QAction * const separator = new QAction{this};
        separator->setSeparator(true);
        return separator;
    };
    m_popup->addAction(m_actions[NewFolder]);
    m_popup->addAction(m_actions[ChangeFolder]);
    m_popup->addAction(createSeparator());
    m_popup->addAction(m_actions[EditBookmark]);
    m_popup->addAction(m_actions[SortFolderBookmarks]);
    m_popup->addAction(m_actions[SortAllBookmarks]);
    m_popup->addAction(m_actions[ImportBookmarks]);
    m_popup->addAction(m_actions[ExportBookmarks]);
    m_popup->addAction(m_actions[PrintBookmarks]);
    m_popup->addAction(createSeparator());
    m_popup->addAction(m_actions[DeleteEntries]);


    //--------------------------------------------------------------------------
    // Initialize connections:

    BT_CONNECT(this, SIGNAL(activated(QModelIndex const &)),
               this, SLOT(slotExecuted(QModelIndex const &)));
    BT_CONNECT(this, SIGNAL(customContextMenuRequested(QPoint const &)),
               this, SLOT(contextMenu(QPoint const &)));
    BT_CONNECT(&m_magTimer, SIGNAL(timeout()), this, SLOT(magTimeout()));

    //--------------------------------------------------------------------------
    // Initialize tree:

    m_bookmarksModel = new BtBookmarksModel{this};
    setModel(m_bookmarksModel);

    // add the invisible extra item at the end
    if(m_bookmarksModel->insertRows(m_bookmarksModel->rowCount(), 1))
        m_extraItem = m_bookmarksModel->index(m_bookmarksModel->rowCount() - 1, 0);
    showExtraItem();
}

/** \note Hack to get single click and selection working. See slotExecuted. */
void CBookmarkIndex::mouseReleaseEvent(QMouseEvent* event) {
    m_mouseReleaseEventModifiers = event->modifiers();
    QTreeView::mouseReleaseEvent(event);
}

/** Called when an item is clicked with mouse or activated with keyboard. */
void CBookmarkIndex::slotExecuted( const QModelIndex &index ) {
    /** \note HACK: checking the modifier keys from the last mouseReleaseEvent
                    depends on executing order: mouseReleaseEvent first, then
                    itemClicked signal. */
    auto const modifiers = m_mouseReleaseEventModifiers;
    m_mouseReleaseEventModifiers = Qt::NoModifier;
    if (modifiers != Qt::NoModifier || !index.isValid())
        return;

    // Clicked on a bookmark:
    if (m_bookmarksModel->isBookmark(index))
        if (CSwordModuleInfo * const mod = m_bookmarksModel->module(index))
            emit createReadDisplayWindow(QList<CSwordModuleInfo *>() << mod,
                                         m_bookmarksModel->key(index));
}

/** Creates a drag mime data object for the current selection. */
QMimeData * CBookmarkIndex::dragObject() {
    BTMimeData * const mimeData = new BTMimeData{};

    Q_FOREACH(QModelIndex const & widgetItem, selectedIndexes()) {
        if (!widgetItem.isValid())
            break;
        if (m_bookmarksModel->isBookmark(widgetItem)) {
            /* Take care of bookmarks which have no valid module any more, e.g.
               if these were uninstalled: */
            CSwordModuleInfo * const module =
                    m_bookmarksModel->module(widgetItem);
            const QString moduleName = module ? module->name() : QString::null;
            mimeData->appendBookmark(moduleName, m_bookmarksModel->key(widgetItem),
                                     m_bookmarksModel->description(widgetItem));
        }
    }
    return mimeData;
}

void CBookmarkIndex::dragEnterEvent(QDragEnterEvent * event) {
    if (event->mimeData()->hasFormat("BibleTime/Bookmark")) {
        event->acceptProposedAction();
        setState(DraggingState);
    } else {
        QAbstractItemView::dragEnterEvent(event);
    }
}

void CBookmarkIndex::dragMoveEvent(QDragMoveEvent * event) {
    // Do this first, otherwise the event may be ignored:
    QTreeView::dragMoveEvent(event);

    event->acceptProposedAction();
    event->accept();

    // Do this to paint the arrow:
    m_dragMovementPosition = event->pos();
    viewport()->update();
}

void CBookmarkIndex::dragLeaveEvent(QDragLeaveEvent *) {
    setState(QAbstractItemView::NoState); // Not dragging anymore
    viewport()->update(); // Clear the arrow
}

void CBookmarkIndex::paintEvent(QPaintEvent * event) {
    // Do the normal painting first
    QTreeView::paintEvent(event);

    // Don't paint the arrow if not dragging:
    if (state() != QAbstractItemView::DraggingState)
        return;

    static QPixmap pix;
    static int halfPixHeight;
    static bool arrowInitialized = false;

    // Initialize the static variables, including the arrow pixmap
    if (!arrowInitialized) {
        arrowInitialized = true;
        pix = BtIcons::instance().icon_pointing_arrow.pixmap(util::tool::mWidth(this, 1));
        halfPixHeight = pix.height() / 2;
    }

    // Find the place for the arrow:
    QModelIndex const index = indexAt(m_dragMovementPosition);
    int xCoord, yCoord;
    if (m_bookmarksModel->isBookmark(index)) {
        QRect const rect = visualRect(index);
        xCoord = QApplication::isRightToLeft() ? rect.right() : rect.left();
        if (m_dragMovementPosition.y() > rect.bottom() - rect.height() / 2) {
            yCoord = rect.bottom() - halfPixHeight; // bottom
        } else {
            yCoord = rect.top() - halfPixHeight - 1; // top
        }
    } else {
        if (m_bookmarksModel->isFolder(index)) {
            QRect const rect = visualRect(index);
            if (m_dragMovementPosition.y()
                > rect.bottom() - (2 * rect.height() / 3))
            {
                yCoord = rect.bottom() - halfPixHeight; // bottom
                xCoord = QApplication::isRightToLeft()
                         ? (rect.right() - indentation())
                         : (rect.left() + indentation());
            } else {
                yCoord = rect.top() - halfPixHeight - 1; // top
                xCoord = QApplication::isRightToLeft()
                         ? rect.right()
                         : rect.left();
            }
        } else if (index.isValid()) { // the extra item
            QRect const rect = visualRect(index);
            xCoord = QApplication::isRightToLeft() ? rect.right() : rect.left();
            yCoord = rect.top() - halfPixHeight - 1;
        } else { // empty area
            QRect const rect = visualRect(m_extraItem);
            yCoord = rect.top() - halfPixHeight - 1;
            xCoord = QApplication::isRightToLeft() ? rect.right() : rect.left();
        }
    }
    QPainter{this->viewport()}.drawPixmap(xCoord, yCoord, pix);
}


void CBookmarkIndex::dropEvent(QDropEvent * event) {
    // Try to prevent annoying timed autocollapsing:
    BT_CONNECT(this, SIGNAL(collapsed(QModelIndex const &)),
               this, SLOT(expandAutoCollapsedItem(QModelIndex const &)));
    BT_SCOPE_EXIT(
        QObject::disconnect(
                this, SIGNAL(collapsed(QModelIndex const &)),
                this, SLOT(expandAutoCollapsedItem(QModelIndex const &)));
    );
    QModelIndex const index = indexAt(event->pos());
    QModelIndex parentIndex;
    int indexUnderParent = 0;

    // Find the place where the drag is dropped
    if (index.isValid()) {
        if (m_bookmarksModel->isFolder(index)) {
            QRect const rect = visualRect(index);
            if (event->pos().y() > rect.bottom() - (2* rect.height() / 3) ) {
                parentIndex = index;
            } else {
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current folder
            }
        } else {
            if (m_bookmarksModel->isBookmark(index)) {
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current bookmark
                QRect const rect = visualRect(index);
                if (event->pos().y() > rect.bottom() - rect.height() / 2)
                    indexUnderParent++; // after the current bookmark
            } else { // item is the extra item
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current bookmark
            }
        }
    } else { // no item under event point: drop to the end
        indexUnderParent = m_bookmarksModel->rowCount() - 1;
    }

    if (event->source() != this) {
        // Take the bookmark data from the mime source
        if (BTMimeData const * const mdata =
                dynamic_cast<BTMimeData const *>(event->mimeData()))
        {
            //create the new bookmark
            QString const moduleName(mdata->bookmark().module());
            QString const keyText(mdata->bookmark().key());
            QString const description(mdata->bookmark().description());
            CSwordModuleInfo * minfo =
                    CSwordBackend::instance()->findModuleByName(moduleName);
            BT_ASSERT(minfo);

            /// \todo add title
            m_bookmarksModel->addBookmark(indexUnderParent,
                                          parentIndex,
                                          *minfo,
                                          keyText,
                                          description);
        }
    } else {
        event->accept();

        bool bookmarksOnly = true;
        bool targetIncluded = false;
        bool moreThanOneFolder = false;

        QModelIndexList const list = selectedIndexes();
        QModelIndexList newList;

        Q_FOREACH(QModelIndex const & index, list) {
            if (m_bookmarksModel->isFolder(index)) {
                bookmarksOnly = false;
                // Only one item allowed if a folder is selected:
                if (list.count() > 1) {
                    moreThanOneFolder = true;
                    break;
                }
                // Dropping to self or descendand not allowed:
                if (m_bookmarksModel->hasDescendant(index, parentIndex)) {
                    targetIncluded = true;
                    break;
                }
            } else {
                newList.append(index);
            }
        }

        if (!bookmarksOnly && list.count() == 1) {
            newList.append(list[0]); // list contain only one folder item
        } else if (!bookmarksOnly && list.count() > 1) {
            moreThanOneFolder = true; // wrong amount of items
        }

        if (moreThanOneFolder) {
            QToolTip::showText(QCursor::pos(), tr("Can drop only bookmarks or one folder"));
            return;
        }
        if (targetIncluded) {
            QToolTip::showText(QCursor::pos(), tr("Can't drop folder into the folder itself or into its subfolder"));
            return;
        }

        // Ask whether to copy or move with a popup menu
        std::unique_ptr<QMenu> dropPopupMenu{new QMenu{this}};
        dropPopupMenu->setEnabled(!newList.empty());
        QAction * const copy = dropPopupMenu->addAction(tr("Copy"));
        QAction * const move = dropPopupMenu->addAction(tr("Move"));
        QAction * const dropAction = dropPopupMenu->exec(QCursor::pos());
        dropPopupMenu.reset();

        if (dropAction == copy) {
            m_bookmarksModel->copyItems(indexUnderParent, parentIndex, newList);
        } else if (dropAction == move) {
            m_bookmarksModel->copyItems(indexUnderParent, parentIndex, newList);
            deleteEntries();
        } else { // user canceled
            return;
        }
    }
    setState(QAbstractItemView::NoState);
}

bool CBookmarkIndex::enableAction(QModelIndex const & index,
                                  CBookmarkIndex::MenuAction const type) const
{
    switch (type) {
        case NewFolder:
        case ChangeFolder:
        case SortFolderBookmarks:
        case ImportBookmarks:
        case ExportBookmarks:
            return m_bookmarksModel->isFolder(index);
        case DeleteEntries:
            return true;
        case PrintBookmarks:
            return hasBookmarksRecursively(QModelIndexList{} << index);
        case EditBookmark:
            return m_bookmarksModel->isBookmark(index);
        default:
            return false;
    }
}

bool CBookmarkIndex::hasBookmarksRecursively(QModelIndexList items) const {
    while (!items.empty()) {
        QModelIndex const index = items.takeFirst();
        if (m_bookmarksModel->isBookmark(index))
            return true;
        if (m_bookmarksModel->isFolder(index)) {
            int const numChildren = m_bookmarksModel->rowCount(index);
            for (int i = 0; i < numChildren; i++)
                items.append(index.child(i, 0));
        }
    }
    return false;
}

void CBookmarkIndex::showExtraItem() {
    model()->setData(m_extraItem,
                     tr("Drag references from text views to this view"));
}

void CBookmarkIndex::hideExtraItem()
{ model()->setData(m_extraItem, QVariant()); }

void CBookmarkIndex::leaveEvent(QEvent * event) {
    showExtraItem();
    update();
    QTreeView::leaveEvent(event);
}

/** Shows the context menu at the given position. */
void CBookmarkIndex::contextMenu(QPoint const & p) {
    // Enable actions based on the selected items (if any):
    QModelIndex const i = indexAt(p);
    QModelIndexList const items = selectedIndexes();
    if (items.isEmpty()) { // Special handling for no selection:
        for (int index = ActionBegin; index < ActionEnd; ++index)
            m_actions[index]->setEnabled((index == NewFolder)
                                         || (index == SortAllBookmarks));
    } else if (items.count() == 1) { // Special handling for one selected item:
        for (int index = ActionBegin; index < ActionEnd; ++index)
            m_actions[index]->setEnabled(
                enableAction(items.at(0), static_cast<MenuAction>(index)));
    } else if (!i.isValid()) { // Disable all actions for invalid index:
        for (int index = ActionBegin; index < ActionEnd; ++index)
            m_actions[index]->setEnabled(false);
    } else { // Enable actions depending on the the selected items:
        for (int index = ActionBegin; index < ActionEnd; ++index)
            m_actions[index]->setEnabled(
                    (index == DeleteEntries)
                    || ((index == PrintBookmarks)
                        && hasBookmarksRecursively(items)));
    }
    m_popup->exec(mapToGlobal(p));
}

/** Adds a new subfolder to the current item. */
void CBookmarkIndex::createNewFolder() {
    if (!selectedIndexes().empty()) {
        if (m_bookmarksModel->isFolder(currentIndex()))
            setCurrentIndex(m_bookmarksModel->addFolder(
                                    m_bookmarksModel->rowCount(currentIndex()),
                                    currentIndex()));
    } else { // create a top level folder
        setCurrentIndex(
                m_bookmarksModel->addFolder(m_bookmarksModel->rowCount() - 1,
                                            QModelIndex{}));
    }
}

/** Opens a dialog to change the current folder. */
void CBookmarkIndex::changeFolder() {
    BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    edit(currentIndex());
}

/** Edits the current bookmark. */
void CBookmarkIndex::editBookmark() {
    QModelIndex const index = currentIndex();
    BT_ASSERT(m_bookmarksModel->isBookmark(index));
    CSwordModuleInfo * const module = m_bookmarksModel->module(index);
    BtEditBookmarkDialog d{
            QString::fromLatin1("%1 (%2)").arg(m_bookmarksModel->key(index))
                                          .arg(module
                                               ? module->name()
                                               : QObject::tr("unknown")),
            index.data().toString(),
            m_bookmarksModel->description(index),
            this};
    if (d.exec() == QDialog::Accepted) {
        m_bookmarksModel->setData(index, d.titleText());
        m_bookmarksModel->setDescription(index, d.descriptionText());
    }
}

/** Sorts the current folder bookmarks. */
void CBookmarkIndex::sortFolderBookmarks() {
    BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    m_bookmarksModel->sortItems(currentIndex());
}

/** Sorts all bookmarks. */
void CBookmarkIndex::sortAllBookmarks() {
    m_bookmarksModel->sortItems();
    if (m_extraItem.row() != m_bookmarksModel->rowCount() - 1) {
        m_bookmarksModel->removeRow(m_extraItem.row(), m_extraItem.parent());
        if (m_bookmarksModel->insertRows(m_bookmarksModel->rowCount(), 1))
            m_extraItem = m_bookmarksModel->index(m_bookmarksModel->rowCount() - 1, 0);
    }
}

/** Exports the bookmarks being in the selected folder. */
void CBookmarkIndex::exportBookmarks() {
    BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    QString const fileName =
            QFileDialog::getSaveFileName(
                nullptr,
                QObject::tr("Export Bookmarks"),
                "",
                QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;")
                + QObject::tr("All files") + QString(" (*.*)"));
    if (!fileName.isEmpty())
        m_bookmarksModel->save(fileName, currentIndex());
}

/** Import bookmarks from a file and add them to the selected folder. */
void CBookmarkIndex::importBookmarks() {
    BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    QString const fileName =
            QFileDialog::getOpenFileName(
                    nullptr,
                    QObject::tr("Import bookmarks"),
                    "",
                    QObject::tr("BibleTime bookmark files")
                    + QString(" (*.btb);;")
                    + QObject::tr("All files") + QString(" (*.*)"));
    if (!fileName.isEmpty())
        m_bookmarksModel->load(fileName, currentIndex());
}

/** Prints the selected bookmarks. */
void CBookmarkIndex::printBookmarks() {
    BT_ASSERT(hasBookmarksRecursively(selectedIndexes()));
    BtPrinter::KeyTree tree;
    {
        BtPrinter::KeyTreeItem::Settings const settings{
                false,
                BtPrinter::KeyTreeItem::Settings::CompleteShort};
        QModelIndexList items(selectedIndexes());
        while (!items.empty()) {
            QModelIndex const index(items.takeFirst());
            if (m_bookmarksModel->isBookmark(index)) {
                tree.append(
                        new BtPrinter::KeyTreeItem{
                                m_bookmarksModel->key(index),
                                m_bookmarksModel->module(index),
                                settings});
            } else if (m_bookmarksModel->isFolder(index)) {
                int const numChildren = m_bookmarksModel->rowCount(index);
                for (int i = 0; i < numChildren; i++)
                    items.append(index.child(i, 0));
            }
        }
    }
    BT_ASSERT(!tree.isEmpty());

    BtPrinter{btConfig().getDisplayOptions(),
              btConfig().getFilterOptions(),
              this}.printKeyTree(tree);
}

void CBookmarkIndex::confirmDeleteEntries() {
    if (message::showQuestion(
            this,
            tr("Delete Items"),
            tr("Do you really want to delete the selected items and folders?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No) == QMessageBox::Yes)
        deleteEntries();
}

void CBookmarkIndex::deleteEntries() {
    /* We need to use QPersistentModelIndex because after removeRows QModelIndex
       will be invalidated. Need to delete per index because selected indexes
       might be under different parents. */
    QList<QPersistentModelIndex> list;
    Q_FOREACH(QModelIndex const & i, selectedIndexes())
        list.append(i);

    Q_FOREACH(QModelIndex const & i, list)
        model()->removeRows(i.row(), 1, i.parent());
}


/*
Reimplementation from QAbstractItemView/QTreeWidget. Takes care of movable items.
It's easier to use this than to start drag with mouse event handlers.
The default implementation would drag items, but we don't call it. Instead we create
a BibleTime mimedata object. It can be dragged and dropped to a text view or somewhere else.
The internal drag is handled differently, it doesn't use the mimedata (see dropEvent()).
*/
void CBookmarkIndex::startDrag(Qt::DropActions) {
    // Create the data which can be used in other widgets:
    QMimeData * const mData = dragObject();
    QDrag * const drag = new QDrag{this};
    drag->setMimeData(mData);
    drag->exec();

    viewport()->update(); // because of the arrow
}

void CBookmarkIndex::mouseMoveEvent(QMouseEvent * event) {
    /* Restart the mag timer if we have moved to another item and shift was not
       pressed: */
    QModelIndex const itemUnderPointer = indexAt(event->pos());
    if (itemUnderPointer.isValid()
        && (itemUnderPointer != m_previousEventItem)
        && !(event->modifiers() & Qt::ShiftModifier))
        m_magTimer.start();
    m_previousEventItem = itemUnderPointer;

    if (!itemUnderPointer.isValid() || itemUnderPointer == m_extraItem) {
        showExtraItem();
    } else {
        hideExtraItem();
    }
    update();

    QTreeView::mouseMoveEvent(event);
}

void CBookmarkIndex::magTimeout() {
    if (!underMouse())
        return;

    /* Update the Mag only if the mouse pointer have been over the same item
       since the timer was started. */
    QModelIndex const itemUnderPointer{indexAt(mapFromGlobal(QCursor::pos()))};
    if (itemUnderPointer.isValid()
        && m_previousEventItem == itemUnderPointer
        && m_bookmarksModel->isBookmark(itemUnderPointer))
    {
        BT_ASSERT(BibleTime::instance()->infoDisplay());
        InfoDisplay::CInfoDisplay & infoDisplay =
                *(BibleTime::instance()->infoDisplay());
        if (CSwordModuleInfo const * const module =
                    m_bookmarksModel->module(itemUnderPointer))
        {
            infoDisplay.setInfo(
                Rendering::CrossReference,
                module->name() + ":" + m_bookmarksModel->key(itemUnderPointer));
        } else {
            infoDisplay.setInfo(Rendering::Text,
                                tr("The work to which the bookmark points to "
                                   "is not installed."));
        }
    }
}
