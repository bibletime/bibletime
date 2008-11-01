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
* Base class for book/ch/v dropdown list chooser buttons.
*/
class BtDropdownChooserButton : public QToolButton
{
	Q_OBJECT
public:
	BtDropdownChooserButton(CKeyReferenceWidget* ref);

	virtual ~BtDropdownChooserButton() {}
	/** The item list is constructed here just before the menu is shown.*/
	virtual void mousePressEvent(QMouseEvent* event);
	/** Recreates the menu list.*/
	virtual void newList() = 0;
public slots:
	/** When a menu item is selected the key will be changed.*/
	virtual void slotMenuTriggered(QAction* action) = 0;

	/** Returns the verse reference widget which this button belongs to.*/
	CKeyReferenceWidget* ref() {return m_ref;}

protected:
	CKeyReferenceWidget* m_ref;
};

/** See BtDropdownChooserButton.*/
class BtBookDropdownChooserButton : public BtDropdownChooserButton
{
	Q_OBJECT
public:
	BtBookDropdownChooserButton(CKeyReferenceWidget* ref);
	~BtBookDropdownChooserButton() {}
	virtual void newList();
public slots:
	virtual void slotMenuTriggered(QAction* action);
};

/** See BtDropdownChooserButton.*/
class BtChapterDropdownChooserButton : public BtDropdownChooserButton
{
public:
	BtChapterDropdownChooserButton(CKeyReferenceWidget* ref);
	~BtChapterDropdownChooserButton() {}
	virtual void newList();
	virtual void slotMenuTriggered(QAction* action);
};

/** See BtDropdownChooserButton.*/
class BtVerseDropdownChooserButton : public BtDropdownChooserButton
{
public:
	BtVerseDropdownChooserButton(CKeyReferenceWidget* ref);
	~BtVerseDropdownChooserButton() {}
	virtual void newList();
	virtual void slotMenuTriggered(QAction* action);
};
#endif
