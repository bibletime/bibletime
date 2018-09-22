/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef SESSION_INTERFACE_H
#define SESSION_INTERFACE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>


namespace btm {

class SessionInterface : public QObject {
    Q_OBJECT

//    Q_PROPERTY(QVariant sourceModel   READ sourceModel   NOTIFY sourceModelChanged)

public:
    SessionInterface();

    Q_INVOKABLE int getColorTheme();
    Q_INVOKABLE int getWindowArrangementMode();
    Q_INVOKABLE QStringList getWindowList();
    Q_INVOKABLE QStringList getWindowModuleList(const QString& win);
    Q_INVOKABLE QString getWindowKey(const QString& win);

    Q_INVOKABLE void setColorTheme(const QString& color);
    Q_INVOKABLE void setWindowArrangementMode(int mode);
    Q_INVOKABLE void setWindowList(const QStringList& list);
    Q_INVOKABLE void setWindowModuleList(int index, const QStringList& modules);
    Q_INVOKABLE void setWindowKey(int index, const QString& key);

signals:
//    void sourceModelChanged();

private slots:
//    void slotStopInstall();

private:

};

}
#endif
