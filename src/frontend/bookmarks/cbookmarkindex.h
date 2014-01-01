/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBOOKMARKINDEX_H
#define CBOOKMARKINDEX_H

#include "frontend/bookmarks/btbookmarkitembase.h"

#include <QList>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolTip>
#include "frontend/displaywindow/cdisplaywindow.h"


class BTMimeData;
class CSearchDialog;
class CSwordModuleInfo;
class QAction;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QMenu;
class QMouseEvent;
class QPaintEvent;
class QWidget;

/**
* The widget which manages all bookmarks.
* @author The BibleTime team
*/
class CBookmarkIndex : public QTreeWidget {
        Q_OBJECT
    public:
        CBookmarkIndex(QWidget *parent);
        virtual ~CBookmarkIndex();

        void initTree();

        /**
        * Saves the bookmarks to disk
        */
        void saveBookmarks();

    signals:
        /**
        * Is emitted when a module should be opened,
        */
        void createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& );

    public slots:

        /**
         * Indicates a need to save the bookmarks.
         * This is needed to provide a way for a bookmarkitem stored in the
         * treeWidget to inform us that it has been modified, namely its
         * description text.  It only sets a dirty-bit so we don't execute many
         * consecutive saves.
         */
        void needToSaveBookmarks();
        void needToSaveBookmarks(QTreeWidgetItem* treeItem);


    protected: // Protected methods

        /** A hack to get the modifiers. */
        virtual void mouseReleaseEvent(QMouseEvent* event);

        /** Needed to paint an drag pointer arrow. */
        virtual void paintEvent(QPaintEvent* event);

        /** Initialize the SIGNAL<->SLOT connections. */
        void initConnections();

        /** Returns the drag object for the current selection. */
        virtual QMimeData* dragObject();

        /**
        * D'n'd methods are reimplementations from QTreeWidget or its ancestors.
        * In these we handle creating, moving and copying bookmarks with d'n'd.
        */
        virtual void dragEnterEvent( QDragEnterEvent* event );
        virtual void dragMoveEvent( QDragMoveEvent* event );
        virtual void dropEvent( QDropEvent* event );
        virtual void dragLeaveEvent( QDragLeaveEvent* event );

        /** Returns the correct action object for the given type of action. */
        QAction* action( BtBookmarkItemBase::MenuAction type ) const;

        /** Reimplementation from QAbstractItemView. Takes care of movable items. */
        virtual void startDrag(Qt::DropActions supportedActions);


        /** Handle mouse moving (mag updates) */
        virtual void mouseMoveEvent(QMouseEvent* event);


    protected slots:

        /** Prevents annoying folder collapsing while dropping. */
        void expandAutoCollapsedItem(QTreeWidgetItem* i) {
            expandItem(i);
        }

        /** Is called when an item was clicked or activated. */
        void slotExecuted( QTreeWidgetItem* );

        /** Shows the context menu at the given position. */
        void contextMenu(const QPoint&);

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

        /** Helps with the extra item. */
        void slotItemEntered(QTreeWidgetItem*, int);

        /** Import bookmarks from a file and add them to the selected folder. */
        void importBookmarks();

        /** Deletes the selected entries. */
        void deleteEntries(bool confirm = true);

        /** Prints the selected bookmarks. */
        void printBookmarks();

        /** Slot for the mag update timer. */
        void magTimeout();

    private:

        /** Initializes the view. */
        void initView();

        /** Convenience function for creating a new action. */
        QAction* newQAction(const QString& text, const QString& pix, int shortcut, const QObject* receiver, const char* slot, QObject* parent);

        /**
        * Returns true if more than one entry is supported by this action type.
        * Returns false for actions which support only one entry.
        */
        bool isMultiAction( const BtBookmarkItemBase::MenuAction type ) const;

        /** A helper function for d'n'd which creates a new bookmark item when drop happens. */
        void createBookmarkFromDrop(QDropEvent* event, QTreeWidgetItem* parentItem, int indexInParent);

        /**
        * Returns a list of new items created from the selection.
        * Sets flags which indicate whether the selection was legal for dropping.
        */
        QList<QTreeWidgetItem*> addItemsToDropTree(QTreeWidgetItem* target, bool& bookmarksOnly, bool& targetIncluded, bool& moreThanOneFolder);

        struct Actions {
            QAction* newFolder;
            QAction* changeFolder;

            QAction* editBookmark;
        QAction* sortFolderBookmarks;
        QAction* sortAllBookmarks;
            QAction* importBookmarks;
            QAction* exportBookmarks;
            QAction* printBookmarks;

            QAction* deleteEntries;
        }
        m_actions;

        QMenu* m_popup;
        QTimer m_magTimer;
        int m_mouseReleaseEventModifiers;
        QTreeWidgetItem* m_previousEventItem;
        QPoint m_dragMovementPosition;
        QPoint m_dragStartPosition;
        QTreeWidgetItem* m_extraItem;

        // The following is for managing saving bookmarks.  It uses a QTimer to
        // determine whether the bookmarks should be saved.  This may seem like
        // a hassle, but it is to prevent many saves from being executed at a
        // time.

        /** Flag indicating that bookmarks have been modified. */
        bool m_bookmarksModified;
        QTimer bookmarkSaveTimer;

    private slots:
        /**
         * Saves the bookmarks.
         * It checks m_bookmarksModified and resets it at the end. It should be
         * connected to a timer that periodically calls this. */
        void considerSavingBookmarks();
};

#endif
