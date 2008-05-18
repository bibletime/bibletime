/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btaboutmoduledialog.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include <QWidget>
#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>

BTAboutModuleDialog::BTAboutModuleDialog(QWidget* parent, CSwordModuleInfo* info)
	: QDialog(parent)
{
	//Set the flag to destroy when closed - otherwise eats memory
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Information About") + QString(" ") + info->name());
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
    vboxLayout->addWidget(buttonBox);


    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}
