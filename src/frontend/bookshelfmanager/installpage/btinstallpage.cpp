/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallpage.h"

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QTabBar>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallpathdialog.h"
#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"
#include "frontend/bookshelfmanager/installpage/btsourcewidget.h"
#include "frontend/bookshelfmanager/installpage/btsourcearea.h"
#include "backend/btinstallbackend.h"
#include "util/directory.h"
#include "util/cresmgr.h"

// Sword includes:
#include <swversion.h>


// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage(QWidget *parent)
        : BtConfigPage(parent)
{
    qDebug() << "BtInstallPage::BtInstallPage() start";
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

    qDebug() << "void BtInstallPage::initView() start";
    Q_ASSERT(qobject_cast<QVBoxLayout*>(layout()) != 0);
    QVBoxLayout *mainLayout = static_cast<QVBoxLayout*>(layout());

    // installation path chooser
    QHBoxLayout* pathLayout = new QHBoxLayout();
    // beautify the layout
    int top;
    int bottom;
    int left;
    int right;
    pathLayout->getContentsMargins(&left, &top, &right, &bottom);
    pathLayout->setContentsMargins(left, top + 7, right, bottom + 7 );
    QLabel* pathLabel = new QLabel(tr("Install folder:"));
    m_pathCombo = new QComboBox();
    m_pathCombo->setMinimumContentsLength(20);
    m_pathCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_pathCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_pathCombo->setToolTip(tr("The folder where the new works will be installed"));
    m_pathCombo->view()->setTextElideMode(Qt::ElideMiddle);
    initPathCombo(); // set the paths and the current path
    m_configurePathButton = new QToolButton(this);
    m_configurePathButton->setToolTip(tr("Configure folders where works are installed and found"));
    m_configurePathButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::path_icon));

    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(m_pathCombo);
    pathLayout->addWidget(m_configurePathButton);
    mainLayout->addLayout(pathLayout);

    // Source widget
    m_sourceWidget = new BtSourceWidget(this);
    mainLayout->addWidget(m_sourceWidget);
    // Install button
    QHBoxLayout *installButtonLayout = new QHBoxLayout();
    installButtonLayout->setContentsMargins(0, 5, 0, 5);
    QSpacerItem *installButtonSpacer = new QSpacerItem(371, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    installButtonLayout->addItem(installButtonSpacer);
    m_installButton = new QPushButton(tr("Install..."), this);
    m_installButton->setToolTip(tr("Install or update selected works"));
    m_installButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::install_icon));
    m_installButton->setEnabled(false);
    installButtonLayout->addWidget(m_installButton);

    mainLayout->addLayout(installButtonLayout);
}

void BtInstallPage::initConnections() {
    qDebug() << "void BtInstallPage::initConnections() start";
    QObject::connect(m_pathCombo, SIGNAL(activated(const QString&)), this , SLOT(slotPathChanged(const QString&)));
    QObject::connect(m_configurePathButton, SIGNAL(clicked()), this, SLOT(slotEditPaths()));
    QObject::connect(m_installButton, SIGNAL(clicked()), m_sourceWidget, SLOT(slotInstall()) );

    QObject::connect(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), this, SLOT(slotSwordSetupChanged()));
    //source widget has its own connections, not here
}

void BtInstallPage::initPathCombo() {
    qDebug() << "void BtInstallPage::initPathCombo() start";
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

void BtInstallPage::slotPathChanged(const QString& /*pathText*/) {
    CBTConfig::set(CBTConfig::installPathIndex, m_pathCombo->currentIndex( ) );
}

void BtInstallPage::slotEditPaths() {
    qDebug() << "void BtInstallPage::slotEditPaths() start";

    BtInstallPathDialog* dlg = new BtInstallPathDialog();
    int result = dlg->exec();
    if (result == QDialog::Accepted) {
        //dynamic_cast<BtModuleManagerDialog*>(parentDialog())->slotSwordSetupChanged();
        CSwordBackend::instance()->reloadModules(CSwordBackend::PathChanged);
    }
}

// implement the BtConfigPage methods

const QIcon &BtInstallPage::icon() const {
    return util::directory::getIcon(CResMgr::bookshelfmgr::installpage::icon);
}

QString BtInstallPage::label() const {
    // \todo move the warning to a dialog which is shown when adding a source.
    return tr("Install and update works. Add remote or local sources, refresh them, select the works to be installed/updated and click Install.<br/><b>WARNING:</b> If you live in a persecuted country and don't want to risk detection don't use remote sources.");
}

QString BtInstallPage::header() const {
    return tr("Install/Update");
}

void BtInstallPage::slotSwordSetupChanged() {
    qDebug() << "BtInstallPage::slotSwordSetupChanged";
    initPathCombo();
//     for (int i = 0; i < m_sourceWidget->count(); i++ ) {
//         BtSourceArea* sourceArea = dynamic_cast<BtSourceArea*>(m_sourceWidget->widget(i));
//         Q_ASSERT(sourceArea);
//         sourceArea->createModuleTree();
//     }
}



