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
#include "frontend/bookshelfmanager/instbackend.h"
#include "util/dialogutil.h"

const QString PROTO_FILE( QObject::tr("Local") ); //Local path
const QString PROTO_FTP( QObject::tr("Remote") ); //Remote path

CSwordSetupInstallSourcesDialog::CSwordSetupInstallSourcesDialog(/*QWidget *parent*/)
        : QDialog() {
    setWindowTitle(tr("New  Installation Source"));

    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    mainLayout->setMargin( 10 );
    mainLayout->setSpacing( 5 );

    QHBoxLayout *captionLayout = new QHBoxLayout( this );
    mainLayout->addLayout(captionLayout);
    QLabel *label = new QLabel( tr("Caption"), this );
    captionLayout->addWidget( label );

    m_captionEdit = new QLineEdit( this );
    m_captionEdit->setText("Crosswire Bible Society");
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

    QHBoxLayout* buttonLayout = new QHBoxLayout( this );
    mainLayout->addLayout(buttonLayout);
    buttonLayout->addStretch();
    QPushButton* okButton = new QPushButton( tr("Ok"), this);
    QPushButton* discardButton = new QPushButton( tr("Discard"), this);
    buttonLayout->addWidget( discardButton);
    buttonLayout->addWidget( okButton);
    buttonLayout->addStretch();

    connect( okButton, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( discardButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
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
                               tr("A source with this caption already exists.<br>Please provide a different caption."));
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

sword::InstallSource CSwordSetupInstallSourcesDialog::getSource() {

    boost::scoped_ptr<CSwordSetupInstallSourcesDialog> dlg( new CSwordSetupInstallSourcesDialog() );
    sword::InstallSource newSource(""); //empty, invalid Source

    if (dlg->exec() == QDialog::Accepted) {
        if (dlg->m_protocolCombo->currentText() == PROTO_FTP) {
            newSource.type = "FTP";
            newSource.source = dlg->m_serverEdit->text().toUtf8();

            //a message to the user would be nice, but we're in message freeze right now (1.5.1)
            if (dlg->m_serverEdit->text().right(1) == "/") { //remove a trailing slash
                newSource.source  = dlg->m_serverEdit->text().mid(0, dlg->m_serverEdit->text().length() - 1).toUtf8();
            }
        }
        else {
            newSource.type = "DIR";
            newSource.source = "local";
        }
        newSource.caption = dlg->m_captionEdit->text().toUtf8();
        newSource.directory = dlg->m_pathEdit->text().toUtf8();
    }
    return newSource;
}


