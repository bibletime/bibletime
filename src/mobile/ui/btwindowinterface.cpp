/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btwindowinterface.h"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
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
#include "mobile/btmmain.h"
#include "mobile/keychooser/bookkeychooser.h"
#include "mobile/keychooser/keynamechooser.h"
#include "mobile/keychooser/versechooser.h"
#include "mobile/ui/modulechooser.h"
#include "mobile/ui/viewmanager.h"
#include "util/btconnect.h"

namespace btm {

BtWindowInterface::BtWindowInterface(QObject* parent)
    : QObject(parent),
      m_bookKeyChooser(nullptr),
      m_firstHref(false),
      m_footnoteVisible(true),
      m_historyIndex(-1),
      m_key(nullptr),
      m_keyNameChooser(nullptr),
      m_magView(false),
      m_moduleTextModel(new BtModuleTextModel(this)),
      m_textModel(new RoleItemModel()),
      m_verseKeyChooser(nullptr) {

    m_prompt = tr("Select a reference.");

    BT_CONNECT(CSwordBackend::instance(),
               SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
               this,
               SLOT(reloadModules(CSwordBackend::SetupChangedReason)));
}

BtWindowInterface::~BtWindowInterface() {

}

VerseChooser* BtWindowInterface::getVerseKeyChooser() {
    if (!m_verseKeyChooser) {
        ViewManager* viewManager = getViewManager();
        Q_ASSERT(viewManager);
        QtQuick2ApplicationViewer* viewer = viewManager->getViewer();
        m_verseKeyChooser = new VerseChooser(viewer, this);
        BT_CONNECT(m_verseKeyChooser, SIGNAL(referenceChanged()),
                   this,              SLOT(referenceChosen()));
    }
    return m_verseKeyChooser;
}

BookKeyChooser* BtWindowInterface::getBookKeyChooser() {
    if (!m_bookKeyChooser) {
        ViewManager* viewManager = getViewManager();
        Q_ASSERT(viewManager);
        QtQuick2ApplicationViewer* viewer = viewManager->getViewer();
        m_bookKeyChooser = new BookKeyChooser(viewer, this);
        BT_CONNECT(m_bookKeyChooser, SIGNAL(referenceChanged()),
                   this,             SLOT(referenceChosen()));
    }
    return m_bookKeyChooser;
}

KeyNameChooser* BtWindowInterface::getKeyNameChooser() {
    if (!m_keyNameChooser) {
        ViewManager* viewManager = getViewManager();
        Q_ASSERT(viewManager);
        QtQuick2ApplicationViewer* viewer = viewManager->getViewer();
        m_keyNameChooser = new KeyNameChooser(viewer, this);
        BT_CONNECT(m_keyNameChooser, SIGNAL(referenceChanged(int)),
                   this,             SLOT(referenceChosen(int)));
    }
    return m_keyNameChooser;
}

void BtWindowInterface::reloadModules(CSwordBackend::SetupChangedReason /* reason */ ) {
    //first make sure all used Sword modules are still present

    if (CSwordBackend::instance()->findModuleByName(m_moduleName)) {
        QString moduleName = m_moduleName;
        m_moduleName = "";
        setModuleName(moduleName);
        ;
    } else {
        // close window ?
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
    QString moduleName;
    if (m_key)
        moduleName = m_key->module()->name();
    return moduleName;
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
            QString title = m_moduleName + "   " + getReference();
            setReferencesViewTitle(title);
        }
    }
}

void BtWindowInterface::decodeLemmaMorph(const QString& attributes) {

    Rendering::ListInfoData infoList(Rendering::detectInfo(attributes));
    BtConstModuleList l;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(m_moduleName);
    if(m != nullptr)
        l.append(m);
    QString text = Rendering::formatInfo(infoList, l);
    QString lang;
    displayText(text, lang);
}

void BtWindowInterface::decodeFootnote(const QString& keyName, const QString& footnote) {

    QString text;
    CSwordModuleInfo * const module = CSwordBackend::instance()->findModuleByName(m_moduleName);
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
    if (moduleIsBibleOrCommentary(m_key->module())) {
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
        verseKey->setPosition(sword::TOP);
        emit referenceChange();
    }
}

void BtWindowInterface::setModuleName(const QString& moduleName) {
    if (m_key && m_moduleName == moduleName)
        return;
    if (moduleName.isEmpty())
        return;
    m_moduleName = moduleName;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
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

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey)
        treeKey->firstChild();

    QStringList moduleNames;
    moduleNames.append(moduleName);
    m_moduleTextModel->setModules(moduleNames);

    emit moduleChanged();
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

void BtWindowInterface::changeModule() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    ModuleChooser* dlg = new ModuleChooser(viewer, this);
    dlg->open();
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
    m_moduleTextModel->setTextFilter(&m_textFilter);
}

static void parseKey(CSwordTreeKey* currentKey, QStringList* keyPath, QStringList* children)
{
    if (currentKey == nullptr)
        return;

    CSwordTreeKey localKey(*currentKey);

    QString oldKey = localKey.key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        localKey.firstChild();
        oldKey = localKey.key();
    }

    QStringList siblings; //split up key
    if (!oldKey.isEmpty()) {
        siblings = oldKey.split('/', QString::SkipEmptyParts);
    }

    int depth = 0;
    int index = 0;
    localKey.root(); //start iteration at root node

    while ( localKey.firstChild() && (depth < siblings.count()) ) {
        QString key = localKey.key();
        index = (depth == 0) ? -1 : 0;

        bool found = false;
        do { //look for matching sibling
            ++index;
            found = (localKey.getLocalNameUnicode() == siblings[depth]);
        }
        while (!found && localKey.nextSibling());

        if (found)
            key = localKey.key(); //found: change key to this level
        else
            localKey.setKey(key); //not found: restore old key

        *keyPath << key;

        //last iteration: get child entries
        if (depth == siblings.count() - 1 && localKey.hasChildren()) {
            localKey.firstChild();
            ++depth;
            do {
                *children << localKey.getLocalNameUnicode();
            }
            while (localKey.nextSibling());
        }
        depth++;
    }
}

static QString getEnglishKey(CSwordKey* m_key) {
    sword::VerseKey * vk = dynamic_cast<sword::VerseKey*>(m_key);
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

void BtWindowInterface::saveWindowStateToConfig(int windowIndex) {
    const QString windowKey = QString::number(windowIndex);
    const QString windowGroup = "window/" + windowKey + '/';

    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    conf.setSessionValue("key", getEnglishKey(m_key));
    QStringList modules;
    QString moduleName = getModuleName();
    modules.append(moduleName);
    conf.setSessionValue("modules", modules);
    conf.endGroup();
}

void BtWindowInterface::changeReference() {
    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
    if (verseKey != nullptr) {
        getVerseKeyChooser()->open(verseKey);
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey != nullptr) {
        QStringList keyPath;
        QStringList children;
        parseKey(treeKey, &keyPath, &children);
        getBookKeyChooser()->open();
    }
    CSwordLDKey* lexiconKey = dynamic_cast<CSwordLDKey*>(m_key);
    if (lexiconKey != nullptr) {
        getKeyNameChooser()->open(m_moduleTextModel);
    }
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

void BtWindowInterface::referenceChosen(int index) {
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
} // end namespace
