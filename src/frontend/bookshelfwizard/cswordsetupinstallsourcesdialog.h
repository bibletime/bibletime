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

#include <QDialog>

#include <QObject>
#include <QString>


class QComboBox;
class QLabel;
class QLineEdit;
namespace sword { class InstallSource; }

class CSwordSetupInstallSourcesDialog final: public QDialog  {
        Q_OBJECT

    public:
        sword::InstallSource getSource();
        bool wasRemoteListAdded() {
            return m_remoteListAdded;
        }
        CSwordSetupInstallSourcesDialog();

    private:
        QLabel    *m_serverLabel;
        QLineEdit *m_captionEdit, *m_serverEdit, *m_pathEdit;
        QComboBox *m_protocolCombo;
        bool m_remoteListAdded;
};
