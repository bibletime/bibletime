/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//own includes
#include "cinputdialog.h"

//Qt includes
#include <qwidget.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

//KDE includes
#include <klocale.h>
#include <kseparator.h>

CInputDialog::CInputDialog(const QString& caption, const QString& description, const QString& text, QWidget *parent, const char *name, const bool modal ) : KDialog(parent,name,modal) {
	setPlainCaption(caption);

	QVBoxLayout* topLayout = new QVBoxLayout(this, 5,5);

	QLabel* l = new QLabel(description, this);
	topLayout->addWidget(l);

	topLayout->addSpacing(10);

	m_editWidget = new QTextEdit(this, "edit widget");
	m_editWidget->setWordWrap( QTextEdit::WidgetWidth );
	m_editWidget->setText(text);
	if (!text.isEmpty())
		m_editWidget->selectAll();

	topLayout->addWidget(m_editWidget);

	KSeparator* separator = new KSeparator(KSeparator::HLine, this);
	topLayout->addWidget(separator);

	QHBoxLayout* buttonLayout = new QHBoxLayout(topLayout);

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
	return m_editWidget->text();
}

/** A static function to get some using CInputDialog. */
const QString CInputDialog::getText( const QString& caption, const QString& description, const QString& text, bool* ok, QWidget* parent, bool modal) {
	CInputDialog* dlg = new CInputDialog(caption, description, text, parent, "", modal);
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
