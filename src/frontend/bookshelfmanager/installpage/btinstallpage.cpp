/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallpage.h"

#include <memory>
#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QToolButton>
#include "backend/config/btconfig.h"
#include "backend/btinstallbackend.h"
#include "bibletimeapp.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallpageworkswidget.h"
#include "frontend/bookshelfmanager/installpage/btinstallpathdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"
#include "frontend/btbookshelfview.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/tool.h"


namespace {
const QString groupingOrderKey ("GUI/BookshelfManager/InstallPage/grouping");
const QString headerStateKey   ("GUI/BookshelfManager/InstallPage/headerState");
const QString selectedModuleKey("GUI/BookshelfManager/InstallPage/selectedModule");
const QString installPathKey   ("GUI/BookshelfManager/InstallPage/installPathIndex");
} // anonymous namespace


// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage(BtModuleManagerDialog *parent)
        : BtConfigDialog::Page(CResMgr::bookshelfmgr::installpage::icon(),
                               parent)
        , m_groupingOrder(groupingOrderKey)
        , m_modulesSelected(0)
        , m_modulesSelectedSources(0)
{
    // Read settings:
    m_headerState = btConfig().value<QByteArray>(headerStateKey, QByteArray());

    // Initialize widgets:
    initView();
    initConnections();

    retranslateUi();
}

void BtInstallPage::setInstallEnabled(bool b) {
    m_installButton->setEnabled(b);
}

QString BtInstallPage::selectedInstallPath() {
    return m_pathCombo->currentText();
}

void BtInstallPage::initView() {
    // Warning label:
    m_warningLabel = new QLabel(this);

    // Source chooser:
    m_sourceGroupBox = new QGroupBox(this);
    m_sourceGroupBox->setFlat(true);

    m_sourceComboBox = new QComboBox(this);
    m_sourceComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    initSourcesCombo();

    m_sourceAddButton = new QPushButton(this);
    m_sourceAddButton->setIcon(CResMgr::bookshelfmgr::installpage::icon_addSource());

    m_sourceDeleteButton = new QPushButton(this);
    m_sourceDeleteButton->setIcon(CResMgr::bookshelfmgr::installpage::icon_deleteSource());

    QHBoxLayout *sourceChooserLayout = new QHBoxLayout();
    sourceChooserLayout->setContentsMargins(0, 8, 0, 0);
    sourceChooserLayout->addWidget(m_sourceComboBox, 1);
    sourceChooserLayout->addWidget(m_sourceAddButton);
    sourceChooserLayout->addWidget(m_sourceDeleteButton);
    m_sourceGroupBox->setLayout(sourceChooserLayout);

    // Works chooser:
    m_worksGroupBox = new QGroupBox(this);
    m_worksGroupBox->setFlat(true);
    m_worksLayout = new QStackedLayout();
    m_worksGroupBox->setLayout(m_worksLayout);
    slotSourceIndexChanged(m_sourceComboBox->currentIndex());

    // Installation path chooser:
    m_installGroupBox = new QGroupBox(this);
    m_installGroupBox->setFlat(true);
    retranslateInstallGroupBox();

    m_pathLabel = new QLabel(this);
    m_pathCombo = new QComboBox(this);
    m_pathCombo->setMinimumContentsLength(20);
    m_pathCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_pathCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_pathCombo->view()->setTextElideMode(Qt::ElideMiddle);
    m_pathLabel->setBuddy(m_pathCombo);
    initPathCombo();

    m_configurePathButton = new QToolButton(this);
    m_configurePathButton->setIcon(CResMgr::bookshelfmgr::installpage::icon_path());

    m_installButton = new QPushButton(this);
    m_installButton->setIcon(CResMgr::bookshelfmgr::installpage::icon_install());
    m_installButton->setEnabled(false);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->setContentsMargins(0, 8, 0, 0);
    pathLayout->addWidget(m_pathLabel);
    pathLayout->addWidget(m_pathCombo);
    pathLayout->addWidget(m_configurePathButton);
    pathLayout->addWidget(m_installButton);
    m_installGroupBox->setLayout(pathLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_warningLabel);
    mainLayout->addWidget(m_sourceGroupBox);
    mainLayout->addWidget(m_worksGroupBox, 1);
    mainLayout->addWidget(m_installGroupBox);
}

