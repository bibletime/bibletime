//
// C++ Implementation: cmodulechooserdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmodulechooserdialog.h"

#include <QDialog>
#include "backend/drivers/cswordmoduleinfo.h"

namespace Search { namespace Options {

CModuleChooserDialog::CModuleChooserDialog( QWidget* parentDialog, ListCSwordModuleInfo modules )
	: QDialog(parentDialog)
	//KDialogBase(Plain, i18n("Choose work(s)"), Ok, Ok, parentDialog, "CModuleChooser", false, true)
{
	initView();
	initConnections();

	setModules(modules);
};

CModuleChooserDialog::~CModuleChooserDialog() {}
;

/** Initializes the view of this dialog */
void CModuleChooserDialog::initView() {
	//TODO: choose the button text
	//setButtonOKText(i18n("Use chosen work(s)"));
}

void CModuleChooserDialog::setModules(ListCSwordModuleInfo& modules)
{
	//Populate the chooser widget with items
	m_moduleChooser;
}

/** Initializes the connections of this dialog. */
void CModuleChooserDialog::initConnections()
{
	QObject::connect(this, SIGNAL(accepted()), this, SLOT(slotOk()) );
}

void CModuleChooserDialog::slotOk()
{
	//create the list here?
	ListCSwordModuleInfo mi; //TODO: testing
	emit modulesChanged( /*m_moduleChooser->modules()*/ mi); //TODO: testing

	//TODO: forwad this message to QDialog?;
}

}} //end of namespaces Options and Search
