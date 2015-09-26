/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
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


CReadWindow::CReadWindow(QList<CSwordModuleInfo *> modules, CMDIArea * parent)
    : CDisplayWindow(modules, parent)
    , m_readDisplayWidget(NULL)
{}

void CReadWindow::setDisplayWidget(CDisplay * newDisplay) {
    // Lets be orwellianly paranoid here:
    Q_ASSERT(dynamic_cast<CReadDisplay *>(newDisplay));

    CDisplayWindow::setDisplayWidget(newDisplay);
    if (m_readDisplayWidget) {
        disconnect(m_readDisplayWidget->connectionsProxy(),
                   SIGNAL(referenceClicked(QString const &, QString const &)),
                   this,
                   SLOT(lookupModKey(QString const &, QString const &)));
        disconnect(m_readDisplayWidget->connectionsProxy(),
                   SIGNAL(referenceDropped(QString const &)),
                   this,
                   SLOT(lookupKey(QString const &)));

        if (BtHtmlReadDisplay * const v =
                dynamic_cast<BtHtmlReadDisplay *>(m_readDisplayWidget))
            QObject::disconnect(v,    SIGNAL(completed()),
                                this, SLOT(slotMoveToAnchor()));
    }

    m_readDisplayWidget = static_cast<CReadDisplay *>(newDisplay);
    connect(m_readDisplayWidget->connectionsProxy(),
            SIGNAL(referenceClicked(QString const &, QString const &)),
            this,
            SLOT(lookupModKey(QString const &, QString const &)));

    connect(m_readDisplayWidget->connectionsProxy(),
            SIGNAL(referenceDropped(QString const &)),
            this,
            SLOT(lookupKey(QString const &)));

    if (BtHtmlReadDisplay * const v =
            dynamic_cast<BtHtmlReadDisplay *>(m_readDisplayWidget))
        QObject::connect(v,    SIGNAL(completed()),
                         this, SLOT(slotMoveToAnchor()));
}

void CReadWindow::lookupSwordKey(CSwordKey * newKey) {
    Q_ASSERT(newKey);

    if (!isReady() || !newKey || modules().empty() || !modules().first())
        return;

    if (key() != newKey)
        key()->setKey(newKey->key());

    /// \todo next-TODO how about options?
    Rendering::CEntryDisplay * const display = modules().first()->getDisplay();
    Q_ASSERT(display);
    displayWidget()->setText(display->text(modules(),
                                           newKey->key(),
                                           displayOptions(),
                                           filterOptions()));

    setWindowTitle(windowCaption());
    /* Moving to anchor happens in slotMoveToAnchor which catches the
       completed() signal from KHTMLPart. */
}

void CReadWindow::slotMoveToAnchor() {
    static_cast<CReadDisplay *>(displayWidget())->moveToAnchor(
            Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key()));
}

void CReadWindow::insertKeyboardActions(BtActionCollection * const)
{}

void CReadWindow::copyDisplayedText()
{ CExportManager().copyKey(key(), CExportManager::Text, true); }

void CReadWindow::resizeEvent(QResizeEvent * e) {
    Q_UNUSED(e)
    if (displayWidget())
        static_cast<CReadDisplay *>(displayWidget())->moveToAnchor(
                Rendering::CDisplayRendering::keyToHTMLAnchor(key()->key()));
}

void CReadWindow::openSearchStrongsDialog() {
    QString searchText;
    Q_FOREACH(QString const & strongNumber,
              displayWidget()->getCurrentNodeInfo().split(
                    '|',
                    QString::SkipEmptyParts))
        searchText.append("strong:").append(strongNumber).append(' ');
    Search::CSearchDialog::openDialog(modules(), searchText, NULL);
}
