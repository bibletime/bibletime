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

#include "btplainorhtmlpage.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QVBoxLayout>


BtPlainOrHtmlPage::BtPlainOrHtmlPage(QWidget * parent)
    : QWizardPage(parent)
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->addItem(
                new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                QSizePolicy::Expanding));

    m_groupBox = new QGroupBox(this);

    // Setup radio buttons:
    QVBoxLayout * const vLayout = new QVBoxLayout(m_groupBox);
    m_plainTextRadioButton = new QRadioButton(m_groupBox);
    vLayout->addWidget(m_plainTextRadioButton);
    m_htmlRadioButton = new QRadioButton(m_groupBox);
    m_htmlRadioButton->setChecked(true);
    vLayout->addWidget(m_htmlRadioButton);

    m_verticalLayout->addWidget(m_groupBox);
    m_verticalLayout->addItem(
                new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                QSizePolicy::Expanding));
    retranslateUi();
}

bool BtPlainOrHtmlPage::htmlMode() const {
    return m_htmlRadioButton->isChecked();
}

void BtPlainOrHtmlPage::setHtmlMode(bool html) {
    m_plainTextRadioButton->setChecked(!html);
    m_htmlRadioButton->setChecked(html);
}

void BtPlainOrHtmlPage::retranslateUi() {
    m_plainTextRadioButton->setText(
                tr("Personal Commentary Editor", "Edit Plain text"));
    m_htmlRadioButton->setText(
                tr("Personal Commentary Editor", "Edit HTML text"));
}
