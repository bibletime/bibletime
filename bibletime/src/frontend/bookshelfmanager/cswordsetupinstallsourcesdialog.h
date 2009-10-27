/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef CSWORDSETUPINSTALLSOURCESDIALOG_H
#define CSWORDSETUPINSTALLSOURCESDIALOG_H

#include "frontend/bookshelfmanager/btinstallmgr.h"

#include <QDialog>

#include <installmgr.h>

class QLabel;
class QComboBox;
class QLineEdit;



class CSwordSetupInstallSourcesDialog : public QDialog  {
        Q_OBJECT

    public:
        static sword::InstallSource getSource();

    protected:
        CSwordSetupInstallSourcesDialog();

    protected slots:
        void slotOk();
        void slotProtocolChanged();

    private:
        QLabel    *m_serverLabel;
        QLineEdit *m_captionEdit, *m_serverEdit, *m_pathEdit;
        QComboBox *m_protocolCombo;
};


#endif //CSWORDSETUPINSTALLSOURCESDIALOG_H
