/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cwritewindow.h"
#include "cwritewindow.moc"

#include "backend/drivers/cswordmoduleinfo.h"

#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/display/cwritedisplay.h"

#include <QMessageBox>



using namespace Profile;

CWriteWindow::CWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent)
: CDisplayWindow(modules, parent), m_writeDisplay(0) {}

CWriteWindow::~CWriteWindow() {}


void CWriteWindow::insertKeyboardActions( KActionCollection* const ) {}

void CWriteWindow::initConnections() {
	Q_ASSERT(keyChooser());
	QObject::connect(keyChooser(), SIGNAL(beforeKeyChange(const QString&)), this, SLOT(beforeKeyChange(const QString&)));
}

void CWriteWindow::initActions() {}


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
			vk->setLocale(oldLang.toLatin1());
		}
	}

	QStringList mods;
	ListCSwordModuleInfo allMods = modules();
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
		parentWidget()->resize(rect.width(), rect.height());
		parentWidget()->move(rect.x(), rect.y());
		//setGeometry( settings->geometry() );
	}
	// displayWidget()->view()->horizontalScrollBar()->setValue( settings->scrollbarPositions().horizontal );
	// m_htmlWidget->view()->verticalScrollBar()->setValue( settings->scrollbarPositions().vertical );

	setUpdatesEnabled(true);
};

void CWriteWindow::setDisplayWidget( CWriteDisplay* display ) {
	CDisplayWindow::setDisplayWidget(display);
	m_writeDisplay = display;
}

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

CWriteDisplay* const CWriteWindow::displayWidget() {
	return m_writeDisplay;
}

bool CWriteWindow::queryClose() {
	//save the text if it has changed
	if (m_writeDisplay->isModified()) {
		switch (QMessageBox::question( this, tr("Confirmation"), tr("Save text before closing?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes) ) {
			case QMessageBox::Yes: //save and close
				saveCurrentText();
				m_writeDisplay->setModified( false );
				return true;
			case QMessageBox::No: //don't save and close
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
	if (!isReady()) return;

	//If the text changed and we'd do a lookup ask the user if the text should be saved
	if (modules().first() && displayWidget()->isModified()) {
		
		switch (QMessageBox::question( this, tr("Confirmation"), tr("Save changed text?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) ) {
			case QMessageBox::Yes: { //save the changes
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
