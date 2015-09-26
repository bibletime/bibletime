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

#ifndef INSTALL_PROGRESS_H
#define INSTALL_PROGRESS_H

#include <QList>
#include <QObject>
#include <QMultiMap>
#include <QMap>

class QQuickItem;
class CSwordModuleInfo;
class BtInstallThread;

namespace btm {

class InstallProgress: public QObject {
    Q_OBJECT

public:
    InstallProgress(QObject* parent = 0);

    void openProgress(const QList<CSwordModuleInfo*>& modules);

private slots:
    void cancel();
    void close();
    void slotStopInstall();
    void slotInstallStarted(int moduleIndex);
    void slotDownloadStarted(int moduleIndex);
    void slotStatusUpdated(int moduleIndex, int status);
    void slotOneItemCompleted(int moduleIndex, bool status);
    void slotThreadFinished();

private:
    void findProgressObject();
    QString getModuleName(int moduleIndex);
    QString getSourcePath();
    void oneItemStoppedOrCompleted(QString module, QString source, QString message);
    void setProperties();

    QQuickItem* m_progressObject;
    BtInstallThread * m_thread;
    int m_nextInstallIndex;
    QList<CSwordModuleInfo*> m_modules;
};

}

#endif
