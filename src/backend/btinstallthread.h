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

#include <QThread>

#include <atomic>
#include <QList>
#include <QObject>
#include <QString>
#include <Qt>
#include "btinstallmgr.h"
#include "../util/btconnect.h"


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
            BT_CONNECT(&m_iMgr, &BtInstallMgr::percentCompleted,
                       this,    &BtInstallThread::slotManagerStatusUpdated,
                       Qt::QueuedConnection);
            BT_CONNECT(&m_iMgr, &BtInstallMgr::downloadStarted,
                       this,    &BtInstallThread::slotDownloadStarted,
                       Qt::QueuedConnection);
        }

        void stopInstall()
        { m_stopRequested.store(true, std::memory_order_relaxed); }

    Q_SIGNALS:

        /** Emitted when starting the installation. */
        void preparingInstall(int moduleIndex);

        /** Emitted when the first file download has been started. */
        void downloadStarted(int moduleIndex);

        /** Emitted when the install progress status is updated. */
        void statusUpdated(int moduleIndex, int progressPercent);

        /** Emitted when installing is complete. */
        void installCompleted(int moduleIndex, bool success);

    protected: // methods:

        void run() override;

    private: // methods:

        void installModule();
        bool removeModule();

    private Q_SLOTS:

        void slotDownloadStarted();
        void slotManagerStatusUpdated(int totalProgress, int fileProgress);

    private: // fields:

        const QList<CSwordModuleInfo *> & m_modules;
        const QString m_destination;
        BtInstallMgr m_iMgr;
        int m_currentModuleIndex = 0;
        std::atomic<bool> m_stopRequested;

};
