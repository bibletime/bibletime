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
#include "util/dialogutil.h"

#include <QWidget>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QLayout>

CConfigurationDialog::CConfigurationDialog(QWidget * parent, BtActionCollection* actionCollection )
	: BtConfigDialog(parent),
	  m_actionCollection(actionCollection),
	  m_displayPage(0),
	  m_swordPage(0),
	  m_acceleratorsPage(0),
	  m_languagesPage(0),
	  m_bbox(0)
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
	m_bbox = new QDialogButtonBox(this);
	m_bbox->addButton(QDialogButtonBox::Ok);
	m_bbox->addButton(QDialogButtonBox::Apply);
	m_bbox->addButton(QDialogButtonBox::Cancel);
	util::prepareDialogBox(m_bbox);
	addButtonBox(m_bbox);
	bool ok = connect(m_bbox, SIGNAL(clicked(QAbstractButton *)), SLOT(slotButtonClicked(QAbstractButton *)));
	Q_ASSERT(ok);

	loadDialogSettings();

	slotChangePage(0);
}

CConfigurationDialog::~CConfigurationDialog() 
{
	saveDialogSettings();
}

/** Save the dialog settings **/
void CConfigurationDialog::save()
{
//	m_acceleratorsPage->save();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();
	emit signalSettingsChanged( );
}

/** Called if any button was clicked*/
void CConfigurationDialog::slotButtonClicked(QAbstractButton* button)
{
	if (button == static_cast<QAbstractButton*>(m_bbox->button(QDialogButtonBox::Cancel)))
	{
		close();
		return;
	}

	save();

	if (button == static_cast<QAbstractButton*>(m_bbox->button(QDialogButtonBox::Ok)))
		close();
}

void CConfigurationDialog::loadDialogSettings()
{
	resize(CBTConfig::get(CBTConfig::configDialogWidth), CBTConfig::get(CBTConfig::configDialogHeight));
	move(CBTConfig::get(CBTConfig::configDialogPosX), CBTConfig::get(CBTConfig::configDialogPosY));
}

void CConfigurationDialog::saveDialogSettings()
{
	CBTConfig::set(CBTConfig::configDialogWidth, size().width());
	CBTConfig::set(CBTConfig::configDialogHeight, size().height());
	CBTConfig::set(CBTConfig::configDialogPosX, x());
	CBTConfig::set(CBTConfig::configDialogPosY, y());
}
