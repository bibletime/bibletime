/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef NEWBTINSTALLMGR_H
#define NEWBTINSTALLMGR_H

#include <QObject>

#include <QList>
#include <QString>
#include <QStringList>

// Nasty Sword includes:
#include <installmgr.h>
#include <ftptrans.h>


class CSwordBackend;

typedef QList<sword::InstallSource*> InstallSourceList;

/**
* Our own reimplementation to provide installation and status bar updates.
*/
class BtInstallMgr : public QObject, public sword::InstallMgr, public sword::StatusReporter {
        Q_OBJECT
    public:

        BtInstallMgr();
        virtual ~BtInstallMgr();

        /**
        * Refreshing the source should be cancellable (othewise it might freeze the app if there is
        * for example something wrong with the network).
        */
        void slotRefreshCanceled();

        /** Re-implemented from sword::InstallMgr. */
        virtual bool isUserDisclaimerConfirmed() const;

    protected:
        /* Reimplementations of methods in StatusReporter */
        /**
        * Gets the total and current file status, emits the signal with those values as percents.
        */
        virtual void statusUpdate(double dltotal, double dlnow);
        /**
        * Called before starting to download each file of the module package.
        * The sword message is not i18n'ed, it's in the form "Downloading (1 of 6): nt.bzs".
        * This function is not utilized in the UI ATM.
        */
        virtual void preStatus(long totalBytes, long completedBytes, const char *message);

        long m_totalBytes;
        long m_completedBytes;

    private:
        bool m_firstCallOfPreStatus;

    signals:
        /** Download status. Percent of total and file.*/
        void percentCompleted( const int, const int);
        void downloadStarted();
};


#endif
