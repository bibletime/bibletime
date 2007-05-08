/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


//BibleTime includes
#include "ckeyreferencewidget.h"
#include "cscrollerwidgetset.h"

//BibleTime frontend includes
#include "frontend/cbtconfig.h"
#include "backend/cswordversekey.h"
#include "util/cresmgr.h"

//Qt includes
#include <klineedit.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <kcompletion.h>
#include <kglobalsettings.h>
#include <kcompletionbox.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kguiitem.h>

/* Override the completion box for our references */
CKeyReferenceCompletion::CKeyReferenceCompletion(CSwordBibleModuleInfo *mod) : KCompletion()
{
	m_key = new CSwordVerseKey(mod);
	m_module = mod;
}

QString CKeyReferenceCompletion::makeCompletion(const QString &text) {
	if(!text.isEmpty() && m_key->key(text)) {
		// XXX: key() does not check bounds properly if we only have eg the NT.
		return m_key->key();
	}

	return QString::null;
}

//**********************************************************************************/
/* To get popup working we have to rework KLineEdit too */
CKeyReferenceLineEdit::CKeyReferenceLineEdit(QWidget *parent, const char *name) : KLineEdit(parent,name) {
}

void CKeyReferenceLineEdit::makeCompletion(const QString &text) {
	KCompletion *comp = compObj();
	KGlobalSettings::Completion mode = completionMode();

	if ( !comp || mode == KGlobalSettings::CompletionNone )
		return;  // No completion object...

	QString match = comp->makeCompletion( text );
	if ( mode == KGlobalSettings::CompletionPopup ||
		mode == KGlobalSettings::CompletionPopupAuto )
	{
		if ( match.isNull() )
		{
			KCompletionBox *compbox = completionBox();
			compbox->hide();
			compbox->clear();
		} else {
			QStringList t;
			t.append(match);
			setCompletedItems(t);
		}
	} else {
		KLineEdit::makeCompletion(text);
	}
}

//**********************************************************************************/

CKeyReferenceWidget::CKeyReferenceWidget( CSwordBibleModuleInfo *mod, CSwordVerseKey *key, QWidget *parent, const char *name) : QWidget(parent,name) {

	updatelock = false;
	m_module = mod;

	setFocusPolicy(QWidget::WheelFocus);

	// Erase button
	KGuiItem erase_picture;
	erase_picture.setIconName("locationbar_erase");
	KPushButton *clearRef = new KPushButton(this);
	clearRef->setGuiItem(erase_picture);
	connect(clearRef, SIGNAL(clicked( ) ), SLOT(slotClearRef( )));

	m_bookScroller = new CScrollerWidgetSet(this);

	m_textbox = new CKeyReferenceLineEdit( this );
	setKey(key);	// The order of these two functions is important.
	setModule();

	m_chapterScroller = new CScrollerWidgetSet(this);
	m_verseScroller = new CScrollerWidgetSet(this);

	m_mainLayout = new QHBoxLayout( this );
	m_mainLayout->addWidget(clearRef);
	m_mainLayout->addWidget(m_bookScroller);
	m_mainLayout->addWidget(m_textbox);
	m_mainLayout->addWidget(m_chapterScroller);
	m_mainLayout->addWidget(m_verseScroller);

	setTabOrder(m_textbox, 0);

        m_bookScroller->setToolTips(
                CResMgr::displaywindows::bibleWindow::nextBook::tooltip,
                CResMgr::displaywindows::general::scrollButton::tooltip,
                CResMgr::displaywindows::bibleWindow::previousBook::tooltip
        );
        m_chapterScroller->setToolTips(
                CResMgr::displaywindows::bibleWindow::nextChapter::tooltip,
                CResMgr::displaywindows::general::scrollButton::tooltip,
                CResMgr::displaywindows::bibleWindow::previousChapter::tooltip
        );
        m_verseScroller->setToolTips(
                CResMgr::displaywindows::bibleWindow::nextVerse::tooltip,
                CResMgr::displaywindows::general::scrollButton::tooltip,
                CResMgr::displaywindows::bibleWindow::previousVerse::tooltip
        );


	// signals and slots connections

	connect(m_bookScroller, SIGNAL(change(int)), SLOT(slotBookChange(int)));
	connect(m_bookScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_bookScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
	connect(m_textbox, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
	connect(m_chapterScroller, SIGNAL(change(int)), SLOT(slotChapterChange(int)));
	connect(m_chapterScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_chapterScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
	connect(m_verseScroller, SIGNAL(change(int)), SLOT(slotVerseChange(int)));
	connect(m_verseScroller, SIGNAL(scroller_pressed()), SLOT(slotUpdateLock()));
	connect(m_verseScroller, SIGNAL(scroller_released()), SLOT(slotUpdateUnlock()));
}

void CKeyReferenceWidget::setModule(CSwordBibleModuleInfo *m) {
	if (m)
		m_module = m;

	delete m_textbox->completionObject();
	CKeyReferenceCompletion *comp = new CKeyReferenceCompletion(m_module);
	m_textbox->setCompletionObject(comp);
	m_textbox->setCompletionMode(KGlobalSettings::CompletionPopup);
}

void CKeyReferenceWidget::slotClearRef( ) {
	m_textbox->setText("");
	m_textbox->setFocus();
}

void CKeyReferenceWidget::updateText() {
	m_textbox->setText(m_key->key());
}

bool CKeyReferenceWidget::setKey(CSwordVerseKey *key) {
	m_key = key;
	updateText();
	
	return true;
}

KLineEdit* CKeyReferenceWidget::textbox() {
	return m_textbox;
}

void CKeyReferenceWidget::slotReturnPressed() {
	m_key->key(m_textbox->text());
	updateText();
	
	emit changed(m_key);
}

/* Handlers for the various scroller widgetsets. Do we really want a verse scroller? */
void CKeyReferenceWidget::slotUpdateLock() {
	updatelock = true;
	oldKey = m_key->key();
}

void CKeyReferenceWidget::slotUpdateUnlock() {
	updatelock = false;
	if (oldKey != m_key->key()) emit changed(m_key);
}

void CKeyReferenceWidget::slotBookChange(int n) {
	n > 0 ? m_key->next( CSwordVerseKey::UseBook ) : m_key->previous( CSwordVerseKey::UseBook );
	updateText();
	if (!updatelock) emit changed(m_key);
}

void CKeyReferenceWidget::slotChapterChange(int n) {
	n > 0 ? m_key->next( CSwordVerseKey::UseChapter ) : m_key->previous( CSwordVerseKey::UseChapter );
	updateText();
	if (!updatelock) emit changed(m_key);	
}

void CKeyReferenceWidget::slotVerseChange(int n) {
	n > 0 ? m_key->next( CSwordVerseKey::UseVerse ) : m_key->previous( CSwordVerseKey::UseVerse );
	updateText();
	if (!updatelock) emit changed(m_key);
}
