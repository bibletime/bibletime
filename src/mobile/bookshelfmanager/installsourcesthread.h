/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef INSTALLSOURCESTHREAD_H
#define INSTALLSOURCESTHREAD_H

#include <QThread>

class BtInstallMgr;

namespace btm {

class InstallSourcesThread : public QThread {
        Q_OBJECT
    public:
        InstallSourcesThread(QObject *parent = 0);

        ~InstallSourcesThread();

    public slots:
        void slotStopInstall();
        void slotManagerStatusUpdated(int totalProgress, int fileProgress);
        void slotDownloadStarted();

    public: // data member
        bool done;

    protected:
        virtual void run();
        void refreshSourceList();
        void refreshWorks(const QStringList& sourceNames);

        bool m_cancelled;
        BtInstallMgr* m_iMgr;

    signals:
        void percentComplete(int percent, const QString& title);
};

}
#endif
