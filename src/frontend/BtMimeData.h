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

#include <QMimeData>

#include <QList>
#include <QObject>
#include <QString>
#include "BookmarkItem.h" // IWYU pragma: keep for Qt's moc


/**
* Represents the Drag'n'Drop data.
* Inherits QMimeData as described in its documentation,
* "storing custom data in a QMimeData object": approach 3.
* Any custom data may be added and extracted as with QMimeData, but this class
* includes also a list of bookmark items.
* For further documentation see http://doc.trolltech.com/4.3/dnd.html
*/
class BTMimeData: public QMimeData {

    Q_OBJECT

public: // types:

    /** Type for bookmark item list. Usage: BTMimeData::ItemList. */
    using ItemList = QList<BookmarkItem>;

public: // methods:

    BTMimeData(ItemList bookmarks);

    /** \returns the bookmarks list. */
    ItemList const & bookmarks() const noexcept { return m_bookmarkList; }

private: // fields:

    ItemList m_bookmarkList;

};
