/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfworkspage.h"

#include "backend/btinstallbackend.h"
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/btmoduletreeitem.h"
#include "backend/config/btconfig.h"
#include "backend/btinstallmgr.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallpagemodel.h"
#include "frontend/bookshelfmanager/installpage/btinstallpathdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "frontend/btbookshelfgroupingmenu.h"
#include "frontend/btbookshelfview.h"
#include "frontend/messagedialog.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"
#include "util/directory.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

namespace {
const QString groupingOrderKey ("GUI/BookshelfWizard/InstallPage/grouping");
const QString installPathKey   ("GUI/BookshelfWizard/InstallPage/installPathIndex");
} // anonymous namespace

BtBookshelfWorksPage::BtBookshelfWorksPage(
        BtBookshelfWorksPage::InstallType iType,
        QWidget *parent)
    : QWizardPage(parent),
      m_installType(iType),
      m_groupingOrder(groupingOrderKey),
      m_groupingButton(nullptr),
      m_bookshelfView(nullptr),
      m_pathLabel(nullptr),
      m_groupingLabel(nullptr),
      m_pathCombo(nullptr),
      m_verticalLayout(nullptr),
      m_installPageModel(nullptr),
      m_bookshelfModel(nullptr),
      m_bookshelfFilterModel(nullptr),
      m_contextMenu(nullptr),
      m_groupingMenu(nullptr),
      m_itemContextMenu(nullptr) {

    initMenus();
    setupUi();
    setupModels();
    initConnections();
    retranslateUi();
}

void BtBookshelfWorksPage::initMenus() {
    m_groupingMenu = new BtBookshelfGroupingMenu(this);
    BT_CONNECT(m_groupingMenu,
               SIGNAL(signalGroupingOrderChanged(
                          BtBookshelfTreeModel::Grouping)),
               this,
               SLOT(slotGroupingActionTriggered(
                        BtBookshelfTreeModel::Grouping)));

    m_contextMenu = new QMenu(this);
    m_contextMenu->addMenu(m_groupingMenu);
    m_itemContextMenu = m_contextMenu;
}

