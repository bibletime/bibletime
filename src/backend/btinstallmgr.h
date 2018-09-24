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

#ifndef BTINSTALLMGR_H
#define BTINSTALLMGR_H

#include <QObject>
#include <swordxx/installmgr.h>
#include <swordxx/remotetrans.h>


/**
* Our own reimplementation to provide installation and status bar updates.
*/
class BtInstallMgr
        : public QObject
        , public swordxx::InstallMgr
        , public swordxx::StatusReporter
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

    /** \note Reimplementation of swordxx::StatusReporter::statusUpdate(). */
    void update(std::size_t dltotal, std::size_t dlnow) noexcept override;

    /**
    * \note Reimplementation of swordxx::StatusReporter::preStatus().
    * \warning This method is not always called before statusUpdate().
    * Called before starting to download each file of the module package.
    * The sword message is not i18n'ed, it's in the form "Downloading (1 of 6): nt.bzs".
    * This function is not utilized in the UI ATM.
    */
    void preStatus(std::size_t totalBytes,
                   std::size_t completedBytes,
                   const char * message) noexcept override;

private: /* Fields: */

    std::size_t m_totalBytes;
    std::size_t m_completedBytes;
    bool m_firstCallOfPreStatus;

};

#endif /* BTINSTALLMGR_H */
