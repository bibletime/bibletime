//
// C++ Implementation: cacceleratorsettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cacceleratorsettings.h"

#include "frontend/cbtconfig.h"

#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/clexiconreadwindow.h"
#include "frontend/displaywindow/cbookreadwindow.h"
#include "frontend/displaywindow/creadwindow.h"

#include <QWidget>
#include <QComboBox>
#include <Q3WidgetStack>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

#include <kactioncollection.h>
#include <klocale.h>


CAcceleratorSettingsPage::CAcceleratorSettingsPage(QWidget *parent)
	: QWidget(parent)
{
	//TODO: widget layout may not work. Maybe it would be easier to use .ui file.
	
	QVBoxLayout* vlayout = new QVBoxLayout(this);
	this->setLayout(vlayout);
	
	CBTConfig::setupAccelSettings(
		CBTConfig::application,
		m_application.actionCollection
	);

	QHBoxLayout* hbox = new QHBoxLayout(this);
	vlayout->addLayout(hbox);
	QLabel* label = new QLabel(i18n("Choose type:"), this);
	hbox->addWidget(label);
	m_typeChooser = new QComboBox(this);
	hbox->addWidget(m_typeChooser);

	connect(
		m_typeChooser, SIGNAL(activated(const QString&)),
		SLOT(slotKeyChooserTypeChanged(const QString&))
	);
	QLabel* dummy = new QLabel( this); // empty label for stretch
	hbox->addWidget(dummy);

	hbox->setStretchFactor(label, 0);
	hbox->setStretchFactor(m_typeChooser, 0);
	hbox->setStretchFactor(dummy, 1);

	vlayout->setStretchFactor(hbox, 0);

	m_keyChooserStack = new Q3WidgetStack(this);
	Q_ASSERT(m_keyChooserStack);

	vlayout->setStretchFactor(m_keyChooserStack, 5);

	m_application.title = i18n("BibleTime"); //don't set the app action collection to NULL
	m_general = WindowType(i18n("All text windows"));
	m_bible = WindowType(i18n("Bible windows"));
	m_commentary = WindowType(i18n("Commentary windows"));
	m_lexicon = WindowType(i18n("Lexicon windows"));
	m_book = WindowType(i18n("Book windows"));

	m_typeChooser->addItem(m_application.title);
	m_typeChooser->addItem(m_general.title);
	m_typeChooser->addItem(m_bible.title);
	m_typeChooser->addItem(m_commentary.title);
	m_typeChooser->addItem(m_lexicon.title);
	m_typeChooser->addItem(m_book.title);


	Q_ASSERT(m_application.actionCollection);
	m_application.keyChooser = new KShortcutsEditor(
				m_application.actionCollection,
				m_keyChooserStack
			);

	// ----- All display windows ------ //
	m_general.actionCollection = new KActionCollection(this);
	CDisplayWindow::insertKeyboardActions( m_general.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::allWindows,
		m_general.actionCollection
	);
	m_general.keyChooser = new KShortcutsEditor(
											 m_general.actionCollection,
											 m_keyChooserStack
										 );

	// ----- Bible windows ------ //
	m_bible.actionCollection = new KActionCollection(this);
	CBibleReadWindow::insertKeyboardActions( m_bible.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::bibleWindow,
		m_bible.actionCollection
	);

	// ----- Commentary windows ------ //
	m_commentary.actionCollection = new KActionCollection(this);
	CCommentaryReadWindow::insertKeyboardActions( m_commentary.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::commentaryWindow,
		m_commentary.actionCollection
	);

	// ----- Lexicon windows ------ //
	m_lexicon.actionCollection = new KActionCollection(this);
	CLexiconReadWindow::insertKeyboardActions(  m_lexicon.actionCollection );
	CBTConfig::setupAccelSettings(
		CBTConfig::lexiconWindow,
		m_lexicon.actionCollection
	);

	// ----- Book windows ------ //
	m_book.actionCollection= new KActionCollection(this);
	CBookReadWindow::insertKeyboardActions( m_book.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::bookWindow,
		m_book.actionCollection
	);

	slotKeyChooserTypeChanged(m_application.title);

}


CAcceleratorSettingsPage::~CAcceleratorSettingsPage()
{
}

void CAcceleratorSettingsPage::save()
{
	/* //TODO: does kde4 work without commitChanges?
	if (m_general.keyChooser) {
		m_general.keyChooser->commitChanges();
	}

	if (m_bible.keyChooser) {
		m_bible.keyChooser->commitChanges();
	}

	if (m_commentary.keyChooser) {
		m_commentary.keyChooser->commitChanges();
	}

	if (m_lexicon.keyChooser) {
		m_lexicon.keyChooser->commitChanges();
	}

	if (m_book.keyChooser) {
		m_book.keyChooser->commitChanges();
	}
	*/
	CBTConfig::saveAccelSettings( //application
		CBTConfig::application,
		m_application.actionCollection
	);
	CBTConfig::saveAccelSettings( //read display windows
		CBTConfig::allWindows,
		m_general.actionCollection
	);
	CBTConfig::saveAccelSettings( //bible
		CBTConfig::bibleWindow,
		m_bible.actionCollection
	);
	CBTConfig::saveAccelSettings( //commentary
		CBTConfig::commentaryWindow,
		m_commentary.actionCollection
	);
	CBTConfig::saveAccelSettings( //lexicon
		CBTConfig::lexiconWindow,
		m_lexicon.actionCollection
	);
	CBTConfig::saveAccelSettings( //book
		CBTConfig::bookWindow,
		m_book.actionCollection
	);
}


void CAcceleratorSettingsPage::slotKeyChooserTypeChanged(const QString& title)
{
	
	//delete all KShortcutsEditors which may not share accels, because this class checks in all instances for key conflicts
	typedef QList<KShortcutsEditor*> KeyChooserList;
	KeyChooserList list;
	list.append(m_bible.keyChooser);
	list.append(m_commentary.keyChooser);
	list.append(m_lexicon.keyChooser);
	list.append(m_book.keyChooser);

	//commit all changes in the keychoosers
	//for (KeyChooserList::iterator it(list.begin()); it != list.end(); ++it) {
	//	if (*it) { //the list may contain NULL pointers
	//		(*it)->commitChanges();
	//	}
	//}

	/* Delete all the keychoosers in the list,
	* the keychoosers are set to NULL, because they are QGuardedPtr
	*/
	//list.setAutoDelete(true);
	qDeleteAll(list);
	list.clear();

	WindowType* t = 0;
	if (title == m_application.title) { //Application wide
		t = &m_application;
	}
	else if (title == m_general.title) { // All display windows
		t = &m_general;
	}
	else if (title == m_bible.title) { // Bible windows
		t = &m_bible;
	}
	else if (title == m_commentary.title) { // Commentary windows
		t = &m_commentary;
	}
	else if (title == m_lexicon.title) { // Lexicon windows
		t = &m_lexicon;
	}
	else if (title == m_book.title) { // Book windows
		t = &m_book;
	}

	Q_ASSERT(t);
	if (!t->keyChooser) { //was deleted, create a new one
		t->keyChooser = new KShortcutsEditor(
							m_keyChooserStack
						);
		t->keyChooser->addCollection(
			t->actionCollection,
			t->title
		);
	}

	m_keyChooserStack->raiseWidget(t->keyChooser);
}
