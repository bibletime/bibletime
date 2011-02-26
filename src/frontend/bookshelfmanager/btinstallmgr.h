/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef NEWBTINSTALLMGR_H
#define NEWBTINSTALLMGR_H

#include <QObject>

#include <QList>
#include <QString>
#include <QStringList>

// Sword includes:
#include <installmgr.h>
#include <ftptrans.h>


/**
* Our own reimplementation to provide installation and status bar updates.
*/
class BtInstallMgr : public QObject, public sword::InstallMgr, public sword::StatusReporter {
        Q_OBJECT
    public:
        BtInstallMgr();
        ~BtInstallMgr();

        /** Re-implemented from sword::InstallMgr. */
        bool isUserDisclaimerConfirmed() const;

    signals:
        /** Download status. Percent of total and file.*/
        void percentCompleted(const int total, const int file);
        void downloadStarted();

    protected:
        /**
          Reimplementation of sword::StatusReporter::statusUpdate().
        */
        void statusUpdate(double dltotal, double dlnow);

        /**
        * Reimplementation of sword::StatusReporter::preStatus().
        * \warning This method is not always called before statusUpdate().
        * Called before starting to download each file of the module package.
        * The sword message is not i18n'ed, it's in the form "Downloading (1 of 6): nt.bzs".
        * This function is not utilized in the UI ATM.
        */
        void preStatus(long totalBytes, long completedBytes, const char *message);

    private:
        long m_totalBytes;
        long m_completedBytes;
        bool m_firstCallOfPreStatus;
};

#endif
