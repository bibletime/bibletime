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

#include "btmodulechooserdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include "../backend/drivers/cswordmoduleinfo.h" // IWYU pragma: keep for Qt con
#include "../util/btconnect.h"
#include "btaboutmoduledialog.h"
#include "btbookshelfview.h"
#include "btbookshelfwidget.h"
#include "messagedialog.h"


BtModuleChooserDialog::BtModuleChooserDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_captionLabel = new QLabel(this);
    mainLayout->addWidget(m_captionLabel);

    m_bookshelfWidget = new BtBookshelfWidget(this);
    BT_CONNECT(m_bookshelfWidget->treeView(), &BtBookshelfView::moduleActivated,
               [this](CSwordModuleInfo * const module) {
                   auto * const dialog = new BTAboutModuleDialog(module, this);
                   dialog->setAttribute(Qt::WA_DeleteOnClose);
                   dialog->show();
                   dialog->raise();
               });
    mainLayout->addWidget(m_bookshelfWidget);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
                                       Qt::Horizontal, this);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               this, &BtModuleChooserDialog::accept);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &BtModuleChooserDialog::reject);
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

    retranslateUi();
}

void BtModuleChooserDialog::retranslateUi() {
    message::prepareDialogBox(m_buttonBox);
}
