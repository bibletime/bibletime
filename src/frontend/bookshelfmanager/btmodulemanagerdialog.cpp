/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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

BtModuleManagerDialog* BtModuleManagerDialog::getInstance(QWidget *parent,
                                                          Qt::WindowFlags flags)
{
    if (m_staticModuleManagerDialog) {
        m_staticModuleManagerDialog->setParent(parent, flags);
    } else {
        m_staticModuleManagerDialog = new BtModuleManagerDialog(parent, flags);
    }
    Q_ASSERT(m_staticModuleManagerDialog);
    return m_staticModuleManagerDialog;
}

BtModuleManagerDialog::BtModuleManagerDialog(QWidget *parent,
                                             Qt::WindowFlags flags)
        : BtConfigDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    addPage(new BtInstallPage());
    addPage(new BtRemovePage());
    addPage(new BtIndexPage());

    retranslateUi();

    loadDialogSettings();
    setCurrentPage(0);

}

void BtModuleManagerDialog::retranslateUi() {
    setWindowTitle(tr("Bookshelf Manager"));
}

BtModuleManagerDialog::~BtModuleManagerDialog() {
    saveDialogSettings();
    m_staticModuleManagerDialog = 0;
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
