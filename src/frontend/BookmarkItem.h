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

#include <QString>
#include <utility>


/**
  Class which represents a bookmark. Includes key, module name and description,
  all QStrings which have getter methods.
*/
class BookmarkItem final {

public: // methods:

    BookmarkItem(QString module, QString key, QString description)
        : m_moduleName(std::move(module))
        , m_key(std::move(key))
        , m_description(std::move(description))
    {}

    /** \returns the module name */
    QString const & module() const noexcept { return m_moduleName; }

    /** \returns the key */
    QString const & key() const noexcept { return m_key; }

    /** \returns the bookmark description */
    QString const & description() const noexcept { return m_description; }

private: // fields:

    QString m_moduleName;  /**< The module which is used by this item. */
    QString m_key;         /**< The key of a bookmark. */
    QString m_description; /**< The description of a bookmark. */

};
