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

#include "BtMimeData.h"


BTMimeData::BTMimeData() {}

BTMimeData::BTMimeData(QString const & module,
                       QString const & key,
                       QString const & description)
{ appendBookmark(module, key, description); }

BTMimeData::BTMimeData(QString const & text) { setText(text); }


void BTMimeData::appendBookmark(QString const & module,
                                QString const & key,
                                QString const & description)
{
    m_bookmarkList.append(BookmarkItem(module, key, description));
    setData("BibleTime/Bookmark", QByteArray());
}
