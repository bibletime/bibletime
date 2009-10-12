/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CDRAGDROP_H
#define CDRAGDROP_H

#include <QMimeData>
#include <QString>
#include <QStringList>


/** Class which represents a bookmark.
* Includes key, module name and description, all QStrings which have getter methods.
* Can be created only through BTMimeData object.
*/
class BookmarkItem {
    public:
        /** Returns the key */
        const QString& key() const {
            return m_key;
        } ;
        /** Returns the module name */
        const QString& module() const {
            return m_moduleName;
        } ;
        /** Returns the bookmark description */
        const QString& description() const {
            return m_description;
        };
    protected:
        friend class BTMimeData;
        BookmarkItem(QString, QString, QString);
        QString m_moduleName; //the module which is used by this item
        QString m_key; //the key of a bookmark
        QString m_description; //the description of a bookmark
};


/**
* Represents the Drag'n'Drop data.
* Inherits QMimeData as described in its documentation,
* "storing custom data in a QMimeData object": approach 3.
* Any custom data may be added and extracted as with QMimeData, but this class
* includes also a list of bookmark items.
* For further documentation see http://doc.trolltech.com/4.3/dnd.html
*/
class BTMimeData : public QMimeData {
        Q_OBJECT
    public:

        /** Type for bookmark item list. Usage: BTMimeData::ItemList. */
        typedef QList<BookmarkItem> ItemList;

        /** Creates a new empty BTMimeData. */
        BTMimeData();

        virtual ~BTMimeData();

        /** Creates a new BTMimeData.
        * Creates a new bookmark item and appends it to the list.
        * MIME type "BibleTime/Bookmark" is added.
        * Bookmarks can not be reached by data() method, use bookmark() or bookmarks() instead.
        */
        BTMimeData(QString module, QString key, QString description);
        /** Creates a new BTMimeData, setting the text MIME type (see QMimeData::setText()). */
        BTMimeData(QString text);


        /** Appends a new bookmark item into the list.
        * Creates the item using the arguments.
        * MIME type "BibleTime/Bookmark" is added.
        */
        virtual void appendBookmark(QString module, QString key, QString description);
        /** Returns the bookmarks list. */
        virtual const ItemList& bookmarks() const {
            return m_bookmarkList;
        } ;
        /** Returns the first bookmark item in the list. */
        virtual const BookmarkItem& bookmark() const;

    private:
        ItemList m_bookmarkList;
};

#endif