void BtInstallPage::initConnections() {
    BT_CONNECT(m_sourceComboBox, SIGNAL(currentIndexChanged(int)),
               this,             SLOT(slotSourceIndexChanged(int)));
    BT_CONNECT(m_sourceAddButton, SIGNAL(clicked()),
               this,              SLOT(slotSourceAdd()));
    BT_CONNECT(m_sourceDeleteButton, SIGNAL(clicked()),
               this,                 SLOT(slotSourceDelete()));
    BT_CONNECT(m_installButton, SIGNAL(clicked()),
               this,            SLOT(slotInstall()));
    BT_CONNECT(m_pathCombo, SIGNAL(activated(QString const &)),
               this , SLOT(slotPathChanged(QString const &)));
    BT_CONNECT(m_configurePathButton, SIGNAL(clicked()),
               this, SLOT(slotEditPaths()));
    BT_CONNECT(CSwordBackend::instance(),
               SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
               this, SLOT(slotSwordSetupChanged()));
}

void BtInstallPage::initPathCombo() {
    //populate the combo list
    m_pathCombo->clear();

    QStringList targets = BtInstallBackend::targetList();
    for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
        // Add the path only if it's writable
        if ((*it).isEmpty()) continue;
        QDir dir(*it);
        if (!dir.exists()) continue;
        if (!dir.isReadable()) continue;
        QFileInfo fi( dir.canonicalPath());
        if (!fi.isWritable()) continue;
        m_pathCombo->addItem(util::directory::convertDirSeparators(*it));
    }

    // choose the current value from config but check whether we have so many items
    int configValue = btConfig().value<int>(installPathKey, 0);
    int index = configValue > (m_pathCombo->count() - 1) ? m_pathCombo->count() - 1 : configValue;
    m_pathCombo->setCurrentIndex(index);
}

void BtInstallPage::initSourcesCombo() {
    /// \todo Implement a proper model for this

    m_sourceComboBox->clear();
    QStringList sourceList = BtInstallBackend::sourceNameList();

    // Add a default entry, the Crosswire main repository
    if (sourceList.empty()) {
        /// \todo Open a dialog which asks whether to get list from server and add sources
        sword::InstallSource is("FTP");   //default return value
        is.caption = "CrossWire Bible Society";
        is.source = "ftp.crosswire.org";
        is.directory = "/pub/sword/raw";
        // passive ftp is not needed here, it's the default

        BtInstallBackend::addSource(is);

        sourceList = BtInstallBackend::sourceNameList();
        BT_ASSERT(!sourceList.empty());
    }

    // Read selected module from config:
    const QString selected = btConfig().value<QString>(selectedModuleKey, QString());

    // Populate combo box
    bool selectionOk = false;
    for (int i = 0; i < sourceList.size(); i++) {
        m_sourceComboBox->addItem(sourceList.at(i));

        // Select configured item:
        if (!selectionOk && sourceList.at(i) == selected) {
            m_sourceComboBox->setCurrentIndex(i);
            selectionOk = true;
        }
    }

    // Set selection, if it wasn't properly configured:
    if (!selectionOk) {
        m_sourceComboBox->setCurrentIndex(0);
        btConfig().setValue(selectedModuleKey, sourceList.at(0));
    }
}

