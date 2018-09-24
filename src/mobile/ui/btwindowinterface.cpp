/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btwindowinterface.h"

#include <QClipboard>
#include <QDate>
#include <QDomDocument>
#include <QFile>
#include <QGuiApplication>
#include <QMimeData>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QStringList>
#include <swkey.h>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/models/btmoduletextmodel.h"
#include "backend/rendering/btinforendering.h"
#include "backend/rendering/cdisplayrendering.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/chtmlexportrendering.h"
#include "backend/rendering/cplaintextexportrendering.h"
#include "mobile/btmmain.h"
#include "util/btconnect.h"

namespace btm {

BtWindowInterface::BtWindowInterface(QObject* parent)
    : QObject(parent),
      m_firstHref(false),
      m_footnoteVisible(true),
      m_historyIndex(-1),
      m_key(nullptr),
      m_magView(false),
      m_moduleTextModel(new BtModuleTextModel(this)),
      m_textModel(new RoleItemModel()) {

    m_prompt = tr("Select a reference.");
    m_moduleTextModel->setTextFilter(&m_textFilter);

    BT_CONNECT(CSwordBackend::instance(),
               SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
               this,
               SLOT(reloadModules(CSwordBackend::SetupChangedReason)));
}

BtWindowInterface::~BtWindowInterface() {

}

void BtWindowInterface::reloadModules(CSwordBackend::SetupChangedReason /* reason */ ) {
    QString moduleName = getModuleName();
    QString reference = getReference();
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (module) {
        m_key = CSwordKey::createInstance(module);
        m_key->setKey(reference);
    } else {
        ; // close window ?
    }
}

void BtWindowInterface::updateModel() {
    QString moduleName= getModuleName();
    QStringList moduleList = QStringList() << moduleName;
    QList<const CSwordModuleInfo*> modules =
            CSwordBackend::instance()->getConstPointerList(moduleList);
}

static bool moduleIsBook(const CSwordModuleInfo* module) {
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Books)
        return true;
    return false;
}

static bool moduleIsLexicon(const CSwordModuleInfo* module) {
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Lexicons ||
            category == CSwordModuleInfo::DailyDevotional)
        return true;
    return false;
}

static bool moduleIsBibleOrCommentary(const CSwordModuleInfo* module) {
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Bibles ||
            category == CSwordModuleInfo::Cult ||
            category == CSwordModuleInfo::Commentaries)
        return true;
    return false;
}

bool BtWindowInterface::firstModuleIsBibleOrCommentary() {
    return moduleIsBibleOrCommentary(module());
}

bool BtWindowInterface::firstModuleIsBook() {
    return module()->category() == CSwordModuleInfo::Books;
}

QString BtWindowInterface::getModelTextByIndex(int index) const {
    if (index == 0)
        return "";
    QModelIndex mIndex = m_moduleTextModel->index(index);
    QVariant vText = m_moduleTextModel->data(mIndex, ModuleEntry::TextRole);
    return vText.toString();
}

QString BtWindowInterface::getCurrentModelText() const {
    int index = getCurrentModelIndex();
    return getModelTextByIndex(index);
}

int BtWindowInterface::getCurrentModelIndex() const {
    if (m_key == nullptr)
        return 0;
    if (moduleIsBibleOrCommentary(module())) {
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
        int index = m_moduleTextModel->verseKeyToIndex(*verseKey);
        return index;
    }
    else if (moduleIsBook(module())) {
        const CSwordBookModuleInfo *m = qobject_cast<const CSwordBookModuleInfo*>(module());
        CSwordTreeKey key(m->tree(), m);
        QString keyName = m_key->key();
        key.setKey(keyName);
        CSwordTreeKey p(key);
        p.root();
        if(p != key)
            return key.getIndex()/4;
    }
    else if (moduleIsLexicon(module())){        const CSwordLexiconModuleInfo *li = qobject_cast<const CSwordLexiconModuleInfo*>(m_key->module());
        int index = li->entries().indexOf(m_key->key());
        return index;
    }
    return 0;
}

QString BtWindowInterface::getModuleLanguage() const {
    QString language;
    if (m_key)
        language = m_key->module()->language()->englishName();
    return language;
}

