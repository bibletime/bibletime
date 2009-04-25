/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CKEYREFERENCEWIDGET_H
#define CKEYREFERENCEWIDGET_H

#include "../cscrollerwidgetset.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"

#include <QWidget>

#include <QSharedPointer>


class CLexiconKeyChooser;
class CSwordVerseKey;
class BtDropdownChooserButton;

class QLineEdit;


class CKeyReferenceWidget : public QWidget  {
	Q_OBJECT
public:
	/**
	* the constructor
	*/
	CKeyReferenceWidget(CSwordBibleModuleInfo *, CSwordVerseKey*, QWidget *parent=0, const char *name=0);
	bool setKey(CSwordVerseKey* key);
	QLineEdit* textbox();
	void setModule(CSwordBibleModuleInfo *m = 0);

signals:
	void changed(CSwordVerseKey* key);

protected:
	void updateText();

protected slots: // Protected slots
	/**
	* Is called when the return key was presed in the textbox.
	*/
	void slotReturnPressed();

	void slotClearRef();

	void slotUpdateLock();
	void slotUpdateUnlock();
	void slotStepBook(int);
	void slotStepChapter(int);
	void slotStepVerse(int);
	void slotChangeBook(QString bookname);
	void slotChangeChapter(int chapter);
	void slotChangeVerse(int verse);

private:
	friend class CLexiconKeyChooser;
	friend class BtDropdownChooserButton;
	friend class BtBookDropdownChooserButton;
	friend class BtChapterDropdownChooserButton;
	friend class BtVerseDropdownChooserButton;

	QSharedPointer<CSwordVerseKey> m_key;

	QLineEdit* m_textbox;

	CScrollerWidgetSet *m_bookScroller;
	CScrollerWidgetSet *m_chapterScroller;
	CScrollerWidgetSet *m_verseScroller;

	BtDropdownChooserButton* m_bookDropdownButton;
	BtDropdownChooserButton* m_chapterDropdownButton;
	BtDropdownChooserButton* m_verseDropdownButton;

	bool updatelock;
	QString oldKey;
	CSwordBibleModuleInfo *m_module;
};

#endif