void BtInstallPage::activateSource(const sword::InstallSource &src) {
    qApp->setOverrideCursor(Qt::WaitCursor);
    BtInstallPageWorksWidget *w = m_sourceMap.value(QString(src.caption), nullptr);
    if (w == nullptr) {
        window()->setEnabled(false);
        qApp->processEvents();
        w = new BtInstallPageWorksWidget(src, m_groupingOrder, this);
        m_sourceMap.insert(QString(src.caption), w);
        m_worksLayout->addWidget(w);
        BT_CONNECT(w->treeModel(),
                   SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
                   this,
                   SLOT(slotGroupingOrderChanged(
                                BtBookshelfTreeModel::Grouping const &)));
        BT_CONNECT(w->treeModel(),
                   SIGNAL(moduleChecked(CSwordModuleInfo *, bool)),
                   this, SLOT(slotSelectedModulesChanged()));
        window()->setEnabled(true);
    } else {
        disconnect(w->treeView()->header(), SIGNAL(geometriesChanged()),
                   this,                    SLOT(slotHeaderChanged()));
    }
    m_worksLayout->setCurrentWidget(w);
    w->treeModel()->setGroupingOrder(m_groupingOrder);
    w->treeView()->header()->restoreState(m_headerState);
    BT_CONNECT(w->treeView()->header(), SIGNAL(geometriesChanged()),
               this,                    SLOT(slotHeaderChanged()));
    qApp->restoreOverrideCursor();
}

void BtInstallPage::retranslateInstallGroupBox() {
    if (m_modulesSelected > 0) {
        m_installGroupBox->setTitle(tr("Start installation of %1 works from %2 sources:")
                                    .arg(m_modulesSelected)
                                    .arg(m_modulesSelectedSources));
    } else {
        m_installGroupBox->setTitle(tr("Start installation:"));
    }
}

void BtInstallPage::retranslateUi() {
    setHeaderText(tr("Install/Update"));

    util::tool::initExplanationLabel(m_warningLabel, tr("WARNING"),
            tr("If you live in a persecuted country and don't want to risk "
               "detection don't use remote sources."));

    m_sourceGroupBox->setTitle(tr("Select installation &source:"));
    m_sourceAddButton->setText(tr("&Add..."));
    m_sourceAddButton->setToolTip(tr("Add new source"));
    m_sourceDeleteButton->setText(tr("&Delete..."));
    m_sourceDeleteButton->setToolTip(tr("Delete this source"));

    m_worksGroupBox->setTitle(tr("Select &works to install:"));

    m_pathLabel->setText(tr("Install &folder:"));
    m_pathCombo->setToolTip(tr("The folder where the new works will be installed"));
    m_configurePathButton->setToolTip(tr("Configure folders where works are installed and found"));
    m_installButton->setText(tr("&Install..."));
    m_installButton->setToolTip(tr("Install or update selected works"));

    retranslateInstallGroupBox();
}

void BtInstallPage::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    m_groupingOrder = g;
    m_groupingOrder.saveTo(groupingOrderKey);
}

void BtInstallPage::slotHeaderChanged() {
    using IPWW = BtInstallPageWorksWidget;
    BT_ASSERT(qobject_cast<IPWW*>(m_worksLayout->currentWidget()));
    IPWW *w = static_cast<IPWW*>(m_worksLayout->currentWidget());
    m_headerState = w->treeView()->header()->saveState();
    btConfig().setValue(headerStateKey, m_headerState);
}

void BtInstallPage::slotInstall() {
    // check that the destination path is writable, do nothing if not and user doesn't want to continue
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
        const int result = message::showWarning(this, tr("Warning"), tr("The destination directory is not writable or does not exist. Installation will fail unless this has first been fixed."), QMessageBox::Ignore | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result != QMessageBox::Ignore) {
            return;
        }
    }

    // create the confirmation dialog
    BtInstallModuleChooserDialog *dlg = new BtInstallModuleChooserDialog(m_groupingOrder, this);

    // Add all checked modules from all tabs:
    Q_FOREACH(BtInstallPageWorksWidget const * const w, m_sourceMap.values())
        Q_FOREACH(CSwordModuleInfo * const module,
                  w->treeModel()->checkedModules())
            dlg->addModuleItem(module, QString(w->installSource().caption));

    if (dlg->exec() == QDialog::Accepted) {
        QList<CSwordModuleInfo *> modules(dlg->checkedModules().toList());
        if (modules.empty())
            return;

        /// \todo first remove all modules which will be updated from the module list
        // but what modules? all with the same real name? (there may be _n modules...)

        // progressDialog is WA_DeleteOnClose
        using BIPD = BtInstallProgressDialog;
        BIPD * const progressDialog = new BIPD(modules, selectedInstallPath(), this);
        m_installButton->setEnabled(false);
        // the progress dialog is now modal, it can be made modeless later.
        progressDialog->exec();

    }
    delete dlg;
}

