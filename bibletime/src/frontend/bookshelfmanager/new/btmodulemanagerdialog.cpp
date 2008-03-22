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

#include "installpage/btinstallpage.h"
#include "removepage/btremovepage.h"
#include "indexpage/btindexpage.h"

#include "util/cpointers.h"
#include "backend/managers/cswordbackend.h"

#include <QDialogButtonBox>

BtModuleManagerDialog::BtModuleManagerDialog(QWidget* parent)
	: BtConfigDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("BibleTime Bookshelf Manager"));

	// Install page
	BtInstallPage* installPage = new BtInstallPage();
	addPage(installPage);
	QObject::connect(this, SIGNAL(swordSetupChanged()), installPage, SLOT(slotSwordSetupChanged()));
	//The vice versa signal/slot is connected in a page class

	//Uninstall page
	BtRemovePage* removePage = new BtRemovePage();
	addPage(removePage);
	QObject::connect(this, SIGNAL(swordSetupChanged()), removePage, SLOT(slotSwordSetupChanged()));
	QObject::connect(removePage, SIGNAL(swordSetupChanged()), this, SLOT(slotSwordSetupChanged()));

	//Index page
	BtIndexPage* indexPage = new BtIndexPage();
	addPage(indexPage);
	QObject::connect(this, SIGNAL(swordSetupChanged()), indexPage, SLOT(slotSwordSetupChanged()));
	//QObject::connect(indexPage, SIGNAL(swordSetupChanged()), this, SLOT(slotSwordSetupChanged()));



	//TODO: select the page from config? Now it's always the first one
	slotChangePage(0);

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

void BtModuleManagerDialog::slotSwordSetupChanged()
{
	CPointers::backend()->reloadModules();
	emit swordSetupChanged();
}

