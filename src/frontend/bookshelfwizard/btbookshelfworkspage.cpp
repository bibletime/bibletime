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

#include "btbookshelfworkspage.h"

#include <algorithm>
#include <QAbstractItemView>
#include <QApplication>
#include <QByteArray>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QNonConstOverload>
#include <QSet>
#include <QSizePolicy>
#include <QStringList>
#include <QStringLiteral>
#include <Qt>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWizard>
#include <utility>
#include "../../backend/btinstallbackend.h"
#include "../../backend/bookshelfmodel/btbookshelfmodel.h"
#include "../../backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/btmoduleset.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/language.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../btbookshelfgroupingmenu.h"
#include "../btbookshelfview.h"
#include "btbookshelfwizard.h"
#include "btinstallpagemodel.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <installmgr.h>
#include <swbuf.h>
#include <swversion.h>
#pragma GCC diagnostic pop


namespace {
QString const groupingOrderKey("GUI/BookshelfWizard/InstallPage/grouping");
QString const installPathKey(
        "GUI/BookshelfWizard/InstallPage/installPathIndex");

inline bool filter(WizardTaskType const taskType,
                   QStringList const & languages,
                   CSwordModuleInfo const * const mInfo)
{
    if (taskType == WizardTaskType::installWorks) {
        return !CSwordBackend::instance().findModuleByName(mInfo->name())
               && languages.contains(mInfo->language()->translatedName());
    } else if (taskType == WizardTaskType::updateWorks) {
        using CSMI = CSwordModuleInfo;
        using CSV = sword::SWVersion const;
        CSMI const * const installedModule =
                CSwordBackend::instance().findModuleByName(mInfo->name());
        return installedModule
               && (CSV(installedModule->config(CSMI::ModuleVersion).toLatin1())
                   < CSV(mInfo->config(CSMI::ModuleVersion).toLatin1()));
    } else {
        BT_ASSERT(taskType == WizardTaskType::removeWorks);
        return CSwordBackend::instance().findModuleByName(mInfo->name());
    }
}

} // anonymous namespace

