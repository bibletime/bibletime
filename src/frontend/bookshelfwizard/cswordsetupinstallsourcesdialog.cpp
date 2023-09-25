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

#include "cswordsetupinstallsourcesdialog.h"

#include <QByteArray>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNonConstOverload>
#include <QPushButton>
#include <Qt>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QApplication>
#include "../../backend/btinstallbackend.h"
#include "../../backend/btinstallmgr.h"
#include "../../util/btconnect.h"
#include "../messagedialog.h"

// Sword includes:
#include <installmgr.h>
#include <swbuf.h>


namespace {

enum class SourceProtocol : int { Local, FTP, SFTP, HTTP, HTTPS };

} // anonymous namespace

CSwordSetupInstallSourcesDialog::CSwordSetupInstallSourcesDialog(/*QWidget *parent*/)
        : QDialog(),
        m_remoteListAdded(false) {
    setWindowTitle(tr("New Installation Source"));

    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing( 5 );

    QHBoxLayout *captionLayout = new QHBoxLayout;
    mainLayout->addLayout(captionLayout);
    QLabel *label = new QLabel( tr("Caption"), this );
    captionLayout->addWidget( label );

    m_captionEdit = new QLineEdit( this );
    m_captionEdit->setText(QStringLiteral("CrossWire Bible Society"));
    captionLayout->addWidget( m_captionEdit );

    mainLayout->addSpacing( 10 );

    QGridLayout* layout = new QGridLayout;
    layout->setSpacing(3);
    layout->setContentsMargins(3, 3, 3, 3);
    mainLayout->addLayout(layout);
    layout->setSpacing( 5 );

    label = new QLabel(tr("Type"), this);
    layout->addWidget( label, 0, 0);

    m_serverLabel = new QLabel(tr("Server"), this);
    layout->addWidget( m_serverLabel, 0, 1);

    label = new QLabel(tr("Path"), this);
    layout->addWidget( label, 0, 2 );

    m_protocolCombo = new QComboBox( this );
    layout->addWidget(m_protocolCombo, 1, 0);
    m_protocolCombo->addItem(tr("Remote FTP"), int(SourceProtocol::FTP));
    m_protocolCombo->addItem(tr("Remote SFTP"), int(SourceProtocol::SFTP));
    m_protocolCombo->addItem(tr("Remote HTTP"), int(SourceProtocol::HTTP));
    m_protocolCombo->addItem(tr("Remote HTTPS"), int(SourceProtocol::HTTPS));
    m_protocolCombo->addItem(tr("Local"), int(SourceProtocol::Local));

    m_serverEdit = new QLineEdit( this );
    layout->addWidget( m_serverEdit, 1, 1 );
    m_serverEdit->setText(QStringLiteral("ftp.crosswire.org"));

    m_pathEdit = new QLineEdit( this );
    layout->addWidget( m_pathEdit, 1, 2 );
    m_pathEdit->setText(QStringLiteral("/pub/sword/raw"));

    mainLayout->addSpacing( 10 );

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, Qt::Horizontal, this);
    message::prepareDialogBox(buttonBox);
    QPushButton* getListButton = new QPushButton(tr("Get list..."), this);
    getListButton->setToolTip(tr("Download a list of sources from CrossWire server and add sources"));
    buttonBox->addButton(getListButton, QDialogButtonBox::ActionRole);
    BT_CONNECT(getListButton, &QPushButton::clicked,
               [this]{
                   if (message::showQuestion(
                           this,
                           tr("Get source list from remote server?"),
                           tr("List of sources will be downloaded from a "
                              "remote server. Sources will be added to the "
                              "current list. New source will replace an "
                              "old one if it has the same label. You can "
                              "later remove the sources you don't want to "
                              "keep.\n\nDo you want to continue?"),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::Yes) == QMessageBox::No)
                       return;

                   BtInstallMgr iMgr;

                   QProgressDialog progressDialog(tr("Connecting..."),
                                                  tr("Cancel"),
                                                  0,
                                                  100,
                                                  this);
                   progressDialog.setValue(0);
                   progressDialog.setWindowTitle(tr("Downloading List"));
                   progressDialog.setMinimumDuration(0);
                   BT_CONNECT(&progressDialog, &QProgressDialog::canceled,
                              [&iMgr]{
                                  iMgr.terminate();
                                  qApp->processEvents();
                              });
                   /** \todo connect this directly to the dialog setValue(int)
                             if possible: */
                   BT_CONNECT(&iMgr, &BtInstallMgr::percentCompleted,
                              [&progressDialog](const int, const int current) {
                                  progressDialog.setLabelText(
                                              tr("Refreshing..."));
                                  progressDialog.setValue(current);
                                  qApp->processEvents();
                              });

                   progressDialog.show();
                   qApp->processEvents();

                   if (!iMgr.refreshRemoteSourceConfiguration()) {
                       // make sure the dialog closes with autoClose:
                       progressDialog.setValue(100);
                       m_remoteListAdded = true;
                       accept();
                   } else {
                       qWarning("InstallMgr: getting remote list returned an "
                                "error.");
                   }
               });
    mainLayout->addWidget(buttonBox);
    BT_CONNECT(buttonBox, &QDialogButtonBox::accepted,
               [this]{
                   struct Error { QString msg; };
                   try {
                       if (m_captionEdit->text().trimmed().isEmpty())
                           throw Error{tr("Please provide a caption.")};

                       auto is(BtInstallBackend::source(m_captionEdit->text()));
                       if (is.caption.c_str() == m_captionEdit->text())
                           throw Error{tr("A source with this caption already "
                                          "exists. Please provide a different "
                                          "caption.")};

                       if (m_protocolCombo->currentData().toInt()
                           != int(SourceProtocol::Local)
                           && m_serverEdit->text().trimmed().isEmpty())
                           throw Error{tr("Please provide a server name.")};

                       if (m_protocolCombo->currentData().toInt()
                           == int(SourceProtocol::Local))
                       {
                           if (m_pathEdit->text().isEmpty())
                               throw Error{tr("Please provide a path.")};

                           QFileInfo const fi(m_pathEdit->text());
                           if (!fi.exists() || !fi.isReadable())
                               throw Error{tr("Please provide a valid, "
                                              "readable path.")};
                       }
                   } catch (Error const & e) {
                       message::showCritical(this, tr("Error"), e.msg);
                       return;
                   }
                   accept();
               });
    BT_CONNECT(buttonBox, &QDialogButtonBox::rejected,
               this, &CSwordSetupInstallSourcesDialog::reject);
    BT_CONNECT(m_protocolCombo, qOverload<int>(&QComboBox::activated),
               [this]{
                   if (m_protocolCombo->currentData().toInt()
                       != int(SourceProtocol::Local))
                   {
                       m_serverLabel->setEnabled(true);
                       m_serverEdit->setEnabled(true);
                   } else { // LOCAL, no server needed
                       m_serverLabel->setEnabled(false);
                       m_serverEdit->setEnabled(false);

                       QString dirname = QFileDialog::getExistingDirectory(this);
                       if (dirname.isEmpty()) {
                           return; // user cancelled
                       }
                       QDir dir(dirname);
                       if (dir.exists()) {
                           m_pathEdit->setText( dir.canonicalPath() );
                       }
                   }

               });
}

sword::InstallSource CSwordSetupInstallSourcesDialog::getSource() {
    sword::InstallSource newSource(""); //empty, invalid Source
    auto const protocol =
            static_cast<SourceProtocol>(m_protocolCombo->currentData().toInt());
    if (protocol != SourceProtocol::Local) {
        if (protocol == SourceProtocol::FTP) {
            newSource.type = "FTP";
        } else if (protocol == SourceProtocol::SFTP) {
            newSource.type = "SFTP";
        } else if (protocol == SourceProtocol::HTTP) {
            newSource.type = "HTTP";
        } else if (protocol == SourceProtocol::HTTPS) {
            newSource.type = "HTTPS";
        }
        /// \todo Add validator for server
        // a message to the user would be nice, but we're in message freeze right now (1.5.1)
        auto server = m_serverEdit->text();
        while (server.endsWith('/'))
            server.chop(1);
        newSource.source = server.toUtf8();
    }
    else {
        newSource.type = "DIR";
        newSource.source = "local";
    }
    newSource.caption = m_captionEdit->text().toUtf8();
    newSource.directory = m_pathEdit->text().toUtf8();
    newSource.uid = newSource.source;

    return newSource;
}
