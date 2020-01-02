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

#include "btshortcutsdialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "../../util/btconnect.h"
#include "../messagedialog.h"


// *************** BtShortcutsDialog ***************************************************************************
// A dialog to allow the user to input a shortcut for a primary and alternate key

BtShortcutsDialog::BtShortcutsDialog(QWidget* parent)
        : QDialog(parent), m_primaryLabel(nullptr), m_alternateLabel(nullptr), m_primaryButton(nullptr), m_alternateButton(nullptr) {
    setWindowTitle(tr("Configure shortcuts"));
    setMinimumWidth(350);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    QGridLayout* gridLayout = new QGridLayout();
    vLayout->addLayout(gridLayout);

    QString dialogTooltip = tr("Select first or second shortcut and type the shortcut with keyboard");

    m_primaryButton = new QRadioButton(tr("First shortcut"));
    m_primaryButton->setToolTip(dialogTooltip);
    m_primaryButton->setChecked(true);
    gridLayout->addWidget(m_primaryButton, 0, 0);

    m_alternateButton = new QRadioButton(tr("Second shortcut"));
    m_alternateButton->setToolTip(dialogTooltip);
    gridLayout->addWidget(m_alternateButton, 1, 0);

    m_primaryLabel = new QLabel();
    m_primaryLabel->setToolTip(dialogTooltip);
    m_primaryLabel->setMinimumWidth(100);
    m_primaryLabel->setFrameShape(QFrame::Panel);
    gridLayout->addWidget(m_primaryLabel, 0, 1);

    m_alternateLabel = new QLabel();
    m_alternateLabel->setToolTip(dialogTooltip);
    m_alternateLabel->setMinimumWidth(100);
    m_alternateLabel->setFrameShape(QFrame::Panel);
    gridLayout->addWidget(m_alternateLabel, 1, 1);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    message::prepareDialogBox(buttons);
    vLayout->addWidget(buttons);

    BT_CONNECT(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    BT_CONNECT(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

// get new primary key from dialog
QString BtShortcutsDialog::getFirstKeys() {
    return m_primaryLabel->text();
}

// set the initial value of the primary key
void BtShortcutsDialog::setFirstKeys(const QString& keys) {
    m_primaryLabel->setText(keys);
}

// get new second keys from dialog
QString BtShortcutsDialog::getSecondKeys() {
    return m_alternateLabel->text();
}

// set the initial value of the second keys
void BtShortcutsDialog::setSecondKeys(const QString& keys) {
    m_alternateLabel->setText(keys);
}

// get key from users input, put into primary or alternate label for display to user
void BtShortcutsDialog::keyReleaseEvent(QKeyEvent* event) {
    int key = event->key();
    if ( (key == Qt::Key_Shift) || (key == Qt::Key_Control) || (key == Qt::Key_Meta) || (key == Qt::Key_Alt) )
        return;

    QKeySequence keys(key);
    QString keyStr = keys.toString();
    if ( (event->modifiers() & Qt::AltModifier) == Qt::AltModifier)
        keyStr = "Alt+" + keyStr;
    if ( (event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        keyStr = "Shift+" + keyStr;
    if ( (event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        keyStr = "Ctrl+" + keyStr;

    QKeySequence completeKeys(keyStr);
    QString completeStr = completeKeys.toString();

    keyChangeRequest(completeStr);
}

// complete the keyChangeRequest
void BtShortcutsDialog::changeSelectedShortcut(const QString& keys) {
    if (m_primaryButton->isChecked())
        m_primaryLabel->setText(keys);

    if (m_alternateButton->isChecked())
        m_alternateLabel->setText(keys);
}


