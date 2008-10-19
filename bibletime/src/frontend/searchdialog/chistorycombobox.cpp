/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "chistorycombobox.h"

namespace Search {

CHistoryComboBox::CHistoryComboBox( QWidget* parent)
	: QComboBox(parent)
{
	setEditable(true);
}

CHistoryComboBox::~CHistoryComboBox()
{
}

void CHistoryComboBox::addToHistory(const QString& text)
{
	int index = findText(text);
	if ( index >= 0)
		removeItem(index);
	insertItem(1, text);
	setCurrentIndex(1);
}

QStringList CHistoryComboBox::historyItems() const
{
	QStringList items;
	for (int i=0; i<count(); i++)
	{
		items << itemText(i);
	}
	return items;
}
} //end of namespace Search

