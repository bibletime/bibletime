/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btmodulemanagerdialog.h"
#include "btmodulemanagerdialog.moc"

#include "btinstallpage.h"

#include <QDialogButtonBox>

BtModuleManagerDialog::BtModuleManagerDialog(QWidget* parent)
	: BtConfigDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("BibleTime Bookshelf Manager"));
	BtInstallPage* installPage = new BtInstallPage();
	addPage(installPage);
	QObject::connect(this, SIGNAL(swordSetupChanged()), installPage, SLOT(slotSwordSetupChanged()));
	//Uninstall page
	//Index page

	// Dialog button (Close)
	QDialogButtonBox* bbox = new QDialogButtonBox(this);
	bbox->addButton(QDialogButtonBox::Close);
	addButtonBox(bbox);
	connect(bbox, SIGNAL(rejected()), SLOT(close()));
}

// The QWidget close() sends close event, so does closing by the window X button.
void BtModuleManagerDialog::closeEvent(QCloseEvent*)
{
	qDebug("BtModuleManagerDialog::closeEvent");
	// TODO: should we do this here or after every change? If the dialog is
	// nonmodal the user may never close it. But then the change may be applied
	// in the middle of some other user action - is it OK?
	//writeSwordConfig();
	//emit signalSwordSetupChanged( );
}