QString BtWindowInterface::getModuleName() const {
    if (m_moduleNames.count() < 1)
        return QString();
    return m_moduleNames.at(0);
}

QString BtWindowInterface::getModule2Name() const {
    if (m_moduleNames.count() < 2)
        return QString();
    return m_moduleNames.at(1);
}

QString BtWindowInterface::getModule3Name() const {
    if (m_moduleNames.count() < 3)
        return QString();
    return m_moduleNames.at(2);
}

QString BtWindowInterface::getModule4Name() const {
    if (m_moduleNames.count() < 4)
        return QString();
    return m_moduleNames.at(3);
}

QStringList BtWindowInterface::getModuleNames() const {
    return m_moduleNames;
}

int BtWindowInterface::getNumModules() const {
    return m_moduleNames.count();
}

QString BtWindowInterface::stripHtml(const QString& html) {
    QString t = html;
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer
    return t;
}

QString BtWindowInterface::getRawText(int row, int column) {
    BT_ASSERT(column >= 0 && column <= m_moduleNames.count());
    CSwordVerseKey key = m_moduleTextModel->indexToVerseKey(row);
    QString moduleName = m_moduleNames.at(column);
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    CSwordVerseKey mKey(module);
    mKey.setKey(key.key());
    QString rawText = mKey.rawText();
    return stripHtml(rawText);
}

QString BtWindowInterface::getReferenceFromUrl(const QString& url) {
    QString reference;
    QStringList parts = url.split('/', QString::SkipEmptyParts);
    if (parts.count() == 4 && parts.at(0) == "sword:" && parts.at(1) == "Bible") {
        reference = parts.at(3);
        if (reference.endsWith(';'))
            reference.chop(1);
    }
    if (parts.count() == 6 && parts.at(0) == "sword:" && parts.at(1) == "footnote") {
        reference = parts.at(3) + "/" + parts.at(4);
        QString title = tr("Footnote") + " " + parts.at(5);
        setReferencesViewTitle(title);
    }
    if (parts.count() == 4 && parts.at(0) == "sword:" && parts.at(1) == "lemmamorph") {
        reference = parts.at(2);
        QString title = parts.at(3);
        setReferencesViewTitle(title);
    }
    return reference;
}

int BtWindowInterface::getComboIndexFromUrl(const QString& url) {
    QString reference = getReferenceFromUrl(url);
    int index = m_comboBoxEntries.indexOf(reference);
    return index;
}

void BtWindowInterface::setRawText(int row, int column, const QString& text) {
    QModelIndex index = m_moduleTextModel->index(row, 0);
    int role = ModuleEntry::Text1Role;
    if (column == 1)
        role = ModuleEntry::Text2Role;
    else if (column == 2)
        role = ModuleEntry::Text3Role;
    else if (column == 3)
        role = ModuleEntry::Text4Role;
    m_moduleTextModel->setData(index, text, role);
}

void BtWindowInterface::setReferenceByUrl(const QString& url) {
    QString reference = getReferenceFromUrl(url);
    setReference(reference);
}

void BtWindowInterface::setReference(const QString& key) {
    QString newKey = key;
    if (key.startsWith("lemma") || key.startsWith("morph")) {
        decodeLemmaMorph(key);
        setFootnoteVisible(true);
    } else {
        QStringList parts = newKey.split("/");
        if (parts.count() == 2) {
            m_footnoteNum = parts.at(1);
            newKey = parts.at(0);
            decodeFootnote(newKey, m_footnoteNum);
            setFootnoteVisible(true);
        }
        else if (m_key && m_key->key() != newKey) {
            CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
            if (verseKey)
                verseKey->setIntros(true);
            m_key->setKey(newKey);
            referenceChanged();
            setFootnoteVisible(false);
            QString title = getModuleName() + "   " + getReference();
            setReferencesViewTitle(title);
        }
    }
}

void BtWindowInterface::decodeLemmaMorph(const QString& attributes) {

    Rendering::ListInfoData infoList(Rendering::detectInfo(attributes));
    BtConstModuleList l;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(getModuleName());
    if(m != nullptr)
        l.append(m);
    QString text = Rendering::formatInfo(infoList, l);
    QString lang;
    displayText(text, lang);
}

