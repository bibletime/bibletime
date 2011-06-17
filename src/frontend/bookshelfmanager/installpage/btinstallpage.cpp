/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallpage.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSharedPointer>
#include <QStackedLayout>
#include <QToolButton>
#include "backend/config/cbtconfig.h"
#include "backend/btinstallbackend.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallpageworkswidget.h"
#include "frontend/bookshelfmanager/installpage/btinstallpathdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"
#include "frontend/btbookshelfview.h"
#include "util/cresmgr.h"
#include "util/dialogutil.h"
#include "util/directory.h"
#include "util/tool.h"


namespace {
const QString groupingOrderKey ("GUI/BookshelfManager/InstallPage/grouping");
const QString headerStateKey   ("GUI/BookshelfManager/InstallPage/headerState");
const QString selectedModuleKey("GUI/BookshelfManager/InstallPage/selectedModule");
} // anonymous namespace


// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage(BtModuleManagerDialog *parent)
        : BtConfigDialog::Page(parent)
        , m_groupingOrder(groupingOrderKey)
        , m_modulesSelected(0)
        , m_modulesSelectedSources(0)
{
    // Read settings:
    m_headerState = CBTConfig::getConfig()->value(headerStateKey).toByteArray();

    // Initialize widgets:
    initView();
    initConnections();
}

void BtInstallPage::setInstallEnabled(bool b) {
    qDebug() << "void BtInstallPage::setInstallEnabled(bool b) start";
    m_installButton->setEnabled(b);
}

QString BtInstallPage::selectedInstallPath() {
    return m_pathCombo->currentText();
}

void BtInstallPage::initView() {
    namespace DU = util::directory;

    // Warning label:

    m_warningLabel = util::tool::explanationLabel(this, tr("WARNING!!!"),
        tr("If you live in a persecuted country and don't want to risk "
           "detection don't use remote sources."));

    // Source chooser:
    m_sourceGroupBox = new QGroupBox(tr("Select installation &source:"), this);
    m_sourceGroupBox->setFlat(true);

    m_sourceComboBox = new QComboBox(this);
    m_sourceComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    initSourcesCombo();

    m_sourceAddButton = new QPushButton(tr("&Add..."));
    m_sourceAddButton ->setToolTip(tr("Add new source"));
    m_sourceAddButton ->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::add_icon));

    m_sourceDeleteButton = new QPushButton(tr("&Delete..."));
    m_sourceDeleteButton->setToolTip(tr("Delete this source"));
    m_sourceDeleteButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::delete_icon));

    QHBoxLayout *sourceChooserLayout = new QHBoxLayout();
    sourceChooserLayout->setContentsMargins(0, 0, 0, 0);
    sourceChooserLayout->addWidget(m_sourceComboBox, 1);
    sourceChooserLayout->addWidget(m_sourceAddButton);
    sourceChooserLayout->addWidget(m_sourceDeleteButton);
    m_sourceGroupBox->setLayout(sourceChooserLayout);

    // Works chooser:
    m_worksGroupBox = new QGroupBox(tr("Select &works to install:"), this);
    m_worksGroupBox->setFlat(true);
    m_worksLayout = new QStackedLayout();
    m_worksGroupBox->setLayout(m_worksLayout);
    slotSourceIndexChanged(m_sourceComboBox->currentIndex());

    // Installation path chooser:
    m_installGroupBox = new QGroupBox(this);
    m_installGroupBox->setFlat(true);
    retranslateInstallGroupBox();

    m_pathLabel = new QLabel(tr("Install &folder:"));
    m_pathCombo = new QComboBox(this);
    m_pathCombo->setMinimumContentsLength(20);
    m_pathCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_pathCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_pathCombo->setToolTip(tr("The folder where the new works will be installed"));
    m_pathCombo->view()->setTextElideMode(Qt::ElideMiddle);
    m_pathLabel->setBuddy(m_pathCombo);
    initPathCombo();

    m_configurePathButton = new QToolButton(this);
    m_configurePathButton->setToolTip(tr("Configure folders where works are installed and found"));
    m_configurePathButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::path_icon));

    m_installButton = new QPushButton(tr("&Install..."), this);
    m_installButton->setToolTip(tr("Install or update selected works"));
    m_installButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::install_icon));
    m_installButton->setEnabled(false);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->setContentsMargins(0, 0, 0, 0);
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
    connect(m_sourceComboBox, SIGNAL(currentIndexChanged(int)),
            this,             SLOT(slotSourceIndexChanged(int)));
    connect(m_sourceAddButton, SIGNAL(clicked()),
            this,              SLOT(slotSourceAdd()));
    connect(m_sourceDeleteButton, SIGNAL(clicked()),
            this,                 SLOT(slotSourceDelete()));
    connect(m_installButton, SIGNAL(clicked()),
            this,            SLOT(slotInstall()));
    connect(m_pathCombo, SIGNAL(activated(const QString&)),
            this , SLOT(slotPathChanged(const QString&)));
    connect(m_configurePathButton, SIGNAL(clicked()),
            this, SLOT(slotEditPaths()));
    connect(CSwordBackend::instance(),
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
    int configValue = CBTConfig::get(CBTConfig::installPathIndex);
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
        Q_ASSERT(!sourceList.empty());
    }

    // Read selected module from config:
    QString selected = CBTConfig::getConfig()->value(selectedModuleKey).toString();

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
        CBTConfig::getConfig()->setValue(selectedModuleKey, sourceList.at(0));
    }
}

