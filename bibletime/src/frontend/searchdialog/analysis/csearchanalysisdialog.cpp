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


#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

namespace Search {

CSearchAnalysisDialog::CSearchAnalysisDialog()
 : QDialog()
{
}


CSearchAnalysisDialog::~CSearchAnalysisDialog()
{
}

CSearchAnalysisDialog::CSearchAnalysisDialog( ListCSwordModuleInfo modules, QWidget* parentDialog )
	: QDialog(parentDialog)
{
	initView();
	m_analysis->reset();
	m_analysis->analyse(modules);
	showMaximized();
}

/** Initializes this dialog. */
void CSearchAnalysisDialog::initView() {
	QVBoxLayout* layout = new QVBoxLayout(this);

	QPushButton* button = new QPushButton(plainPage(), "button");
	button->setIcon(util::filesystem::DirectoryUtil::getIcon("filesave"));
	button->setText(i18n("Save search analysis as HTML"));
	button->setFixedSize(button->sizeHint());
	layout->addWidget(button);
	layout->addSpacing(10);

	m_analysis = new CSearchAnalysisScene(this);
	m_analysisView = new CSearchAnalysisView(m_analysis, this);
	m_analysisView->show();
	layout->addWidget(m_analysisView);

	connect(button, SIGNAL(clicked()), m_analysis, SLOT(saveAsHTML()));
}


}
