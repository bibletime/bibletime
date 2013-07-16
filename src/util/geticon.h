/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef GETICON_H
#define GETICON_H

#include <QIcon>
#include <QString>

namespace util {

        /**
           \param[in] name the name of the icon to return.
           \returns a reference to the icon with the given name or to a NULL
                    icon if no such icon is found.
         */
        const QIcon & getIcon(const QString & name);

        void clearIconCache();
}

#endif
