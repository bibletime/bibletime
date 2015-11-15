/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLTHREAD_H
#define BTINSTALLTHREAD_H

#include <QThread>

#include <QMutex>
#include <QMutexLocker>
#include "btinstallmgr.h"


class BtInstallProgressDialog;
class CSwordBackend;
class CSwordModuleInfo;


class BtInstallThread: public QThread {

        Q_OBJECT

    public:

        BtInstallThread(const QList<CSwordModuleInfo *> & modules,
                        const QString & destination,
                        QObject * const parent = nullptr)
            : QThread(parent)
            , m_modules(modules)
            , m_destination(destination)
            , m_stopRequested(false)
        {
            connect(&m_iMgr, SIGNAL(percentCompleted(int, int)),
                    this,    SLOT(slotManagerStatusUpdated(int, int)),
                    Qt::QueuedConnection);
            connect(&m_iMgr, SIGNAL(downloadStarted()),
                    this,    SLOT(slotDownloadStarted()),
                    Qt::QueuedConnection);
        }

        void stopInstall() {
            const QMutexLocker lock(&m_stopRequestedMutex);
            m_stopRequested = true;
        }

    signals:

        /** Emitted when starting the installation. */
        void preparingInstall(int moduleIndex);

        /** Emitted when the first file download has been started. */
        void downloadStarted(int moduleIndex);

        /** Emitted when the install progress status is updated. */
        void statusUpdated(int moduleIndex, int progressPercent);

        /** Emitted when installing is complete. */
        void installCompleted(int moduleIndex, bool success);

    protected: /* Methods: */

        virtual void run() override;

    private: /* Methods: */

        void installModule();
        bool removeModule();

    private slots:

        void slotDownloadStarted();
        void slotManagerStatusUpdated(int totalProgress, int fileProgress);

    private: /* Fields: */

        const QList<CSwordModuleInfo *> & m_modules;
        const QString m_destination;
        BtInstallMgr m_iMgr;
        int m_currentModuleIndex;
        QMutex m_stopRequestedMutex;
        bool m_stopRequested;

};

#endif
