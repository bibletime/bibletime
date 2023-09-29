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

#include "btqmlinterface.h"

#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTimerEvent>
#include <utility>
#include "../../../backend/config/btconfig.h"
#include "../../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "../../../backend/keys/cswordkey.h"
#include "../../../backend/managers/colormanager.h"
#include "../../../backend/managers/cswordbackend.h"
#include "../../../backend/models/btmoduletextmodel.h"
#include "../../../backend/rendering/btinforendering.h"
#include "../../../backend/rendering/cplaintextexportrendering.h"
#include "../../../backend/rendering/ctextrendering.h"
#include "../../../backend/rendering/btinforendering.h"
#include "../../../util/btassert.h"
#include "../../bibletime.h"
#include "../../cinfodisplay.h"
#include "../../edittextwizard/btedittextwizard.h"


BtQmlInterface::BtQmlInterface(QObject * parent)
    : QObject(parent)
    , m_moduleTextModel(new BtModuleTextModel(this))
{ m_moduleTextModel->setTextFilter(&m_textFilter); }

BtQmlInterface::~BtQmlInterface() = default;

bool BtQmlInterface::isBibleOrCommentary() {
    if (!m_swordKey)
        return false;
    switch (m_swordKey->module()->type()) {
    case CSwordModuleInfo::Bible: [[fallthrough]];
    case CSwordModuleInfo::Commentary: return true;
    default: return false;
    }
}

void BtQmlInterface::setFilterOptions(FilterOptions filterOptions) {
    m_moduleTextModel->setFilterOptions(filterOptions);
}

int BtQmlInterface::getContextMenuIndex() const {
    return m_contextMenuIndex;
}

void BtQmlInterface::setContextMenuIndex(int index) {
    m_contextMenuIndex = index;
    Q_EMIT contextMenuIndexChanged();
}

int BtQmlInterface::getContextMenuColumn() const {
    return m_contextMenuColumn;
}

void BtQmlInterface::setContextMenuColumn(int index) {
    m_contextMenuColumn = index;
    Q_EMIT contextMenuColumnChanged();
}

QColor BtQmlInterface::getBackgroundColor() const
{ return ColorManager::getBackgroundColor(); }

QColor BtQmlInterface::getBackgroundHighlightColor() const
{ return ColorManager::getBackgroundHighlightColor(); }

int BtQmlInterface::getBackgroundHighlightColorIndex() const {
    return m_backgroundHighlightColorIndex;
}

QColor BtQmlInterface::getForegroundColor() const
{ return ColorManager::getForegroundColor(); }

int BtQmlInterface::getCurrentModelIndex() const {
    if (m_swordKey == nullptr)
        return 0;
    auto const * const keyModule = m_swordKey->module();
    auto const moduleType = keyModule->type();
    if (moduleType == CSwordModuleInfo::Bible
        || moduleType == CSwordModuleInfo::Commentary)
    {
        return m_moduleTextModel->verseKeyToIndex(
                    *static_cast<CSwordVerseKey *>(m_swordKey));
    } else if (moduleType == CSwordModuleInfo::GenericBook) {
        auto const m = static_cast<CSwordBookModuleInfo const *>(keyModule);
        CSwordTreeKey key(m->tree(), m);
        key.setKey(m_swordKey->key());
        CSwordTreeKey p(key);
        p.positionToRoot();
        if(p != key)
            return static_cast<int>(key.offset() / 4u); /// \todo Check range!
    } else if (moduleType == CSwordModuleInfo::Lexicon) {
        return static_cast<CSwordLexiconModuleInfo const *>(
                    keyModule)->entries().indexOf(m_swordKey->key());
    }
    return 0;
}

int BtQmlInterface::getNumModules() const {
    return m_moduleNames.count();
}

double BtQmlInterface::getPixelsPerMM() const {
    constexpr static double const millimetersPerInch = 25.4;
    return QGuiApplication::screens().first()->physicalDotsPerInchX()
            / millimetersPerInch;
}

void BtQmlInterface::setSelection(int const column,
                                  int const startIndex,
                                  int const endIndex,
                                  QString const & selectedText)
{
    BT_ASSERT(column >= 0);
    BT_ASSERT(startIndex >= 0);
    BT_ASSERT(endIndex >= 0);
    BT_ASSERT(!selectedText.isEmpty());
    m_selection.emplace(Selection{column, startIndex, endIndex, selectedText});
}

void BtQmlInterface::clearSelection() noexcept { m_selection.reset(); }

bool BtQmlInterface::hasSelectedText() const noexcept
{ return m_selection.has_value(); }

QString BtQmlInterface::getSelectedText() const
{ return m_selection.has_value() ? m_selection->selectedText : QString(); }

