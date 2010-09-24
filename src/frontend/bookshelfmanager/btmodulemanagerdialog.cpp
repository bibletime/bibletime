/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"

#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/indexpage/btindexpage.h"
#include "frontend/bookshelfmanager/installpage/btinstallpage.h"
#include "frontend/bookshelfmanager/removepage/btremovepage.h"


static BtModuleManagerDialog *m_staticModuleManagerDialog = 0;

BtModuleManagerDialog* BtModuleManagerDialog::getInstance(QWidget* parent) {
    if (!m_staticModuleManagerDialog) {
        m_staticModuleManagerDialog = new BtModuleManagerDialog(parent);
    };
    Q_ASSERT(m_staticModuleManagerDialog);
    return m_staticModuleManagerDialog;
}

BtModuleManagerDialog::BtModuleManagerDialog(QWidget* parent)
        : BtConfigDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Bookshelf Manager"));

    // Install page
    BtInstallPage* installPage = new BtInstallPage();
    addPage(installPage);

    //Uninstall page
    BtRemovePage* removePage = new BtRemovePage();
    addPage(removePage);

    //Index page
    BtIndexPage* indexPage = new BtIndexPage();
    addPage(indexPage);

    slotChangePage(0);

    loadDialogSettings();
}

BtModuleManagerDialog::~BtModuleManagerDialog() {
    saveDialogSettings();
    m_staticModuleManagerDialog = 0;
}

// The QWidget close() sends close event, so does closing by the window X button.
void BtModuleManagerDialog::closeEvent(QCloseEvent*) {
    qDebug() << "BtModuleManagerDialog::closeEvent";
}

void BtModuleManagerDialog::loadDialogSettings() {
    resize(CBTConfig::get(CBTConfig::bookshelfWidth), CBTConfig::get(CBTConfig::bookshelfHeight));
    move(CBTConfig::get(CBTConfig::bookshelfPosX), CBTConfig::get(CBTConfig::bookshelfPosY));
}

void BtModuleManagerDialog::saveDialogSettings() {
    CBTConfig::set(CBTConfig::bookshelfWidth, size().width());
    CBTConfig::set(CBTConfig::bookshelfHeight, size().height());
    CBTConfig::set(CBTConfig::bookshelfPosX, x());
    CBTConfig::set(CBTConfig::bookshelfPosY, y());
}
