/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/mainindex/bookshelf/btindexitem.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "frontend/mainindex/bookshelf/cbookshelfindex.h"


BTIndexItem::BTIndexItem(QTreeWidgetItem* parent)
        : QTreeWidgetItem(parent) {
    // Intentionally empty
}