void BtBookshelfWorksPage::setupUi() {

    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    m_bookshelfView = new BtBookshelfView(this);
    m_bookshelfView->setObjectName(QStringLiteral("worksTreeView"));
    m_bookshelfView->setHeaderHidden(false);
    m_bookshelfView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_verticalLayout->addWidget(m_bookshelfView);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    if (m_installType != remove) {
        m_pathLabel = new QLabel(this);
        m_pathCombo = new QComboBox(this);
        m_pathCombo->setMinimumContentsLength(20);
        m_pathCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        m_pathCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_pathCombo->view()->setTextElideMode(Qt::ElideMiddle);
        m_pathLabel->setBuddy(m_pathCombo);
        initPathCombo();

        pathLayout->setContentsMargins(0, 8, 0, 0);
        pathLayout->addWidget(m_pathLabel);
        pathLayout->addWidget(m_pathCombo);
    } else {
        pathLayout->addStretch();
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(40, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
    pathLayout->addItem(verticalSpacer);

    m_groupingLabel = new QLabel(this);
    pathLayout->addWidget(m_groupingLabel);

    m_groupingButton = new QToolButton(this);
    m_groupingButton->setPopupMode(QToolButton::InstantPopup);
    m_groupingButton->setMenu(m_groupingMenu);
    m_groupingButton->setIcon(m_groupingMenu->icon());
    m_groupingButton->setAutoRaise(true);

    pathLayout->addWidget(m_groupingButton);

    m_verticalLayout->addLayout(pathLayout);
}

void BtBookshelfWorksPage::setupModels() {
    m_bookshelfFilterModel = new BtBookshelfFilterModel(this);
    m_bookshelfView->setModel(m_bookshelfFilterModel);

    m_installPageModel = new BtInstallPageModel(m_groupingOrder, this);
    if (m_installType == update)
        m_installPageModel->setDefaultChecked(BtBookshelfTreeModel::CHECKED);
    m_bookshelfFilterModel->setSourceModel(m_installPageModel);

    m_bookshelfModel = new BtBookshelfModel(this);
    if (m_installType == remove) {
        m_installPageModel->setSourceModel(CSwordBackend::instance()->model());
    } else {
        m_installPageModel->setSourceModel(m_bookshelfModel);
    }
    connect(m_installPageModel, &BtBookshelfTreeModel::moduleChecked,
            this, &BtBookshelfWorksPage::slotDataChanged);
    if (m_installType == remove)
        m_bookshelfView->setColumnHidden(1,true);
}

void BtBookshelfWorksPage::initConnections() {
    if (m_installType != remove) {
        BT_CONNECT(m_pathCombo, SIGNAL(activated(QString const &)),
                   this , SLOT(slotPathChanged(QString const &)));
        BT_CONNECT(CSwordBackend::instance(),
                   SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
                   this, SLOT(slotSwordSetupChanged()));
    }
    BT_CONNECT(m_installPageModel,
               SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
               this,
               SLOT(slotGroupingOrderChanged(
                        BtBookshelfTreeModel::Grouping const &)));
}

void BtBookshelfWorksPage::retranslateUi() {
    if (m_installType == install) {
        setTitle(QApplication::translate("BookshelfWizard", "Install Works", 0));
        setSubTitle(QApplication::translate("BookshelfWizard", "Choose one or more works to install.", 0));
    }
    else if (m_installType == update) {
        setTitle(QApplication::translate("BookshelfWizard", "Update Works", 0));
        setSubTitle(QApplication::translate("BookshelfWizard", "Choose one or more works to update.", 0));
    }
    else {
        setTitle(QApplication::translate("BookshelfWizard", "Remove Works", 0));
        setSubTitle(QApplication::translate("BookshelfWizard", "Choose one or more works to remove.", 0));
    }

    if (m_installType != remove) {
        m_pathLabel->setText(tr("Install &folder:"));
        m_pathCombo->setToolTip(tr("The folder where the new works will be installed"));
    }
    m_groupingLabel->setText(tr("Grouping:"));
    m_groupingButton->setText(tr("Grouping"));
    m_groupingButton->setToolTip(tr("Change the grouping of items in the bookshelf."));
}

int BtBookshelfWorksPage::nextId() const {
    return -1;
}

void BtBookshelfWorksPage::initializePage() {
    updateModels();
}

void BtBookshelfWorksPage::updateModels() {
    if (m_installType == install) {
        m_sources = btWizard()->selectedSources();
        m_languages = btWizard()->selectedLanguages();
    } else {
        m_sources = BtInstallBackend::sourceNameList();
    }

    m_bookshelfModel->clear();
    m_moduleSourceMap.clear();
    for (auto sourceName : m_sources) {
        sword::InstallSource source = BtInstallBackend::source(sourceName);
        CSwordBackend *backend = BtInstallBackend::backend(source);

        for (auto module : backend->moduleList())
            if (filter(module)) {
                m_bookshelfModel->addModule(module);
                QString sourceName = source.caption.c_str();
                m_moduleSourceMap.insert(module, sourceName);
            }
    }
    if (m_installType == remove || m_installType == update)
        m_bookshelfView->expandAll();
}

void BtBookshelfWorksPage::slotGroupingActionTriggered(
        const BtBookshelfTreeModel::Grouping &grouping) {
    m_installPageModel->setGroupingOrder(grouping);
    m_bookshelfView->setRootIsDecorated(!grouping.isEmpty());
}

void BtBookshelfWorksPage::slotDataChanged() {
    emit completeChanged();
}

bool BtBookshelfWorksPage::isComplete() const {
    int count = selectedWorks().count();
    return count > 0;
}

BtModuleSet BtBookshelfWorksPage::selectedWorks() const {
    return m_installPageModel->checkedModules();
}

bool BtBookshelfWorksPage::filter(const CSwordModuleInfo *mInfo) {
    if (m_installType == install)
        return filterInstalls(mInfo);
    else if (m_installType == update)
        return filterUpdates(mInfo);
    else
        return filterRemoves(mInfo);
}

bool BtBookshelfWorksPage::filterInstalls(const CSwordModuleInfo *mInfo) {
    if (moduleIsInstalled(mInfo))
        return false;
    if (! moduleUsesSelectedLanguage(mInfo))
        return false;
    return true;
}

bool BtBookshelfWorksPage::filterUpdates(const CSwordModuleInfo *mInfo) {
    using CSMI = CSwordModuleInfo;
    using SV = sword::SWVersion;

    const CSMI *installedModule = CSwordBackend::instance()->findModuleByName(mInfo->name());
    if (installedModule) {
        const SV curVersion(installedModule->config(CSMI::ModuleVersion).toLatin1());
        const SV newVersion(mInfo->config(CSMI::ModuleVersion).toLatin1());
        if (curVersion < newVersion) {
            return true;
        }
    }
    return false;
}

bool BtBookshelfWorksPage::filterRemoves(const CSwordModuleInfo *mInfo) {
    using CSMI = CSwordModuleInfo;

    const CSMI *installedModule = CSwordBackend::instance()->findModuleByName(mInfo->name());
    if (installedModule)
        return true;
    return false;
}

void BtBookshelfWorksPage::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    m_groupingOrder = g;
    m_groupingOrder.saveTo(groupingOrderKey);
}

bool BtBookshelfWorksPage::manageWorks() {
    if (m_installType == remove)
        return removeWorks();

    if (!destinationPathIsWritable())
        return false;

    // create the confirmation dialog
    BtInstallModuleChooserDialog *dlg = new BtInstallModuleChooserDialog(m_groupingOrder, this);

    // Add all checked modules from all tabs:
    BtModuleSet moduleSet = selectedWorks();
    for (auto module : moduleSet) {
        QString sourceName = m_moduleSourceMap[module];
        dlg->addModuleItem(module, sourceName);
    }
    if (dlg->exec() == QDialog::Accepted) {
        QList<CSwordModuleInfo *> modules(dlg->checkedModules().toList());
        if (modules.empty())
            return false;

        // progressDialog is WA_DeleteOnClose
        using BIPD = BtInstallProgressDialog;
        auto progressDialog = new BIPD(modules, selectedInstallPath(), this);
        progressDialog->exec();

        delete dlg;
        return true;
    }
    delete dlg;
    return false;
}

bool BtBookshelfWorksPage::removeWorks() {

    if (m_installPageModel->checkedModules().empty())
        return false;

    QStringList moduleNames;
    QStringList prettyModuleNames;
    const int textHeight = fontMetrics().height();
    /// \bug <nobr> is not working, Qt bug
    const QString moduleString("<nobr>%1&nbsp;%2</nobr>");
    for (auto m : m_installPageModel->checkedModules()) {
        prettyModuleNames.append(moduleString
                                 .arg(iconToHtml(CSwordModuleInfo::moduleIcon(*m),
                                                 textHeight))
                                 .arg(m->name()));
        moduleNames.append(m->name());
    }

    const QString message = tr("You selected the following work(s): ")
            .append("<br/><br/>&nbsp;&nbsp;&nbsp;&nbsp;")
            .append(prettyModuleNames.join(",&nbsp; "))
            .append("<br/><br/>")
            .append(tr("Do you really want to remove them from your system?"));

    if ((message::showQuestion(this, tr("Remove Works?"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.

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

        QMessageBox::information(this, tr("Works Removed"),
                             tr("The selected works have been removed."),QMessageBox::Close);

        return true;
    }

    return false;
}

bool BtBookshelfWorksPage::destinationPathIsWritable() {
    // check that the destination path is writable, do nothing if not
    QDir dir = selectedInstallPath();
    bool canWrite = true;
    if (dir.isReadable()) {
        const QFileInfo fi( dir.canonicalPath() );
        if (!fi.exists() || !fi.isWritable()) {
            canWrite = false;
        }
    }
    else {
        canWrite = false;
    }

    if (!canWrite) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("The destination directory is not writable or does not exist."));
        return false;
    }
    return true;
}

QString BtBookshelfWorksPage::selectedInstallPath() {
    return m_pathCombo->currentText();
}

void BtBookshelfWorksPage::slotPathChanged(const QString& /*pathText*/) {
    btConfig().setValue(installPathKey, m_pathCombo->currentIndex());
}

static bool installPathIsUsable(const QString& path) {
    if (path.isEmpty())
        return false;
    QDir dir(path);
    if (!dir.exists())
        return false;
    if (!dir.isReadable())
        return false;
    QFileInfo fi( dir.canonicalPath());
    if (!fi.isWritable())
        return false;
    return true;
}

void BtBookshelfWorksPage::initPathCombo() {
    m_pathCombo->clear();
    QStringList targets = BtInstallBackend::targetList();
    for ( auto target : targets) {
        if (installPathIsUsable(target))
            m_pathCombo->addItem(util::directory::convertDirSeparators(target));
    }

    // choose the current value from config but check whether we have so many items
    int configValue = btConfig().value<int>(installPathKey, 0);
    int index = configValue > (m_pathCombo->count() - 1) ? m_pathCombo->count() - 1 : configValue;
    m_pathCombo->setCurrentIndex(index);
}

void BtBookshelfWorksPage::slotEditPaths() {
    BtInstallPathDialog* dlg = new BtInstallPathDialog();
    int result = dlg->exec();
    if (result == QDialog::Accepted) {
        CSwordBackend::instance()->reloadModules(CSwordBackend::PathChanged);
    }
}

void BtBookshelfWorksPage::slotSwordSetupChanged() {
    initPathCombo();
}

bool BtBookshelfWorksPage::moduleUsesSelectedLanguage(const CSwordModuleInfo *mInfo) {
    const CLanguageMgr::Language *language = mInfo->language();
    QString lang = language->translatedName();
    return m_languages.contains(lang);
}

bool BtBookshelfWorksPage::moduleIsInstalled(const CSwordModuleInfo *mInfo) {
    const CSwordModuleInfo *installedModule = CSwordBackend::instance()->findModuleByName(mInfo->name());
    return (installedModule);
}

BtBookshelfWizard *BtBookshelfWorksPage::btWizard() {
    return qobject_cast<BtBookshelfWizard*>(wizard());
}

