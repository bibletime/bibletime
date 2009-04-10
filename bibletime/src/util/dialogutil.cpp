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
		replaceText(box, QDialogButtonBox::Ok      , QPushButton::tr("Ok"        , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Open    , QPushButton::tr("Open"      , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Save    , QPushButton::tr("Save"      , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Cancel  , QPushButton::tr("Cancel"    , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Close   , QPushButton::tr("Close"     , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Discard , QPushButton::tr("Discard"   , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Apply   , QPushButton::tr("Apply"     , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Reset   , QPushButton::tr("Reset"     , "Dialog Button"));
		replaceText(box, QDialogButtonBox::RestoreDefaults, QPushButton::tr("Restore Defaults", "Dialog Button"));
		replaceText(box, QDialogButtonBox::Help    , QPushButton::tr("Help"      , "Dialog Button"));
		replaceText(box, QDialogButtonBox::SaveAll , QPushButton::tr("Save All"  , "Dialog Button"));
		replaceText(box, QDialogButtonBox::Yes     , QPushButton::tr("Yes"       , "Dialog Button"));
		replaceText(box, QDialogButtonBox::YesToAll, QPushButton::tr("Yes To All", "Dialog Button"));
		replaceText(box, QDialogButtonBox::No      , QPushButton::tr("No"        , "Dialog Button"));
		replaceText(box, QDialogButtonBox::NoToAll , QPushButton::tr("No To All" , "Dialog Button"));
	}

}