void BtWindowInterface::decodeFootnote(const QString& keyName, const QString& footnote) {

    QString text;
    CSwordModuleInfo * const module = CSwordBackend::instance()->findModuleByName(getModuleName());
    if (!module)
        return;

    QSharedPointer<CSwordKey> key(CSwordKey::createInstance(module));
    key->setKey(keyName);
    key->renderedText(CSwordKey::ProcessEntryAttributesOnly); // force entryAttributes

    auto & m = module->module();
    const char * const note =
            m.getEntryAttributes()
            ["Footnote"][footnote.toLatin1().data()]["body"].c_str();

    text = module->isUnicode() ? QString::fromUtf8(note) : QString(note);
    text = QString::fromUtf8(m.renderText(
                                 module->isUnicode()
                                 ? static_cast<const char *>(text.toUtf8())
                                 : static_cast<const char *>(text.toLatin1())));
    QString lang = module->language()->abbrev();
    displayText(text, lang);
}

void BtWindowInterface::displayText(const QString& text, const QString& lang) {
    CDisplayTemplateMgr *mgr = CDisplayTemplateMgr::instance();
    BT_ASSERT(mgr);

    CDisplayTemplateMgr::Settings settings;
    settings.pageCSS_ID = "infodisplay";

    QString div = "<div class=\"infodisplay\"";
    if (!lang.isEmpty())
        div.append(" lang=\"").append(lang).append("\"");
    div.append(">");

    QString content(mgr->fillTemplate(CDisplayTemplateMgr::activeTemplateName(),
                                      div + text + "</div>",
                                      settings));
    content.replace("#CHAPTERTITLE#", "");
    content.replace("#LINK_COLOR#", "blue");
    content.replace("#HIGHLIGHT_COLOR#", "blue");
    content.replace("#JESUS_WORDS_COLOR#", "red");
    m_footnoteText = content;
    footnoteTextChanged();
}

QString BtWindowInterface::getFootnoteText() const {
    return m_footnoteText;
}

bool BtWindowInterface::getFootnoteVisible() const {
    return m_footnoteVisible;
}

void BtWindowInterface::setFootnoteVisible(bool visible) {
    if (visible == m_footnoteVisible)
        return;
    m_footnoteVisible = visible;
    emit footnoteVisibleChanged();
}

void BtWindowInterface::setPrompt(const QString& prompt) {
    m_prompt = prompt;
    emit promptChanged();
}

void BtWindowInterface::setReferencesViewTitle(const QString& title) {
    m_referencesViewTitle = title;
    emit referencesViewTitleChanged();
}

void BtWindowInterface::moduleNameChanged(const QString& moduleName)
{
    setModuleName(moduleName);
    setHistoryPoint();
}

void BtWindowInterface::setModuleToBeginning() {
    if (m_key == nullptr)
        return;
    if (moduleIsBibleOrCommentary(m_key->module())) {
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
        verseKey->setPosition(swordxx::TOP);
        emit referenceChange();
    }
}

void BtWindowInterface::setModuleName(const QString& moduleName) {
    if (moduleName.isEmpty())
        return;
    if (m_moduleNames.count() > 0 && m_moduleNames.at(0) == moduleName)
        return;

    if (m_moduleNames.count() == 0) {
        m_moduleNames.append(moduleName);
        emit numModulesChanged();
    }
    else
        m_moduleNames[0] = moduleName;

    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!m)
        return;

    if (!m_key) {
        m_key = CSwordKey::createInstance(m);
    }
    else {
        if (moduleIsBibleOrCommentary(m) &&
                moduleIsBibleOrCommentary(m_key->module())) {
            m_key->setModule(m);
        }
        else if (moduleIsBook(m) &&
                 moduleIsBook(m_key->module())) {
            m_key->setModule(m);
        }

        else {
            delete m_key;
            m_key = CSwordKey::createInstance(m);
        }

    }

    if (m->category() == CSwordModuleInfo::DailyDevotional) {
        QDate date = QDate::currentDate();
        QString today = date.toString("MM.dd");
        setReference(today);
        referenceChosen();
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey)
        treeKey->firstChild();

    m_moduleTextModel->setModules(m_moduleNames);

    emit moduleChanged();
    emit referenceChange();
    emit textChanged();
    emit referencesChanged();
    updateModel();
}

