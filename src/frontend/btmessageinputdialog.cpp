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

#include "btmessageinputdialog.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QTextBrowser>
#include <QVBoxLayout>
#include "../util/btconnect.h"
#include "../util/cresmgr.h"
#include "messagedialog.h"


BtMessageInputDialog::BtMessageInputDialog(QString const & title,
                                           QString const & inputLabel,
                                           InputType inputType,
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
        auto * const infoTextView = new QTextBrowser(this);
        infoTextView->setPlainText(infoMessage);
        infoTextView->setReadOnly(true);
        infoTextView->setOpenLinks(false);
        verticalLayout->addWidget(infoTextView);
        resize(480, 200);
    }

    QSpacerItem * verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    QLabel * unlockTextLabel = new QLabel(inputLabel, this);
    verticalLayout->addWidget(unlockTextLabel);

    m_inputTextEdit = new QLineEdit(inputText, parent);
    if (inputType == Password) {
        m_inputTextEdit->setEchoMode(QLineEdit::Password);

        // Add action to show/hide password:
        auto * const passwordVisibilityAction =
                m_inputTextEdit->addAction(CResMgr::mainIndex::showHide::icon(),
                                           QLineEdit::TrailingPosition);
        passwordVisibilityAction->setText(tr("Show password"));
        passwordVisibilityAction->setCheckable(true);
        BT_CONNECT(passwordVisibilityAction, &QAction::toggled,
                   [this,passwordVisibilityAction](bool enable) {
                       if (enable) {
                           m_inputTextEdit->setEchoMode(QLineEdit::Normal);
                           passwordVisibilityAction->setText(
                                       tr("Hide password"));
                       } else {
                           m_inputTextEdit->setEchoMode(QLineEdit::Password);
                           passwordVisibilityAction->setText(
                                       tr("Show password"));
                       }
                   });
    }
    verticalLayout->addWidget(m_inputTextEdit);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    message::prepareDialogBox(buttonBox);
    verticalLayout->addWidget(buttonBox);

    BT_CONNECT(buttonBox, &QDialogButtonBox::accepted,
               this, &BtMessageInputDialog::accept);
    BT_CONNECT(buttonBox, &QDialogButtonBox::rejected,
               this, &BtMessageInputDialog::reject);
}

QString BtMessageInputDialog::getUserInput() const {
    return m_inputTextEdit->text();
}

