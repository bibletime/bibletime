/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTDROPDOWNCHOOSERBUTTON_H
#define BTDROPDOWNCHOOSERBUTTON_H


#include <QToolButton>

class CKeyReferenceWidget;

/**
	@author The BibleTime team <info@bibletime.info>
*/
class BtDropdownChooserButton : public QToolButton
{
public:
	enum Type { Book, Chapter, Verse };

	BtDropdownChooserButton(CKeyReferenceWidget* ref, Type type);

	~BtDropdownChooserButton();
	virtual void mousePressEvent(QMouseEvent* event);

private:
	CKeyReferenceWidget* m_ref;
	Type m_type;

};

#endif
