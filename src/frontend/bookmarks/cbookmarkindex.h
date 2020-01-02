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

#ifndef CBOOKMARKINDEX_H
#define CBOOKMARKINDEX_H

#include <QTimer>
#include <QToolTip>
#include <QTreeView>


class BTMimeData;
class BtBookmarksModel;
class CSwordModuleInfo;
class QAction;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QMenu;
class QMouseEvent;
class QPaintEvent;

/**
* The widget which manages all bookmarks.
*
* \author The BibleTime team
*/
class CBookmarkIndex: public QTreeView {

    Q_OBJECT

public: /* Types: */

    enum MenuAction {
        NewFolder = 0,
        ChangeFolder,

        EditBookmark,
        SortFolderBookmarks,
        SortAllBookmarks,
        ImportBookmarks,
        ExportBookmarks,
        PrintBookmarks,

        DeleteEntries,

        ActionCount,
        ActionBegin = NewFolder,
        ActionEnd = ActionCount
    };

public: /* Methods: */

    CBookmarkIndex(QWidget * const parent = nullptr);

signals:

    /** \brief Emitted when a module should be opened. */
    void createReadDisplayWindow(QList<CSwordModuleInfo *>, QString const &);

protected: /* Methods: */

    QMimeData * dragObject();

    void mouseReleaseEvent(QMouseEvent * event) override;
    void paintEvent(QPaintEvent * event) override;
    void dragEnterEvent(QDragEnterEvent * event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;
    void dropEvent(QDropEvent * event) override;
    void dragLeaveEvent(QDragLeaveEvent * event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void leaveEvent(QEvent * event) override;

protected slots:

    /** Prevents annoying folder collapsing while dropping. */
    void expandAutoCollapsedItem(QModelIndex const & index) { expand(index); }

    /** Is called when an item was clicked or activated. */
    void slotExecuted(QModelIndex const & index);

    /** Shows the context menu at the given position. */
    void contextMenu(QPoint const & p);

    /** Adds a new subfolder to the current item. */
    void createNewFolder();

    /** Opens a dialog to change the current folder. */
    void changeFolder();

    /** Exports the bookmarks from the selected folder. */
    void exportBookmarks();

    /** Changes the current bookmark. */
    void editBookmark();

    /** Sorts the current folder bookmarks. */
    void sortFolderBookmarks();

    /** Sorts all bookmarks. */
    void sortAllBookmarks();

    /** Import bookmarks from a file and add them to the selected folder. */
    void importBookmarks();

    /** Deletes the selected entries after user confirmation. */
    void confirmDeleteEntries();

    /** Deletes the selected entries. */
    void deleteEntries();

    /** Prints the selected bookmarks. */
    void printBookmarks();

    /** Slot for the mag update timer. */
    void magTimeout();

private: /* Methods: */

    bool enableAction(QModelIndex const & index,
                      MenuAction const type) const;

    bool hasBookmarksRecursively(QModelIndexList selected) const;

    void showExtraItem();
    void hideExtraItem();

private: /* Fields: */

    QAction * m_actions[ActionCount];

    QMenu * m_popup;
    QTimer m_magTimer;
    int m_mouseReleaseEventModifiers;
    QModelIndex m_previousEventItem;
    QPoint m_dragMovementPosition;
    QPoint m_dragStartPosition;
    QPersistentModelIndex m_extraItem;

    /** Pointer to bookmarks model, added for convenience. */
    BtBookmarksModel * m_bookmarksModel;

};

#endif
