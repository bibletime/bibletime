/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"

#include "backend/config/btconfig.h"
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
    : BtConfigDialog(parent,
                     flags | Qt::CustomizeWindowHint | Qt::WindowTitleHint
                           | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint)
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
    resize(btConfig().value<QSize>("gui/windows/bookshelf/Size"));
    move(btConfig().value<QPoint>("gui/windows/bookshelf/Pos"));
}

void BtModuleManagerDialog::saveDialogSettings() {
    btConfig().setValue("gui/windows/bookshelf/Size", size());
    btConfig().setValue("gui/windows/bookshelf/Pos", pos());
}
