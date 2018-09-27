/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QObject>
#include <QUrl>

class BtUrlHandler : public QObject
{
    Q_OBJECT

public slots:
    void showQt(const QUrl &url);
};

//  QDesktopServices::setUrlHandler("help", helpInstance, "showHelp");

