//
// C++ Implementation: cconfigurationdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cconfigurationdialog.h"
#include "cconfigurationdialog.moc"

#include "cdisplaysettings.h"
#include "cswordsettings.h"
#include "clanguagesettings.h"
#include "cacceleratorsettings.h"


#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QWidget>

#include <kpagedialog.h>
#include <kpagewidgetmodel.h>
#include <kactioncollection.h>


CConfigurationDialog::CConfigurationDialog
	(QWidget *parent, KActionCollection* actionCollection )
	: KPageDialog(0, Qt::Dialog),
	  m_actionCollection(actionCollection)
{
	
	setCaption(tr("Configuration"));
	setFaceType(KPageDialog::List);
	
	//General buttons
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);
	connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

	// Add "Display" page
	m_displayPage = new CDisplaySettingsPage(this);
	KPageWidgetItem* displayPage = new KPageWidgetItem(m_displayPage);
	displayPage->setHeader( tr( "Display" ) );
	displayPage->setName( tr( "Display" ) );
	displayPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::startup::icon)) );
	addPage(displayPage);

	// Add "Desk" (sword) page
	m_swordPage = new CSwordSettingsPage(this);
	KPageWidgetItem* swordPage = new KPageWidgetItem(m_swordPage);
	swordPage->setHeader( tr( "Desk" ) );
	swordPage->setName( tr( "Desk" ) );
	swordPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::sword::icon)) );
	addPage(swordPage);

	// Add "Languages" (fonts) page
	m_languagesPage = new CLanguageSettingsPage(this);
	KPageWidgetItem* langPage = new KPageWidgetItem(m_languagesPage);
	langPage->setHeader( tr( "Languages" ) );
	langPage->setName( tr( "Languages" ) );
	langPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::fonts::icon)) );
	addPage(langPage);

	// Add "Keyboard" (accelerators) page
	m_acceleratorsPage = new CAcceleratorSettingsPage(this);
	KPageWidgetItem* accelPage = new KPageWidgetItem(m_acceleratorsPage);
	accelPage->setHeader( tr( "HotKeys" ) );
	accelPage->setName( tr( "HotKeys" ) );
	accelPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::keys::icon)) );
	addPage(accelPage);


	setCurrentPage(displayPage);
	
}

CConfigurationDialog::~CConfigurationDialog() {}

/** Called if the OK button was clicked */
void CConfigurationDialog::slotOk()
{
	m_acceleratorsPage->save();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();

	emit signalSettingsChanged( );
}

/** Called if the apply button was clicked*/
void CConfigurationDialog::slotApply()
{
	qDebug("CConfigurationDialog::slotApply");
	m_acceleratorsPage->save();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();
	emit signalSettingsChanged( );
	qDebug("CConfigurationDialog::slotApply end");
}

