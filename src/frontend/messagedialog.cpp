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

#include "messagedialog.h"

#include <QAbstractButton>
#include <QAction>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <Qt>
#include "../util/btassert.h"


namespace message {

namespace {

void replaceText(QDialogButtonBox *box, QDialogButtonBox::StandardButton flag,
                 const QString &text) {
    QPushButton *button(box->button(flag));
    if (button != nullptr) {
        button->setText(text);
    }
}

QMessageBox::StandardButton bt_messageBox(QMessageBox::Icon icon,
                                          QWidget * parent,
                                          const QString &title,
                                          const QString &text,
                                          QMessageBox::StandardButtons buttons,
                                          QMessageBox::StandardButton defaultButton)
{
    QMessageBox messageBox(icon, title, text, QMessageBox::Ok, parent);
    messageBox.setTextFormat(Qt::RichText);
    //We need the button box to translate the strings (the idea of this whole function)
    QDialogButtonBox* box = dynamic_cast<QDialogButtonBox*>(messageBox.button(QMessageBox::Ok)->parent());
    BT_ASSERT(box);
    messageBox.setStandardButtons(buttons);
    messageBox.setDefaultButton(defaultButton);
    prepareDialogBox(box);
    return static_cast<QMessageBox::StandardButton>(messageBox.exec());
}

} // anonymous namespace

void setQActionCheckedNoTrigger(QAction * const action, const bool checked) {
    BT_ASSERT(action);
    const bool signalsWereBlocked = action->blockSignals(true);
    action->setChecked(checked);
    action->blockSignals(signalsWereBlocked);
}

void prepareDialogBox(QDialogButtonBox *box) {
    replaceText(box, QDialogButtonBox::Ok      , QPushButton::tr("OK"        , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Open    , QPushButton::tr("Open"      , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Save    , QPushButton::tr("Save"      , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Cancel  , QPushButton::tr("Cancel"    , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Close   , QPushButton::tr("Close"     , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Discard , QPushButton::tr("Discard"   , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Apply   , QPushButton::tr("Apply"     , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Reset   , QPushButton::tr("Reset"     , "Dialog Button"));
    replaceText(box, QDialogButtonBox::RestoreDefaults, QPushButton::tr("Restore defaults", "Dialog Button"));
    replaceText(box, QDialogButtonBox::Help    , QPushButton::tr("Help"      , "Dialog Button"));
    replaceText(box, QDialogButtonBox::SaveAll , QPushButton::tr("Save All"  , "Dialog Button"));
    replaceText(box, QDialogButtonBox::Yes     , QPushButton::tr("Yes"       , "Dialog Button"));
    replaceText(box, QDialogButtonBox::YesToAll, QPushButton::tr("Yes to all", "Dialog Button"));
    replaceText(box, QDialogButtonBox::No      , QPushButton::tr("No"        , "Dialog Button"));
    replaceText(box, QDialogButtonBox::NoToAll , QPushButton::tr("No to all" , "Dialog Button"));
}

QMessageBox::StandardButton showWarning(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    return bt_messageBox(QMessageBox::Warning, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton showInformation(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    return bt_messageBox(QMessageBox::Information, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton showCritical(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    return bt_messageBox(QMessageBox::Critical, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton showQuestion(QWidget * parent, const QString & title, const QString & text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    return bt_messageBox(QMessageBox::Question, parent, title, text, buttons, defaultButton);
}

} // namespace message
