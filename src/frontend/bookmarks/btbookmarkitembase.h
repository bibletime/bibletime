/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKITEMBASE_H
#define BTBOOKMARKITEMBASE_H

#include <QTreeWidgetItem>


class BtBookmarkItemBase: public QTreeWidgetItem {

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

            ActionBegin = NewFolder,
            ActionEnd = DeleteEntries
        };

    public: /* Methods: */

        inline BtBookmarkItemBase(QTreeWidgetItem *parent = 0)
            : QTreeWidgetItem(parent) {}

        /** Returns true if the given action should be enabled in the popup menu. */
        virtual bool enableAction(MenuAction action) = 0;

        /** Rename the item. */
        virtual void rename() = 0;

        /** Update the item (icon etc.) after creating or changing it. */
        virtual void update() = 0;

};

#endif

