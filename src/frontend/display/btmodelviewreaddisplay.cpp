/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btmodelviewreaddisplay.h"

#include <memory>
#include <QClipboard>
#include <QDebug>
#include <QDrag>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QQuickItem>
#include <QScrollBar>
#include <QString>
#ifdef BUILD_TEXT_TO_SPEECH
#include <QTextToSpeech>
#endif
#include <QTimer>
#include <QToolBar>
#include "../../backend/keys/cswordkey.h"
#include "../../backend/drivers/cswordbiblemoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../backend/managers/referencemanager.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/tool.h"
#include "../bibletime.h"
#include "../btcopybyreferencesdialog.h"
#include "../BtMimeData.h"
#include "../cexportmanager.h"
#include "../displaywindow/cdisplaywindow.h"
#include "../keychooser/ckeychooser.h"
#include "modelview/btqmlinterface.h"
#include "modelview/btquickwidget.h"


BtModelViewReadDisplay::BtModelViewReadDisplay(
        CDisplayWindow * const displayWindow,
        QWidget * const parentWidget)
    : QWidget(parentWidget)
    , m_parentWindow(displayWindow)
    , m_quickWidget(new BtQuickWidget(this))
    , m_qmlInterface(m_quickWidget->qmlInterface())
    , m_scrollBar(new QScrollBar(this))
{
    auto * const layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickWidget->show();
    BT_CONNECT(m_quickWidget, &BtQuickWidget::referenceDropped,
               [this](QString const & reference) { /// \todo Fix me
                   auto key(m_parentWindow->key());
                   key->setKey(reference);
                   m_parentWindow->lookupKey(reference);
               });
    layout->addWidget(m_quickWidget);

    m_scrollBar->setRange(-100,100);
    m_scrollBar->setValue(0);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderMoved,
               this, &BtModelViewReadDisplay::slotSliderMoved);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderPressed,
               this, &BtModelViewReadDisplay::slotSliderPressed);
    BT_CONNECT(m_scrollBar, &QScrollBar::sliderReleased,
               this, &BtModelViewReadDisplay::slotSliderReleased);
    layout->addWidget(m_scrollBar);

    BT_CONNECT(m_qmlInterface, &BtQmlInterface::updateReference,
               [this](QString const & reference) {
                   auto * const key = m_parentWindow->key();
                   key->setKey(reference);
                   m_parentWindow->keyChooser()->updateKey(key);
                   m_parentWindow->updateWindowTitle();
               });
    BT_CONNECT(m_qmlInterface, &BtQmlInterface::dragOccuring,
               [this](const QString& moduleName, const QString& keyName) {
                   auto & drag = *new QDrag(this);
                   auto mimedata =
                           std::make_unique<BTMimeData>(
                               BTMimeData::ItemList{{moduleName, keyName, {}}});
                   //add real Bible text from module/key
                   if (auto * const module =
                        CSwordBackend::instance().findModuleByName(moduleName))
                   {
                       drag.setPixmap(
                               module->moduleIcon().pixmap(
                                   m_parentWindow->mainToolBar()->iconSize()));
                       std::unique_ptr<CSwordKey> key(module->createKey());
                       key->setKey(keyName);
                       // This works across applications:
                       mimedata->setText(key->strippedText());
                   }
                   drag.setMimeData(mimedata.release());
                   drag.exec(Qt::CopyAction, Qt::CopyAction);
               });
    BT_CONNECT(m_qmlInterface, &BtQmlInterface::setBibleReference,
               this, &BtModelViewReadDisplay::setBibleReference);
    BT_CONNECT(BibleTime::instance(), &BibleTime::colorThemeChanged,
               m_qmlInterface, &BtQmlInterface::changeColorTheme);

    setLayout(layout);
}

BtModelViewReadDisplay::~BtModelViewReadDisplay() = default;

void BtModelViewReadDisplay::setBibleReference(const QString& reference) {
    m_parentWindow->setBibleReference(reference);
}

void BtModelViewReadDisplay::slotSliderMoved(int value) {
    int speed = 25 * (1 + std::abs(value/30));
    int relative = value - m_scrollBarPosition;
    m_quickWidget->scroll(relative * speed);
    m_scrollBarPosition = value;
    BibleTime::instance()->autoScrollStop();
}

