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

#ifndef BIBLETIME_BTMIMEDATA_H
#define BIBLETIME_BTMIMEDATA_H

#include <QMimeData>

#include <QList>
#include <QString>
#include "BookmarkItem.h"


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

public: /* Types: */

    /** Type for bookmark item list. Usage: BTMimeData::ItemList. */
    using ItemList = QList<BookmarkItem>;

public: /* Methods: */

    /** Creates a new empty BTMimeData. */
    BTMimeData();

    /** Creates a new BTMimeData.
    * Creates a new bookmark item and appends it to the list.
    * MIME type "BibleTime/Bookmark" is added.
    * Bookmarks can not be reached by data() method, use bookmark() or bookmarks() instead.
    */
    BTMimeData(QString const & module,
               QString const & key,
               QString const & description);

    /** Creates a new BTMimeData, setting the text MIME type (see QMimeData::setText()). */
    BTMimeData(QString const & text);


    /** Appends a new bookmark item into the list.
    * Creates the item using the arguments.
    * MIME type "BibleTime/Bookmark" is added.
    */
    void appendBookmark(QString const & module,
                        QString const & key,
                        QString const & description);

    /** \returns the bookmarks list. */
    ItemList const & bookmarks() const noexcept { return m_bookmarkList; }

    /** \returns the first bookmark item in the list. */
    BookmarkItem const & bookmark() const { return m_bookmarkList.first(); }

private: /* Fields: */

    ItemList m_bookmarkList;

};

#endif /* BIBLETIME_BTMIMEDATA_H */
