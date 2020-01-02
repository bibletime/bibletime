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

#ifndef CSWORDSETUPINSTALLSOURCESDIALOG_H
#define CSWORDSETUPINSTALLSOURCESDIALOG_H

#include <QDialog>

#include "../../backend/btinstallmgr.h"

// Sword includes:
#include <installmgr.h>


class QComboBox;
class QLabel;
class QLineEdit;
class QProgressDialog;

class CSwordSetupInstallSourcesDialog final: public QDialog  {
        Q_OBJECT

    public:
        sword::InstallSource getSource();
        bool wasRemoteListAdded() {
            return m_remoteListAdded;
        }
        CSwordSetupInstallSourcesDialog();

    protected slots:
        void slotOk();
        void slotProtocolChanged();
        void slotGetListClicked();
        void slotRefreshCanceled();
        void slotRefreshProgress(const int, const int current);

    private:
        bool isRemote(const QString&);
        QLabel    *m_serverLabel;
        QLineEdit *m_captionEdit, *m_serverEdit, *m_pathEdit;
        QComboBox *m_protocolCombo;
        QProgressDialog* m_progressDialog;
        BtInstallMgr* m_currentInstallMgr;
        bool m_remoteListAdded;
};


#endif //CSWORDSETUPINSTALLSOURCESDIALOG_H
