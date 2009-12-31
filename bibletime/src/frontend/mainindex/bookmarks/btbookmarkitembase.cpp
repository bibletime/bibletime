/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/mainindex/bookmarks/btbookmarkitembase.h"

#include <QDropEvent>
#include <QTreeWidgetItem>
#include "frontend/mainindex/bookmarks/cbookmarkindex.h"


BtBookmarkItemBase::BtBookmarkItemBase() {}

BtBookmarkItemBase::BtBookmarkItemBase(QTreeWidgetItem* parent)
        : QTreeWidgetItem(parent) {}

CBookmarkIndex* BtBookmarkItemBase::bookmarkWidget() const {
    return dynamic_cast<CBookmarkIndex*>(treeWidget());
}

// void BtBookmarkItemBase::dropped(QDropEvent* e)
// {
//
// }
//
// void BtBookmarkItemBase::addPreviousSibling(BtBookmarkItemBase* item)
// {
//
// }
//
// void BtBookmarkItemBase::addNextSibling(BtBookmarkItemBase* item)
// {
//
// }
