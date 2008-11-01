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
	Q_OBJECT
public:
	BtDropdownChooserButton(CKeyReferenceWidget* ref);

	virtual ~BtDropdownChooserButton() {}
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void newList() = 0;
public slots:
	virtual void slotMenuTriggered(QAction* action) = 0;

	CKeyReferenceWidget* ref() {return m_ref;}

protected:
	CKeyReferenceWidget* m_ref;
};

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

class BtChapterDropdownChooserButton : public BtDropdownChooserButton
{
public:
	BtChapterDropdownChooserButton(CKeyReferenceWidget* ref);
	~BtChapterDropdownChooserButton() {}
	virtual void newList();
	virtual void slotMenuTriggered(QAction* action);
};

class BtVerseDropdownChooserButton : public BtDropdownChooserButton
{
public:
	BtVerseDropdownChooserButton(CKeyReferenceWidget* ref);
	~BtVerseDropdownChooserButton() {}
	virtual void newList();
	virtual void slotMenuTriggered(QAction* action);
};
#endif
