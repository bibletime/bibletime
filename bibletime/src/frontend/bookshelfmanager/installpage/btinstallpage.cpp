/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "btinstallpage.h"
#include "btinstallpage.moc"

#include "btinstallpathdialog.h"

#include "btinstallprogressdialog.h"
#include "btsourcewidget.h"
#include "btsourcearea.h"

#include "frontend/bookshelfmanager/bt_installmgr.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "frontend/bookshelfmanager/backend.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"

#include "frontend/cmodulechooserdialog.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "util/cpointers.h"
#include "util/ctoolclass.h"

#include <boost/scoped_ptr.hpp>


#include <QAction>
#include <QApplication>
#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTabBar>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>
#include <QProgressBar>

#include <QHeaderView>

#include <swversion.h>


// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage()
	: BtConfigPage()
{
	qDebug("BtInstallPage::BtInstallPage() start");
	initView();
	initConnections();
}

void BtInstallPage::setInstallEnabled(bool b)
{
	qDebug("void BtInstallPage::setInstallEnabled(bool b) start");
	m_installButton->setEnabled(b);
}

QString BtInstallPage::selectedInstallPath()
{
	return m_pathCombo->currentText();
}

void BtInstallPage::initView()
{
	qDebug("void BtInstallPage::initView() start");
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// installation path chooser
	QHBoxLayout* pathLayout = new QHBoxLayout();
	// beautify the layout
	int top; int bottom; int left; int right;
	pathLayout->getContentsMargins(&left, &top, &right, &bottom);
	pathLayout->setContentsMargins(left, top + 7, right, bottom + 7 );
	QSpacerItem *pathSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QLabel* pathLabel = new QLabel(tr("Install Path:"));
	m_pathCombo = new QComboBox();
	initPathCombo(); // set the paths and the current path
	m_configurePathButton = new QPushButton(tr("Configure...")); //TODO: icon only?
	
	pathLayout->addItem(pathSpacer);
	pathLayout->addWidget(pathLabel);
	pathLayout->addWidget(m_pathCombo);
	pathLayout->addWidget(m_configurePathButton);
	mainLayout->addLayout(pathLayout);
	
	// Source widget
	//QTabWidget* m_sourcesTabWidget;
	m_sourceWidget = new BtSourceWidget(this);
	mainLayout->addWidget(m_sourceWidget);
	// Install button
	QHBoxLayout *installButtonLayout = new QHBoxLayout();
	installButtonLayout->setContentsMargins(0,5,0,5);
	QSpacerItem *installButtonSpacer = new QSpacerItem(371, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	installButtonLayout->addItem(installButtonSpacer);
	m_installButton = new QPushButton(tr("Install"), this);
	m_installButton->setEnabled(false);
	installButtonLayout->addWidget(m_installButton);
	
	mainLayout->addLayout(installButtonLayout);
}

void BtInstallPage::initConnections()
{
	qDebug("void BtInstallPage::initConnections() start");
	QObject::connect(m_pathCombo, SIGNAL(activated(const QString&)), this , SLOT(slotPathChanged()));
	QObject::connect(m_configurePathButton, SIGNAL(clicked()), this, SLOT(slotEditPaths()));
	QObject::connect(m_installButton, SIGNAL(clicked()), m_sourceWidget, SLOT(slotInstall()) );
	//source widget has its own connections, not here
}

void BtInstallPage::initPathCombo()
{
	qDebug("void BtInstallPage::initPathCombo() start");
	//populate the combo list
	m_pathCombo->clear();

	QStringList targets = backend::targetList();
	for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
		if ((*it).isEmpty()) continue;
		m_pathCombo->addItem(*it);
	}
	
	// TODO:choose the current value from config
}

void BtInstallPage::slotPathChanged(const QString& pathText)
{
	// TODO: save to config
}

void BtInstallPage::slotEditPaths()
{
	qDebug("void BtInstallPage::slotEditPaths() start");
	// Now: do nothing, editing is done in another page
	// (we have to catch the signal sent from there to refresh the combo?)
	//return;
 
	// Later: open the dialog
	BtInstallPathDialog* dlg = new BtInstallPathDialog();
	int result = dlg->exec();
	if (result == QDialog::Accepted) {
		initPathCombo();
		dynamic_cast<BtModuleManagerDialog*>(parentDialog())->slotSwordSetupChanged();	
	}
	// if the dialog was accepted, set the paths and save them
	// and repopulate the combo
}

// implement the BtConfigPage methods

QString BtInstallPage::iconName()
{
	return "install_modules.svg";
}
QString BtInstallPage::label()
{
	// TODO: move the warning to a dialog which is shown when adding a source.
	return tr("Install and update works. Add remote or local sources, refresh them, select the works to be installed/updated and click Install.<br/><b>WARNING:</b> If you live in a persecuted country and don't want to risk detection don't use remote sources.");
}
QString BtInstallPage::header()
{
	return tr("Install/Update");
}

void BtInstallPage::slotSwordSetupChanged()
{
	qDebug() << "BtInstallPage::slotSwordSetupChanged, does nothing yet, should update sources";
	//m_sourceWidget, each sourceArea->createModuleTree
	// BUG: the backend is not updated
	for (int i = 0; i < m_sourceWidget->count(); i++ ) {
		BtSourceArea* sourceArea = dynamic_cast<BtSourceArea*>(m_sourceWidget->widget(i));
		Q_ASSERT(sourceArea);
		sourceArea->createModuleTree();
	}
}