BtBookshelfWorksPage::BtBookshelfWorksPage(WizardTaskType iType,
                                           QWidget * parent)
    : BtBookshelfWizardPage(parent)
    , m_taskType(iType)
    , m_groupingOrder(btConfig(), groupingOrderKey)
{
    // Initialize menus:
    m_groupingMenu = new BtBookshelfGroupingMenu(this);
    m_contextMenu = new QMenu(this);
    m_contextMenu->addMenu(m_groupingMenu);
    m_itemContextMenu = m_contextMenu;

    // Setup UI:
    QVBoxLayout * const verticalLayout = new QVBoxLayout(this);

    QHBoxLayout * horizontalLayout = new QHBoxLayout;
    m_nameFilterLabel = new QLabel(this);
    horizontalLayout->addWidget(m_nameFilterLabel);
    m_nameFilterEdit = new QLineEdit(this);
    m_nameFilterLabel->setBuddy(m_nameFilterEdit);
    horizontalLayout->addWidget(m_nameFilterEdit);
    verticalLayout->addLayout(horizontalLayout);

    m_bookshelfView = new BtBookshelfView(this);
    m_bookshelfView->setHeaderHidden(false);
    m_bookshelfView->header()->setSectionResizeMode(
            QHeaderView::ResizeToContents);
    verticalLayout->addWidget(m_bookshelfView);

    m_msgLabel = new QLabel(this);
    m_msgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_msgLabel->setWordWrap(true);
    verticalLayout->addWidget(m_msgLabel);

    QHBoxLayout * const pathLayout = new QHBoxLayout();
    if (m_taskType != WizardTaskType::removeWorks) {
        m_pathCombo = new QComboBox(this);
        m_pathCombo->setMinimumContentsLength(20);
        m_pathCombo->setSizeAdjustPolicy(
                QComboBox::AdjustToMinimumContentsLengthWithIcon);
        m_pathCombo->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Minimum);
        m_pathCombo->view()->setTextElideMode(Qt::ElideMiddle);

        m_pathLabel = new QLabel(this);
        m_pathLabel->setBuddy(m_pathCombo);
        slotInitPathCombo();

        pathLayout->setContentsMargins(0, 8, 0, 0);
        pathLayout->addWidget(m_pathLabel);
        pathLayout->addWidget(m_pathCombo);
    }
    pathLayout->addStretch();

    m_groupingLabel = new QLabel(this);
    pathLayout->addWidget(m_groupingLabel);

    m_groupingButton = new QToolButton(this);
    m_groupingButton->setPopupMode(QToolButton::InstantPopup);
    m_groupingButton->setMenu(m_groupingMenu);
    m_groupingButton->setIcon(m_groupingMenu->icon());
    m_groupingButton->setAutoRaise(true);
    pathLayout->addWidget(m_groupingButton);

    verticalLayout->addLayout(pathLayout);

    // Setup models:
    BtBookshelfFilterModel * const filterModel =
            new BtBookshelfFilterModel(this);
    m_bookshelfView->setModel(filterModel);

    m_installPageModel = new BtInstallPageModel(m_groupingOrder, this);
    if (m_taskType == WizardTaskType::updateWorks)
        m_installPageModel->setDefaultChecked(BtBookshelfTreeModel::CHECKED);
    filterModel->setSourceModel(m_installPageModel);

    /// \todo is this useless if m_taskType == WizardTaskType::removeWorks?
    m_bookshelfModel = BtBookshelfModel::newInstance();
    if (m_taskType == WizardTaskType::removeWorks) {
        m_installPageModel->setSourceModel(CSwordBackend::instance().model());
    } else {
        m_installPageModel->setSourceModel(m_bookshelfModel);
    }
    if (m_taskType == WizardTaskType::removeWorks)
        m_bookshelfView->setColumnHidden(1, true);

    // Initialize connections:
    BT_CONNECT(m_groupingMenu,
               &BtBookshelfGroupingMenu::signalGroupingOrderChanged,
               this,
               &BtBookshelfWorksPage::slotGroupingActionTriggered);
    if (m_taskType != WizardTaskType::removeWorks) {
        BT_CONNECT(m_pathCombo, qOverload<int>(&QComboBox::currentIndexChanged),
                   this, &BtBookshelfWorksPage::slotPathChanged);
        BT_CONNECT(
                &CSwordBackend::instance(), &CSwordBackend::sigSwordSetupChanged,
                this, &BtBookshelfWorksPage::slotInitPathCombo);
    }
    BT_CONNECT(m_installPageModel, &BtBookshelfTreeModel::moduleChecked,
               this,               &BtBookshelfWorksPage::completeChanged);
    BT_CONNECT(m_installPageModel, &BtInstallPageModel::groupingOrderChanged,
               this, &BtBookshelfWorksPage::slotGroupingOrderChanged);
    BT_CONNECT(m_nameFilterEdit, &QLineEdit::textEdited,
               filterModel, &BtBookshelfFilterModel::setNameFilterFixedString);

    retranslateUi();
}

void BtBookshelfWorksPage::retranslateUi() {
    m_nameFilterLabel->setText(tr("Fi&lter:"));
    if (m_taskType == installWorks) {
        setTitle(QApplication::translate("BookshelfWizard", "Install Works"));
        setSubTitle(QApplication::translate("BookshelfWizard",
                                            "Choose one or more works to "
                                            "install."));
        setButtonText(QWizard::NextButton,tr("Install Works >"));
    } else if (m_taskType == WizardTaskType::updateWorks) {
        setTitle(QApplication::translate("BookshelfWizard", "Update Works"));
        setSubTitle(QApplication::translate("BookshelfWizard",
                                            "Choose one or more works to "
                                            "update."));
        setButtonText(QWizard::NextButton,tr("Update Works >"));
    } else {
        BT_ASSERT(m_taskType == WizardTaskType::removeWorks);
        setTitle(QApplication::translate("BookshelfWizard", "Remove Works"));
        setSubTitle(QApplication::translate("BookshelfWizard",
                                            "Choose one or more works to "
                                            "remove."));
        setButtonText(QWizard::NextButton,tr("Remove Works >"));
    }

    if (m_taskType != WizardTaskType::removeWorks) {
        m_pathLabel->setText(tr("Install &folder:"));
        m_pathCombo->setToolTip(tr("The folder where the new works will be "
                                   "installed"));
    }

    if (m_taskType == WizardTaskType::updateWorks) {
        m_msgLabel->setText(tr("There are no works to update."));
    } else if (m_taskType == WizardTaskType::removeWorks) {
        m_msgLabel->setText(tr("No works are currently installed so they "
                               "cannot be removed."));
    } else {
        BT_ASSERT(m_taskType == WizardTaskType::installWorks);
        m_msgLabel->setText(
                tr("No works can be installed with the current selection of "
                   "remote libraries and languages. Please go back and make a "
                   "different selection."));
    }

    m_groupingLabel->setText(tr("Grouping:"));
    m_groupingButton->setText(tr("Grouping"));
    m_groupingButton->setToolTip(
            tr("Change the grouping of items in the bookshelf."));
}

