//
// C++ Implementation: cacceleratorsettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cacceleratorsettings.h"
#include "cacceleratorsettings.moc"

#include "frontend/cbtconfig.h"

#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/clexiconreadwindow.h"
#include "frontend/displaywindow/cbookreadwindow.h"
#include "frontend/displaywindow/creadwindow.h"

#include <QWidget>
#include <QComboBox>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

#include <kactioncollection.h>



CAcceleratorSettingsPage::CAcceleratorSettingsPage(QWidget *parent)
	: QWidget(parent)
{
	qDebug("CAcceleratorSettingsPage::CAcceleratorSettingsPage");
	//TODO: widget layout may not work. Maybe it would be easier to use .ui file.
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	this->setLayout(mainLayout);
	//TODO: actionCollection must exist, but is this the right way?
	m_application.actionCollection = new KActionCollection(this);
	CBTConfig::setupAccelSettings(
		CBTConfig::application,
		m_application.actionCollection
	);
	qDebug("create layout for window type chooser");
	QHBoxLayout* layoutForWindowTypeChooser = new QHBoxLayout(this);
	mainLayout->addLayout(layoutForWindowTypeChooser);
	QLabel* label = new QLabel(tr("Choose type:"), this);
	layoutForWindowTypeChooser->addWidget(label);
	m_typeChooser = new QComboBox(this);
	layoutForWindowTypeChooser->addWidget(m_typeChooser);

	connect(
		m_typeChooser, SIGNAL(activated(const QString&)),
		SLOT(slotKeyChooserTypeChanged(const QString&))
	);	
	//too ugly! change!
	//QLabel* dummy = new QLabel( this); // empty label for stretch
	//hbox->addWidget(dummy);

	//hbox->setStretchFactor(label, 0);
	//hbox->setStretchFactor(m_typeChooser, 0);
	//hbox->setStretchFactor(dummy, 1);

	//mainLayout->setStretchFactor(hbox, 0);
	qDebug("create stack");
	m_keyChooserStack = new QStackedWidget(this);

	//mainLayout->setStretchFactor(m_keyChooserStack, 5);

	m_application.title = tr("BibleTime"); //don't set the app action collection to NULL
	m_general = WindowType(tr("All text windows"));
	m_bible = WindowType(tr("Bible windows"));
	m_commentary = WindowType(tr("Commentary windows"));
	m_lexicon = WindowType(tr("Lexicon windows"));
	m_book = WindowType(tr("Book windows"));

	m_typeChooser->addItem(m_application.title);
	m_typeChooser->addItem(m_general.title);
	m_typeChooser->addItem(m_bible.title);
	m_typeChooser->addItem(m_commentary.title);
	m_typeChooser->addItem(m_lexicon.title);
	m_typeChooser->addItem(m_book.title);

	qDebug("create shortcuteditors");
	Q_ASSERT(m_application.actionCollection);
	m_application.keyChooser = new KShortcutsEditor(
				m_application.actionCollection,
				m_keyChooserStack
			);
	qDebug("add first w");
	m_keyChooserStack->addWidget(m_application.keyChooser);

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
	qDebug("add second w");
	m_keyChooserStack->addWidget(m_general.keyChooser);

	// ----- Bible windows ------ //
	m_bible.actionCollection = new KActionCollection(this);
	CBibleReadWindow::insertKeyboardActions( m_bible.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::bibleWindow,
		m_bible.actionCollection
	);
	//m_keyChooserStack->addWidget(m_bible.keyChooser);
	
	// ----- Commentary windows ------ //
	m_commentary.actionCollection = new KActionCollection(this);
	CCommentaryReadWindow::insertKeyboardActions( m_commentary.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::commentaryWindow,
		m_commentary.actionCollection
	);
	//m_keyChooserStack->addWidget(m_commentary.keyChooser);
	
	// ----- Lexicon windows ------ //
	m_lexicon.actionCollection = new KActionCollection(this);
	CLexiconReadWindow::insertKeyboardActions(  m_lexicon.actionCollection );
	CBTConfig::setupAccelSettings(
		CBTConfig::lexiconWindow,
		m_lexicon.actionCollection
	);
	//m_keyChooserStack->addWidget(m_lexicon.keyChooser);

	// ----- Book windows ------ //
	m_book.actionCollection= new KActionCollection(this);
	CBookReadWindow::insertKeyboardActions( m_book.actionCollection);
	CBTConfig::setupAccelSettings(
		CBTConfig::bookWindow,
		m_book.actionCollection
	);
	//m_keyChooserStack->addWidget(m_book.keyChooser);
	
	mainLayout->addWidget(m_keyChooserStack);
	slotKeyChooserTypeChanged(m_application.title);

	qDebug("CAcceleratorSettingsPage::CAcceleratorSettingsPage end");
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
	qDebug("CAcceleratorSettingsPage::slotKeyChooserTypeChanged");
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

	m_keyChooserStack->setCurrentWidget(t->keyChooser);
	qDebug("CAcceleratorSettingsPage::slotKeyChooserTypeChanged end");
}
