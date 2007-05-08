/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BT includes
#include "cswordsetupinstallsourcesdialog.h"
#include "util/scoped_resource.h"

//Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfileinfo.h>


#include <klocale.h>
#include <kdirselectdialog.h>

namespace BookshelfManager {

	const QString PROTO_FILE( i18n("Local") ); //Local path
	const QString PROTO_FTP( i18n("Remote") ); //Remote path


	CSwordSetupInstallSourcesDialog::CSwordSetupInstallSourcesDialog(/*QWidget *parent*/)
: QDialog() {

		QVBoxLayout* mainLayout = new QVBoxLayout( this );
		mainLayout->setMargin( 10 );
		mainLayout->setSpacing( 5 );

		QHBoxLayout *captionLayout = new QHBoxLayout( mainLayout );
		QLabel *label = new QLabel( i18n("Caption"), this );
		captionLayout->addWidget( label );

		m_captionEdit = new QLineEdit( this );
		m_captionEdit->setText("Crosswire Bible Society");
		captionLayout->addWidget( m_captionEdit );

		mainLayout->addSpacing( 10 );

		QGridLayout* layout = new QGridLayout( mainLayout, 3, 3 );
		layout->setSpacing( 5 );

		label = new QLabel(i18n("Type"), this);
		layout->addWidget( label, 0, 0);

		m_serverLabel = new QLabel(i18n("Server"), this);
		layout->addWidget( m_serverLabel, 0, 1);

		label = new QLabel(i18n("Path"), this);
		layout->addWidget( label, 0, 2 );

		m_protocolCombo = new QComboBox( this );
		layout->addWidget(m_protocolCombo, 1, 0);
		m_protocolCombo->insertItem( PROTO_FTP  );
		m_protocolCombo->insertItem( PROTO_FILE );

		m_serverEdit = new QLineEdit( this );
		layout->addWidget( m_serverEdit, 1, 1 );
		m_serverEdit->setText("ftp.crosswire.org");

		m_pathEdit = new QLineEdit( this );
		layout->addWidget( m_pathEdit, 1, 2 );
		m_pathEdit->setText("/pub/sword/raw");

		mainLayout->addSpacing( 10 );

		QHBoxLayout* buttonLayout = new QHBoxLayout( mainLayout );
		buttonLayout->addStretch();
		QPushButton* okButton = new QPushButton( i18n("Ok"), this);
		QPushButton* discardButton = new QPushButton( i18n("Discard"), this);
		buttonLayout->addWidget( discardButton);
		buttonLayout->addWidget( okButton);
		buttonLayout->addStretch();

		connect( okButton, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
		connect( discardButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
		connect( m_protocolCombo, SIGNAL( activated(int) ), this, SLOT( slotProtocolChanged() ) );

	}
	void CSwordSetupInstallSourcesDialog::slotOk() {
		//run a few tests to validate the input first
		if ( m_captionEdit->text().stripWhiteSpace().isEmpty() ) { //no caption
			QMessageBox::information( this, i18n( "Error" ), i18n("Please provide a caption."), QMessageBox::Retry);
			return;
		}

		BTInstallMgr iMgr;
		sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source( &iMgr, m_captionEdit->text() );
		if ( (QString)is.caption.c_str() == m_captionEdit->text() ) { //source already exists
			QMessageBox::information( this, i18n( "Error" ),
									  i18n("A source with this caption already exists.<br>Please provide a different caption."), QMessageBox::Retry);
			return;
		}

		if ( m_protocolCombo->currentText() == PROTO_FTP &&
				m_serverEdit->text().stripWhiteSpace().isEmpty() ) { //no server name
			QMessageBox::information( this, i18n( "Error" ), i18n("Please provide a server name."), QMessageBox::Retry);
			return;
		}

		if ( m_protocolCombo->currentText() == PROTO_FILE) {
			const QFileInfo fi( m_pathEdit->text() );
			if (!fi.exists() || !fi.isReadable()) { //no valid and readable path
				QMessageBox::information( this, i18n( "Error" ), i18n("Please provide a valid, readable path."), QMessageBox::Retry);
				return;
			}
			else if ( m_pathEdit->text().isEmpty() ) {
				QMessageBox::information( this, i18n( "Error" ), i18n("Please provide a path."), QMessageBox::Retry);

			}
		}

		accept(); //only if nothing else failed
	}

	void CSwordSetupInstallSourcesDialog::slotProtocolChanged() {
		if (m_protocolCombo->currentText() == PROTO_FTP) { //REMOTE
			m_serverLabel->show();
			m_serverEdit->show();
		}
		else { //LOCAL, no server needed
			m_serverLabel->hide();
			m_serverEdit->hide();

			KURL url = KDirSelectDialog::selectDirectory(QString::null, true);
			if (url.isValid()) {
				m_pathEdit->setText( url.path() );
			}
		}

	}

	sword::InstallSource CSwordSetupInstallSourcesDialog::getSource() {

		util::scoped_ptr<CSwordSetupInstallSourcesDialog> dlg( new CSwordSetupInstallSourcesDialog() );
		sword::InstallSource newSource(""); //empty, invalid Source

		if (dlg->exec() == QDialog::Accepted) {
			if (dlg->m_protocolCombo->currentText() == PROTO_FTP) {
				newSource.type = "FTP";
				newSource.source = dlg->m_serverEdit->text().utf8();

				//a message to the user would be nice, but we're in message freeze right now (1.5.1)
				if (dlg->m_serverEdit->text().right(1) == "/") { //remove a trailing slash
					newSource.source  = dlg->m_serverEdit->text().mid(0, dlg->m_serverEdit->text().length()-1).utf8();
				}
			}
			else {
				newSource.type = "DIR";
				newSource.source = "local";
			}
			newSource.caption = dlg->m_captionEdit->text().utf8();
			newSource.directory = dlg->m_pathEdit->text().utf8();
		}

		return newSource;
	}


} //namespace
