/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "ckeyreferencewidget.h"

#include "../cscrollerwidgetset.h"
#include "btdropdownchooserbutton.h"

#include "backend/config/cbtconfig.h"

#include "backend/keys/cswordversekey.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

//Qt includes
#include <QString>
#include <QStringList>
#include <QEvent>
#include <QPixmap>
#include <QApplication>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDebug>
#include <QLineEdit>



CKeyReferenceWidget::CKeyReferenceWidget( CSwordBibleModuleInfo *mod, CSwordVerseKey *key, QWidget *parent, const char* /*name*/) : 
	QWidget(parent),
	m_key(new CSwordVerseKey(mod))
{

	updatelock = false;
	m_module = mod;

	setFocusPolicy(Qt::WheelFocus);

	QToolButton* clearRef = new QToolButton(this);
	clearRef->setIcon(util::filesystem::DirectoryUtil::getIcon("edit_clear_locationbar"));
	clearRef->setAutoRaise(true);
	clearRef->setStyleSheet("QToolButton{margin:0px;}");
	connect(clearRef, SIGNAL(clicked()), SLOT(slotClearRef()) );

	m_bookScroller = new CScrollerWidgetSet(this);

	m_textbox = new QLineEdit( this );
	m_textbox->setStyleSheet("QLineEdit{margin:0px;}");

	setKey(key);	// The order of these two functions is important.
	setModule();

	m_chapterScroller = new CScrollerWidgetSet(this);
	m_verseScroller = new CScrollerWidgetSet(this);

	m_bookDropdownButton = new BtBookDropdownChooserButton(this);
	m_chapterDropdownButton = new BtChapterDropdownChooserButton(this);
	m_verseDropdownButton = new BtVerseDropdownChooserButton(this);

	QHBoxLayout* dropdownButtonsLayout = new QHBoxLayout();
	QVBoxLayout* editorAndButtonsLayout = new QVBoxLayout();
	dropdownButtonsLayout->setContentsMargins(0,0,0,0);
	editorAndButtonsLayout->setContentsMargins(0,0,0,0);
	dropdownButtonsLayout->setSpacing(0);
	editorAndButtonsLayout->setSpacing(0);
	
	dropdownButtonsLayout->addWidget(m_bookDropdownButton, 2);
	dropdownButtonsLayout->addWidget(m_chapterDropdownButton,1);
	dropdownButtonsLayout->addWidget(m_verseDropdownButton,1);
	editorAndButtonsLayout->addWidget(m_textbox);
	editorAndButtonsLayout->addLayout(dropdownButtonsLayout);

	QHBoxLayout* m_mainLayout = new QHBoxLayout( this );
	m_mainLayout->setContentsMargins(0,0,0,0);
	m_mainLayout->setSpacing(0);
	m_mainLayout->addWidget(clearRef);
	m_mainLayout->addWidget(m_bookScroller);
	m_mainLayout->addLayout(editorAndButtonsLayout);
	m_mainLayout->addWidget(m_chapterScroller);
	m_mainLayout->addWidget(m_verseScroller);


	setTabOrder(m_textbox, 0);

	QString scrollButtonToolTip(tr("Scroll through the entries of the list. Press the button and move the mouse to increase or decrease the item."));
    m_bookScroller->setToolTips(
	    tr("Next book"),
		scrollButtonToolTip,
		tr("Previous book")
    );
    m_chapterScroller->setToolTips(
        tr("Next chapter"),
        scrollButtonToolTip,
        tr("Previous chapter")
    );
    m_verseScroller->setToolTips(
        tr("Next verse"),
        scrollButtonToolTip,
        tr("Previous verse")
    );

	// signals and slots connections

	connect(m_bookScroller, SIGNAL(change(int)), SLOT(slotStepBook(int)));
	connect(m_bookScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_bookScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
	connect(m_textbox, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
	connect(m_chapterScroller, SIGNAL(change(int)), SLOT(slotStepChapter(int)));
	connect(m_chapterScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_chapterScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
	connect(m_verseScroller, SIGNAL(change(int)), SLOT(slotStepVerse(int)));
	connect(m_verseScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_verseScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
}

void CKeyReferenceWidget::setModule(CSwordBibleModuleInfo *m)
{
	if (m) //can be null
	{
		m_module = m;
		m_key->module(m);
	}
}

void CKeyReferenceWidget::slotClearRef( )
{
	m_textbox->setText("");
	m_textbox->setFocus();
}

void CKeyReferenceWidget::updateText()
{
	m_textbox->setText(m_key->key());
}

bool CKeyReferenceWidget::setKey(CSwordVerseKey *key)
{
	if (!key) return false;
	
	m_key->key(key->key());
	updateText();
	return true;
}

QLineEdit* CKeyReferenceWidget::textbox()
{
	return m_textbox;
}

void CKeyReferenceWidget::slotReturnPressed()
{
	m_key->key(m_textbox->text());
	updateText();
	
	emit changed(m_key.get());
}

/* Handlers for the various scroller widgetsets. Do we really want a verse scroller? */
void CKeyReferenceWidget::slotUpdateLock()
{
	updatelock = true;
	oldKey = m_key->key();
}

void CKeyReferenceWidget::slotUpdateUnlock()
{
	updatelock = false;
	if (oldKey != m_key->key()) emit changed(m_key.get());
}

void CKeyReferenceWidget::slotStepBook(int n)
{
	n > 0 ? m_key->next( CSwordVerseKey::UseBook ) : m_key->previous( CSwordVerseKey::UseBook );
	updateText();
	if (!updatelock) emit changed(m_key.get());
}

void CKeyReferenceWidget::slotStepChapter(int n)
{
	n > 0 ? m_key->next( CSwordVerseKey::UseChapter ) : m_key->previous( CSwordVerseKey::UseChapter );
	updateText();
	if (!updatelock) emit changed(m_key.get());	
}

void CKeyReferenceWidget::slotStepVerse(int n)
{
	n > 0 ? m_key->next( CSwordVerseKey::UseVerse ) : m_key->previous( CSwordVerseKey::UseVerse );
	updateText();
	if (!updatelock) emit changed(m_key.get());
}


void CKeyReferenceWidget::slotChangeVerse(int n)
{
	if (m_key->Verse() != n) {
		m_key->Verse( n );
		setKey( m_key.get() );
	}
	updateText();
	if (!updatelock) emit changed(m_key.get());
}

void CKeyReferenceWidget::slotChangeChapter(int n)
{
	if (m_key->Chapter() != n) {
		m_key->Chapter( n );
		setKey( m_key.get() );
	}
	updateText();
	if (!updatelock) emit changed(m_key.get());
}

void CKeyReferenceWidget::slotChangeBook(QString bookname)
{
	if (m_key->book() != bookname) {
		m_key->book( bookname );
		setKey( m_key.get() );
	}
	updateText();
	if (!updatelock) emit changed(m_key.get());
}

