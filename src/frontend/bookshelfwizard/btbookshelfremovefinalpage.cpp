/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelfremovefinalpage.h"

#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "btbookshelfwizard.h"


namespace {
const QString groupingOrderKey ("GUI/BookshelfWizard/InstallPage/grouping");
const QString installPathKey   ("GUI/BookshelfWizard/InstallPage/installPathIndex");
} // anonymous namespace

BtBookshelfRemoveFinalPage::BtBookshelfRemoveFinalPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    m_verticalLayout->addItem(new QSpacerItem(20,
                                              40,
                                              QSizePolicy::Minimum,
                                              QSizePolicy::Expanding));

    m_msgLabel = new QLabel(this);
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setObjectName(QStringLiteral("msgLabel"));
    m_msgLabel->setWordWrap(true);
    m_verticalLayout->addWidget(m_msgLabel);

    m_verticalLayout->addItem(new QSpacerItem(20,
                                              40,
                                              QSizePolicy::Minimum,
                                              QSizePolicy::Expanding));
}

void BtBookshelfRemoveFinalPage::retranslateUi()
{ m_msgLabel->setText(tr("The selected works have been removed.")); }

void BtBookshelfRemoveFinalPage::initializePage() {
    retranslateUi();

    // Remove works:
    CSwordBackend::instance()->uninstallModules(btWizard().selectedWorks());
}

bool BtBookshelfRemoveFinalPage::isComplete() const { return true; }

int BtBookshelfRemoveFinalPage::nextId() const { return -1; }