void BtWindowInterface::setModule2Name(const QString& moduleName) {
    if (m_moduleNames.count() == 0)
        return;

    if (moduleName.isEmpty() && m_moduleNames.count() == 2) {
        m_moduleNames.removeLast();
        emit numModulesChanged();
    } else if (m_moduleNames.count() == 1) {
        m_moduleNames.append(moduleName);
        emit numModulesChanged();
    } else
        m_moduleNames[1] = moduleName;

    m_moduleTextModel->setModules(m_moduleNames);
    emit module2Changed();
    emit referenceChange();
    emit textChanged();
    emit referencesChanged();
    updateModel();
}

void BtWindowInterface::setModule3Name(const QString& moduleName) {
    if (m_moduleNames.count() < 2)
        return;

    if (moduleName.isEmpty() && m_moduleNames.count() == 3) {
        m_moduleNames.removeLast();
        emit numModulesChanged();
    } else if (m_moduleNames.count() == 2) {
        m_moduleNames.append(moduleName);
        emit numModulesChanged();
    } else
        m_moduleNames[2] = moduleName;

    m_moduleTextModel->setModules(m_moduleNames);
    emit module3Changed();
    emit referenceChange();
    emit textChanged();
    emit referencesChanged();
    updateModel();
}

void BtWindowInterface::setModule4Name(const QString& moduleName) {
    if (m_moduleNames.count() < 3)
        return;

    if (moduleName.isEmpty() && m_moduleNames.count() == 4) {
        m_moduleNames.removeLast();
        emit numModulesChanged();
    } else if (m_moduleNames.count() == 3) {
        m_moduleNames.append(moduleName);
        emit numModulesChanged();
    } else
        m_moduleNames[3] = moduleName;

    m_moduleTextModel->setModules(m_moduleNames);
    emit module4Changed();
    emit referenceChange();
    emit textChanged();
    emit referencesChanged();
    updateModel();
}

QString BtWindowInterface::getReference() const {
    QString reference;
    if (m_key)
        reference = m_key->key();
    return reference;
}

void BtWindowInterface::updateCurrentModelIndex() {
    emit currentModelIndexChanged();
}

void BtWindowInterface::updateTextFonts() {
    emit textChanged();
}

void BtWindowInterface::parseVerseForReferences(const QString& verse) {
    m_firstHref = true;
    QDomDocument doc;
    doc.setContent(verse);
    QDomNodeList nodes = doc.elementsByTagName("html");
    if (nodes.count() != 1)
        return;
    QDomNode htmlNode = nodes.at(0);
    QDomElement bodyElement = htmlNode.firstChildElement("body");
    if (bodyElement.isNull())
        return;
    QDomNodeList spanNodes = bodyElement.elementsByTagName("span");
    processNodes(spanNodes);
}

void BtWindowInterface::updateReferences() {
    m_references.clear();
    m_comboBoxEntries.clear();
    int index = getCurrentModelIndex();

    QString text = getModelTextByIndex(index);
    parseVerseForReferences(text);

    text = getModelTextByIndex(index+1);
    parseVerseForReferences(text);

    text = getModelTextByIndex(index+2);
    parseVerseForReferences(text);

    emit referencesChanged();
    emit comboBoxEntriesChanged();
}

