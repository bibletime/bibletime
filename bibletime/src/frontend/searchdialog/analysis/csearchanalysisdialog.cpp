//
// C++ Implementation: csearchanalysisdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "csearchanalysisdialog.h"
#include "csearchanalysisscene.h"
#include "csearchanalysisview.h"

#include "backend/drivers/cswordmoduleinfo.h"


#include <QDialog>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>



namespace Search {


CSearchAnalysisDialog::CSearchAnalysisDialog( ListCSwordModuleInfo modules, QWidget* parentDialog )
	: QDialog(parentDialog)
{
	initView();
	m_analysis->reset();
	m_analysis->analyse(modules);
	showMaximized();
}

/** Initializes this dialog. */
void CSearchAnalysisDialog::initView()
{

	QVBoxLayout *vboxLayout = new QVBoxLayout(this);

	m_analysis = new CSearchAnalysisScene(this);
	m_analysisView = new CSearchAnalysisView(m_analysis, this);
//	m_analysisView->show();
	vboxLayout->addWidget(m_analysisView);

	m_buttonBox = new QDialogButtonBox(this);
	m_buttonBox->setOrientation(Qt::Horizontal);
	m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
	m_buttonBox->addButton(QDialogButtonBox::Save);
	//tr("Save as HTML"), 

	vboxLayout->addWidget(m_buttonBox);

	QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	QObject::connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
}

void CSearchAnalysisDialog::buttonClicked(QAbstractButton* button)
{
	if (m_buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
		m_analysis->saveAsHTML();
	}
}

}
