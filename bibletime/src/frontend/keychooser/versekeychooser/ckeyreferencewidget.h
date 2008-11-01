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

#include <klineedit.h>
#include <kcompletion.h>

#include <boost/scoped_ptr.hpp>


class CLexiconKeyChooser;
class CSwordVerseKey;
class BtDropdownChooserButton;


class CKeyReferenceCompletion : public KCompletion {
	Q_OBJECT
public:
	CKeyReferenceCompletion(CSwordBibleModuleInfo*);
	~CKeyReferenceCompletion();
	QString makeCompletion(const QString &);
	QStringList allMatches();
private:
	boost::scoped_ptr<CSwordVerseKey> m_key;
	CSwordBibleModuleInfo *m_module;
};

class CKeyReferenceLineEdit : public KLineEdit {
	Q_OBJECT
public:
	CKeyReferenceLineEdit(QWidget *parent=0, const char *name=0);
	void makeCompletion(const QString &);
};

class CKeyReferenceWidget : public QWidget  {
	Q_OBJECT
public:
	/**
	* the constructor
	*/
	CKeyReferenceWidget(CSwordBibleModuleInfo *, CSwordVerseKey*, QWidget *parent=0, const char *name=0);
	bool setKey(CSwordVerseKey* key);
	KLineEdit* textbox();
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
	void slotBookChange(int);
	void slotChapterChange(int);
	void slotVerseChange(int);

private:
	friend class CLexiconKeyChooser;
	friend class BtDropdownChooserButton;
	friend class BtBookDropdownChooserButton;
	friend class BtChapterDropdownChooserButton;
	friend class BtVerseDropdownChooserButton;

	boost::scoped_ptr<CSwordVerseKey> m_key;

	CKeyReferenceLineEdit *m_textbox;
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