void BtWindowInterface::processNodes(const QDomNodeList& nodes) {
    int count = nodes.count();
    for (int i=0; i<count; i++) {
        QDomNode node = nodes.at(i);
        QDomElement element = node.toElement();

        QString nodeReferences;
        QString comboBoxEntries;
        QDomNamedNodeMap nodeMap = element.attributes();
        for (int index=0; index<nodeMap.count(); ++index) {
            QDomNode node = nodeMap.item(index);
            if (!node.isNull()) {
                QDomAttr attr = node.toAttr();
                QString name = attr.name();
                if (name == "href") {
                    if (!m_firstHref) {
                        QString text = element.text();
                        QString value = attr.value();
                        if (value.endsWith(';'))
                            value.chop(1);
                        nodeReferences += name + "=" + value + ";||";
                        comboBoxEntries += value;
                    }
                    m_firstHref = false;
                }
            }
        }
        if (!nodeReferences.isEmpty()) {
            QStringList parts = comboBoxEntries.split('/', QString::SkipEmptyParts);
            if (parts.count() == 4 && parts.at(0) == "sword:" && parts.at(1) == "Bible") {
                m_references += nodeReferences;
                m_comboBoxEntries += parts.at(3);
            }
        }

        QDomNodeList childrenNodes = node.childNodes();
        processNodes(childrenNodes);
    }
}

QStringList BtWindowInterface::getComboBoxEntries() const {
    return m_comboBoxEntries;
}

QStringList BtWindowInterface::getReferences() const {
    return m_references;
}

QString BtWindowInterface::getPrompt() const {
    return m_prompt;
}

QString BtWindowInterface::getReferencesViewTitle() const {
    return m_referencesViewTitle;
}

bool BtWindowInterface::isMagView() const {
    return m_magView;
}

void BtWindowInterface::setMagView(bool magView) {
    m_magView = magView;
    FilterOptions filterOptions = m_moduleTextModel->getFilterOptions();
    filterOptions.scriptureReferences = magView ? 1 : 0;
    filterOptions.strongNumbers = magView ? 1 : 0;
    filterOptions.footnotes = magView ? 1 : 0;
    m_moduleTextModel->setFilterOptions(filterOptions);
    m_textFilter.setShowReferences(magView);
}

static QString getEnglishKey(CSwordKey* m_key) {
    swordxx::VerseKey * vk = dynamic_cast<swordxx::VerseKey*>(m_key);
    QString oldLang;
    if (vk) {
        // Save keys in english only:
        const QString oldLang = QString::fromLatin1(vk->getLocale());
        vk->setLocale("en");
        QString englishKey = m_key->key();
        vk->setLocale(oldLang.toLatin1());
        return englishKey;
    } else {
        return m_key->key();
    }
}

QString BtWindowInterface::getEnglishReference() const {
    return getEnglishKey(m_key);
}

void BtWindowInterface::saveWindowStateToConfig(int windowIndex) {
    const QString windowKey = QString::number(windowIndex);
    const QString windowGroup = "window/" + windowKey + '/';

    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    conf.setSessionValue("key", getEnglishKey(m_key));
    conf.setSessionValue("modules", m_moduleNames);
    conf.endGroup();
}

void BtWindowInterface::referenceChanged() {
    emit referenceChange();
}

void BtWindowInterface::referenceChosen() {
    emit referenceChange();
    updateModel();
    emit currentModelIndexChanged();
    setHistoryPoint();
}

void BtWindowInterface::lexiconReferenceChoosen(int index) {
    updateKeyText(index);
    QString keyName = m_moduleTextModel->indexToKeyName(index);
    m_key->setKey(keyName);
    setReference(keyName);
    emit currentModelIndexChanged();
    setHistoryPoint();
}

const CSwordModuleInfo* BtWindowInterface::module() const {
    if (!m_key)
        return nullptr;
    const CSwordModuleInfo* module = m_key->module();
    return module;
}

CSwordKey* BtWindowInterface::getKey() const {
    return m_key;
}

QString BtWindowInterface::getFontName() const {
    const CSwordModuleInfo* m = module();
    if (m == nullptr)
        return QString();
    const CLanguageMgr::Language* lang = m->language();
    if (lang == nullptr)
        return QString();
    BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
    if (fontPair.first) {
        QFont font = fontPair.second;
        QString fontName = font.family();
        return fontName;
    }
    QFont font = getDefaultFont();
    QString fontName = font.family();
    return fontName;
}

int BtWindowInterface::getFontSize() const {
    if (!module())
        return 22;
    const CLanguageMgr::Language* lang = module()->language();
    BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
    if (fontPair.first) {
        QFont font = fontPair.second;
        int fontPointSize = font.pointSize();
        return fontPointSize;
    }
    int fontPointSize = btConfig().value<int>("ui/textFontSize",22);
    return fontPointSize;
}