QString BtQmlInterface::getRawText(int row, int column) {
    BT_ASSERT(column >= 0 && column <= m_moduleNames.count());
    CSwordVerseKey key = m_moduleTextModel->indexToVerseKey(row);
    QString moduleName = m_moduleNames.at(column);
    auto * const module =
            CSwordBackend::instance().findModuleByName(moduleName);
    CSwordVerseKey mKey(module);
    mKey.setKey(key.key());
    auto rawText = mKey.rawText();

    /* Since rawText is a complete HTML page at the moment, strip away headers
       and footers of a HTML page, keeping only the contents of <body>: */
    static auto const reFlags =
            QRegularExpression::CaseInsensitiveOption
            | QRegularExpression::DotMatchesEverythingOption
            | QRegularExpression::DontCaptureOption
            | QRegularExpression::UseUnicodePropertiesOption;
    static QRegularExpression const reBefore(
                QStringLiteral("^.*?<body(\\s[^>]*?)?>"), reFlags);
    static QRegularExpression const reAfter(
                QStringLiteral("</body>.*?$"), reFlags);
    if (auto const m = reBefore.match(rawText); m.hasMatch())
        rawText.remove(0, m.capturedLength());
    if (auto const m = reAfter.match(rawText); m.hasMatch())
        rawText.chop(m.capturedLength());
    return rawText;
}

void BtQmlInterface::openEditor(int row, int column) {
    BtEditTextWizard wiz;
    wiz.setTitle(tr("Edit %1").arg(m_moduleTextModel->indexToKeyName(row)));
    wiz.setText(getRawText(row, column));
    wiz.setFont(m_fonts.at(column));
    if (wiz.exec() == QDialog::Accepted)
        setRawText(row, column, wiz.text());
}

int BtQmlInterface::indexToVerse(int index) {
    return m_moduleTextModel->indexToVerse(index);
}

