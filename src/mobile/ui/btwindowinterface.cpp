/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btwindowinterface.h"

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/managers/cswordbackend.h"
#include "backend/models/btmoduletextmodel.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cdisplayrendering.h"
#include "mobile/btmmain.h"
#include "mobile/keychooser/bookkeychooser.h"
#include "mobile/keychooser/keynamechooser.h"
#include "mobile/keychooser/versechooser.h"
#include "mobile/ui/modulechooser.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QStringList>
#include <swkey.h>


namespace btm {

BtWindowInterface::BtWindowInterface(QObject* parent)
    : QObject(parent),
      m_key(nullptr),
      m_textModel(new RoleItemModel()),
      m_moduleTextModel(new BtModuleTextModel(this)),
      m_bookKeyChooser(nullptr),
      m_keyNameChooser(nullptr),
      m_verseKeyChooser(nullptr),
      m_historyIndex(-1) {

    ViewManager* viewManager = getViewManager();
    if (viewManager == nullptr)
        return;
    QtQuick2ApplicationViewer* viewer = viewManager->getViewer();
    m_verseKeyChooser = new VerseChooser(viewer, this);

    bool ok = connect(m_verseKeyChooser, SIGNAL(referenceChanged()), this, SLOT(referenceChosen()));
    Q_ASSERT(ok);

    m_bookKeyChooser = new BookKeyChooser(viewer, this);
    ok = connect(m_bookKeyChooser, SIGNAL(referenceChanged()), this, SLOT(referenceChosen()));
    Q_ASSERT(ok);

    m_keyNameChooser = new KeyNameChooser(viewer, this);
    ok = connect(m_keyNameChooser, SIGNAL(referenceChanged(int)), this, SLOT(referenceChosen(int)));
    Q_ASSERT(ok);

    ok = connect(CSwordBackend::instance(),
            SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
                 this,
            SLOT(reloadModules(CSwordBackend::SetupChangedReason)));
    Q_ASSERT(ok);
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

void BtWindowInterface::setReference(const QString& key) {
    if (m_key && m_key->key() == key)
        return;
    if (m_key) {
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
        if (verseKey)
            verseKey->setIntros(true);
        m_key->setKey(key);
        referenceChanged();
    }
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
        m_verseKeyChooser->open(verseKey);
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey != nullptr) {
        QStringList keyPath;
        QStringList children;
        parseKey(treeKey, &keyPath, &children);
        m_bookKeyChooser->open();
    }
    CSwordLDKey* lexiconKey = dynamic_cast<CSwordLDKey*>(m_key);
    if (lexiconKey != nullptr) {
        m_keyNameChooser->open(m_moduleTextModel);
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

} // end namespace