void BtModelViewReadDisplay::slotSliderPressed()  {
    m_scrollBarPosition = m_scrollBar->value();
}

void BtModelViewReadDisplay::slotSliderReleased() {
    m_scrollBar->setValue(0);
    m_quickWidget->updateReferenceText();
}

void BtModelViewReadDisplay::copyAsPlainText(TextPart const part)
{ QGuiApplication::clipboard()->setText(text(part)); }

void BtModelViewReadDisplay::contextMenuEvent(QContextMenuEvent * event) {
    auto const & activeLink = m_qmlInterface->activeLink();
    m_activeAnchor = m_qmlInterface->getBibleUrlFromLink(activeLink);
    setLemma(m_qmlInterface->getLemmaFromLink(activeLink));

    if (m_popup)
        m_popup->exec(event->globalPos());
}

void BtModelViewReadDisplay::copySelectedText()
{ QGuiApplication::clipboard()->setText(qmlInterface()->getSelectedText()); }

void BtModelViewReadDisplay::copyByReferences() {
    auto const & qml = *qmlInterface();
    BtCopyByReferencesDialog dlg(qml.textModel(),
                                 qml.selection(),
                                 m_parentWindow);
    if (dlg.exec() != QDialog::Accepted)
        return;

    auto const & result = dlg.result();
    BT_ASSERT(result.module);
    auto const & module = *result.module;
    if (module.type() == CSwordModuleInfo::Bible
        || module.type() == CSwordModuleInfo::Commentary)
    {
        qml.copyVerseRange(static_cast<CSwordVerseKey const &>(*result.key1),
                           static_cast<CSwordVerseKey const &>(*result.key2));
    } else {
        qml.copyRange(result.index1, result.index2);
    }
}

void BtModelViewReadDisplay::save(TextPart const part) {
    auto const filename =
            QFileDialog::getSaveFileName(
                nullptr,
                QObject::tr("Save document ..."),
                "",
                QObject::tr("Text files") + " (*.txt);;"
                + QObject::tr("All files") + " (*)");
    if (!filename.isEmpty())
        util::tool::savePlainFile(filename, text(part));
}

void BtModelViewReadDisplay::print(TextPart const type,
                                   DisplayOptions const & displayOptions,
                                   FilterOptions const & filterOptions)
{
    using CSBiMI = CSwordBibleModuleInfo;
    CSwordKey* const key = m_parentWindow->key();
    const CSwordModuleInfo *module = key->module();

    CExportManager mgr(false,
                       QString(),
                       m_parentWindow->filterOptions(),
                       m_parentWindow->displayOptions());

    switch (type) {
    case Document: {
        if (module->type() == CSwordModuleInfo::Bible) {
            CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>(key);

            CSwordVerseKey startKey(*vk);
            startKey.setVerse(1);

            CSwordVerseKey stopKey(*vk);

            const CSBiMI *bible = dynamic_cast<const CSBiMI*>(module);
            if (bible) {
                stopKey.setVerse(bible->verseCount(bible->bookNumber(startKey.bookName()), startKey.chapter()));
            }

            mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
        }
        else if (module->type() == CSwordModuleInfo::Lexicon || module->type() == CSwordModuleInfo::Commentary ) {
            mgr.printKey(module, key->key(), key->key(), displayOptions, filterOptions);
        }
        else if (module->type() == CSwordModuleInfo::GenericBook) {
            CSwordTreeKey* tree = dynamic_cast<CSwordTreeKey*>(key);

            CSwordTreeKey startKey(*tree);
            //        while (startKey.previousSibling()) { // go to first sibling on this level!
            //        }

            CSwordTreeKey stopKey(*tree);
            //    if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(module)) {
            //          while ( stopKey.nextSibling() ) { //go to last displayed sibling!
            //          }
            //        }
            mgr.printKey(module, startKey.key(), stopKey.key(), displayOptions, filterOptions);
        }
        break;
    }

    case AnchorWithText: {
        if (hasActiveAnchor()) {
            mgr.printByHyperlink(m_activeAnchor, displayOptions, filterOptions );
        }
        break;
    }

    default:
        break;
    }
}

