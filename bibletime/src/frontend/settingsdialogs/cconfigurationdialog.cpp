//
// C++ Implementation: cconfigurationdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2007
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
#include <klocale.h>

CConfigurationDialog::CConfigurationDialog
	(QWidget *parent, KActionCollection* actionCollection )
	: KPageDialog(0, Qt::Dialog),
	  m_actionCollection(actionCollection)
{
	setCaption(i18n("Configuration"));
	setFaceType(KPageDialog::List);
	
	//General buttons
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);
	connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

	// Add "Display" page
	m_displayPage = new CDisplaySettingsPage(this);
	KPageWidgetItem* displayPage = new KPageWidgetItem(m_displayPage);
	displayPage->setHeader( i18n( "Display" ) );
	displayPage->setName( i18n( "Display" ) );
	displayPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::startup::icon)) );
	addPage(displayPage);

	// Add "Desk" (sword) page
	m_swordPage = new CSwordSettingsPage(this);
	KPageWidgetItem* swordPage = new KPageWidgetItem(m_swordPage);
	swordPage->setHeader( i18n( "Desk" ) );
	swordPage->setName( i18n( "Desk" ) );
	swordPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::sword::icon)) );
	addPage(swordPage);

	// Add "Languages" (fonts) page
	m_languagesPage = new CLanguageSettingsPage(this);
	KPageWidgetItem* langPage = new KPageWidgetItem(m_languagesPage);
	langPage->setHeader( i18n( "Languages" ) );
	langPage->setName( i18n( "Languages" ) );
	langPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::fonts::icon)) );
	addPage(langPage);

	// Add "Keyboard" (accelerators) page
	m_acceleratorsPage = new CAcceleratorSettingsPage(this);
	KPageWidgetItem* accelPage = new KPageWidgetItem(m_acceleratorsPage);
	accelPage->setHeader( i18n( "HotKeys" ) );
	accelPage->setName( i18n( "HotKeys" ) );
	accelPage->setIcon( KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::settings::keys::icon)) );
	addPage(accelPage);


	setCurrentPage(displayPage);
	
}

CConfigurationDialog::~CConfigurationDialog() {}

/** Called if the OK button was clicked */
void CConfigurationDialog::slotOk() {
	//saveAccelerators();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();

	emit signalSettingsChanged( );
}

/** Called if the apply button was clicked*/
void CConfigurationDialog::slotApply() {
	//saveAccelerators();
	qDebug("CConfigurationDialog::slotApply");
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();
	emit signalSettingsChanged( );
	qDebug("CConfigurationDialog::slotApply end");
}

