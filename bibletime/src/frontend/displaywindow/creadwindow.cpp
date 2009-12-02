/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/creadwindow.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QResizeEvent>
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/rendering/centrydisplay.h"
#include "frontend/cexportmanager.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/bthtmlreaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/searchdialog/csearchdialog.h"


using namespace Profile;

typedef BtHtmlReadDisplay HTMLREADDISPLAY;

CReadWindow::CReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent)
        : CDisplayWindow(modules, parent),
        m_displayWidget(0) {
    qDebug() << "CReadWindow::CReadWindow";
    //   installEventFilter(this);
}

CReadWindow::~CReadWindow() {
    //  qWarning("destructor of CReadWindow");
}

/** Sets the display widget of this display window. */
void CReadWindow::setDisplayWidget( CDisplay* newDisplay ) {
    // Lets be orwellianly paranoid here:
    Q_ASSERT(dynamic_cast<CReadDisplay*>(newDisplay) != 0);

    CDisplayWindow::setDisplayWidget(newDisplay);
    if (m_displayWidget) {
        disconnect(m_displayWidget->connectionsProxy(), SIGNAL(referenceClicked(const QString&, const QString&)),
                   this, SLOT(lookupModKey(const QString&, const QString&)));
        disconnect(m_displayWidget->connectionsProxy(), SIGNAL(referenceDropped(const QString&)),
                   this, SLOT(lookupKey(const QString&)));

        HTMLREADDISPLAY* v = dynamic_cast<HTMLREADDISPLAY*>(m_displayWidget);
        if (v) {
            QObject::disconnect(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()) );
        }

    }

    m_displayWidget = static_cast<CReadDisplay*>(newDisplay);
    connect(
        m_displayWidget->connectionsProxy(),
        SIGNAL(referenceClicked(const QString&, const QString&)),
        this,
        SLOT(lookupModKey(const QString&, const QString&))
    );

    connect(
        m_displayWidget->connectionsProxy(),
        SIGNAL(referenceDropped(const QString&)),
        this,
        SLOT(lookupKey(const QString&))
    );
    HTMLREADDISPLAY* v = dynamic_cast<HTMLREADDISPLAY*>(m_displayWidget);
    if (v) {
        QObject::connect(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()) );
    }
}

/** Lookup the given entry. */
void CReadWindow::lookupSwordKey( CSwordKey* newKey ) {
    qDebug() << "CReadWindow::lookup newKey" << newKey->key();
    Q_ASSERT(newKey);

    using namespace Rendering;

//	Q_ASSERT(isReady() && newKey && modules().first());
    if (!isReady() || !newKey || modules().empty() || !modules().first()) {
        return;
    }

    if (key() != newKey) {
        key()->key(newKey->key());
    }

    /// \todo next-TODO how about options?
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

    setWindowTitle(windowCaption());

    // moving to anchor happens in slotMoveToAnchor which catches the completed() signal from KHTMLPart

    qDebug() << "CReadWindow::lookup end, key is :" << newKey->key();
}

void CReadWindow::slotMoveToAnchor() {
    qDebug() << "CReadWindow::slotMoveToAnchor";
    ((CReadDisplay*)displayWidget())->moveToAnchor( Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key()) );
}

/** Store the settings of this window in the given CProfileWindow object. */
void CReadWindow::storeProfileSettings(CProfileWindow * const settings) {
    QRect rect;
    rect.setX(parentWidget()->x());
    rect.setY(parentWidget()->y());
    rect.setWidth(parentWidget()->width());
    rect.setHeight(parentWidget()->height());
    settings->setGeometry(rect);

    // settings->setScrollbarPositions( m_htmlWidget->view()->horizontalScrollBar()->value(), m_htmlWidget->view()->verticalScrollBar()->value() );
    settings->setType(modules().first()->type());
    settings->setMaximized(isMaximized() || parentWidget()->isMaximized());
    settings->setFocus( (this == dynamic_cast<CReadWindow*>(mdi()->activeSubWindow()) ) ); //set property to true if this window is the active one.

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

    QList<CSwordModuleInfo*> allMods = modules();
    QList<CSwordModuleInfo*>::iterator end_it = allMods.end();
    for (QList<CSwordModuleInfo*>::iterator it(allMods.begin()); it != end_it; ++it) {
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
        parentWidget()->resize(rect.width(), rect.height());
        parentWidget()->move(rect.x(), rect.y());
    }

    setUpdatesEnabled(true);
}

void CReadWindow::insertKeyboardActions( BtActionCollection* const ) {}

/** No descriptions */
void CReadWindow::copyDisplayedText() {
    CExportManager mgr(QString::null);
    mgr.copyKey(key(), CExportManager::Text, true);
}


/*!
    \fn CReadWindow::resizeEvent(QResizeEvent* e)
 */
void CReadWindow::resizeEvent(QResizeEvent* /*e*/) {
    if (displayWidget()) {
        ((CReadDisplay*)displayWidget())->moveToAnchor(Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key()));
    }
}

void CReadWindow::openSearchStrongsDialog() {
//	qWarning("looking for lemma %s",  displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma].latin1() );
    QString searchText = QString::null;

    if (displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null) {
        Q_FOREACH(QString strongNumber, displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma].split("|")) {
            searchText.append("strong:").append( strongNumber ).append(" ");
        }
    }

    Search::CSearchDialog::openDialog( modules(), searchText, 0 );
}
