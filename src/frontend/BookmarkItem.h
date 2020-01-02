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

#ifndef BIBLETIME_BOOKMARKITEM_H
#define BIBLETIME_BOOKMARKITEM_H

#include <QString>


class BTMimeData;

/**
  Class which represents a bookmark. Includes key, module name and description,
  all QStrings which have getter methods. Can be created only through
  BTMimeData object.
*/
class BookmarkItem {

friend class BTMimeData;

public: /* Methods: */

    /** \returns the key */
    QString const & key() const noexcept { return m_key; }

    /** \returns the module name */
    QString const & module() const noexcept { return m_moduleName; }

    /** \returns the bookmark description */
    QString const & description() const noexcept { return m_description; }

protected: /* Methods: */

    /** Creates a new bookmark item. */
    BookmarkItem(QString const & module,
                 QString const & key,
                 QString const & description);

protected: /* Fields: */

    QString m_moduleName;  /**< The module which is used by this item. */
    QString m_key;         /**< The key of a bookmark. */
    QString m_description; /**< The description of a bookmark. */

};

#endif /* BIBLETIME_BOOKMARKITEM_H */
