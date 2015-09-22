/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDrag>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScopedPointer>
#include <QTimer>
#include <QToolTip>

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/btbookmarksmodel.h"
#include "backend/managers/referencemanager.h"
#include "bibletime.h"
#include "bibletimeapp.h"
#include "frontend/cdragdrop.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cprinter.h"
#include "frontend/messagedialog.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/bookmarks/bteditbookmarkdialog.h"
#include "frontend/bookmarks/cbookmarkindex.h"
#include "util/bticons.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"


CBookmarkIndex::CBookmarkIndex(QWidget *parent)
        : QTreeView(parent)
        , m_magTimer(this)
        , m_bookmarksModel(0)
{
    setMouseTracking(true);
    m_magTimer.setSingleShot(true);
    m_magTimer.setInterval(btConfig().value<int>("GUI/magDelay", 400));
    setContextMenuPolicy(Qt::CustomContextMenu);
	setHeaderHidden(true);
    initView();
    initConnections();
    initTree();
}

CBookmarkIndex::~CBookmarkIndex() {
    ;
}

/** Initializes the view. */
void CBookmarkIndex::initView() {
    setHeaderHidden(true);

    setFocusPolicy(Qt::WheelFocus);

    //d'n'd related settings
    setDragEnabled( true );
    setAcceptDrops( true );
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
    m_popup = new QMenu(viewport());
    m_popup->setTitle(tr("Bookmarks"));

    m_actions[NewFolder] = newQAction(tr("New folder"), CResMgr::mainIndex::newFolder::icon(), 0, this, SLOT(createNewFolder()), this);
    m_actions[ChangeFolder] = newQAction(tr("Rename folder"), CResMgr::mainIndex::changeFolder::icon(), 0, this, SLOT(changeFolder()), this);

    m_actions[EditBookmark] = newQAction(tr("Edit bookmark..."), CResMgr::mainIndex::editBookmark::icon(), 0, this, SLOT(editBookmark()), this);
    /// \todo Add icons for sorting bookmarks
    m_actions[SortFolderBookmarks] = newQAction(tr("Sort folder bookmarks..."), CResMgr::mainIndex::sortFolderBookmarks::icon(), 0, this, SLOT(sortFolderBookmarks()), this);
    m_actions[SortAllBookmarks] = newQAction(tr("Sort all bookmarks..."), CResMgr::mainIndex::sortAllBookmarks::icon(), 0, this, SLOT(sortAllBookmarks()), this);
    m_actions[ImportBookmarks] = newQAction(tr("Import to folder..."), CResMgr::mainIndex::importBookmarks::icon(), 0, this, SLOT(importBookmarks()), this);
    m_actions[ExportBookmarks] = newQAction(tr("Export from folder..."), CResMgr::mainIndex::exportBookmarks::icon(), 0, this, SLOT(exportBookmarks()), this);
    m_actions[PrintBookmarks] = newQAction(tr("Print bookmarks..."), CResMgr::mainIndex::printBookmarks::icon(), 0, this, SLOT(printBookmarks()), this);

    m_actions[DeleteEntries] = newQAction(tr("Remove selected items..."), CResMgr::mainIndex::deleteItems::icon(), 0, this, SLOT(confirmDeleteEntries()), this);


    //fill the popup menu itself
    m_popup->addAction(m_actions[NewFolder]);
    m_popup->addAction(m_actions[ChangeFolder]);
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    m_popup->addAction(separator);
    m_popup->addAction(m_actions[EditBookmark]);
    m_popup->addAction(m_actions[SortFolderBookmarks]);
    m_popup->addAction(m_actions[SortAllBookmarks]);
    m_popup->addAction(m_actions[ImportBookmarks]);
    m_popup->addAction(m_actions[ExportBookmarks]);
    m_popup->addAction(m_actions[PrintBookmarks]);
    separator = new QAction(this);
    separator->setSeparator(true);
    m_popup->addAction(separator);
    m_popup->addAction(m_actions[DeleteEntries]);
}

