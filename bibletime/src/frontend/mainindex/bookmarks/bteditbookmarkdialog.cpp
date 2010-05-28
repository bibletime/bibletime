/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bteditbookmarkdialog.h"

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "util/dialogutil.h"

 
BtEditBookmarkDialog::BtEditBookmarkDialog
(const QString& caption,  const QString& keyText,
 const QString& title, const QString& titleText, 
 const QString& description, const QString& descriptionText, 
 QWidget *parent, Qt::WindowFlags wflags )
        : QDialog(parent, wflags) {
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    setWindowTitle(caption);

    resize(400, 300);
    vboxLayout = new QVBoxLayout(this);
    
    label = new QLabel(keyText, this);
    label->setAlignment(Qt::AlignHCenter);
    vboxLayout->addWidget(label);

    spacerItem = new QSpacerItem(12,12);
    vboxLayout->addSpacerItem(spacerItem);
    
    label = new QLabel(title, this);
    vboxLayout->addWidget(label);

    m_titleEdit = new QLineEdit(this);
    vboxLayout->addWidget(m_titleEdit);
    m_titleEdit->setText(titleText);
    
    spacerItem = new QSpacerItem(12,12);
    vboxLayout->addSpacerItem(spacerItem);
    
    label = new QLabel(description, this);
    vboxLayout->addWidget(label);

    m_descriptionEdit = new QTextEdit(this);
    vboxLayout->addWidget(m_descriptionEdit);
    m_descriptionEdit->setWordWrapMode( QTextOption::WordWrap );
    m_descriptionEdit->setText(descriptionText);

    hboxLayout = new QHBoxLayout();
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

    m_descriptionEdit->setFocus();
}

/** Returns the description. */
const QString BtEditBookmarkDialog::descriptionText() {
    return m_descriptionEdit->toPlainText();
}

/** Returns the title. */
const QString BtEditBookmarkDialog::titleText() {
    return m_titleEdit->text();
}

/** A static function to edit a bookmark's title and description using BtEditBookmarkDialog. */
void BtEditBookmarkDialog::getText
( const QString& caption, const QString& keyText,
  const QString& title, QString* titleText, 
  const QString& description, QString* descriptionText, 
  bool* ok, QWidget* parent, Qt::WindowFlags wflags) {
    BtEditBookmarkDialog* dlg = new BtEditBookmarkDialog(caption, keyText,
							 title, *titleText,
							 description, *descriptionText, parent, wflags);

    *ok = (dlg->exec() == QDialog::Accepted) ? true : false;
    if (*ok) {
        //qDebug() << "dialog was accepted, return text: " << dlg->text();
        *descriptionText = dlg->descriptionText();
	*titleText = dlg->titleText();
    }

    delete dlg;
}