#ifdef BUILD_TEXT_TO_SPEECH
void BtModelViewReadDisplay::speakSelectedText() {
    const BtQmlInterface* qml = qmlInterface();
    if (!qml)
        return;

    const std::optional<BtQmlInterface::Selection>& selection = qml->selection();
    if (!selection)
        return;

    BibleTime::instance()->speakText(qml->getSelectedText());
}
#endif

void BtModelViewReadDisplay::reloadModules() {
    qmlInterface()->textModel()->reloadModules();
}

QString
BtModelViewReadDisplay::text(TextPart const part) {
    QString text;
    switch (part) {
    case Document: {
        CSwordKey* const key = m_parentWindow->key();
        const CSwordModuleInfo *module = key->module();
        //This is never used for Bibles, so it is not implemented for
        //them.  If it should be, see CReadDisplay::print() for example
        //code.
        BT_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
                  module->type() == CSwordModuleInfo::Commentary ||
                  module->type() == CSwordModuleInfo::GenericBook);
        FilterOptions filterOptions;
        CSwordBackend::instance().setFilterOptions(filterOptions);

        text = QStringLiteral("%1\n(%2, %3)")
               .arg(key->strippedText(), key->key(), key->module()->name());
        break;
    }

    case AnchorOnly: {
        if (auto const decodedLink =
                    ReferenceManager::decodeHyperlink(m_activeAnchor))
            return decodedLink->key;
        return {};
    }

    case AnchorTextOnly: {
        auto const decodedLink(
                    ReferenceManager::decodeHyperlink(m_activeAnchor));
        if (decodedLink && decodedLink->module) {
            std::unique_ptr<CSwordKey> key(decodedLink->module->createKey());
            key->setKey(decodedLink->key);
            return key->strippedText();
        }
        return {};
    }

    case AnchorWithText: {
        auto const decodedLink(
                    ReferenceManager::decodeHyperlink(m_activeAnchor));
        if (decodedLink && decodedLink->module) {
            std::unique_ptr<CSwordKey> key(decodedLink->module->createKey());
            key->setKey(decodedLink->key);

            FilterOptions filterOptions;
            CSwordBackend::instance().setFilterOptions(filterOptions);

            return QStringLiteral("%1\n(%2, %3)")
                   .arg(key->strippedText(), key->key(), key->module()->name());
        }
        return {};
    }
    default:
        break;
    }
    return QString();

}

void BtModelViewReadDisplay::setDisplayFocus() { m_quickWidget->setFocus(); }

void BtModelViewReadDisplay::setDisplayOptions(const DisplayOptions &displayOptions) {
    m_qmlInterface->textModel()->setDisplayOptions(displayOptions);
}

void BtModelViewReadDisplay::setModules(const QStringList &modules) {
    m_qmlInterface->setModules(modules);
}

void BtModelViewReadDisplay::scrollToKey(CSwordKey * key) {
    m_qmlInterface->scrollToSwordKey(key);
}

void BtModelViewReadDisplay::scroll(int value) { m_quickWidget->scroll(value); }

void BtModelViewReadDisplay::setFilterOptions(FilterOptions filterOptions) {
    m_qmlInterface->setFilterOptions(filterOptions);
}

void BtModelViewReadDisplay::settingsChanged() {
    m_qmlInterface->settingsChanged();
}

void BtModelViewReadDisplay::updateReferenceText() {
    m_quickWidget->updateReferenceText();
}

void BtModelViewReadDisplay::pageDown() { m_quickWidget->pageDown(); }

void BtModelViewReadDisplay::pageUp() { m_quickWidget->pageUp(); }

void BtModelViewReadDisplay::highlightText(const QString& text, bool caseSensitive) {
    m_qmlInterface->setHighlightWords(text, caseSensitive);
}

void BtModelViewReadDisplay::findText(bool const backward)
{ m_qmlInterface->findText(backward); }

// Save the Lemma (Strongs number) attribute
void BtModelViewReadDisplay::setLemma(const QString& lemma) {
    m_nodeInfo = lemma;
}
