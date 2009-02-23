/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchanalysisdialog.h"
#include "csearchanalysisdialog.moc"
#include "csearchanalysisscene.h"
#include "csearchanalysisview.h"

#include "backend/drivers/cswordmoduleinfo.h"


#include <QDialog>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

namespace Search {

static const int DIALOG_HEIGHT=400;
static const int DIALOG_BORDER=30;

CSearchAnalysisDialog::CSearchAnalysisDialog( QList<CSwordModuleInfo*> modules, QWidget* parentDialog )
	: QDialog(parentDialog)
{
	initView();
	m_analysis->reset();
	m_analysis->analyse(modules);
	
	// Set initial width based on the search data, but limit to the
	// width of the desktop
	int width = m_analysis->width()+DIALOG_BORDER;
	int desktopWidth = QApplication::desktop()->screenGeometry(this).width();
	if (width > desktopWidth)
		width = desktopWidth;
	resize(width, DIALOG_HEIGHT);
	
}

/** Initializes this dialog. */
void CSearchAnalysisDialog::initView()
{

	QVBoxLayout *vboxLayout = new QVBoxLayout(this);

	m_analysis = new CSearchAnalysisScene(this);
	m_analysisView = new CSearchAnalysisView(m_analysis, this);
////	m_analysisView->show();
	vboxLayout->addWidget(m_analysisView);

	m_buttonBox = new QDialogButtonBox(this);
	m_buttonBox->setOrientation(Qt::Horizontal);
	m_buttonBox->setStandardButtons(QDialogButtonBox::Close);
	m_buttonBox->addButton(QDialogButtonBox::Save);
	//tr("Save as HTML"), 

	vboxLayout->addWidget(m_buttonBox);

	bool ok = QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	Q_ASSERT(ok);
	ok = QObject::connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
	Q_ASSERT(ok);
}

void CSearchAnalysisDialog::buttonClicked(QAbstractButton* button)
{
	if (m_buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
		m_analysis->saveAsHTML();
	}
}

void CSearchAnalysisDialog::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);
	m_analysis->resizeHeight(height());
}

}