/** Convenience function for creating a new QAction.
* Should be replaced with something better; it was easier to make a new function
* than to modify all QAction constructors.
*/
QAction* CBookmarkIndex::newQAction(QString const & text,
                                    QIcon const & pix,
                                    int const /*shortcut*/,
                                    QObject const * receiver,
                                    char const * slot,
                                    QObject * parent)
{
    QAction * const action = new QAction(pix, text, parent);
    QObject::connect(action, SIGNAL(triggered()), receiver, slot);
    return action;
}

/** Initialize the SIGNAL<->SLOT connections */
void CBookmarkIndex::initConnections() {
    bool ok;
    ok = connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(slotExecuted(const QModelIndex &)));

    Q_ASSERT(ok);
    ok = connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                 SLOT(contextMenu(const QPoint&)));
    Q_ASSERT(ok);
    ok = connect(&m_magTimer, SIGNAL(timeout()), this, SLOT(magTimeout()));
    Q_ASSERT(ok);
}


/**
* Hack to get single click and selection working. See slotExecuted.
*/
void CBookmarkIndex::mouseReleaseEvent(QMouseEvent* event) {
    m_mouseReleaseEventModifiers = event->modifiers();
    QTreeView::mouseReleaseEvent(event);
}

/** Called when an item is clicked with mouse or activated with keyboard. */
void CBookmarkIndex::slotExecuted( const QModelIndex &index ) {
    //HACK: checking the modifier keys from the last mouseReleaseEvent
    //depends on executing order: mouseReleaseEvent first, then itemClicked signal
    int modifiers = m_mouseReleaseEventModifiers;
    m_mouseReleaseEventModifiers = Qt::NoModifier;
    if (modifiers != Qt::NoModifier) {
        return;
    }

    if(!index.isValid()) return;

    //clicked on a bookmark
    if (m_bookmarksModel->isBookmark(index)) {
        if (CSwordModuleInfo * mod = m_bookmarksModel->module(index)) {
            emit createReadDisplayWindow(QList<CSwordModuleInfo*>() << mod,
                                         m_bookmarksModel->key(index));
        }
    }
}

/** Creates a drag mime data object for the current selection. */
QMimeData* CBookmarkIndex::dragObject() {
    BTMimeData* mimeData = new BTMimeData;

    Q_FOREACH(QModelIndex const & widgetItem, selectedIndexes()) {
        if (!widgetItem.isValid())
            break;
        if (m_bookmarksModel->isBookmark(widgetItem)) {
            //take care of bookmarks which have no valid module any more, e.g. if it was uninstalled
            CSwordModuleInfo * module = m_bookmarksModel->module(widgetItem);
            const QString moduleName = module ? module->name() : QString();
            mimeData->appendBookmark(moduleName, m_bookmarksModel->key(widgetItem),
                                     m_bookmarksModel->description(widgetItem));
        }
    }
    return mimeData;
}

void CBookmarkIndex::dragEnterEvent( QDragEnterEvent* event ) {
    setState(QAbstractItemView::DraggingState);
    QTreeView::dragEnterEvent(event);
    if (event->source() == this || event->mimeData()->hasFormat("BibleTime/Bookmark")) {
        event->acceptProposedAction();
    }
}


void CBookmarkIndex::dragMoveEvent( QDragMoveEvent* event ) {
    // do this first, otherwise the event may be ignored
    QTreeView::dragMoveEvent(event);

    event->acceptProposedAction();
    event->accept();

    // do this to paint the arrow
    m_dragMovementPosition = event->pos();
    viewport()->update();

}

void CBookmarkIndex::dragLeaveEvent( QDragLeaveEvent* ) {
    setState(QAbstractItemView::NoState); // not dragging anymore
    viewport()->update(); // clear the arrow
}


