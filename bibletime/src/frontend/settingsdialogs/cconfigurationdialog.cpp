/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cconfigurationdialog.h"

#include "cdisplaysettings.h"
#include "cswordsettings.h"
#include "clanguagesettings.h"
//#include "cacceleratorsettings.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QWidget>
#include <QDialogButtonBox>
#include <QAbstractButton>

#include <kactioncollection.h>


CConfigurationDialog::CConfigurationDialog(QWidget * parent, KActionCollection* actionCollection )
	: BtConfigDialog(parent),
	  m_actionCollection(actionCollection)
{
	setWindowTitle(tr("Configure BibleTime"));
	setAttribute(Qt::WA_DeleteOnClose);
	
	// Add "Display" page
	m_displayPage = new CDisplaySettingsPage(this);
	addPage(m_displayPage);

	// Add "Desk" (sword) page
	m_swordPage = new CSwordSettingsPage(this);
	addPage(m_swordPage);

	// Add "Languages" (fonts) page
	m_languagesPage = new CLanguageSettingsPage(this);
	addPage(m_languagesPage);

//	// Add "Keyboard" (accelerators) page
//	m_acceleratorsPage = new CAcceleratorSettingsPage(this);
//	KPageWidgetItem* accelPage = new KPageWidgetItem(m_acceleratorsPage);
//	accelPage->setHeader( tr( "HotKeys" ) );
//	accelPage->setName( tr( "HotKeys" ) );
//	accelPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::keys::icon)) );
//	addPage(accelPage);

	// Dialog buttons
	QDialogButtonBox* bbox = new QDialogButtonBox(this);
	bbox->addButton(QDialogButtonBox::Ok);
	bbox->addButton(QDialogButtonBox::Apply);
	bbox->addButton(QDialogButtonBox::Cancel);
	addButtonBox(bbox);
	bool ok = connect(bbox, SIGNAL(clicked(QAbstractButton *)), SLOT(slotButtonClicked(QAbstractButton *)));
	Q_ASSERT(ok);

	slotChangePage(0);
}

CConfigurationDialog::~CConfigurationDialog() {}

/** Called if any button was clicked*/
void CConfigurationDialog::slotButtonClicked(QAbstractButton* button)
{
	if (button->text() == "&Cancel")
		close();

//	m_acceleratorsPage->save();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();
	emit signalSettingsChanged( );

	if (button->text() == "&OK")
		close();
}

