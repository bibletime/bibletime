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

#include "btmodelviewreaddisplay.h"

#include <memory>
#include <QDebug>
#include <QDrag>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QString>
#include <QTimer>
#include <QToolBar>
#include "../../backend/keys/cswordkey.h"
#include "../../backend/managers/referencemanager.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/directory.h"
#include "../bibletime.h"
#include "../BtMimeData.h"
#include "../cinfodisplay.h"
#include "../cmdiarea.h"
#include "../displaywindow/cdisplaywindow.h"
#include "../keychooser/ckeychooser.h"
#include "modelview/btqmlscrollview.h"
#include "modelview/btqmlinterface.h"
#include "modelview/btquickwidget.h"


using namespace InfoDisplay;

BtModelViewReadDisplay::BtModelViewReadDisplay(CDisplayWindow * displayWindow,
                                               QWidget * parentWidget)
    : QWidget(parentWidget)
    , CReadDisplay(displayWindow)
    , m_magTimerId(0)
    , m_widget(nullptr)
{
    setObjectName("BtModelViewReadDisplay");
    QHBoxLayout* layout = new QHBoxLayout(this);
    setLayout(layout);
    m_widget = new BtQmlScrollView(this, this);
    layout->addWidget(m_widget);
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    BT_CONNECT(m_widget->qmlInterface(), &BtQmlInterface::updateReference,
               [this](QString const & reference) {
                   auto & window = *parentWindow();
                   auto & key = *window.key();
                   key.setKey(reference);
                   window.keyChooser()->updateKey(&key);
                   window.setWindowTitle(window.windowCaption());
               });
    BT_CONNECT(m_widget->qmlInterface(), &BtQmlInterface::dragOccuring,
               [this](const QString& moduleName, const QString& keyName) {
                   auto & drag = *new QDrag(this);
                   auto & mimedata =
                           *new BTMimeData(moduleName, keyName, QString());
                   drag.setMimeData(&mimedata);
                   //add real Bible text from module/key
                   if (auto * const module =
                        CSwordBackend::instance()->findModuleByName(moduleName))
                   {
                       drag.setPixmap(
                               module->moduleIcon().pixmap(
                                   parentWindow()->mainToolBar()->iconSize()));
                       std::unique_ptr<CSwordKey> key(
                                   CSwordKey::createInstance(module));
                       key->setKey(keyName);
                       // This works across applications:
                       mimedata.setText(key->strippedText());
                   }
                   drag.exec(Qt::CopyAction, Qt::CopyAction);
               });
    BT_CONNECT(m_widget, &BtQmlScrollView::referenceDropped,
               [this](QString const & reference) { /// \todo Fix me
                   auto & window = *parentWindow();
                   auto key(window.key());
                   key->setKey(reference);
                   window.lookupKey(reference);
               });
}

BtModelViewReadDisplay::~BtModelViewReadDisplay() {
}

void BtModelViewReadDisplay::reloadModules() {
    qmlInterface()->textModel()->reloadModules();
}

const QString BtModelViewReadDisplay::text( const CDisplay::TextType format,
                                            const CDisplay::TextPart part) {
    QString text;
    switch (part) {
    case Document: {
        if (format == HTMLText) {
            text = getCurrentSource();
        }
        else {
            CDisplayWindow* window = parentWindow();
            CSwordKey* const key = window->key();
            const CSwordModuleInfo *module = key->module();
            //This is never used for Bibles, so it is not implemented for
            //them.  If it should be, see CReadDisplay::print() for example
            //code.
            BT_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
                      module->type() == CSwordModuleInfo::Commentary ||
                      module->type() == CSwordModuleInfo::GenericBook);
            if (module->type() == CSwordModuleInfo::Lexicon ||
                    module->type() == CSwordModuleInfo::Commentary ||
                    module->type() == CSwordModuleInfo::GenericBook) {

                FilterOptions filterOptions;
                CSwordBackend::instance()->setFilterOptions(filterOptions);

                text = QString(key->strippedText()).append("\n(")
                        .append(key->key())
                        .append(", ")
                        .append(key->module()->name())
                        .append(")");
            }
        }
        break;
    }

    case AnchorOnly: {
        if (auto const decodedLink =
                    ReferenceManager::decodeHyperlink(activeAnchor()))
            return decodedLink->key;
        return {};
    }

    case AnchorTextOnly: {
        auto const decodedLink(
                    ReferenceManager::decodeHyperlink(activeAnchor()));
        if (decodedLink && decodedLink->module) {
            std::unique_ptr<CSwordKey> key(
                        CSwordKey::createInstance(decodedLink->module));
            key->setKey(decodedLink->key);
            return key->strippedText();
        }
        return {};
    }

    case AnchorWithText: {
        auto const decodedLink(
                    ReferenceManager::decodeHyperlink(activeAnchor()));
        if (decodedLink && decodedLink->module) {
            std::unique_ptr<CSwordKey> key(
                        CSwordKey::createInstance(decodedLink->module));
            key->setKey(decodedLink->key);

            FilterOptions filterOptions;
            CSwordBackend::instance()->setFilterOptions(filterOptions);

            return QString(key->strippedText()).append("\n(")
                    .append(key->key())
                    .append(", ")
                    .append(key->module()->name())
                    .append(")");
        }
        return {};
    }
    default:
        break;
    }
    return QString();

}

