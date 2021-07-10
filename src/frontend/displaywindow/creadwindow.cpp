/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "creadwindow.h"
#include "../bibletime.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMdiSubWindow>
#include <QResizeEvent>
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../backend/rendering/cdisplayrendering.h"
#include "../../backend/rendering/centrydisplay.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../bibletime.h"
#include "../btcopybyreferencesdialog.h"
#include "../cexportmanager.h"
#include "../cmdiarea.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../display/modelview/btqmlinterface.h"
#include "../display/modelview/btquickwidget.h"
#include "../searchdialog/csearchdialog.h"
#include "btactioncollection.h"

CReadWindow::CReadWindow(QList<CSwordModuleInfo *> modules, CMDIArea * parent)
    : CDisplayWindow(modules, parent)
    , m_readDisplayWidget(nullptr)
{}

void CReadWindow::setDisplayWidget(CDisplay * newDisplay) {
    // Lets be orwellianly paranoid here:
    BT_ASSERT(dynamic_cast<CReadDisplay *>(newDisplay));

    setObjectName("CReadWindow");
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

        if (BtModelViewReadDisplay * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
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

    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        BT_CONNECT(v, SIGNAL(completed()), this, SLOT(slotMoveToAnchor()));

    BibleTime* bt = btMainWindow();
    BT_CONNECT(bt, SIGNAL(colorThemeChanged()),
               this, SLOT(colorThemeChangedSlot()));
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

    displayWidget()->setDisplayOptions(displayOptions());
    displayWidget()->setFilterOptions(filterOptions());
    displayWidget()->scrollToKey(newKey);
    BibleTime::instance()->autoScrollStop();

    setWindowTitle(windowCaption());
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
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList nodeInfo = displayWidget()->getCurrentNodeInfo().split('|', QString::SkipEmptyParts);
#else
    QStringList nodeInfo = displayWidget()->getCurrentNodeInfo().split('|', Qt::SkipEmptyParts);
#endif
    Q_FOREACH(QString const& strongNumber, nodeInfo)
        searchText.append("strong:").append(strongNumber).append(' ');
    Search::CSearchDialog::openDialog(modules(), searchText, nullptr);
}

void CReadWindow::pageDown() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        v->pageDown();
}

void CReadWindow::pageUp() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        v->pageUp();
}

void CReadWindow::copySelectedText() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        QString text = v->qmlInterface()->getSelectedText();
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(text);
    }
}

void CReadWindow::colorThemeChangedSlot() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        v->qmlInterface()->changeColorTheme();
    }
}

void CReadWindow::copyByReferences() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        auto model = v->qmlInterface()->textModel();
        BtCopyByReferencesDialog  dlg(modules(), history(), key(), model, this);
        int rtn = dlg.exec();
        if (rtn == QDialog::Rejected)
            return;

        int column = dlg.getColumn();
        auto m = modules().at(column);

        if (m->type() == CSwordModuleInfo::Bible ||
                m->type() == CSwordModuleInfo::Commentary) {
            v->qmlInterface()->copyVerseRange(dlg.getReference1(), dlg.getReference2(), m);
        } else {
            v->qmlInterface()->copyRange(dlg.getIndex1(), dlg.getIndex2());
        }
    }
}

bool CReadWindow::hasSelectedText() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        bool selected = v->qmlInterface()->hasSelectedText();
        return selected;
    }
    return false;
}

int CReadWindow::getSelectedColumn() const {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        return  v->quickWidget()->getSelectedColumn();
    }
    return 0;
}

int CReadWindow::getFirstSelectedIndex() const {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        return  v->quickWidget()->getFirstSelectedIndex();
    }
    return 0;
}

int CReadWindow::getLastSelectedIndex() const {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        return  v->quickWidget()->getLastSelectedIndex();
    }
    return 0;
}

CSwordKey* CReadWindow::getMouseClickedKey() const {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        return  v->quickWidget()->getMouseClickedKey();
    }
    return 0;
}
