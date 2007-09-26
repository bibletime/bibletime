/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "creadwindow.h"
#include "creadwindow.moc"

#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/keys/cswordkey.h"

#include "frontend/cexportmanager.h"
#include "frontend/cmdiarea.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/searchdialog/csearchdialog.h"


#include <kactioncollection.h>

#include <QResizeEvent>

using namespace Profile;

CReadWindow::CReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent)
	: CDisplayWindow(modules,parent),
	m_displayWidget(0)
{
	qDebug("CReadWindow::CReadWindow");
	//   installEventFilter(this);
}

CReadWindow::~CReadWindow() {
	//  qWarning("destructor of CReadWindow");
}

/** Returns the display widget of this window. */
CReadDisplay* const CReadWindow::displayWidget() {
	return m_displayWidget;
}

/** Sets the display widget of this display window. */
void CReadWindow::setDisplayWidget( CReadDisplay* newDisplay ) {
	CDisplayWindow::setDisplayWidget(newDisplay);
	if (m_displayWidget) {
		disconnect(m_displayWidget->connectionsProxy(), SIGNAL(referenceClicked(const QString&, const QString&)), this, SLOT(lookup(const QString&, const QString&)));

		disconnect(m_displayWidget->connectionsProxy(), SIGNAL(referenceDropped(const QString&)), this, SLOT(lookup(const QString&)));
	}

	m_displayWidget = newDisplay;
	connect(
		m_displayWidget->connectionsProxy(),
		SIGNAL(referenceClicked(const QString&, const QString&)),
		this,
		SLOT(lookup(const QString&, const QString&))
	);

	connect(
		m_displayWidget->connectionsProxy(),
		SIGNAL(referenceDropped(const QString&)),
		this,
		SLOT(lookup(const QString&))
	);
}

/** Lookup the given entry. */
void CReadWindow::lookup( CSwordKey* newKey ) {
	qDebug("CReadWindow::lookup");
	Q_ASSERT(newKey);

	using namespace Rendering;

	Q_ASSERT(isReady() && newKey && modules().first());
	if (!isReady() || !newKey || !modules().first()) {
		return;
	}

	//next-TODO: how about options?
	Q_ASSERT(modules().first()->getDisplay());
	CEntryDisplay* display = modules().first()->getDisplay();
	if (display) { //do we have a display object?
		displayWidget()->setText(
			display->text(
				modules(),
				newKey->key(),
				displayOptions(),
				filterOptions()
			)
		);
	}

	if (key() != newKey) {
		key()->key(newKey->key());
	}

	setCaption( windowCaption() );

	//  qDebug("[CReadWindow::lookup] Moving to anchor %s", CDisplayRendering::keyToHTMLAnchor(key()->key()).latin1());

	displayWidget()->moveToAnchor( CDisplayRendering::keyToHTMLAnchor(key()->key()) );
	qDebug("CReadWindow::lookup end");
}

/** Store the settings of this window in the given CProfileWindow object. */
void CReadWindow::storeProfileSettings(CProfileWindow * const settings) {
	QRect rect;
	rect.setX(parentWidget()->x());
	rect.setY(parentWidget()->y());
	rect.setWidth(width());
	rect.setHeight(height());
	settings->setGeometry(rect);

	// settings->setScrollbarPositions( m_htmlWidget->view()->horizontalScrollBar()->value(), m_htmlWidget->view()->verticalScrollBar()->value() );
	settings->setType(modules().first()->type());
	settings->setMaximized(isMaximized() || parentWidget()->isMaximized());
	settings->setFocus( (this == dynamic_cast<CReadWindow*>(mdi()->activeWindow()) ) ); //set property to true if this window is the active one.

	if (key()) {
		sword::VerseKey* vk = dynamic_cast<sword::VerseKey*>(key());
		QString oldLang;
		if (vk) {
			oldLang = QString(vk->getLocale());
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
}

void CReadWindow::applyProfileSettings(CProfileWindow * const settings) {
	//  parentWidget()->setUpdatesEnabled(false);
	setUpdatesEnabled(false);

	if (settings->maximized()) { //maximize this window
		// Use parentWidget() to call showMaximized. Otherwise we'd get lot's of X11 errors
		parentWidget()->showMaximized();
	}
	else {
		const QRect rect = settings->geometry();
		resize(rect.width(), rect.height());
		parentWidget()->move(rect.x(), rect.y());
	}

	setUpdatesEnabled(true);
}

void CReadWindow::insertKeyboardActions( KActionCollection* const ) {}

/** No descriptions */
void CReadWindow::copyDisplayedText() {
	CExportManager mgr(QString::null);
	mgr.copyKey(key(), CExportManager::Text, true);
}


/*!
    \fn CReadWindow::resizeEvent(QResizeEvent* e)
 */
void CReadWindow::resizeEvent(QResizeEvent* /*e*/) {
	displayWidget()->moveToAnchor(
		Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key())
	);
}

void CReadWindow::openSearchStrongsDialog() {
//	qWarning("looking for lemma %s",  displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma].latin1() );
	QString searchText = QString::null;
	
	if (displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null) {
		searchText.append("strong:").append(displayWidget()->getCurrentNodeInfo() [CDisplay::Lemma]);
	}
	
	Search::CSearchDialog::openDialog( modules(), searchText, 0 );
}
