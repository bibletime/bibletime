/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/creadwindow.h"

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
#include "frontend/searchdialog/csearchdialog.h"


CReadWindow::CReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent)
        : CDisplayWindow(modules, parent),
        m_readDisplayWidget(0) {
    //   installEventFilter(this);
}

/** Sets the display widget of this display window. */
void CReadWindow::setDisplayWidget( CDisplay* newDisplay ) {
    // Lets be orwellianly paranoid here:
    Q_ASSERT(dynamic_cast<CReadDisplay*>(newDisplay) != 0);

    CDisplayWindow::setDisplayWidget(newDisplay);
    if (m_readDisplayWidget) {
        disconnect(m_readDisplayWidget->connectionsProxy(), SIGNAL(referenceClicked(const QString&, const QString&)),
                   this, SLOT(lookupModKey(const QString&, const QString&)));
        disconnect(m_readDisplayWidget->connectionsProxy(), SIGNAL(referenceDropped(const QString&)),
                   this, SLOT(lookupKey(const QString&)));

        BtHtmlReadDisplay* v = dynamic_cast<BtHtmlReadDisplay*>(m_readDisplayWidget);
        if (v) {
            QObject::disconnect(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()) );
        }

    }

    m_readDisplayWidget = static_cast<CReadDisplay*>(newDisplay);
    connect(
        m_readDisplayWidget->connectionsProxy(),
        SIGNAL(referenceClicked(const QString&, const QString&)),
        this,
        SLOT(lookupModKey(const QString&, const QString&))
    );

    connect(
        m_readDisplayWidget->connectionsProxy(),
        SIGNAL(referenceDropped(const QString&)),
        this,
        SLOT(lookupKey(const QString&))
    );
    BtHtmlReadDisplay* v = dynamic_cast<BtHtmlReadDisplay*>(m_readDisplayWidget);
    if (v) {
        QObject::connect(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()) );
    }
}

/** Lookup the given entry. */
void CReadWindow::lookupSwordKey( CSwordKey* newKey ) {
    Q_ASSERT(newKey);

    using namespace Rendering;

//    Q_ASSERT(isReady() && newKey && modules().first());
    if (!isReady() || !newKey || modules().empty() || !modules().first()) {
        return;
    }

    if (key() != newKey) {
        key()->setKey(newKey->key());
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
}

void CReadWindow::slotMoveToAnchor() {
    ((CReadDisplay*)displayWidget())->moveToAnchor( Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key()) );
}

void CReadWindow::insertKeyboardActions( BtActionCollection* const ) {}

/** No descriptions */
void CReadWindow::copyDisplayedText() {
    CExportManager().copyKey(key(), CExportManager::Text, true);
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
    QString searchText;
    Q_FOREACH (const QString &strongNumber,
               displayWidget()->getCurrentNodeInfo().split('|', QString::SkipEmptyParts))
    {
        searchText.append("strong:").append(strongNumber).append(' ');
    }

    Search::CSearchDialog::openDialog( modules(), searchText, 0 );
}
