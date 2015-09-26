/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDRAGDROP_H
#define CDRAGDROP_H

#include <QMimeData>


/**
  Class which represents a bookmark. Includes key, module name and description,
  all QStrings which have getter methods. Can be created only through
  BTMimeData object.
*/
class BookmarkItem {

    friend class BTMimeData;

    public: /* Methods: */

        /** \returns the key */
        inline const QString &key() const {
            return m_key;
        }

        /** \returns the module name */
        inline const QString &module() const {
            return m_moduleName;
        }

        /** \returns the bookmark description */
        inline const QString &description() const {
            return m_description;
        }

    protected: /* Methods: */

        /** Creates a new bookmark item. */
        BookmarkItem(const QString &module, const QString &key,
                     const QString &description)
            : m_moduleName(module), m_key(key), m_description(description) {}

    protected: /* Fields: */

        QString m_moduleName;  /**< The module which is used by this item. */
        QString m_key;         /**< The key of a bookmark. */
        QString m_description; /**< The description of a bookmark. */

};


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
        typedef QList<BookmarkItem> ItemList;

    public: /* Methods: */

        /** Creates a new empty BTMimeData. */
        inline BTMimeData() {}

        /** Creates a new BTMimeData.
        * Creates a new bookmark item and appends it to the list.
        * MIME type "BibleTime/Bookmark" is added.
        * Bookmarks can not be reached by data() method, use bookmark() or bookmarks() instead.
        */
        inline BTMimeData(const QString &module,
                          const QString &key,
                          const QString &description)
        {
            appendBookmark(module, key, description);
        }

        /** Creates a new BTMimeData, setting the text MIME type (see QMimeData::setText()). */
        inline BTMimeData(const QString &text) {
            setText(text);
        }


        /** Appends a new bookmark item into the list.
        * Creates the item using the arguments.
        * MIME type "BibleTime/Bookmark" is added.
        */
        inline void appendBookmark(const QString &module,
                                   const QString &key,
                                   const QString &description)
        {
            m_bookmarkList.append(BookmarkItem(module, key, description));
            setData("BibleTime/Bookmark", QByteArray());
        }

        /** \returns the bookmarks list. */
        const ItemList &bookmarks() const {
            return m_bookmarkList;
        }

        /** \returns the first bookmark item in the list. */
        const BookmarkItem &bookmark() const {
            return m_bookmarkList.first();
        }

    private: /* Fields: */

        ItemList m_bookmarkList;

};

#endif
