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

#include "btshortcutsdialog.h"

#include <QDialogButtonBox>
#include <QFlags>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <memory>
#include "../../util/btconnect.h"
#include "../messagedialog.h"


// *************** BtShortcutsDialog ***************************************************************************
// A dialog to allow the user to input a shortcut for a primary and alternate key

BtShortcutsDialog::BtShortcutsDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setMinimumWidth(350);

    auto * const vLayout = new QVBoxLayout(this);

    {
        auto gridLayout = std::make_unique<QGridLayout>();

        m_primaryButton = new QRadioButton(this);
        m_primaryButton->setChecked(true);
        gridLayout->addWidget(m_primaryButton, 0, 0);

        m_alternateButton = new QRadioButton(this);
        gridLayout->addWidget(m_alternateButton, 1, 0);

        m_primaryLabel = new QLabel(this);
        m_primaryLabel->setMinimumWidth(100);
        m_primaryLabel->setFrameShape(QFrame::Panel);
        gridLayout->addWidget(m_primaryLabel, 0, 1);

        m_alternateLabel = new QLabel(this);
        m_alternateLabel->setMinimumWidth(100);
        m_alternateLabel->setFrameShape(QFrame::Panel);
        gridLayout->addWidget(m_alternateLabel, 1, 1);

        vLayout->addLayout(gridLayout.get());
        gridLayout.release();
    }

    auto * const buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                                | QDialogButtonBox::Cancel,
                                                this);
    message::prepareDialogBox(buttons);
    BT_CONNECT(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    BT_CONNECT(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    vLayout->addWidget(buttons);

    setLayout(vLayout);

    retranslateUi();
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
        keyStr = QStringLiteral("Alt+") + keyStr;
    if ( (event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        keyStr = QStringLiteral("Shift+") + keyStr;
    if ( (event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        keyStr = QStringLiteral("Ctrl+") + keyStr;

    keyChangeRequest(keyStr);
}

// complete the keyChangeRequest
void BtShortcutsDialog::changeSelectedShortcut(QKeySequence const & keys) {
    if (!m_primaryButton->isChecked() || !m_alternateButton->isChecked())
        return;

    auto const keysString(keys.toString());
    if (m_primaryButton->isChecked())
        m_primaryLabel->setText(keysString);

    if (m_alternateButton->isChecked())
        m_alternateLabel->setText(keysString);
}

void BtShortcutsDialog::retranslateUi() {
    setWindowTitle(tr("Configure shortcuts"));
    auto dialogTooltip = tr("Select first or second shortcut and type the "
                            "shortcut with keyboard");

    m_primaryButton->setText(tr("First shortcut"));
    m_primaryButton->setToolTip(dialogTooltip);

    m_alternateButton->setText(tr("Second shortcut"));
    m_alternateButton->setToolTip(dialogTooltip);

    m_primaryLabel->setToolTip(dialogTooltip);

    m_alternateLabel->setToolTip(dialogTooltip);
}