void BtInstallPage::activateSource(const sword::InstallSource &src) {
    qDebug() << "Selected source" << src.caption;
    qApp->setOverrideCursor(Qt::WaitCursor);
    BtInstallPageWorksWidget *w = m_sourceMap.value(QString(src.caption), 0);
    if (w == 0) {
        window()->setEnabled(false);
        qApp->processEvents();
        w = new BtInstallPageWorksWidget(src, m_groupingOrder, this);
        m_sourceMap.insert(QString(src.caption), w);
        m_worksLayout->addWidget(w);
        connect(w->treeModel(), SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
                this,           SLOT(slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping&)));
        connect(w->treeModel(), SIGNAL(moduleChecked(CSwordModuleInfo*,bool)),
                this,           SLOT(slotSelectedModulesChanged()));
        window()->setEnabled(true);
    } else {
        disconnect(w->treeView()->header(), SIGNAL(geometriesChanged()),
                   this,                    SLOT(slotHeaderChanged()));
    }
    m_worksLayout->setCurrentWidget(w);
    w->treeModel()->setGroupingOrder(m_groupingOrder);
    w->treeView()->header()->restoreState(m_headerState);
    connect(w->treeView()->header(), SIGNAL(geometriesChanged()),
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

void BtInstallPage::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    m_groupingOrder = g;
    m_groupingOrder.saveTo(groupingOrderKey);
}

void BtInstallPage::slotHeaderChanged() {
    typedef BtInstallPageWorksWidget IPWW;
    Q_ASSERT(qobject_cast<IPWW*>(m_worksLayout->currentWidget()) != 0);
    IPWW *w = static_cast<IPWW*>(m_worksLayout->currentWidget());
    m_headerState = w->treeView()->header()->saveState();
    CBTConfig::getConfig()->setValue(headerStateKey, m_headerState);
}

