/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bteditbookmarkdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include "util/cresmgr.h"
#include "util/dialogutil.h"
#include "util/directory.h"


BtEditBookmarkDialog::BtEditBookmarkDialog(const QString &key,
                                           const QString &title,
                                           const QString &description,
                                           QWidget *parent,
                                           Qt::WindowFlags wflags)
    : QDialog(parent, wflags)
{
    namespace DU = util::directory;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    resize(400, 300);
    setWindowIcon(DU::getIcon(CResMgr::mainIndex::bookmark::icon));

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
    util::prepareDialogBox(m_buttonBox);
    mainLayout->addWidget(m_buttonBox);

    QObject::connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

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