void BtInstallPage::slotPathChanged(const QString& /*pathText*/) {
    btConfig().setValue(installPathKey, m_pathCombo->currentIndex());
}

void BtInstallPage::slotEditPaths() {
    BtInstallPathDialog* dlg = new BtInstallPathDialog();
    int result = dlg->exec();
    if (result == QDialog::Accepted) {
        //dynamic_cast<BtModuleManagerDialog*>(parentDialog())->slotSwordSetupChanged();
        CSwordBackend::instance()->reloadModules(CSwordBackend::PathChanged);
    }
}

void BtInstallPage::slotSourceAdd() {
    using SSISD = CSwordSetupInstallSourcesDialog;

    std::unique_ptr<SSISD> dlg(new SSISD());
    if (dlg->exec() != QDialog::Accepted)
        return;
    if (dlg->wasRemoteListAdded()) {
        initSourcesCombo();
        return;
    }
    sword::InstallSource newSource = dlg->getSource();
    if (*(newSource.type.c_str()) != '\0') // we have a valid source to add
        BtInstallBackend::addSource(newSource);
    initSourcesCombo();
    for (int i = 0; i < m_sourceComboBox->count(); i++) {
        if (m_sourceComboBox->itemText(i) == newSource.caption) {
            m_sourceComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void BtInstallPage::slotSourceDelete() {
    using IPWW = BtInstallPageWorksWidget;

    int ret = message::showWarning(this, tr("Delete Source?"),
                                tr("Do you really want to delete this source?"),
                                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        qApp->setOverrideCursor(Qt::WaitCursor);
        window()->setEnabled(false);
        BT_ASSERT(qobject_cast<IPWW*>(m_worksLayout->currentWidget()));
        IPWW *w = static_cast<IPWW*>(m_worksLayout->currentWidget());
        m_sourceMap.remove(QString(w->installSource().caption));
        w->deleteSource();
        initSourcesCombo();
        slotSourceIndexChanged(m_sourceComboBox->currentIndex());
        delete w;
        window()->setEnabled(true);
        qApp->restoreOverrideCursor();
    }
}

void BtInstallPage::slotSourceIndexChanged(int index) {
    if (index < 0) {
        if(!m_sourceComboBox->count())
            return;
        index = 0;
    }

    /// \todo use pointers instead of text
    QString moduleName = m_sourceComboBox->itemText(index);
    btConfig().setValue(selectedModuleKey, moduleName);
    activateSource(BtInstallBackend::source(moduleName));
}

void BtInstallPage::slotSelectedModulesChanged() {
    m_modulesSelected = 0;
    m_modulesSelectedSources = 0;
    Q_FOREACH(BtInstallPageWorksWidget const * const w, m_sourceMap.values()) {
        if (int const selected = w->treeModel()->checkedModules().size()) {
            m_modulesSelectedSources++;
            m_modulesSelected += selected;
        }
    }

    m_installButton->setEnabled(m_modulesSelected > 0);
    retranslateInstallGroupBox();
}

void BtInstallPage::slotSwordSetupChanged() {
    QString moduleName = m_sourceComboBox->currentText();

    // clean m_sourceMap before initSourcesCombo() make too much work
    qDeleteAll(m_sourceMap.values());
    m_sourceMap.clear();
    initSourcesCombo();
    m_sourceComboBox->setCurrentIndex(m_sourceComboBox->findText(moduleName));
    initPathCombo();
    m_modulesSelected = 0;
    m_modulesSelectedSources = 0;
    retranslateInstallGroupBox();
}
