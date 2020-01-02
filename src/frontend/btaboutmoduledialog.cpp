/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../util/btconnect.h"
#include "btaboutmoduledialog.h"
#include "messagedialog.h"


BTAboutModuleDialog::BTAboutModuleDialog(const CSwordModuleInfo *moduleInfo,
                                         QWidget *parent,
                                         Qt::WindowFlags flags)
        : QDialog(parent, flags)
        , m_moduleInfo(moduleInfo)
{
    resize(650, 400);
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vboxLayout->addWidget(m_textEdit);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    BT_CONNECT(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
    vboxLayout->addWidget(m_buttons);

    retranslateUi();

    BT_CONNECT(moduleInfo, SIGNAL(destroyed()), this, SLOT(close()));
}

void BTAboutModuleDialog::retranslateUi() {
    setWindowTitle(tr("Information About %1").arg(m_moduleInfo->name()));
    m_textEdit->setHtml(m_moduleInfo->aboutText());
    message::prepareDialogBox(m_buttons);
}
