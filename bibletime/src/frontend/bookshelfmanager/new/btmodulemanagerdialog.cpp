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
	//Uninstall page
	//Index page

	// Dialog button (Close)
	QDialogButtonBox* bbox = new QDialogButtonBox(this);
	bbox->addButton(QDialogButtonBox::Close);
	addButtonBox(bbox);
}


void BtModuleManagerDialog::slotClose()
{

}