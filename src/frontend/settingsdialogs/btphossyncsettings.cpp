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

#include "btphossyncsettings.h"

#include <QApplication>
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../backend/phossync/btphoshttpclient.h"
#include "../../backend/phossync/btphossync.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "cconfigurationdialog.h"


BtPhosSyncSettingsPage::BtPhosSyncSettingsPage(CConfigurationDialog * parent)
    : BtConfigDialog::Page(CResMgr::mainIndex::bookmark::icon(), parent)
{
    auto * const mainLayout = new QVBoxLayout(this);

    m_infoLabel = new QLabel(this);
    m_infoLabel->setWordWrap(true);
    mainLayout->addWidget(m_infoLabel);

    m_enabledCheckBox = new QCheckBox(this);
    mainLayout->addWidget(m_enabledCheckBox);

    auto * const formLayout = new QFormLayout();
    m_serverLabel = new QLabel(this);
    m_serverEdit = new QLineEdit(this);
    m_serverEdit->setPlaceholderText(QStringLiteral("https://cloud.example.org"));
    m_serverLabel->setBuddy(m_serverEdit);
    formLayout->addRow(m_serverLabel, m_serverEdit);

    m_userLabel = new QLabel(this);
    m_userEdit = new QLineEdit(this);
    m_userLabel->setBuddy(m_userEdit);
    formLayout->addRow(m_userLabel, m_userEdit);

    m_passwordLabel = new QLabel(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordLabel->setBuddy(m_passwordEdit);
    formLayout->addRow(m_passwordLabel, m_passwordEdit);
    mainLayout->addLayout(formLayout);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    mainLayout->addWidget(m_statusLabel);

    auto * const buttons = new QHBoxLayout();
    m_syncNowButton = new QPushButton(this);
    m_resetButton = new QPushButton(this);
    buttons->addWidget(m_syncNowButton);
    buttons->addWidget(m_resetButton);
    buttons->addStretch();
    mainLayout->addLayout(buttons);
    mainLayout->addStretch();

    auto const s = BtPhosSync::settings();
    m_enabledCheckBox->setChecked(s.enabled);
    m_serverEdit->setText(s.server);
    m_userEdit->setText(s.user);

    BT_CONNECT(m_syncNowButton, &QPushButton::clicked,
               this, &BtPhosSyncSettingsPage::syncNow);
    BT_CONNECT(m_resetButton, &QPushButton::clicked,
               this, &BtPhosSyncSettingsPage::forgetServerState);
    if (auto * const sync = BtPhosSync::instance()) {
        BT_CONNECT(sync, &BtPhosSync::started,
                   this, &BtPhosSyncSettingsPage::showStatus);
        BT_CONNECT(sync, &BtPhosSync::finished,
                   this, &BtPhosSyncSettingsPage::showStatus);
    }

    retranslateUi();
}

void BtPhosSyncSettingsPage::retranslateUi() {
    setHeaderText(tr("Bookmark synchronization"));
    m_infoLabel->setText(
        tr("Keeps your bookmarks and bookmark folders in step with other "
           "devices through the Phos app on a Nextcloud server. Folders "
           "appear there as labels."));
    m_enabledCheckBox->setText(tr("Synchronize bookmarks"));
    m_serverLabel->setText(tr("Nextcloud server:"));
    m_userLabel->setText(tr("User name:"));
    m_passwordLabel->setText(tr("Password:"));
    m_passwordEdit->setPlaceholderText(
        BtPhosSync::settings().appPassword.isEmpty()
        ? QString()
        : tr("(app password stored)"));
    m_passwordEdit->setToolTip(
        tr("Your login password is only used once to obtain an app password "
           "from the server. Only that app password is stored; you can revoke "
           "it in the security settings of your Nextcloud."));
    m_syncNowButton->setText(tr("Synchronize now"));
    m_resetButton->setText(tr("Forget server state"));
    m_resetButton->setToolTip(
        tr("Use this after switching to another server or account. Your "
           "bookmarks stay as they are."));
    showStatus();
}

void BtPhosSyncSettingsPage::showStatus() {
    auto * const sync = BtPhosSync::instance();
    if (sync && sync->isRunning()) {
        m_statusLabel->setText(tr("Synchronizing…"));
        m_syncNowButton->setEnabled(false);
        m_resetButton->setEnabled(false);
        return;
    }
    m_syncNowButton->setEnabled(sync != nullptr);
    m_resetButton->setEnabled(sync != nullptr);
    auto const last = BtPhosSync::lastRun();
    if (!last.isValid()) {
        m_statusLabel->setText(tr("Not synchronized yet."));
    } else {
        m_statusLabel->setText(
            tr("Last synchronization: %1, %2")
            .arg(QLocale().toString(last.toLocalTime(), QLocale::ShortFormat),
                 BtPhosSync::lastResult()));
    }
}

void BtPhosSyncSettingsPage::save() const {
    auto s = BtPhosSync::settings();
    s.enabled = m_enabledCheckBox->isChecked();
    auto const server = BtPhosHttpClient::normalizeServerUrl(m_serverEdit->text());
    auto const user = m_userEdit->text().trimmed();
    if (server != s.server || user != s.user)
        s.appPassword.clear();
    s.server = server;
    s.user = user;
    auto const password = m_passwordEdit->text();
    if (!password.isEmpty() && !server.isEmpty() && !user.isEmpty()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        try {
            s.appPassword = BtPhosSync::exchangePassword(server, user, password);
            m_passwordEdit->clear();
            m_passwordEdit->setPlaceholderText(tr("(app password stored)"));
        } catch (std::exception const & e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(
                const_cast<BtPhosSyncSettingsPage *>(this),
                tr("Bookmark synchronization"),
                tr("Could not log in to the server: %1")
                .arg(QString::fromUtf8(e.what())));
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }
        QApplication::restoreOverrideCursor();
    }
    BtPhosSync::saveSettings(s);
    if (auto * const sync = BtPhosSync::instance()) {
        sync->reloadSettings();
        if (s.configured())
            sync->syncNow();
    }
}

void BtPhosSyncSettingsPage::syncNow() {
    save();
    if (!BtPhosSync::settings().configured())
        m_statusLabel->setText(
            tr("Enter the server, your user name and password, and switch on "
               "the synchronization first."));
}

void BtPhosSyncSettingsPage::forgetServerState() {
    if (auto * const sync = BtPhosSync::instance()) {
        sync->reset();
        showStatus();
    }
}

#endif
