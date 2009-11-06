/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"

#include <boost/scoped_ptr.hpp>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QApplication>
#include <QDebug>
#include "frontend/bookshelfmanager/instbackend.h"
#include "util/dialogutil.h"

const QString PROTO_FILE( QObject::tr("Local") ); //Local path
const QString PROTO_FTP( QObject::tr("Remote") ); //Remote path

CSwordSetupInstallSourcesDialog::CSwordSetupInstallSourcesDialog(/*QWidget *parent*/)
        : QDialog(),
        m_remoteListAdded(false) {
    setWindowTitle(tr("New  Installation Source"));

    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    mainLayout->setMargin( 10 );
    mainLayout->setSpacing( 5 );

    QHBoxLayout *captionLayout = new QHBoxLayout( this );
    mainLayout->addLayout(captionLayout);
    QLabel *label = new QLabel( tr("Caption"), this );
    captionLayout->addWidget( label );

    m_captionEdit = new QLineEdit( this );
    m_captionEdit->setText("CrossWire Bible Society");
    captionLayout->addWidget( m_captionEdit );

    mainLayout->addSpacing( 10 );

    QGridLayout* layout = new QGridLayout( this );
    layout->setSpacing(3);
    layout->setMargin(3);
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
    m_protocolCombo->addItem( PROTO_FTP  );
    m_protocolCombo->addItem( PROTO_FILE );

    m_serverEdit = new QLineEdit( this );
    layout->addWidget( m_serverEdit, 1, 1 );
    m_serverEdit->setText("ftp.crosswire.org");

    m_pathEdit = new QLineEdit( this );
    layout->addWidget( m_pathEdit, 1, 2 );
    m_pathEdit->setText("/pub/sword/raw");

    mainLayout->addSpacing( 10 );

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Save, Qt::Horizontal, this);
    util::prepareDialogBox(buttonBox);
    QPushButton* getListButton = new QPushButton(tr("Get list..."), this);
    getListButton->setToolTip(tr("Download a list of sources from CrossWire server and add sources"));
    buttonBox->addButton(getListButton, QDialogButtonBox::ActionRole);
    connect(getListButton, SIGNAL(clicked()), SLOT(slotGetListClicked()));
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), SLOT(slotOk()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    connect( m_protocolCombo, SIGNAL( activated(int) ), this, SLOT( slotProtocolChanged() ) );
}

void CSwordSetupInstallSourcesDialog::slotOk() {
    //run a few tests to validate the input first
    if ( m_captionEdit->text().trimmed().isEmpty() ) { //no caption
        util::showInformation( this, tr( "Error" ), tr("Please provide a caption."));
        return;
    }

    //BTInstallMgr iMgr;
    //sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source( &iMgr, m_captionEdit->text() );
    sword::InstallSource is = instbackend::source(m_captionEdit->text());
    if ( (QString)is.caption.c_str() == m_captionEdit->text() ) { //source already exists
        util::showInformation( this, tr( "Error" ),
                               tr("A source with this caption already exists.<br/>Please provide a different caption."));
        return;
    }

    if ( m_protocolCombo->currentText() == PROTO_FTP &&
            m_serverEdit->text().trimmed().isEmpty() ) { //no server name
        util::showInformation( this, tr( "Error" ), tr("Please provide a server name."));
        return;
    }

    if ( m_protocolCombo->currentText() == PROTO_FILE) {
        const QFileInfo fi( m_pathEdit->text() );
        if (!fi.exists() || !fi.isReadable()) { //no valid and readable path
            util::showInformation( this, tr( "Error" ), tr("Please provide a valid, readable path."));
            return;
        }
        else if ( m_pathEdit->text().isEmpty() ) {
            util::showInformation( this, tr( "Error" ), tr("Please provide a path."));

        }
    }

    accept(); //only if nothing else failed
}

void CSwordSetupInstallSourcesDialog::slotProtocolChanged() {
    if (m_protocolCombo->currentText() == PROTO_FTP) { //REMOTE
        m_serverLabel->setEnabled(true);
        m_serverEdit->setEnabled(true);
    }
    else { //LOCAL, no server needed
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

}

void CSwordSetupInstallSourcesDialog::slotGetListClicked() {
    /// \todo show message "gets list, adds sources, OK/Cancel", accept() or continue
    QString message("List of sources will be downloaded from a remote server. Sources will be added to the current list. New source will replace an old one if it has the same label. You can later remove the sources you don't want to keep.\n\nDo you want to continue?");
    QMessageBox::StandardButton answer = util::showQuestion(this, tr("Get source list from remote server?"), message, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if (answer == QMessageBox::No) {
        return;
    }
    qDebug() << "Ok, create installmgr";
    BtInstallMgr iMgr;

    m_progressDialog = new QProgressDialog("", tr("Cancel"), 0 , 100, this);
    m_progressDialog->setWindowTitle(tr("Downloading List"));
    m_progressDialog->setMinimumDuration(0);
    connect(m_progressDialog, SIGNAL(canceled()), SLOT(slotRefreshCanceled()));
    m_currentInstallMgr = &iMgr; //for the progress dialog
    // connect this directly to the dialog setValue(int) if possible
    connect(&iMgr, SIGNAL(percentCompleted(const int, const int)), SLOT(slotRefreshProgress(const int, const int)));

    m_progressDialog->show();
    qApp->processEvents();
    this->slotRefreshProgress(0, 0);
    m_progressDialog->setLabelText(tr("Connecting..."));
    m_progressDialog->setValue(0);
    qApp->processEvents();
    qWarning() << "Start downloading the list of sources";
    int ret = iMgr.refreshRemoteSourceConfiguration();
    bool success = false;
    if ( !ret ) { //make sure the sources were updated sucessfully
        qDebug() << "download succeeded";
        success = true;
        m_progressDialog->setValue(100); //make sure the dialog closes
        m_remoteListAdded = true; 
        accept();
    }
    else {
        qWarning("InstallMgr: getting remote list returned an error.");
        success = false;
    }
    delete m_progressDialog;
    m_progressDialog = 0;
}

void CSwordSetupInstallSourcesDialog::slotRefreshProgress(const int, const int current) {
    if (m_progressDialog) {
        if (m_progressDialog->labelText() != tr("Refreshing...")) {
            m_progressDialog->setLabelText(tr("Refreshing..."));
        }
        m_progressDialog->setValue(current);
    }
    qApp->processEvents();
}

void CSwordSetupInstallSourcesDialog::slotRefreshCanceled() {
    Q_ASSERT(m_currentInstallMgr);
    if (m_currentInstallMgr) {
        m_currentInstallMgr->terminate();
    }
    qApp->processEvents();
}

sword::InstallSource CSwordSetupInstallSourcesDialog::getSource() {
    sword::InstallSource newSource(""); //empty, invalid Source
    if (m_protocolCombo->currentText() == PROTO_FTP) {
        newSource.type = "FTP";
        newSource.source = m_serverEdit->text().toUtf8();
        //a message to the user would be nice, but we're in message freeze right now (1.5.1)
        if (m_serverEdit->text().right(1) == "/") { //remove a trailing slash
            newSource.source  = m_serverEdit->text().mid(0, m_serverEdit->text().length() - 1).toUtf8();
        }
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


