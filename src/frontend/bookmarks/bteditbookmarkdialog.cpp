/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bteditbookmarkdialog.h"

#include <QDialogButtonBox>
#include <QFlags>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextOption>
#include <QVBoxLayout>
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../messagedialog.h"


BtEditBookmarkDialog::BtEditBookmarkDialog(const QString &key,
                                           const QString &title,
                                           const QString &description,
                                           QWidget *parent,
                                           Qt::WindowFlags wflags)
    : QDialog(parent, wflags)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    resize(400, 300);
    setWindowIcon(CResMgr::mainIndex::bookmark::icon());

    m_layout = new QFormLayout;

    m_keyLabel = new QLabel(this);
    m_keyTextLabel = new QLabel(key, this);
    m_layout->addRow(m_keyLabel, m_keyTextLabel);

    m_titleLabel = new QLabel(this);
    m_titleEdit = new QLineEdit(title, this);
    m_layout->addRow(m_titleLabel, m_titleEdit);

    m_descriptionLabel = new QLabel(this);
    m_descriptionEdit = new QTextEdit(description, this);
    m_descriptionEdit->setWordWrapMode(QTextOption::WordWrap);
    m_layout->addRow(m_descriptionLabel, m_descriptionEdit);

    mainLayout->addLayout(m_layout);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel
                                       | QDialogButtonBox::NoButton
                                       | QDialogButtonBox::Ok,
                                       Qt::Horizontal,
                                       this);
    message::prepareDialogBox(m_buttonBox);
    mainLayout->addWidget(m_buttonBox);

    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               this, &BtEditBookmarkDialog::accept);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &BtEditBookmarkDialog::reject);

    retranslateUi();

    m_titleEdit->setFocus();
}

void BtEditBookmarkDialog::retranslateUi() {
    setWindowTitle(tr("Edit Bookmark"));
    m_keyLabel->setText(tr("Location:"));
    m_titleLabel->setText(tr("Title:"));
    m_descriptionLabel->setText(tr("Description:"));

    /// \todo Add tooltips and what's this texts etc.
}
