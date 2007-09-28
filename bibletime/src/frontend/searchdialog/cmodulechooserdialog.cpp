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
#include "cmodulechooserdialog.moc"

#include "backend/drivers/cswordmoduleinfo.h"

#include <QDialog>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QVBoxLayout>


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
	QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;

	vboxLayout = new QVBoxLayout(this);

    m_moduleChooser = new QTreeWidget(this);
    vboxLayout->addWidget(m_moduleChooser);

    hboxLayout = new QHBoxLayout();

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    hboxLayout->addWidget(m_buttonBox);

    vboxLayout->addLayout(hboxLayout);

}

void CModuleChooserDialog::setModules(ListCSwordModuleInfo& modules)
{
	//Populate the chooser widget with items
	
}

/** Initializes the connections of this dialog. */
void CModuleChooserDialog::initConnections()
{
	//QObject::connect(this, SIGNAL(accepted()), this, SLOT(slotOk()) );
	QObject::connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
}

void CModuleChooserDialog::slotOk()
{
	//create the list here?
	ListCSwordModuleInfo mi; //TODO: testing
	emit modulesChanged( /*m_moduleChooser->modules()*/ mi); //TODO: testing

	//TODO: forwad this message to QDialog?;
	QDialog::done(0);
}

}} //end of namespaces Options and Search
