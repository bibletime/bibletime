/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfwizard/btbookshelftaskpage.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"

#include <QApplication>
#include <QDate>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>


BtBookshelfTaskPage::BtBookshelfTaskPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    m_warningLabel = new QLabel(this);
    m_warningLabel->setObjectName(QStringLiteral("warningLabel"));
    m_warningLabel->setWordWrap(true);

    m_verticalLayout->addWidget(m_warningLabel);
    m_verticalLayout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    m_groupBox = new QGroupBox(this);
    { // Setup radio buttons:
        QVBoxLayout * const vLayout = new QVBoxLayout(m_groupBox);
        vLayout->setObjectName(QStringLiteral("verticalLayout2"));

        m_installRadioButton = new QRadioButton(m_groupBox);
        m_installRadioButton->setObjectName(
                QStringLiteral("installRadioButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(
                m_installRadioButton->sizePolicy().hasHeightForWidth());
        m_installRadioButton->setSizePolicy(sizePolicy);
        m_installRadioButton->setChecked(true);
        vLayout->addWidget(m_installRadioButton);

        m_updateRadioButton = new QRadioButton(m_groupBox);
        m_updateRadioButton->setObjectName(QStringLiteral("updateRadioButton"));
        sizePolicy.setHeightForWidth(
                m_updateRadioButton->sizePolicy().hasHeightForWidth());
        m_updateRadioButton->setSizePolicy(sizePolicy);
        vLayout->addWidget(m_updateRadioButton);

        m_removeRadioButton = new QRadioButton(m_groupBox);
        m_removeRadioButton->setObjectName(QStringLiteral("removeRadioButton"));
        vLayout->addWidget(m_removeRadioButton);
    }
    m_verticalLayout->addWidget(m_groupBox);

    m_verticalLayout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    m_verticalLayout->setStretch(0, 1);
    m_verticalLayout->setStretch(1, 1);
    m_verticalLayout->setStretch(2, 1);
    m_verticalLayout->setStretch(3, 3);

    retranslateUi();
}

void BtBookshelfTaskPage::retranslateUi() {
    setTitle(QApplication::translate("BookshelfWizard",
                                     "Bookshelf Manager"));
    setSubTitle(QApplication::translate("BookshelfWizard",
                                        "Install, update, or remove works on your bookshelf."));
    m_warningLabel->setText(
            QApplication::translate(
                    "BookshelfWizard",
                    "<html><head/><body><p><span style=\" font-weight:600;\">"
                    "WARNING</span>: Installing or updating works uses the "
                    "internet. If you live in a persecuted country you may not "
                    "want to do this.</p></body></html>"));
    m_groupBox->setTitle(QApplication::translate("BookshelfWizard",
                                                 "Bookshelf task"));
    m_installRadioButton->setText(
            QApplication::translate(
                    "BookshelfWizard",
                    "Install additional works (uses internet)"));
    m_updateRadioButton->setText(
            QApplication::translate("BookshelfWizard",
                                    "Update installed works (uses internet)"));
    m_removeRadioButton->setText(
            QApplication::translate("BookshelfWizard",
                                    "Remove installed works"));
}

bool BtBookshelfTaskPage::timeToUpdate() const {
    BtBookshelfModel *bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel->moduleList().empty())
        return true;

    return BtBookshelfWizard::autoUpdateSources();
}

int BtBookshelfTaskPage::nextId() const {
    if (m_installRadioButton->isChecked()) {
        if (timeToUpdate())
            return WizardPage::sourcesProgressPage;
        return WizardPage::sourcesPage;
    } else if (m_updateRadioButton->isChecked()) {
        if (timeToUpdate())
            return WizardPage::sourcesProgressPage;
        return WizardPage::updateWorksPage;
    } else if (m_removeRadioButton->isChecked()) {
        return WizardPage::removeWorksPage;
    } else {
        return -1;
    }
}

WizardTaskType BtBookshelfTaskPage::taskType() const {
    if (m_installRadioButton->isChecked())
        return WizardTaskType::installWorks;
    if (m_updateRadioButton->isChecked())
        return WizardTaskType::updateWorks;
    return WizardTaskType::removeWorks;
}
