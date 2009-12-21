/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/btmodulechooserdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "frontend/btbookshelfview.h"
#include "util/dialogutil.h"
#include "util/tool.h"


BtModuleChooserDialog::BtModuleChooserDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_captionLabel = new QLabel(this);
    mainLayout->addWidget(m_captionLabel);

    m_treeView = new BtBookshelfView(this);
    mainLayout->addWidget(m_treeView);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
                                       Qt::Horizontal, this);
    connect(m_buttonBox, SIGNAL(accepted()),
            this,        SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()),
            this,        SLOT(reject()));
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

    retranslateUi();
}

BtModuleChooserDialog::~BtModuleChooserDialog() {
    // Intentionally empty
}

void BtModuleChooserDialog::retranslateUi() {
    util::prepareDialogBox(m_buttonBox);
}
