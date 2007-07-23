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
#include "cdisplaysettings.h"
#include "cswordsettings.h"
#include "clanguagesettings.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"


#include <QWidget>

#include <kpagedialog.h>
#include <kpagewidgetmodel.h>
#include <kactioncollection.h>
#include <kiconloader.h>
#include <klocale.h>

CConfigurationDialog::CConfigurationDialog
	(QWidget *parent, KActionCollection* actionCollection )
	: KPageDialog(0, Qt::Dialog),
	  m_actionCollection(actionCollection)
{
	setFaceType(KPageDialog::List);
	
	//General buttons
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);
	connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

	// Add "Display" page
	m_displayPage = new CDisplaySettingsPage(this);
	KPageWidgetItem* displayPage = new KPageWidgetItem(m_displayPage);
	displayPage->setHeader( i18n( "Display" ) );
	displayPage->setIcon( KIcon(DesktopIcon(CResMgr::settings::startup::icon,32)) );
	addPage(displayPage);

	// Add "Desk" (sword) page
	m_swordPage = new CSwordSettingsPage(this);
	KPageWidgetItem* swordPage = new KPageWidgetItem(m_swordPage);
	swordPage->setHeader( i18n( "Desk" ) );
	swordPage->setIcon( KIcon(DesktopIcon(CResMgr::settings::sword::icon,32)) );
	addPage(swordPage);

	// Add "Languages" (fonts) page
	m_languagesPage = new CLanguageSettingsPage(this);
	KPageWidgetItem* langPage = new KPageWidgetItem(m_languagesPage);
	langPage->setHeader( i18n( "Languages" ) );
	langPage->setIcon( KIcon(DesktopIcon(CResMgr::settings::fonts::icon,32)) );
	addPage(langPage);


	setCurrentPage(displayPage);
	
}

/** Called if the OK button was clicked */
void CConfigurationDialog::slotOk() {
	//saveAccelerators();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();

	KPageDialog::slotButtonClicked(KDialog::Ok);
	emit signalSettingsChanged( );
}

/** Called if the apply button was clicked*/
void CConfigurationDialog::slotApply() {
	//saveAccelerators();
	m_languagesPage->save();
	m_swordPage->save();
	m_displayPage->save();

	KPageDialog::slotButtonClicked(KDialog::Apply);
	emit signalSettingsChanged( );
}

