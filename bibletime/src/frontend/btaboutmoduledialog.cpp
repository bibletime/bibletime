/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/btaboutmoduledialog.h"
#include "util/dialogutil.h"


BTAboutModuleDialog::BTAboutModuleDialog(QWidget* parent, CSwordModuleInfo* info)
        : QDialog(parent) {
    //Set the flag to destroy when closed - otherwise eats memory
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Information About %1").arg(info->name()));
    resize(650, 400);
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);

    QTextEdit* textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vboxLayout->addWidget(textEdit);
    textEdit->setHtml(info->aboutText());

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    util::prepareDialogBox(buttonBox);
    vboxLayout->addWidget(buttonBox);


    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}
