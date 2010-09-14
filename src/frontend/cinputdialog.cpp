/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//own includes
#include "cinputdialog.h"
#include "util/dialogutil.h"

//Qt includes
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>


CInputDialog::CInputDialog
(const QString& caption, const QString& description, const QString& text, QWidget *parent, Qt::WindowFlags wflags )
        : QDialog(parent, wflags) {
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QHBoxLayout *hboxLayout;
    QPushButton *clearButton;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    setWindowTitle(caption);

    resize(400, 300);
    vboxLayout = new QVBoxLayout(this);
    label = new QLabel(description, this);
    vboxLayout->addWidget(label);

    m_textEdit = new QTextEdit(this);
    vboxLayout->addWidget(m_textEdit);
    m_textEdit->setWordWrapMode( QTextOption::WordWrap );
    m_textEdit->setText(text);
    if (!text.isEmpty())
        m_textEdit->selectAll();

    hboxLayout = new QHBoxLayout();
    clearButton = new QPushButton(this);
    clearButton->setText(tr("Clear"));
    hboxLayout->addWidget(clearButton);
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::NoButton | QDialogButtonBox::Ok);
    util::prepareDialogBox(buttonBox);
    hboxLayout->addWidget(buttonBox);

    vboxLayout->addLayout(hboxLayout);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(clearButton, SIGNAL(clicked()), m_textEdit, SLOT(clear()));

    m_textEdit->setFocus();
}

/** Returns the text entered at the moment. */
const QString CInputDialog::text() {
    return m_textEdit->toPlainText();
}

/** A static function to get some using CInputDialog. */
const QString CInputDialog::getText
( const QString& caption, const QString& description, const QString& text, bool* ok, QWidget* parent, Qt::WindowFlags wflags) {
    CInputDialog* dlg = new CInputDialog(caption, description, text, parent, wflags);

    QString ret = QString::null;
    *ok = (dlg->exec() == QDialog::Accepted) ? true : false;
    if (*ok) {
        //qDebug() << "dialog was accepted, return text: " << dlg->text();
        ret = dlg->text();
    }

    delete dlg;
    return ret;
}