void CBookmarkIndex::paintEvent(QPaintEvent* event) {
    namespace DU = util::directory;

    static QPixmap pix;
    static int halfPixHeight;
    static bool arrowInitialized = false;

    // Initialize the static variables, including the arrow pixmap
    if (!arrowInitialized) {
        arrowInitialized = true;
        int arrowSize = util::tool::mWidth(this, 1);
        pix = BtIcons::instance().icon_pointing_arrow.pixmap(arrowSize);
        halfPixHeight = pix.height() / 2;
    }

    // Do the normal painting first
    QTreeView::paintEvent(event);

    // Paint the arrow if the drag is going on
    if (QAbstractItemView::DraggingState == state()) {
        bool rtol = QApplication::isRightToLeft();

        QPainter painter(this->viewport());
        QModelIndex index = indexAt(m_dragMovementPosition);
        bool isFolder = m_bookmarksModel->isFolder(index);
        bool isBookmark = m_bookmarksModel->isBookmark(index);

        // Find the place for the arrow
        QRect rect = visualRect(index);
        int xCoord = rtol ? rect.right() : rect.left();
        int yCoord;
        if (isFolder) {
            if (m_dragMovementPosition.y() > rect.bottom() - (2* rect.height() / 3) ) {
                yCoord = rect.bottom() - halfPixHeight; // bottom
                xCoord = rtol ? (xCoord - indentation()) : (xCoord + indentation());
            }
            else {
                yCoord = rect.top() - halfPixHeight - 1; // top
            }

        }
        else {
            if (isBookmark) {
                if (m_dragMovementPosition.y() > rect.bottom() - rect.height() / 2) {
                    yCoord = rect.bottom() - halfPixHeight; // bottom
                }
                else {
                    yCoord = rect.top() - halfPixHeight - 1; // top
                }
            }
            else {
                if (index.isValid()) { // the extra item
                    yCoord = rect.top() - halfPixHeight - 1;
                }
                else { // empty area
                    rect = visualRect(m_extraItem);
                    yCoord = rect.top() - halfPixHeight - 1;
                    xCoord = rtol ? rect.right() : rect.left();
                }
            }
        }

        painter.drawPixmap(xCoord, yCoord, pix);
    }
}


void CBookmarkIndex::dropEvent( QDropEvent* event ) {

    //setState(QAbstractItemView::NoState);
    // Try to prevent annoying timed autocollapsing. Remember to disconnect before return.
    QObject::connect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(expandAutoCollapsedItem(const QModelIndex &)));
    QModelIndex index = indexAt(event->pos());
    QModelIndex parentIndex;
    int indexUnderParent = 0;

    // Find the place where the drag is dropped
    if (index.isValid()) {
        QRect rect = visualRect(index);

        if (m_bookmarksModel->isFolder(index)) {
            if (event->pos().y() > rect.bottom() - (2* rect.height() / 3) ) {
                parentIndex = index;
            }
            else {
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current folder
            }
        }
        else {
            if (m_bookmarksModel->isBookmark(index)) {
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current bookmark
                if (event->pos().y() > rect.bottom() - rect.height() / 2) {
                    indexUnderParent++; // after the current bookmark
                }
            }
            else { // item is the extra item
                parentIndex = index.parent();
                indexUnderParent = index.row(); // before the current bookmark
            }
        }

    }
    else { // no item under event point: drop to the end
        //parentItem = invisibleRootItem();
        indexUnderParent = m_bookmarksModel->rowCount() - 1; //parentItem->childCount() - 1;
    }


    if ( event->source() == this ) {
        event->accept();

        bool bookmarksOnly = true;
        bool targetIncluded = false;
        bool moreThanOneFolder = false;

        QModelIndexList list(selectedIndexes());
        QModelIndexList newList;

        Q_FOREACH(QModelIndex const & index, list) {
            if (m_bookmarksModel->isFolder(index)) {
                bookmarksOnly = false;
                if (list.count() > 1) { // only one item allowed if a folder is selected
                    moreThanOneFolder = true;
                    break;
                }
                if (m_bookmarksModel->hasDescendant(index, parentIndex)) { // dropping to self or descendand not allowed
                    targetIncluded = true;
                    break;
                }
            }
            else {
                newList.append(index);
            }
        }

        if (!bookmarksOnly && list.count() == 1) {
            newList.append(list[0]); // list contain only one folder item
        }
        else if (!bookmarksOnly && list.count() > 1) {
            moreThanOneFolder = true; // wrong amount of items
        }

        if (moreThanOneFolder) {
            QToolTip::showText(QCursor::pos(), tr("Can drop only bookmarks or one folder"));
            QObject::disconnect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(expandAutoCollapsedItem(const QModelIndex &)));
            return;
        }
        if (targetIncluded) {
            QToolTip::showText(QCursor::pos(), tr("Can't drop folder into the folder itself or into its subfolder"));
            QObject::disconnect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(expandAutoCollapsedItem(const QModelIndex &)));
            return;
        }

        // Ask whether to copy or move with a popup menu
        QMenu * dropPopupMenu = new QMenu(this);
        QAction * copy = dropPopupMenu->addAction(tr("Copy"));
        QAction * move = dropPopupMenu->addAction(tr("Move"));
        QAction * dropAction = dropPopupMenu->exec(QCursor::pos());

        if (dropAction == copy) {
            m_bookmarksModel->copyItems(indexUnderParent, parentIndex, newList);
        }
        else if (dropAction == move) {
            m_bookmarksModel->copyItems(indexUnderParent, parentIndex, newList);
            deleteEntries();
        }
        else  {
            QObject::disconnect(this, SIGNAL(collapsed(const QModelIndex &)),
                                this, SLOT(expandAutoCollapsedItem(const QModelIndex &)));
            return; // user canceled
        }
    }
    else {
        createBookmarkFromDrop(event, parentIndex, indexUnderParent);
    }

    QObject::disconnect(this, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(expandAutoCollapsedItem(const QModelIndex &)));
    setState(QAbstractItemView::NoState);
}


