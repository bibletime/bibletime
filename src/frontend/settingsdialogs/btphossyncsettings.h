/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifdef BUILD_PHOS_SYNC

#pragma once

#include "btconfigdialog.h"

#include <QObject>


class CConfigurationDialog;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

/** Settings of the bookmark synchronization with the Phos app for Nextcloud. */
class BtPhosSyncSettingsPage: public BtConfigDialog::Page {

    Q_OBJECT

public: // methods:

    BtPhosSyncSettingsPage(CConfigurationDialog * parent = nullptr);
    void save() const final override;

private: // methods:

    void retranslateUi();
    void showStatus();
    void syncNow();
    void forgetServerState();

private: // fields:

    QLabel * m_infoLabel;
    QCheckBox * m_enabledCheckBox;
    QLabel * m_serverLabel;
    QLineEdit * m_serverEdit;
    QLabel * m_userLabel;
    QLineEdit * m_userEdit;
    QLabel * m_passwordLabel;
    QLineEdit * m_passwordEdit;
    QLabel * m_statusLabel;
    QPushButton * m_syncNowButton;
    QPushButton * m_resetButton;

};

#endif