// Puts html text the view
void BtModelViewReadDisplay::setText( const QString& /*newText*/ ) {
}

void BtModelViewReadDisplay::setDisplayFocus() {
    m_widget->quickWidget()->setFocus();
}

void BtModelViewReadDisplay::setDisplayOptions(const DisplayOptions &displayOptions) {
    m_widget->qmlInterface()->textModel()->setDisplayOptions(displayOptions);
}

void BtModelViewReadDisplay::contextMenu(QContextMenuEvent* event) {
    QString activeLink = m_widget->qmlInterface()->getActiveLink();
    QString reference = m_widget->qmlInterface()->getBibleUrlFromLink(activeLink);
    setActiveAnchor(reference);
    QString lemma = m_widget->qmlInterface()->getLemmaFromLink(activeLink);
    setLemma(lemma);

    if (QMenu* popup = installedPopup()) {
        popup->exec(event->globalPos());
    }
}

QString BtModelViewReadDisplay::getCurrentSource( ) {
    return this->currentSource;
}

BtQmlInterface * BtModelViewReadDisplay::qmlInterface() const {
    return m_widget->qmlInterface();
}

void BtModelViewReadDisplay::setModules(const QStringList &modules) {
    m_widget->qmlInterface()->setModules(modules);
}

void BtModelViewReadDisplay::scrollToKey(CSwordKey * key) {
    m_widget->scrollToSwordKey(key);
}

void BtModelViewReadDisplay::scroll(int value) {
    m_widget->quickWidget()->scroll(value);
}

void BtModelViewReadDisplay::setFilterOptions(FilterOptions filterOptions) {
    m_widget->setFilterOptions(filterOptions);
}

void BtModelViewReadDisplay::settingsChanged() {
    m_widget->settingsChanged();
}

void BtModelViewReadDisplay::updateReferenceText() {
    m_widget->quickWidget()->updateReferenceText();
}

void BtModelViewReadDisplay::pageDown() {
    m_widget->pageDown();
}

void BtModelViewReadDisplay::pageUp() {
    m_widget->pageUp();
}

void BtModelViewReadDisplay::highlightText(const QString& text, bool caseSensitive) {
    m_widget->qmlInterface()->setHighlightWords(text, caseSensitive);
}

void BtModelViewReadDisplay::findText(const QString& text,
                                      bool caseSensitive, bool backward) {
    m_widget->qmlInterface()->findText(text, caseSensitive, backward);
}

// Reimplementation
// Returns the BtModelViewReadDisplayView object
QWidget* BtModelViewReadDisplay::view() {
    return m_widget;
}

// Select all text in the viewer
void BtModelViewReadDisplay::selectAll() {
}

// Scroll to the correct location as specified by the anchor
void BtModelViewReadDisplay::moveToAnchor( const QString& /*anchor*/ ) {
}

// Scroll the view to the correct location specified by anchor
void BtModelViewReadDisplay::slotGoToAnchor(const QString& /*anchor*/) {
}

// Save the Lemma (Strongs number) attribute
void BtModelViewReadDisplay::setLemma(const QString& lemma) {
    m_nodeInfo = lemma;
}

// Open the Find text dialog
void BtModelViewReadDisplay::openFindTextDialog()
{ parentWindow()->btMainWindow()->openFindWidget(); }
