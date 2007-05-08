/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cwritewindow.h"

#include "backend/cswordmoduleinfo.h"

#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cprofilewindow.h"

//KDE includes
#include <kmessagebox.h>
#include <klocale.h>

using namespace Profile;

CWriteWindow::CWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name )
: CDisplayWindow(modules, parent,name), m_writeDisplay(0) {}

CWriteWindow::~CWriteWindow() {}


// void CWriteWindow::insertKeyboardActions( KAccel* const /*a*/ ) {
//
// };
void CWriteWindow::insertKeyboardActions( KActionCollection* const ) {
	/* new KAction(
	  i18n("Next book"), CResMgr::displaywindows::bibleWindow::nextBook::accel,
	  a, "nextBook"
	 );*/
}

void CWriteWindow::initConnections() {
	//  qWarning("CWriteWindow::initConnections()");
	Q_ASSERT(keyChooser());

	connect(keyChooser(), SIGNAL(beforeKeyChange(const QString&)),
			this, SLOT(beforeKeyChange(const QString&))
		   );
};

void CWriteWindow::initActions() {}
;

void CWriteWindow::storeProfileSettings(CProfileWindow * const settings) {

	settings->setWriteWindowType( writeWindowType() );

	QRect rect;
	rect.setX(parentWidget()->x());
	rect.setY(parentWidget()->y());
	rect.setWidth(width());
	rect.setHeight(height());
	settings->setGeometry(rect);

	// settings->setScrollbarPositions( m_htmlWidget->view()->horizontalScrollBar()->value(), m_htmlWidget->view()->verticalScrollBar()->value() );
	settings->setType(modules().first()->type());
	settings->setMaximized(isMaximized() || parentWidget()->isMaximized());

	if (key()) {
		sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(key());
		QString oldLang;
		if (vk) {
			oldLang = QString::fromLatin1(vk->getLocale());
			vk->setLocale("en"); //save english locale names as default!
		}
		settings->setKey( key()->key() );
		if (vk) {
			vk->setLocale(oldLang.latin1());
		}
	}

	QStringList mods;
	ListCSwordModuleInfo allMods = modules();
	//  for (CSwordModuleInfo* m = modules().first(); m; m = modules().next()) {
	ListCSwordModuleInfo::iterator end_it = allMods.end();
	for (ListCSwordModuleInfo::iterator it(allMods.begin()); it != end_it; ++it) {
		mods.append((*it)->name());
	}
	settings->setModules(mods);
};

void CWriteWindow::applyProfileSettings(CProfileWindow * const settings) {
	setUpdatesEnabled(false);

	if (settings->maximized()) {
		parentWidget()->showMaximized();
	}
	else {
		const QRect rect = settings->geometry();
		resize(rect.width(), rect.height());
		parentWidget()->move(rect.x(), rect.y());
		//setGeometry( settings->geometry() );
	}
	// displayWidget()->view()->horizontalScrollBar()->setValue( settings->scrollbarPositions().horizontal );
	// m_htmlWidget->view()->verticalScrollBar()->setValue( settings->scrollbarPositions().vertical );

	setUpdatesEnabled(true);
};

/** Sets the write display-widget for this write display window. */
void CWriteWindow::setDisplayWidget( CWriteDisplay* display ) {
	CDisplayWindow::setDisplayWidget(display);
	m_writeDisplay = display;
}

/** Look up the given key and display the text. In our case we offer to edit the text. */
void CWriteWindow::lookup( CSwordKey* newKey ) {
	//set the raw text to the display widget
	if (!newKey)
		return;

	if (key() != newKey) { //set passage of newKey to key() if they're different, otherwise we'd get mixed up if we look up newkey which may have a different module set
		key()->key(newKey->key());
	}

	if ( modules().count() ) {
		displayWidget()->setText( key()->rawText() );
	}
	setCaption( windowCaption() );
}

/** Returns the write display widget used by this window. */
CWriteDisplay* const CWriteWindow::displayWidget() {
	return m_writeDisplay;
}

/** Saves settings */
bool CWriteWindow::queryClose() {
	//  qWarning("queryClose called!");
	//save the text if it has changed
	if (m_writeDisplay->isModified()) {
		switch (KMessageBox::warningYesNoCancel( this, i18n("Save text before closing?")) ) {
			case KMessageBox::Yes: { //save and close
				saveCurrentText();
				m_writeDisplay->setModified( false );
				return true;
			}
			case KMessageBox::No: //don't save and close
			return true;
			default: // cancel, don't close
			return false;
		}
	}
	return true;
}

void CWriteWindow::beforeKeyChange(const QString& key) {
	Q_ASSERT(displayWidget());
	Q_ASSERT(keyChooser());
	if (!isReady()) {
		return;
	}

	//If the text changed and we'd do a lookup ask the user if the text should be saved
	if (modules().first() && displayWidget()->isModified()) {
		switch (KMessageBox::warningYesNo( this, i18n("Save changed text?")) ) {
			case KMessageBox::Yes: { //save the changes
				saveCurrentText( key );
				break;
			}
			default: {// set modified to false so it won't ask again
				displayWidget()->setModified(false);
				break;
			}
		}
	}
}
