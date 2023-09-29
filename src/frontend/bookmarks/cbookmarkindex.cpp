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
#include <QScopeGuard>
#include <QTimer>
#include <QToolTip>
#include "../../backend/btbookmarksmodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/bticons.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bookmarks/bteditbookmarkdialog.h"
#include "../bookmarks/cbookmarkindex.h"
#include "../btprinter.h"
#include "../BtMimeData.h"
#include "../messagedialog.h"


namespace {

QString fileDialogFilter() {
    return QStringLiteral("%1 (*.btb);;%2 (*)")
            .arg(QObject::tr("BibleTime bookmark files"),
                 QObject::tr("All files"));
}

} // anonymous namespace

CBookmarkIndex::CBookmarkIndex(QWidget * const parent)
        : QTreeView{parent}
        , m_magTimer{this}
        , m_bookmarksModel{nullptr}
{
    setMouseTracking(true);
    m_magTimer.setSingleShot(true);
    m_magTimer.setInterval(
                btConfig().value<int>(QStringLiteral("GUI/magDelay"), 400));
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
    auto const addMenuAction =
            [this](MenuAction const menuAction,
                   QString const & text,
                   QIcon const & pix,
                   auto && slot)
            {
                auto * const action = new QAction(pix, text, this);
                BT_CONNECT(action, &QAction::triggered,
                           std::forward<decltype(slot)>(slot));
                m_actions[menuAction] = action;
                m_popup->addAction(action);
            };
    namespace MI = CResMgr::mainIndex;
    addMenuAction(NewFolder, tr("New folder"), MI::newFolder::icon(),
                  [this]{
                      if (!selectedIndexes().empty()) {
                          if (m_bookmarksModel->isFolder(currentIndex()))
                              setCurrentIndex(
                                          m_bookmarksModel->addFolder(
                                              m_bookmarksModel->rowCount(
                                                  currentIndex()),
                                              currentIndex()));
                      } else { // create a top level folder
                          setCurrentIndex(
                                  m_bookmarksModel->addFolder(
                                          m_bookmarksModel->rowCount() - 1,
                                          QModelIndex()));
                      }
                  });
    addMenuAction(ChangeFolder, tr("Rename folder"), MI::changeFolder::icon(),
                  [this]{
                      BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
                      edit(currentIndex());
                  });
    m_popup->addSeparator();
    addMenuAction(EditBookmark, tr("Edit bookmark..."),
                  MI::editBookmark::icon(),
                  [this]{
                      QModelIndex const index = currentIndex();
                      BT_ASSERT(m_bookmarksModel->isBookmark(index));
                      auto * const module = m_bookmarksModel->module(index);
                      BtEditBookmarkDialog d(
                              QStringLiteral("%1 (%2)")
                                  .arg(m_bookmarksModel->key(index))
                                  .arg(module
                                       ? module->name()
                                       : QObject::tr("unknown")),
                              index.data().toString(),
                              m_bookmarksModel->description(index),
                              this);
                      if (d.exec() == QDialog::Accepted) {
                          m_bookmarksModel->setData(index, d.titleText());
                          m_bookmarksModel->setDescription(index,
                                                           d.descriptionText());
                      }
                  });
    addMenuAction(SortFolderBookmarks, tr("Sort folder bookmarks..."),
                  MI::sortFolderBookmarks::icon(),
                  [this]{
                      BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
                      m_bookmarksModel->sortItems(currentIndex());
                  });
    addMenuAction(SortAllBookmarks, tr("Sort all bookmarks..."),
                  MI::sortAllBookmarks::icon(),
                  [this]{
                      m_bookmarksModel->sortItems();
                      auto const numRows = m_bookmarksModel->rowCount();
                      if (m_extraItem.row() != numRows - 1) {
                          m_bookmarksModel->removeRow(m_extraItem.row(),
                                                      m_extraItem.parent());
                          if (m_bookmarksModel->insertRows(numRows - 1, 1))
                              m_extraItem =
                                      m_bookmarksModel->index(numRows - 1, 0);
                      }
                  });
    addMenuAction(ImportBookmarks, tr("Import to folder..."),
                  MI::importBookmarks::icon(),
                  [this]{
                      BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
                      QString const fileName =
                              QFileDialog::getOpenFileName(
                                      nullptr,
                                      QObject::tr("Import bookmarks"),
                                      QString(),
                                      fileDialogFilter());
                      if (!fileName.isEmpty())
                          m_bookmarksModel->load(fileName, currentIndex());
                  });
    addMenuAction(ExportBookmarks, tr("Export from folder..."),
                  MI::exportBookmarks::icon(),
                  [this]{
                      BT_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
                      QString const fileName =
                              QFileDialog::getSaveFileName(
                                  nullptr,
                                  QObject::tr("Export Bookmarks"),
                                  QString(),
                                  fileDialogFilter());
                      if (!fileName.isEmpty())
                          m_bookmarksModel->save(fileName, currentIndex());
                  });
    addMenuAction(PrintBookmarks, tr("Print bookmarks..."),
                  MI::printBookmarks::icon(),
                  [this]{
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
                                  tree.emplace_back(
                                              m_bookmarksModel->key(index),
                                              m_bookmarksModel->module(index),
                                              settings);
                              } else if (m_bookmarksModel->isFolder(index)) {
                                  int const numChildren =
                                          m_bookmarksModel->rowCount(index);
                                  for (int i = 0; i < numChildren; i++)
                                      items.append(index.model()->index(i,
                                                                        0,
                                                                        index));
                              }
                          }
                      }
                      BT_ASSERT(!tree.empty());

                      BtPrinter{btConfig().getDisplayOptions(),
                                btConfig().getFilterOptions(),
                                this}.printKeyTree(tree);
                  });
    m_popup->addSeparator();
    addMenuAction(DeleteEntries, tr("Remove selected items..."),
                  MI::deleteItems::icon(),
                  [this]{
                      if (message::showQuestion(
                              this,
                              tr("Delete Items"),
                              tr("Do you really want to delete the selected "
                                 "items and folders?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::Yes)
                          deleteEntries();
                  });

    //--------------------------------------------------------------------------
    // Initialize connections:

    BT_CONNECT(this, &CBookmarkIndex::activated,
               [this](QModelIndex const & index) {
                   /** \note HACK: checking the modifier keys from the last
                             mouseReleaseEvent depends on executing order:
                             mouseReleaseEvent first, then itemClicked signal.*/
                   auto const modifiers = m_mouseReleaseEventModifiers;
                   m_mouseReleaseEventModifiers = Qt::NoModifier;
                   if (modifiers != Qt::NoModifier || !index.isValid())
                       return;

                   // Clicked on a bookmark:
                   if (m_bookmarksModel->isBookmark(index))
                       if (auto * const mod = m_bookmarksModel->module(index))
                           Q_EMIT createReadDisplayWindow(
                                   QList<CSwordModuleInfo *>() << mod,
                                   m_bookmarksModel->key(index));
               });
    BT_CONNECT(this, &CBookmarkIndex::customContextMenuRequested,
               [this](QPoint const & p) {
                   // Enable actions based on the selected items (if any):
                   QModelIndex const i(indexAt(p));
                   QModelIndexList const items(selectedIndexes());
                   if (items.isEmpty()) { // Special handling for no selection:
                       for (int index = ActionBegin; index < ActionEnd; ++index)
                           m_actions[index]->setEnabled(
                                       (index == NewFolder)
                                       || (index == SortAllBookmarks));
                   } else if (items.count() == 1) {
                       // Special handling for one selected item:
                       for (int index = ActionBegin; index < ActionEnd; ++index)
                           m_actions[index]->setEnabled(
                               enableAction(items.at(0),
                                            static_cast<MenuAction>(index)));
                   } else if (!i.isValid()) {
                       // Disable all actions for invalid index:
                       for (int index = ActionBegin; index < ActionEnd; ++index)
                           m_actions[index]->setEnabled(false);
                   } else {
                       // Enable actions depending on the the selected items:
                       for (int index = ActionBegin; index < ActionEnd; ++index)
                           m_actions[index]->setEnabled(
                                   (index == DeleteEntries)
                                   || ((index == PrintBookmarks)
                                       && hasBookmarksRecursively(items)));
                   }
                   m_popup->exec(mapToGlobal(p));
               });
    BT_CONNECT(&m_magTimer, &QTimer::timeout,
               [this]{
                   if (!underMouse())
                       return;

                   /* Update the Mag only if the mouse pointer have been over
                      the same item since the timer was started. */
                   QModelIndex const itemUnderPointer(
                               indexAt(mapFromGlobal(QCursor::pos())));
                   if (itemUnderPointer.isValid()
                       && m_previousEventItem == itemUnderPointer
                       && m_bookmarksModel->isBookmark(itemUnderPointer))
                   {
                       if (CSwordModuleInfo const * const module =
                                   m_bookmarksModel->module(itemUnderPointer))
                       {
                           Q_EMIT magInfoProvided(
                               Rendering::CrossReference,
                               QStringLiteral("%1:%2")
                                       .arg(module->name(),
                                            m_bookmarksModel->key(
                                                itemUnderPointer)));
                       } else {
                           Q_EMIT magInfoProvided(
                                       Rendering::Text,
                                       tr("The work to which the bookmark "
                                          "points to is not installed."));
                       }
                   }
               });

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

/** Creates a drag mime data object for the current selection. */
QMimeData * CBookmarkIndex::dragObject() {
    BTMimeData::ItemList bookmarks;
    for (auto const & widgetItem : selectedIndexes()) {
        if (!widgetItem.isValid())
            break;
        if (m_bookmarksModel->isBookmark(widgetItem)) {
            /* Take care of bookmarks which have no valid module any more, e.g.
               if these were uninstalled: */
            CSwordModuleInfo * const module =
                    m_bookmarksModel->module(widgetItem);
            const QString moduleName = module ? module->name() : QString();
            bookmarks.append({moduleName,
                              m_bookmarksModel->key(widgetItem),
                              m_bookmarksModel->description(widgetItem)});
        }
    }
    return new BTMimeData(std::move(bookmarks));
}

void CBookmarkIndex::dragEnterEvent(QDragEnterEvent * event) {
    if (event->mimeData()->hasFormat(QStringLiteral("BibleTime/Bookmark"))) {
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
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    m_dragMovementPosition = event->pos();
    #else
    m_dragMovementPosition = event->position().toPoint();
    #endif
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
        pix =
            BtIcons::instance().icon_pointing_arrow.pixmap(
                util::tool::mWidth(*this, 1));
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
    auto const connection =
            BT_CONNECT(this, &CBookmarkIndex::collapsed,
                       [this](QModelIndex const & index) { expand(index); });
    auto cleanup = qScopeGuard([&connection]() noexcept
                               { disconnect(connection); });
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    auto const pos = event->pos();
    #else
    auto const pos = event->position().toPoint();
    #endif
    auto const index = indexAt(pos);
    QModelIndex parentIndex;
    int indexUnderParent = 0;

    // Find the place where the drag is dropped
    if (index.isValid()) {
        if (m_bookmarksModel->isFolder(index)) {
            QRect const rect = visualRect(index);
            if (pos.y() > rect.bottom() - (2 * rect.height() / 3)) {
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
                if (pos.y() > rect.bottom() - rect.height() / 2)
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
            auto const & bookmark = mdata->bookmarks().first();
            QString moduleName(bookmark.module());
            QString keyText(bookmark.key());
            QString description(bookmark.description());
            auto * const minfo =
                    CSwordBackend::instance().findModuleByName(
                        std::move(moduleName));
            BT_ASSERT(minfo);

            /// \todo add title
            m_bookmarksModel->addBookmark(indexUnderParent,
                                          parentIndex,
                                          *minfo,
                                          std::move(keyText),
                                          std::move(description));
        }
    } else {
        event->accept();

        bool bookmarksOnly = true;
        bool targetIncluded = false;
        bool moreThanOneFolder = false;

        QModelIndexList const list = selectedIndexes();
        QModelIndexList newList;

        for (auto const & index : list) {
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
                items.append(index.model()->index(i, 0, index));
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

void CBookmarkIndex::deleteEntries() {
    /* We need to use QPersistentModelIndex because after removeRows QModelIndex
       will be invalidated. Need to delete per index because selected indexes
       might be under different parents. */
    QList<QPersistentModelIndex> list;
    for (auto const & i : selectedIndexes())
        list.append(i);

    for (auto const & i : list)
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
