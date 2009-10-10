/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/dialogutil.h"

#include <QDialogButtonBox>
#include <QPushButton>


namespace util {

namespace {

void replaceText(QDialogButtonBox *box, QDialogButtonBox::StandardButton flag,
                 const QString &text)
{
    QPushButton *button(box->button(flag));
    if (button != 0) {
        button->setText(text);
    }
}

} // anonymous namespace

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

} // namespace util
