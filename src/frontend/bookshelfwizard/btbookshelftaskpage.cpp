/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelftaskpage.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

BtBookshelfTaskPage::BtBookshelfTaskPage(QWidget *parent)
    : QWizardPage(parent) {

    setupUi();
    retranslateUi();
}

void BtBookshelfTaskPage::setupUi() {

    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    m_warningLabel = new QLabel(this);
    m_warningLabel->setObjectName(QStringLiteral("warningLabel"));
    m_warningLabel->setWordWrap(true);
    m_verticalLayout->addWidget(m_warningLabel);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer);

    setupRadioButtons();

    QSpacerItem *verticalSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer2);

    m_verticalLayout->setStretch(0, 1);
    m_verticalLayout->setStretch(1, 1);
    m_verticalLayout->setStretch(2, 1);
    m_verticalLayout->setStretch(3, 3);
}

void BtBookshelfTaskPage::setupRadioButtons() {

    m_groupBox = new QGroupBox(this);

    QVBoxLayout *verticalLayout2 = new QVBoxLayout(m_groupBox);
    verticalLayout2->setObjectName(QStringLiteral("verticalLayout2"));

    m_installRadioButton = new QRadioButton(m_groupBox);
    m_installRadioButton->setObjectName(QStringLiteral("installRadioButton"));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_installRadioButton->sizePolicy().hasHeightForWidth());
    m_installRadioButton->setSizePolicy(sizePolicy);
    m_installRadioButton->setChecked(true);
    verticalLayout2->addWidget(m_installRadioButton);

    m_updateRadioButton = new QRadioButton(m_groupBox);
    m_updateRadioButton->setObjectName(QStringLiteral("updateRadioButton"));
    sizePolicy.setHeightForWidth(m_updateRadioButton->sizePolicy().hasHeightForWidth());
    m_updateRadioButton->setSizePolicy(sizePolicy);
    verticalLayout2->addWidget(m_updateRadioButton);

    m_removeRadioButton = new QRadioButton(m_groupBox);
    m_removeRadioButton->setObjectName(QStringLiteral("removeRadioButton"));
    verticalLayout2->addWidget(m_removeRadioButton);

    m_verticalLayout->addWidget(m_groupBox);
}

void BtBookshelfTaskPage::retranslateUi()
{
    setTitle(QApplication::translate("BookshelfWizard", "Bookshelf Management", 0));
    setSubTitle(QApplication::translate("BookshelfWizard", "Manage works in your bookshelf.", 0));
    m_warningLabel->setText(QApplication::translate("BookshelfWizard",
                                                    "<html><head/><body><p><span style=\" font-weight:600;\">WARNING</span>: Installing or updating works uses the internet. If you live in a persecuted country you may not want to do this.</p></body></html>", 0));
    m_groupBox->setTitle(QApplication::translate("BookshelfWizard", "Bookshelf task", 0));
    m_installRadioButton->setText(QApplication::translate("BookshelfWizard", "Install additional works (uses internet)", 0));
    m_updateRadioButton->setText(QApplication::translate("BookshelfWizard", "Update installed works (uses internet)", 0));
    m_removeRadioButton->setText(QApplication::translate("BookshelfWizard", "Remove installed works", 0));

}

int BtBookshelfTaskPage::nextId() const {
    if (m_installRadioButton->isChecked())
        return WizardPage::sourcesPage;
    if (m_updateRadioButton->isChecked())
        return WizardPage::updateWorksPage;
    if (m_removeRadioButton->isChecked())
        return WizardPage::removeWorksPage;
    return -1;
}

WizardTaskType BtBookshelfTaskPage::taskType() const {
    if (m_installRadioButton->isChecked())
        return WizardTaskType::installWorks;
    if (m_updateRadioButton->isChecked())
        return WizardTaskType::updateWorks;
    return WizardTaskType::removeWorks;
}