void BtWindowInterface::setFontSize(int size) {
    const CLanguageMgr::Language* lang = module()->language();
    BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
    fontPair.second.setPointSize(size);
    btConfig().setFontForLanguage(*lang, fontPair);
    emit textChanged();
}

QVariant BtWindowInterface::getTextModel() {
    QVariant var;
    var.setValue(m_moduleTextModel);
    return var;
}

void BtWindowInterface::updateKeyText(int index) {
    QString keyName = m_moduleTextModel->indexToKeyName(index);
    setReference(keyName);
}

QString BtWindowInterface::getHighlightWords() const {
    return m_highlightWords;
}

void BtWindowInterface::setHighlightWords(const QString& words) {
    m_highlightWords = words;
    m_moduleTextModel->setHighlightWords(words);
}

void BtWindowInterface::setHistoryPoint() {
    History history;
    while (  m_history.count()>0  &&  (m_historyIndex<m_history.count()-1)  )
        m_history.pop_back();
    history.moduleName = getModuleName();
    history.reference = getReference();
    if (m_history.count() > 0) {
        History prevHistory = m_history.last();
        if (prevHistory.reference == history.reference &&
                prevHistory.moduleName == history.moduleName)
            return;
    }
    m_history.append(history);
    m_historyIndex = m_history.count() - 1;
    emit historyChanged();
}

bool BtWindowInterface::getHistoryForwardVisible() const {
    return m_historyIndex < (m_history.count() - 1);
}

bool BtWindowInterface::getHistoryBackwardVisible() const {
    return (m_historyIndex > 0) && (m_history.count() > 1);
}

bool BtWindowInterface::moduleIsWritable(int column) {
    BT_ASSERT(column >= 0 && column <= m_moduleNames.count());
    QString moduleName = m_moduleNames.at(column);
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    return module->isWritable();
}

void BtWindowInterface::moveHistoryBackward() {
    if ( ! getHistoryBackwardVisible())
        return;
    m_historyIndex--;
    History history = m_history.at(m_historyIndex);
    setModuleName(history.moduleName);
    setReference(history.reference);
    emit currentModelIndexChanged();
    emit historyChanged();
}

void BtWindowInterface::moveHistoryForward() {
    if ( ! getHistoryForwardVisible())
        return;
    m_historyIndex++;
    History history = m_history.at(m_historyIndex);
    setModuleName(history.moduleName);
    setReference(history.reference);
    emit currentModelIndexChanged();
    emit historyChanged();
}

// If no default module is configed, choose one an update config.
void BtWindowInterface::updateDefaultModules() {
    lookupAvailableModules();
    configModuleByType("standardGreekStrongsLexicon", m_greekStrongsLexicons);
    configModuleByType("standardHebrewStrongsLexicon", m_hebrewStrongsLexicons);
    configModuleByType("standardBible", m_bibles);
}

void BtWindowInterface::lookupAvailableModules() {
    m_greekStrongsLexicons.clear();
    m_hebrewStrongsLexicons.clear();
    m_bibles.clear();
    Q_FOREACH(CSwordModuleInfo const * const m,
              CSwordBackend::instance()->moduleList()) {
        if (m->type() ==  CSwordModuleInfo::Bible) {
            m_bibles += m->name();
        }
        if (m->type() ==  CSwordModuleInfo::Lexicon) {
            if (m->has(CSwordModuleInfo::HebrewDef)) {
                m_hebrewStrongsLexicons += m->name();
            }
            if (m->has(CSwordModuleInfo::GreekDef)) {
                m_greekStrongsLexicons += m->name();
            }
        }
    }
}

void BtWindowInterface::configModuleByType(const QString& type, const QStringList& availableModuleNames) {
    CSwordModuleInfo* module = btConfig().getDefaultSwordModuleByType(type);
    if (!module && availableModuleNames.count() > 0) {
        QString moduleName = availableModuleNames.at(0);
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        btConfig().setDefaultSwordModuleByType(type, module);
    }
}

QString BtWindowInterface::getDefaultSwordModuleByType(const QString& type) {
    CSwordModuleInfo* module = btConfig().getDefaultSwordModuleByType(type);
    QString moduleName;
    if (module)
        moduleName = module->name();
    return moduleName;
}

