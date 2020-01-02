/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTINSTALLMGR_H
#define BTINSTALLMGR_H

#include <QObject>

// Sword includes:
#include <installmgr.h>
#include <remotetrans.h>


/**
* Our own reimplementation to provide installation and status bar updates.
*/
class BtInstallMgr
        : public QObject
        , public sword::InstallMgr
        , public sword::StatusReporter
{

    Q_OBJECT

public: /* Methods: */

    BtInstallMgr(QObject * parent = nullptr);
    ~BtInstallMgr() override;

    bool isUserDisclaimerConfirmed() const override;

signals:

    /**
      Download status. Percent of total and file.
      \warning Use these values for display only, since they might be incorrect.
    */
    void percentCompleted(const int total, const int file);

    void downloadStarted();

protected: /* Methods: */

    /** \note Reimplementation of sword::StatusReporter::statusUpdate(). */
    void statusUpdate(double dltotal, double dlnow) override;

    /**
    * \note Reimplementation of sword::StatusReporter::preStatus().
    * \warning This method is not always called before statusUpdate().
    * Called before starting to download each file of the module package.
    * The sword message is not i18n'ed, it's in the form "Downloading (1 of 6): nt.bzs".
    * This function is not utilized in the UI ATM.
    */
    void preStatus(long totalBytes,
                   long completedBytes,
                   const char * message) override;

private: /* Fields: */

    long m_totalBytes;
    long m_completedBytes;
    bool m_firstCallOfPreStatus;

};

#endif /* BTINSTALLMGR_H */