int BtBookshelfWorksPage::nextId() const {
    if (btWizard().taskType() == WizardTaskType::removeWorks)
        return WizardPage::removeFinalPage;
    return WizardPage::installFinalPage;
}

void BtBookshelfWorksPage::initializePage() {
    // Update models:
    QStringList sources;
    QStringList languages;
    if (m_taskType == installWorks) {
        sources = btWizard().selectedSources();
        languages = btWizard().selectedLanguages();
    } else {
        sources = BtInstallBackend::sourceNameList();
    }

    {
        QSet<QString> addedModuleNames;
        m_bookshelfModel->clear();
        m_usedBackends.clear();
        for (auto const & sourceName : sources) {
            sword::InstallSource const source =
                    BtInstallBackend::source(sourceName);
            std::unique_ptr<CSwordBackend const> backend(
                        BtInstallBackend::backend(source));
            bool backendUsed = false;
            for (auto * const module : backend->moduleList()) {
                if (filter(m_taskType, languages, module)) {
                    QString const & moduleName = module->name();
                    if (addedModuleNames.contains(moduleName))
                        continue;
                    addedModuleNames.insert(moduleName);
                    m_bookshelfModel->addModule(module);
                    module->setProperty("installSourceName",
                                        QString(source.caption.c_str()));
                    backendUsed = true;
                }
            }
            if (backendUsed)
                m_usedBackends.emplace_back(std::move(backend));
        }
        if (m_taskType != WizardTaskType::installWorks)
            m_bookshelfView->expandAll();

        bool const noWorks = (addedModuleNames.count() == 0);
        m_msgLabel->setVisible(noWorks);
        m_bookshelfView->setVisible(!noWorks);
        m_groupingButton->setVisible(!noWorks);
        m_groupingLabel->setVisible(!noWorks);
    }

    // Set grouping:
    static BtBookshelfTreeModel::Grouping const cat(
            BtBookshelfTreeModel::GROUP_CATEGORY);
    static BtBookshelfTreeModel::Grouping const catLang; // No grouping
    if (languages.count() == 1) {
        slotGroupingActionTriggered(cat);
    } else if (languages.count() > 1 && m_groupingOrder == cat) {
        slotGroupingActionTriggered(catLang);
    }
}

void BtBookshelfWorksPage::slotGroupingActionTriggered(
        BtBookshelfTreeModel::Grouping const & grouping)
{
    m_installPageModel->setGroupingOrder(grouping);
    m_bookshelfView->setRootIsDecorated(!grouping.isEmpty());
}

bool BtBookshelfWorksPage::isComplete() const
{ return checkedModules().count() > 0; }

void BtBookshelfWorksPage::slotGroupingOrderChanged(
        BtBookshelfTreeModel::Grouping const & g)
{
    m_groupingOrder = g;
    m_groupingOrder.saveTo(btConfig(), groupingOrderKey);
}

BtModuleSet const & BtBookshelfWorksPage::checkedModules() const
{ return m_installPageModel->checkedModules(); }

QString BtBookshelfWorksPage::installPath() const
{ return m_pathCombo->currentText(); }

void BtBookshelfWorksPage::slotPathChanged(int const index)
{ btConfig().setValue(installPathKey, index); }

static bool installPathIsUsable(QString const & path) {
    if (path.isEmpty())
        return false;
    QDir const dir(path);
    if (!dir.exists() || !dir.isReadable())
        return false;
    return QFileInfo(dir.canonicalPath()).isWritable();
}

void BtBookshelfWorksPage::slotInitPathCombo() {
    m_pathCombo->clear();
    bool haveUsableTargets = false;
    for (auto const & target : CSwordBackend::instance().swordDirList()) {
        if (installPathIsUsable(target)) {
            m_pathCombo->addItem(QDir::toNativeSeparators(target));
            haveUsableTargets = true;
        }
    }

    m_pathCombo->setVisible(haveUsableTargets);
    m_pathLabel->setVisible(haveUsableTargets);
    if (haveUsableTargets) {
        /* Choose the current value from config but check whether we have so
           many items: */
        int const cfgValue = btConfig().value<int>(installPathKey, 0);
        int const lastIdx = m_pathCombo->count() - 1;
        m_pathCombo->setCurrentIndex(cfgValue > lastIdx ? lastIdx : cfgValue);
    }
}