bool BtWindowInterface::isCopyToLarge(const QString& ref1, const QString& ref2) {
    RefIndexes ri = normalizeReferences(ref1, ref2);
    CSwordModuleInfo::ModuleType type = m_key->module()->type();
    if (type == CSwordModuleInfo::Bible ||
            type == CSwordModuleInfo::Commentary) {
        if ((ri.index2-ri.index1) > 2500)
            return true;
    } else {
        if ( ri.index2-ri.index1 > 25)
            return true;
    }
    return false;
}

RefIndexes BtWindowInterface::normalizeReferences(const QString& ref1, const QString& ref2) {
    RefIndexes ri;
    CSwordKey * key = m_key->copy();
    key->setKey(ref1);
    QString x1 = key->key();
    ri.index1 = m_moduleTextModel->keyToIndex(key);
    key->setKey(ref2);
    QString x2 = key->key();
    ri.index2 = m_moduleTextModel->keyToIndex(key);
    ri.r1 = ref1;
    ri.r2 = ref2;
    if (ri.index1 > ri.index2) {
        ri.r1.swap(ri.r2);
        std::swap(ri.index1, ri.index2);
    }
    return ri;
}

bool BtWindowInterface::copy(const QString& moduleName, const QString& ref1, const QString& ref2) {
    RefIndexes ri = normalizeReferences(ref1, ref2);
    CSwordModuleInfo::ModuleType type = m_key->module()->type();
    if (type == CSwordModuleInfo::Bible ||
            type == CSwordModuleInfo::Commentary) {
        if ((abs(ri.index2-ri.index1)) > 2500)
            return false;
        copyDisplayedText(ri.r1, ri.r2);
    } else {
        if ( ri.index2-ri.index1 > 25)
            return false;
        copyRange(ri.index1, ri.index2);
    }
    return true;
}

void BtWindowInterface::copyRange(int index1, int index2) {
    QString text;
    for (int i=index1; i<=index2; ++i) {
        QString keyName = m_moduleTextModel->indexToKeyName(i);
        setReference(keyName);
        text += m_key->strippedText() + "\n\n";
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void BtWindowInterface::copyDisplayedText(const QString& ref1, const QString& ref2) {
    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);

    if (verseKey) {
        CSwordVerseKey dummy(*verseKey);
        CSwordVerseKey vk(*verseKey);

        dummy.setKey(ref1);
        vk.setLowerBound(dummy);
        dummy.setKey(ref2);
        vk.setUpperBound(dummy);

        copyKey(&vk, BtWindowInterface::Text, true);
    } else {
        copyKey(m_key,BtWindowInterface::Text, true);
    }
}

bool BtWindowInterface::copyKey(CSwordKey const * const key,
                                Format const format,
                                bool const addText)
{
    if (!key || !key->module())
        return false;

    QString text;
    BtConstModuleList modules;
    modules.append(key->module());

    auto const render = newRenderer(format, addText);
    CSwordVerseKey const * const vk =
            dynamic_cast<CSwordVerseKey const *>(key);
    if (vk && vk->isBoundSet()) {
        text = render->renderKeyRange(
                    QString::fromUtf8(vk->getLowerBound()),
                    QString::fromUtf8(vk->getUpperBound()),
                    modules
                    );
    } else {
        text = render->renderSingleKey(key->key(), modules);
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
    return true;
}

std::unique_ptr<Rendering::CTextRendering> BtWindowInterface::newRenderer(Format const format,
                                                                          bool const addText)
{
    DisplayOptions displayOptions;
    displayOptions.lineBreaks = true;
    displayOptions.verseNumbers = true;

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

    using R = std::unique_ptr<Rendering::CTextRendering>;
    BT_ASSERT((format == Text) || (format == HTML));
    if (format == HTML)
        return R{new Rendering::CHTMLExportRendering(addText,
                                                     displayOptions,
                                                     filterOptions)};
    return R{new Rendering::CPlainTextExportRendering(addText,
                                                      displayOptions,
                                                      filterOptions)};
}

} // end namespace