void BtInstallPage::slotInstall() {
    qDebug() << "BtInstallPage::slotInstall";

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
        const int result = util::showWarning(this, tr("Warning"), tr("The destination directory is not writable or does not exist. Installation will fail unless this has first been fixed."), QMessageBox::Ignore | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result != QMessageBox::Ignore) {
            return;
        }
    }

    // create the confirmation dialog
    BtInstallModuleChooserDialog *dlg = new BtInstallModuleChooserDialog(m_groupingOrder, this);

    // Add all checked modules from all tabs:
    Q_FOREACH (BtInstallPageWorksWidget *w, m_sourceMap.values()) {
        Q_FOREACH (CSwordModuleInfo *module, w->treeModel()->checkedModules()) {
            dlg->addModuleItem(module, QString(w->installSource().caption));
        }
    }

    if (dlg->exec() == QDialog::Accepted) {
        QSet<const CSwordModuleInfo*> cm;
        Q_FOREACH(const CSwordModuleInfo *m, dlg->checkedModules()) {
            cm.insert(m);
        }

        if (cm.empty())
            return;

        /// \todo first remove all modules which will be updated from the module list
        // but what modules? all with the same real name? (there may be _n modules...)

        // progressDialog is WA_DeleteOnClose
        BtInstallProgressDialog *progressDialog = new BtInstallProgressDialog(cm, selectedInstallPath(), this);

        m_installButton->setEnabled(false);

        // the progress dialog is now modal, it can be made modeless later.
        progressDialog->exec();

        qDebug() << "BtSourceWidget::slotInstallAccepted end";
    }
    delete dlg;
}

void BtInstallPage::slotPathChanged(const QString& /*pathText*/) {
    CBTConfig::set(CBTConfig::installPathIndex, m_pathCombo->currentIndex( ) );
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
    typedef CSwordSetupInstallSourcesDialog SSISD;

    QSharedPointer<SSISD> dlg(new SSISD());
    if (dlg->exec() == QDialog::Accepted) {
        if (!dlg->wasRemoteListAdded()) {
            sword::InstallSource newSource = dlg->getSource();
            if ( !((QString)newSource.type.c_str()).isEmpty() ) { // we have a valid source to add
                BtInstallBackend::addSource(newSource);
            }
            initSourcesCombo();
            for (int i = 0; i < m_sourceComboBox->count(); i++) {
                if (m_sourceComboBox->itemText(i) == newSource.caption) {
                    m_sourceComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
}

void BtInstallPage::slotSourceDelete() {
    typedef BtInstallPageWorksWidget IPWW;

    int ret = util::showWarning(this, tr("Delete Source?"),
                                tr("Do you really want to delete this source?"),
                                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        Q_ASSERT(qobject_cast<IPWW*>(m_worksLayout->currentWidget()));
        IPWW *w = static_cast<IPWW*>(m_worksLayout->currentWidget());
        w->deleteSource();
        initSourcesCombo();
        slotSourceIndexChanged(m_sourceComboBox->currentIndex());
        delete w;
    }
}

void BtInstallPage::slotSourceIndexChanged(int index) {
    if (index < 0) index = 0;

    /// \todo use pointers instead of text
    QString moduleName = m_sourceComboBox->itemText(index);
    CBTConfig::getConfig()->setValue(selectedModuleKey, moduleName);
    activateSource(BtInstallBackend::source(moduleName));
}

void BtInstallPage::slotSelectedModulesChanged() {
    m_modulesSelected = 0;
    m_modulesSelectedSources = 0;
    Q_FOREACH (BtInstallPageWorksWidget *w, m_sourceMap.values()) {
        int selected = w->treeModel()->checkedModules().size();
        if (selected > 0) {
            m_modulesSelectedSources++;
            m_modulesSelected += selected;
        }
    }

    m_installButton->setEnabled(m_modulesSelected > 0);
    retranslateInstallGroupBox();
}

// implement the BtConfigPage methods

const QIcon &BtInstallPage::icon() const {
    return util::directory::getIcon(CResMgr::bookshelfmgr::installpage::icon);
}

QString BtInstallPage::header() const {
    return tr("Install/Update");
}

void BtInstallPage::slotSwordSetupChanged() {
    QString moduleName = m_sourceComboBox->currentText();

    initSourcesCombo();
    qDeleteAll(m_sourceMap.values());
    m_sourceMap.clear();
    m_sourceComboBox->setCurrentIndex(m_sourceComboBox->findText(moduleName));
    initPathCombo();
    m_modulesSelected = 0;
    m_modulesSelectedSources = 0;
    retranslateInstallGroupBox();
}
