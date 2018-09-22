/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btmodelviewreaddisplay.h"

#include <memory>
#include <QDebug>
#include <QDrag>
#include <QHBoxLayout>
#include <QMenu>
#include <QString>
#include <QTimer>
#include "backend/keys/cswordkey.h"
#include "backend/managers/referencemanager.h"
#include "frontend/bibletime.h"
#include "frontend/BtMimeData.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/modelview/btqmlscrollview.h"
#include "frontend/display/modelview/btqmlinterface.h"
#include "frontend/display/modelview/btquickwidget.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/directory.h"

using namespace InfoDisplay;

BtModelViewReadDisplay::BtModelViewReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
    : QWidget(parentWidget), CReadDisplay(readWindow), m_magTimerId(0), m_widget(nullptr)

{
    setObjectName("BtModelViewReadDisplay");
    QHBoxLayout* layout = new QHBoxLayout(this);
    setLayout(layout);
    m_widget = new BtQmlScrollView(this, this);
    layout->addWidget(m_widget);
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    BT_CONNECT(m_widget->qmlInterface(), SIGNAL(updateReference(const QString&)),
               this, SLOT(slotUpdateReference(const QString&)));
    BT_CONNECT(m_widget->qmlInterface(), SIGNAL(dragOccuring(const QString&,const QString&)),
               this, SLOT(slotDragOccuring(const QString&, const QString&)));
    BT_CONNECT(m_widget, SIGNAL(referenceDropped(const QString&)),
               this, SLOT(slotReferenceDropped(const QString&)));
}

BtModelViewReadDisplay::~BtModelViewReadDisplay() {
}


const QString BtModelViewReadDisplay::text( const CDisplay::TextType format,
                                            const CDisplay::TextPart part) {
    switch (part) {
        case Document: {
            if (format == HTMLText) {
                return getCurrentSource();
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

                    return QString(key->strippedText()).append("\n(")
                           .append(key->key())
                           .append(", ")
                           .append(key->module()->name())
                           .append(")");
                }
            }
        }

        case SelectedText: {
//            if (!hasSelection()) {
//                return QString::null;
//            }
//            else if (format == HTMLText) {
//                //    \todo It does not appear this is ever called
//            }
//            else { //plain text requested
//                return selectedText();
//            }
        }

        case AnchorOnly: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            return keyName;
        }

        case AnchorTextOnly: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                key->setKey(keyName);

                return key->strippedText();
            }
            return QString::null;
        }

        case AnchorWithText: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                key->setKey(keyName);

                FilterOptions filterOptions;
                CSwordBackend::instance()->setFilterOptions(filterOptions);

                return QString(key->strippedText()).append("\n(")
                       .append(key->key())
                       .append(", ")
                       .append(key->module()->name())
                       .append(")");
                /*    ("%1\n(%2, %3)")
                    .arg()
                    .arg(key->key())
                    .arg(key->module()->name());*/
            }
            return QString::null;
        }
        default:
            return QString::null;
    }
    return QString::null;

}

// Puts html text and javascript into BtWebEngineView
void BtModelViewReadDisplay::setText( const QString& /*newText*/ ) {

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

void BtModelViewReadDisplay::setModules(const QStringList &modules) {
    m_widget->qmlInterface()->setModules(modules);
}

void BtModelViewReadDisplay::scrollToKey(CSwordKey * key) {
    m_widget->scrollToSwordKey(key);
}

void BtModelViewReadDisplay::setFilterOptions(FilterOptions filterOptions) {
    m_widget->setFilterOptions(filterOptions);
}

void BtModelViewReadDisplay::settingsChanged() {
    m_widget->settingsChanged();
}

void BtModelViewReadDisplay::pageDown() {
    m_widget->pageDown();
}

void BtModelViewReadDisplay::pageUp() {
    m_widget->pageUp();
}

// See if any text is selected
bool BtModelViewReadDisplay::hasSelection() const {
    return false;
}

void BtModelViewReadDisplay::highlightText(const QString& text, bool caseSensitive) {
    m_widget->qmlInterface()->setHighlightWords(text, caseSensitive);
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

void BtModelViewReadDisplay::slotDelayedMoveToAnchor() {
}

// Scroll the BtWebEngineView to the correct location specified by anchor
void BtModelViewReadDisplay::slotGoToAnchor(const QString& /*anchor*/) {
}

void BtModelViewReadDisplay::slotUpdateReference(const QString& reference) {
    CDisplayWindow* window = parentWindow();
    auto key = window->key();
    key->setKey(reference);
    window->keyChooser()->updateKey(key);
    QString caption = window->windowCaption();
    window->setWindowTitle(window->windowCaption());
}

void BtModelViewReadDisplay::slotDragOccuring(const QString& moduleName, const QString& keyName) {
    QDrag* drag = new QDrag(this);
    BTMimeData* mimedata = new BTMimeData(moduleName, keyName, QString::null);
    drag->setMimeData(mimedata);
    //add real Bible text from module/key
    if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
        std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
        key->setKey(keyName);
        mimedata->setText(key->strippedText()); // This works across applications!
    }
    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void BtModelViewReadDisplay::slotReferenceDropped(const QString& reference) {  // TODO - Fix me
    CDisplayWindow* window = parentWindow();
    auto key = window->key();
    key->setKey(reference);
    window->lookupKey(reference);
}

// Save the Lemma (Strongs number) attribute
void BtModelViewReadDisplay::setLemma(const QString& lemma) {
    m_nodeInfo = lemma;
}

// Open the Find text dialog
void BtModelViewReadDisplay::openFindTextDialog() {
    BibleTime* bibleTime = parentWindow()->mdi()->bibleTimeWindow();
    bibleTime->openFindWidget();
}
