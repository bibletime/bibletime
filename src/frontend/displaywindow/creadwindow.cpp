/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
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
#include "util/btassert.h"
#include "util/btconnect.h"


CReadWindow::CReadWindow(QList<CSwordModuleInfo *> modules, CMDIArea * parent)
    : CDisplayWindow(modules, parent)
    , m_readDisplayWidget(nullptr)
{}

void CReadWindow::setDisplayWidget(CDisplay * newDisplay) {
    // Lets be orwellianly paranoid here:
    BT_ASSERT(dynamic_cast<CReadDisplay *>(newDisplay));

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
    BT_CONNECT(m_readDisplayWidget->connectionsProxy(),
               SIGNAL(referenceClicked(QString const &, QString const &)),
               this,
               SLOT(lookupModKey(QString const &, QString const &)));

    BT_CONNECT(m_readDisplayWidget->connectionsProxy(),
               SIGNAL(referenceDropped(QString const &)),
               this,
               SLOT(lookupKey(QString const &)));

    if (BtHtmlReadDisplay * const v =
            dynamic_cast<BtHtmlReadDisplay *>(m_readDisplayWidget))
        BT_CONNECT(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()));
}

void CReadWindow::lookupSwordKey(CSwordKey * newKey) {
    BT_ASSERT(newKey);

    if (!isReady() || !newKey || modules().empty() || !modules().first())
        return;

    if (key() != newKey)
        key()->setKey(newKey->key());

    /// \todo next-TODO how about options?
    Rendering::CEntryDisplay * const display = modules().first()->getDisplay();
    BT_ASSERT(display);
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
    Search::CSearchDialog::openDialog(modules(), searchText, nullptr);
}
