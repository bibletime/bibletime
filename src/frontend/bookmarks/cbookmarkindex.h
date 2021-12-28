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

#pragma once

#include <QTimer>
#include <QToolTip>
#include <QTreeView>
#include "../../backend/rendering/btinforendering.h"


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

public: // types:

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

public: // methods:

    CBookmarkIndex(QWidget * const parent = nullptr);

Q_SIGNALS:

    /** \brief Emitted when a module should be opened. */
    void createReadDisplayWindow(QList<CSwordModuleInfo *>, QString const &);
    void magInfoProvided(Rendering::InfoType const, QString const & data);

protected: // methods:

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

private: // methods:

    void deleteEntries();
    bool enableAction(QModelIndex const & index,
                      MenuAction const type) const;

    bool hasBookmarksRecursively(QModelIndexList selected) const;

    void showExtraItem();
    void hideExtraItem();

private: // fields:

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