void BtQmlInterface::setHoveredLink(QString const & link) {
    if (QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
        return;
    setMagReferenceByUrl(link);
    m_activeLink = link;
}

QString BtQmlInterface::getLemmaFromLink(const QString& url) {
    static QRegularExpression const rx(
        QStringLiteral(R"PCRE(sword://lemmamorph/([a-s]+)=([GH][0-9]+))PCRE"));
    if (auto const match = rx.match(url); match.hasMatch())
        return match.captured(2);
    return {};
}

QString BtQmlInterface::getBibleUrlFromLink(const QString& url) {
    static QRegularExpression const rx(
        QStringLiteral(R"PCRE((sword://Bible/.*)\|\|(.*)=(.*))PCRE"));
    if (auto const match = rx.match(url); match.hasMatch())
        return match.captured(1);
    return {};
}

QString BtQmlInterface::getReferenceFromUrl(const QString& url) {
    {
        static QRegularExpression const rx(
            QStringLiteral(
                R"PCRE(sword://(bible|lexicon)/(.*)/(.*)(\|\|))PCRE"),
            QRegularExpression::CaseInsensitiveOption);
        if (auto const match = rx.match(url); match.hasMatch())
            return QStringLiteral("href=sword://%1/%2/%3")
                        .arg(match.capturedView(1),
                             match.capturedView(2),
                             match.capturedView(3));
    }{
        static QRegularExpression const rx(
            QStringLiteral(R"PCRE(sword://(bible|lexicon)/(.*)/(.*))PCRE"),
            QRegularExpression::CaseInsensitiveOption);
        if (auto const match = rx.match(url); match.hasMatch())
            return QStringLiteral("href=sword://%1/%2/%3")
                        .arg(match.capturedView(1),
                             match.capturedView(2),
                             match.capturedView(3));
    }{
        static QRegularExpression const rx(
            QStringLiteral(R"PCRE(sword://footnote/(.*)=(.*))PCRE"),
            QRegularExpression::CaseInsensitiveOption);
        if (auto const match = rx.match(url); match.hasMatch())
            return QStringLiteral("note=%1").arg(match.capturedView(1));
    }{
        static QRegularExpression const rx(
            QStringLiteral(R"PCRE(sword://lemmamorph/(.*)=(.*)/(.*))PCRE"),
            QRegularExpression::CaseInsensitiveOption);
        if (auto const match = rx.match(url); match.hasMatch())
            return QStringLiteral("%1=%2").arg(match.capturedView(1),
                                               match.capturedView(2));
    }
    return {};
}

QString BtQmlInterface::rawText(int const row, int const column) {
    return m_moduleTextModel->data(m_moduleTextModel->index(row, 0),
                                   ModuleEntry::Text0Role + column).toString();
}

void BtQmlInterface::setRawText(int row, int column, const QString& text) {
    QModelIndex index = m_moduleTextModel->index(row, 0);
    int role = ModuleEntry::Text0Role + column;
    m_moduleTextModel->setData(index, text, role);
}

void BtQmlInterface::cancelMagTimer() {
    killTimer(m_linkTimerId);
    m_linkTimerId = 0;
}

void BtQmlInterface::setMagReferenceByUrl(const QString& url) {
    if (url.isEmpty())
        return;
    m_timeoutUrl = url;
    cancelMagTimer();
    m_linkTimerId = startTimer(400);
}

void BtQmlInterface::settingsChanged() {
    getFontsFromSettings();
    changeColorTheme();
    Q_EMIT textChanged();
}

void BtQmlInterface::getFontsFromSettings() {
    decltype(m_fonts) newFonts;
    newFonts.reserve(m_moduleNames.size());
    for (auto const & moduleName : m_moduleNames) {
        if (auto const * const m =
                    CSwordBackend::instance().findModuleByName(moduleName))
        {
            if (auto const lang = m->language()) {
                BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
                if (fontPair.first) {
                    newFonts.append(fontPair.second);
                    continue;
                }
            }
        }
        newFonts.append(btConfig().getDefaultFont());
    }
    m_fonts = std::move(newFonts);
    Q_EMIT fontChanged();
}

void BtQmlInterface::setBibleKey(const QString& link) {
    static QRegularExpression const rx(
        QStringLiteral(R"PCRE(sword://Bible/(.*)/(.*)\|\|(.*)=(.*))PCRE"));
    if (auto const match = rx.match(link); match.hasMatch())
        Q_EMIT setBibleReference(match.captured(2));
}

void BtQmlInterface::scrollToSwordKey(CSwordKey * key) {
    m_backgroundHighlightColorIndex = m_moduleTextModel->keyToIndex(*key);

    /* Convert from sword index to ListView index */
    m_backgroundHighlightColorIndex =
        m_backgroundHighlightColorIndex - m_moduleTextModel->firstEntryIndex();

    Q_EMIT backgroundHighlightColorIndexChanged();
    m_swordKey = key;
    Q_EMIT currentModelIndexChanged();
}

void BtQmlInterface::setModules(const QStringList &modules) {
    m_moduleNames = modules;
    m_moduleTextModel->setModules(modules);
    getFontsFromSettings();
    Q_EMIT numModulesChanged();
}

void BtQmlInterface::changeReference(int i) {
    QString reference = m_moduleTextModel->indexToKeyName(i);
    Q_EMIT updateReference(reference);
}

void BtQmlInterface::dragHandler(int index) {
    QString moduleName;
    QString keyName;

    static QRegularExpression const rx(
        QStringLiteral(R"PCRE(sword://Bible/(.*)/(.*)\|\|(.*)=(.*))PCRE"));

    if (auto const match = rx.match(m_activeLink); match.hasMatch()) {
        moduleName = match.captured(1);
        keyName = match.captured(2);
    } else {
        moduleName = m_moduleNames.at(0);
        keyName = m_moduleTextModel->indexToKeyName(index);
    }

    Q_EMIT dragOccuring(moduleName, keyName);
}

CSwordKey* BtQmlInterface::getMouseClickedKey() const {
    return m_moduleTextModel->indexToKey(m_contextMenuIndex, m_contextMenuColumn);
}

QFont BtQmlInterface::font(int column) const {
    if (column >= 0 && column < m_fonts.count())
        return m_fonts.at(column);
    return QApplication::font();
}

QFont BtQmlInterface::getFont0() const { return font(0); }
QFont BtQmlInterface::getFont1() const { return font(1); }
QFont BtQmlInterface::getFont2() const { return font(2); }
QFont BtQmlInterface::getFont3() const { return font(3); }
QFont BtQmlInterface::getFont4() const { return font(4); }
QFont BtQmlInterface::getFont5() const { return font(5); }
QFont BtQmlInterface::getFont6() const { return font(6); }
QFont BtQmlInterface::getFont7() const { return font(7); }
QFont BtQmlInterface::getFont8() const { return font(8); }
QFont BtQmlInterface::getFont9() const { return font(9); }

QVariant BtQmlInterface::getTextModel() {
    QVariant var;
    var.setValue(m_moduleTextModel);
    return var;
}

BtModuleTextModel * BtQmlInterface::textModel() {
    return m_moduleTextModel;
}

BtModuleTextModel const * BtQmlInterface::textModel() const
{ return m_moduleTextModel; }

bool BtQmlInterface::moduleIsWritable(int column) {
    if (column >= m_moduleNames.count())
        return false;
    QString moduleName = m_moduleNames.at(column);
    auto * const module =
            CSwordBackend::instance().findModuleByName(moduleName);
    BT_ASSERT(module);
    return module->isWritable();
}

void BtQmlInterface::changeColorTheme() {
    Q_EMIT backgroundHighlightColorChanged();
    Q_EMIT backgroundColorChanged();
    Q_EMIT foregroundColorChanged();
}

void BtQmlInterface::copyRange(int index1, int index2) const {
    QString text;
    std::unique_ptr<CSwordKey> key(m_swordKey->copy());

    for (int i=index1; i<=index2; ++i) {
        QString keyName = m_moduleTextModel->indexToKeyName(i);
        key->setKey(keyName);
        text.append(QStringLiteral("%1\n%2\n\n")
                    .arg(keyName, key->strippedText()));
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void BtQmlInterface::copyVerseRange(CSwordVerseKey const & key1,
                                    CSwordVerseKey const & key2)
{
    BT_ASSERT(key1.module());
    BT_ASSERT(key1.module() == key2.module());

    Rendering::CPlainTextExportRendering render(true);
    {
        DisplayOptions displayOptions;
        displayOptions.lineBreaks = true;
        displayOptions.verseNumbers = true;
        render.setDisplayOptions(displayOptions);
    }{
        FilterOptions filterOptions;
        filterOptions.footnotes = 0;
        filterOptions.greekAccents = 1;
        filterOptions.headings = 1;
        filterOptions.hebrewCantillation = 1;
        filterOptions.hebrewPoints = 1;
        filterOptions.lemmas = 0;
        filterOptions.morphSegmentation = 1;
        filterOptions.morphTags = 0;
        filterOptions.redLetterWords = 1;
        filterOptions.scriptureReferences = 0;
        filterOptions.strongNumbers = 0;
        filterOptions.textualVariants = 0;
        render.setFilterOptions(filterOptions);
    }
    QGuiApplication::clipboard()->setText(
                render.renderKeyRange(key1, key2, {key1.module()}));
}

void BtQmlInterface::setHighlightWords(const QString& words, bool caseSensitive) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_moduleTextModel->setHighlightWords(words, caseSensitive);
    m_findState.reset();
    m_moduleTextModel->setFindState(m_findState);
    QApplication::restoreOverrideCursor();
}

void BtQmlInterface::timerEvent(QTimerEvent * const event) {
    auto const timerId = event->timerId();
    BT_ASSERT(timerId);
    if (timerId == m_linkTimerId) {
        event->accept();
        cancelMagTimer();
        auto infoList(Rendering::detectInfo(getReferenceFromUrl(m_timeoutUrl)));
        if (!infoList.isEmpty())
            BibleTime::instance()->infoDisplay()->setInfo(std::move(infoList));
    } else {
        QObject::timerEvent(event);
    }
}

void BtQmlInterface::findText(bool const backward) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!m_findState)
        m_findState = FindState{getCurrentModelIndex(), 0};

    auto const countHighlightsInItem =
        [this](int const index) {
            return m_moduleTextModel->data(m_moduleTextModel->index(index),
                                           ModuleEntry::Text1Role)
                        .toString().count(QStringLiteral("\"highlightwords"));
        };

    auto const num = countHighlightsInItem(m_findState->index);
    if (backward) { // get previous matching item:
        if (num > 0 && m_findState->subIndex == 0) {
            // Found within m_findState->index item
            m_findState->subIndex = 1;
        } else if (auto index = m_findState->index; index > 0) {
            if (m_findState->subIndex == 0)
                --index;
            for (int i = 0; i < 1000; ++i, --index) {
                if (auto const num2 = countHighlightsInItem(index)) {
                    m_findState->index = index;
                    if (m_findState->subIndex == 0) {
                        m_findState->subIndex = num2;
                    } else {
                        --m_findState->subIndex;
                    }
                    if (m_findState->subIndex != 0)
                        break;
                }
            }
        }
    } else { // get next matching item:
        if (num > m_findState->subIndex) {
            // Found within m_findState->index item
            ++m_findState->subIndex;
        } else if (m_findState->index < m_moduleTextModel->rowCount()) {
            auto index = m_findState->index + 1;
            for (int i = 0; i < 1000; ++i, ++index) {
                if (countHighlightsInItem(index)) {
                    m_findState->index = index;
                    m_findState->subIndex = 1;
                    break;
                }
            }
        }
    }

    m_moduleTextModel->setFindState(m_findState);
    Q_EMIT positionItemOnScreen(m_findState->index);
    QApplication::restoreOverrideCursor();
}

int BtQmlInterface::typeId;
