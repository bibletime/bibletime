/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class QQuickItem;

namespace btm {

class SessionManager : public QObject {
    Q_OBJECT

public:
    SessionManager(QObject* parent = 0);
    void loadDefaultSession();
    void saveDefaultSession();

private:
    int getColorTheme();
    int getWindowArrangementMode();
    int getWindowCount();
    void loadWindow(const QStringList& moduleNames, const QString& key);
    void loadWindows();
    void newWindow(const QString& category, const QString& moduleName, const QString& key);
    void saveWindowStateToConfig(int windowIndex);

    QQuickItem* m_windowMgr;
};

}
