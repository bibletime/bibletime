//
// C++ Interface: dialogutil
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dialogutil.h"
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>

namespace util
{

	static void replaceText(QDialogButtonBox* box, QDialogButtonBox::StandardButton flag, const QString& text)
	{
		QPushButton* button = box->button(flag);
		if (button != 0)
			button->setText(text);
	}

	void prepareDialogBox(QDialogButtonBox* box)
	{
		//: Standard button
		replaceText(box, QDialogButtonBox::Ok      , QPushButton::tr("OK"        , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Open    , QPushButton::tr("Open"      , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Save    , QPushButton::tr("Save"      , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Cancel  , QPushButton::tr("Cancel"    , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Close   , QPushButton::tr("Close"     , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Discard , QPushButton::tr("Discard"   , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Apply   , QPushButton::tr("Apply"     , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Reset   , QPushButton::tr("Reset"     , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::RestoreDefaults, QPushButton::tr("Restore defaults", "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Help    , QPushButton::tr("Help"      , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::SaveAll , QPushButton::tr("Save All"  , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::Yes     , QPushButton::tr("Yes"       , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::YesToAll, QPushButton::tr("Yes to all", "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::No      , QPushButton::tr("No"        , "Dialog Button"));
		//: Standard button
		replaceText(box, QDialogButtonBox::NoToAll , QPushButton::tr("No to all" , "Dialog Button"));
	}

}

