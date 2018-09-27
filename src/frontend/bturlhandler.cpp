/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bturlhandler.h"

#include <QApplication>

void BtUrlHandler::showQt(const QUrl &url) {
    if (url.host() == "about") {
            qApp->aboutQt();
    }
}
