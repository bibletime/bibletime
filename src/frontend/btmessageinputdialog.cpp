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
#include "btmessageinputdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QTextBrowser>
#include <QVBoxLayout>

BtMessageInputDialog::BtMessageInputDialog(QString const & title,
                                           QString const & inputLabel,
                                           QString const & inputText,
                                           QString const & infoMessage,
                                           QWidget * parent,
                                           Qt::WindowFlags f)
    : QDialog(parent, f)
{
    resize(280, 90);
    setWindowTitle(title);
    QVBoxLayout * verticalLayout = new QVBoxLayout(this);

    if (!infoMessage.isEmpty()) {
        m_infoTextEdit = new QTextBrowser(this);
        m_infoTextEdit->setPlainText(infoMessage);
        m_infoTextEdit->setReadOnly(true);
        m_infoTextEdit->setOpenLinks(false);
        verticalLayout->addWidget(m_infoTextEdit);
        resize(480, 200);
    }

    QSpacerItem * verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    QLabel * unlockTextLabel = new QLabel(inputLabel, this);
    verticalLayout->addWidget(unlockTextLabel);

    m_inputTextEdit = new QLineEdit(inputText, parent);
    verticalLayout->addWidget(m_inputTextEdit);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString BtMessageInputDialog::getUserInput() const {
    return m_inputTextEdit->text();
}

