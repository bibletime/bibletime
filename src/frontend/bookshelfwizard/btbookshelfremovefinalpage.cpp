/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfremovefinalpage.h"

#include "backend/btinstallbackend.h"
#include "backend/btinstallmgr.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "util/btconnect.h"

#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

namespace {
const QString groupingOrderKey ("GUI/BookshelfWizard/InstallPage/grouping");
const QString installPathKey   ("GUI/BookshelfWizard/InstallPage/installPathIndex");
}

BtBookshelfRemoveFinalPage::BtBookshelfRemoveFinalPage(
        QWidget *parent)
    : BtBookshelfWizardPage(parent),
      m_verticalLayout(nullptr) {

    setupUi();
}

void BtBookshelfRemoveFinalPage::setupUi() {
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer);

    m_msgLabel = new QLabel(this);
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setObjectName(QStringLiteral("msgLabel"));
    m_msgLabel->setWordWrap(true);
    m_verticalLayout->addWidget(m_msgLabel);

    QSpacerItem *verticalSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer2);
}

void BtBookshelfRemoveFinalPage::retranslateUi() {
    m_msgLabel->setText(tr("The selected works have been removed."));
}

void BtBookshelfRemoveFinalPage::initializePage() {
    removeWorks();
    retranslateUi();
}

bool BtBookshelfRemoveFinalPage::isComplete() const {
    return true;
}

int BtBookshelfRemoveFinalPage::nextId() const {
    return -1;
}

void BtBookshelfRemoveFinalPage::removeWorks() {

    BtModuleSet selectedWorks = btWizard().selectedWorks();
    if (selectedWorks.isEmpty())
        return;

    QStringList moduleNames;
    for (auto m : selectedWorks)
        moduleNames.append(m->name());

    // Update the module list before really removing. Remember deleting the pointers later.
    QList<CSwordModuleInfo*> toBeDeleted = CSwordBackend::instance()->takeModulesFromList(moduleNames);
    BT_ASSERT(toBeDeleted.size() == moduleNames.size());

    BtInstallMgr installMgr;
    QMap<QString, sword::SWMgr*> mgrDict; //maps config paths to SWMgr objects
    Q_FOREACH(CSwordModuleInfo const * const mInfo, toBeDeleted) {
        BT_ASSERT(mInfo); // Only installed modules could have been selected and returned by takeModulesFromList
        // Find the install path for the sword manager
        QString prefixPath = mInfo->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
        QString dataPath = mInfo->config(CSwordModuleInfo::DataPath);
        if (dataPath.left(2) == "./") {
            dataPath = dataPath.mid(2);
        }
        if (prefixPath.contains(dataPath)) { //remove module part to get the prefix path
            prefixPath = prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() );
        }
        else { //This is an error, should not happen
            qWarning() << "Removing" << mInfo->name() << "didn't succeed because the absolute path" << prefixPath << "didn't contain the data path" << dataPath;
            continue; // don't remove this, go to next of the for loop
        }

        // Create the sword manager and remove the module
        sword::SWMgr* mgr = mgrDict[ prefixPath ];
        if (!mgr) { //create new mgr if it's not yet available
            mgrDict.insert(prefixPath, new sword::SWMgr(prefixPath.toLocal8Bit()));
            mgr = mgrDict[ prefixPath ];
        }
        qDebug() << "Removing the module" << mInfo->name() << "...";
        installMgr.removeModule(mgr, mInfo->module().getName());
    }
    //delete the removed moduleinfo pointers
    qDeleteAll(toBeDeleted);
    //delete all mgrs which were created above
    qDeleteAll(mgrDict);
    mgrDict.clear();
}
