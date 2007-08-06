/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//own includes
#include "cinputdialog.h"
#include "cinputdialog.moc"

//Qt includes
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
//#include <QLayout>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>

//KDE includes
#include <klocale.h>
#include <kseparator.h>
#include <kdialog.h>

CInputDialog::CInputDialog
	(const QString& caption, const QString& description, const QString& text, QWidget *parent, Qt::WindowFlags wflags )
	: KDialog(parent, wflags)
{
	setPlainCaption(caption);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->setSpacing(5);
	topLayout->setMargin(5);

	QLabel* l = new QLabel(description, this);
	topLayout->addWidget(l);

	topLayout->addSpacing(10);

	m_editWidget = new QTextEdit(this);
	m_editWidget->setWordWrapMode( QTextOption::WordWrap );
	m_editWidget->setText(text);
	if (!text.isEmpty())
		m_editWidget->selectAll();

	topLayout->addWidget(m_editWidget);

	KSeparator* separator = new KSeparator(Qt::Horizontal, this);
	topLayout->addWidget(separator);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	topLayout->addLayout(buttonLayout);

	buttonLayout->addStretch(2);

	QPushButton* cancel = new QPushButton(this);
	cancel->setText(i18n("&Cancel"));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	buttonLayout->addWidget(cancel,1);

	buttonLayout->addSpacing(15);

	QPushButton* clear = new QPushButton(this);
	clear->setText(i18n("C&lear"));
	connect(clear, SIGNAL(clicked()),m_editWidget, SLOT(clear()));
	buttonLayout->addWidget(clear,1);

	buttonLayout->addSpacing(15);

	QPushButton* ok = new QPushButton(this);
	ok->setText(i18n("&Ok"));
	connect(ok, SIGNAL(clicked()), SLOT(accept()));
	buttonLayout->addWidget(ok,1);

	m_editWidget->setFocus();
}

/** Returns the text entered at the moment. */
const QString CInputDialog::text() {
	return m_editWidget->toPlainText();
}

/** A static function to get some using CInputDialog. */
const QString CInputDialog::getText
	( const QString& caption, const QString& description, const QString& text, bool* ok, QWidget* parent, Qt::WindowFlags wflags)
{
	CInputDialog* dlg = new CInputDialog(caption, description, text, parent, wflags);
	QString ret = QString::null;

	const bool isOk = (dlg->exec() == CInputDialog::Accepted);
	if (isOk) {
		ret = dlg->text();
	}

	if (ok) { //change the ok param to return the value
		*ok = isOk;
	}

	delete dlg;
	return ret;
}
