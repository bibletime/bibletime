/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKITEMBASE_H
#define BTBOOKMARKITEMBASE_H


#include <QTreeWidgetItem>
#include <QString>
#include <QMimeData>
#include <QDropEvent>

class CBookmarkIndex;


class BtBookmarkItemBase : public QTreeWidgetItem {
    public:

        enum MenuAction {
            NewFolder = 0,
            ChangeFolder,

            ChangeBookmark,
            ImportBookmarks,
            ExportBookmarks,
            PrintBookmarks,

            DeleteEntries,

            ActionBegin = NewFolder,
            ActionEnd = DeleteEntries
        };

        /** Where to drop/create item(s): above, below or inside an item.*/
        enum Location {Above, Below, Inside};

        BtBookmarkItemBase();
        BtBookmarkItemBase(QTreeWidgetItem* parent);
        virtual ~BtBookmarkItemBase() {}

        virtual QString toolTip() = 0;
        virtual CBookmarkIndex* bookmarkWidget() const;

        /** Returns true if the given action should be enabled in the popup menu. */
        virtual bool enableAction( MenuAction action ) = 0;

        /** Rename the item. */
        virtual void rename() = 0;

        /** Update the item (icon etc.) after creating or changing it. */
        virtual void update() {}

};

#endif

