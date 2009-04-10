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
#include "util/dialogutil.h"
#include "backend/managers/cswordbackend.h"

#include <QDialogButtonBox>


static BtModuleManagerDialog* m_staticModuleManagerDialog = 0;

BtModuleManagerDialog* BtModuleManagerDialog::getInstance(QWidget* parent)
{
	if (!m_staticModuleManagerDialog) {
		m_staticModuleManagerDialog = new BtModuleManagerDialog(parent);
	};
	Q_ASSERT(m_staticModuleManagerDialog);
	return m_staticModuleManagerDialog;
}

BtModuleManagerDialog::BtModuleManagerDialog(QWidget* parent)
	: BtConfigDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Bookshelf Manager"));

	// Install page
	BtInstallPage* installPage = new BtInstallPage();
	addPage(installPage);

	//Uninstall page
	BtRemovePage* removePage = new BtRemovePage();
	addPage(removePage);

	//Index page
	BtIndexPage* indexPage = new BtIndexPage();
	addPage(indexPage);

	slotChangePage(0);

	// Dialog button (Close)
	QDialogButtonBox* bbox = new QDialogButtonBox(this);
	bbox->addButton(QDialogButtonBox::Close);
	util::prepareDialogBox(bbox);
	addButtonBox(bbox);
	connect(bbox, SIGNAL(rejected()), SLOT(close()));
}

BtModuleManagerDialog::~BtModuleManagerDialog()
{
	m_staticModuleManagerDialog = 0;
}

// The QWidget close() sends close event, so does closing by the window X button.
void BtModuleManagerDialog::closeEvent(QCloseEvent*)
{
	qDebug("BtModuleManagerDialog::closeEvent");
}