void CBookmarkIndex::createBookmarkFromDrop(QDropEvent* event, const QModelIndex &parentItem, int indexInParent) {
    //take the bookmark data from the mime source
    const BTMimeData* mdata = dynamic_cast<const BTMimeData*>(event->mimeData());
    if (mdata) {
        //create the new bookmark
        QString const moduleName(mdata->bookmark().module());
        QString const keyText(mdata->bookmark().key());
        QString const description(mdata->bookmark().description());
        CSwordModuleInfo * minfo =
                CSwordBackend::instance()->findModuleByName(moduleName);
        Q_ASSERT(minfo);

        /// \todo add title
        m_bookmarksModel->addBookmark(indexInParent,
                                      parentItem,
                                      *minfo,
                                      keyText,
                                      description);
    }
}

bool CBookmarkIndex::enableAction(const QModelIndex &index, CBookmarkIndex::MenuAction type) const
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
            return hasBookmarksRecursively(QModelIndexList() << index);
        case EditBookmark:
            return m_bookmarksModel->isBookmark(index);
        default:
            return false;
    }
}

bool CBookmarkIndex::hasBookmarksRecursively(QModelIndexList items)
        const
{
    while (!items.empty()) {
        QModelIndex index(items.takeFirst());
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


/** Load the tree from file */
void CBookmarkIndex::initTree() {
    m_bookmarksModel = new BtBookmarksModel(this);
    setModel(m_bookmarksModel);

    // add the invisible extra item at the end
    if(m_bookmarksModel->insertRows(m_bookmarksModel->rowCount(), 1))
        m_extraItem = m_bookmarksModel->index(m_bookmarksModel->rowCount() - 1, 0);
    showExtraItem();
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
void CBookmarkIndex::contextMenu(const QPoint& p) {
    // Enable actions based on the selected items (if any):
    QModelIndex const i(indexAt(p));
    QModelIndexList const items(selectedIndexes());
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
    QModelIndexList selected = selectedIndexes();
    QModelIndex r;
    if (selected.count() > 0) {
        if(m_bookmarksModel->isFolder(currentIndex()))
            r = m_bookmarksModel->addFolder(m_bookmarksModel->rowCount(currentIndex()), currentIndex());
    }
    else {
        // create a top level folder
        r = m_bookmarksModel->addFolder(m_bookmarksModel->rowCount() - 1, QModelIndex());
    }
    setCurrentIndex(r);
}

/** Opens a dialog to change the current folder. */
void CBookmarkIndex::changeFolder() {
    Q_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    edit(currentIndex());
}

/** Edits the current bookmark. */
void CBookmarkIndex::editBookmark() {
    QModelIndex index(currentIndex());
    Q_ASSERT(m_bookmarksModel->isBookmark(index));
    CSwordModuleInfo * module(m_bookmarksModel->module(index));
    BtEditBookmarkDialog d(QString::fromLatin1("%1 (%2)").arg(m_bookmarksModel->key(index)).arg(
                               module ? module->name() : QObject::tr("unknown")),
                           index.data().toString(),
                           m_bookmarksModel->description(index), this);

    if (d.exec() == QDialog::Accepted) {
        m_bookmarksModel->setData(index, d.titleText());
        m_bookmarksModel->setDescription(index, d.descriptionText());
    }
}

/** Sorts the current folder bookmarks. */
void CBookmarkIndex::sortFolderBookmarks() {
    Q_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    m_bookmarksModel->sortItems(currentIndex());
}

/** Sorts all bookmarks. */
void CBookmarkIndex::sortAllBookmarks() {
    m_bookmarksModel->sortItems();
    if(m_extraItem.row() != m_bookmarksModel->rowCount() - 1) {
        m_bookmarksModel->removeRow(m_extraItem.row(), m_extraItem.parent());
        if(m_bookmarksModel->insertRows(m_bookmarksModel->rowCount(), 1))
            m_extraItem = m_bookmarksModel->index(m_bookmarksModel->rowCount() - 1, 0);
    }
}

/** Exports the bookmarks being in the selected folder. */
void CBookmarkIndex::exportBookmarks() {
    Q_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
    QString fileName  = QFileDialog::getSaveFileName(0, QObject::tr("Export Bookmarks"), "", filter);

    if (!fileName.isEmpty()) {
        m_bookmarksModel->save(fileName, currentIndex());
    };
}

/** Import bookmarks from a file and add them to the selected folder. */
void CBookmarkIndex::importBookmarks() {
    Q_ASSERT(m_bookmarksModel->isFolder(currentIndex()));
    QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Import bookmarks"), "", filter);
    if (!fileName.isEmpty()) {
        m_bookmarksModel->load(fileName, currentIndex());
    }
}

/** Prints the selected bookmarks. */
void CBookmarkIndex::printBookmarks() {
    Q_ASSERT(!hasBookmarksRecursively(selectedIndexes()));
    Printing::CPrinter::KeyTree tree;
    {
        Printing::CPrinter::KeyTreeItem::Settings const settings(
                false,
                Printing::CPrinter::KeyTreeItem::Settings::CompleteShort);
        QModelIndexList items(selectedIndexes());
        while (!items.empty()) {
            QModelIndex const index(items.takeFirst());
            if (m_bookmarksModel->isBookmark(index)) {
                typedef Printing::CPrinter::KeyTreeItem KTI;
                tree.append(new KTI(m_bookmarksModel->key(index),
                                m_bookmarksModel->module(index),
                                settings));
            } else if (m_bookmarksModel->isFolder(index)) {
                int const numChildren = m_bookmarksModel->rowCount(index);
                for (int i = 0; i < numChildren; i++)
                    items.append(index.child(i, 0));
            }
        }
    }
    Q_ASSERT(!tree.isEmpty());

    QScopedPointer<Printing::CPrinter>(
            new Printing::CPrinter(
                    this,
                    btConfig().getDisplayOptions(),
                    btConfig().getFilterOptions()))->printKeyTree(tree);
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
    QDrag * const drag = new QDrag(this);
    drag->setMimeData(mData);
    drag->exec();

    viewport()->update(); // because of the arrow
}

/* Saves the bookmarks to the default bookmarks file. */
void CBookmarkIndex::saveBookmarks() {
    m_bookmarksModel->save();
}

void CBookmarkIndex::mouseMoveEvent(QMouseEvent* event) {
    /* Restart the mag timer if we have moved to another item and shift was not
       pressed: */
    QModelIndex const itemUnderPointer(indexAt(event->pos()));
    if (itemUnderPointer.isValid()
        && (itemUnderPointer != m_previousEventItem)
        && !(event->modifiers() & Qt::ShiftModifier))
        m_magTimer.start();
    m_previousEventItem = itemUnderPointer;

    if (!itemUnderPointer.isValid()
        || itemUnderPointer == m_extraItem)
    {
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
    QModelIndex const itemUnderPointer(indexAt(mapFromGlobal(QCursor::pos())));
    if (itemUnderPointer.isValid()
        && m_previousEventItem == itemUnderPointer
        && m_bookmarksModel->isBookmark(itemUnderPointer))
    {
        Q_ASSERT(BibleTime::instance()->infoDisplay());
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
