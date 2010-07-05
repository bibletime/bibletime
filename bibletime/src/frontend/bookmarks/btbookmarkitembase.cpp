/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookmarks/btbookmarkitembase.h"


BtBookmarkItemBase::BtBookmarkItemBase() {
    // Intentionally empty
}

BtBookmarkItemBase::BtBookmarkItemBase(QTreeWidgetItem *parent)
        : QTreeWidgetItem(parent)
{
    // Intentionally empty
}
