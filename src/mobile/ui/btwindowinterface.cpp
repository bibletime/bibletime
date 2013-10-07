
#include "btwindowinterface.h"

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cdisplayrendering.h"
#include "mobile/btmmain.h"
#include "mobile/keychooser/versechooser.h"
#include "mobile/keychooser/bookkeychooser.h"
#include "mobile/ui/modulechooser.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QStringList>


namespace btm {

BtWindowInterface::BtWindowInterface(QObject* parent)
    : QObject(parent),
      m_key(0),
      m_textModel(new RoleItemModel()),
      m_bookKeyChooser(0),
      m_verseKeyChooser(0),
      m_bibleTextModelBuilder(m_textModel),
      m_bookTextModelBuilder(m_textModel){

    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    m_verseKeyChooser = new VerseChooser(viewer, this);
    bool ok = connect(m_verseKeyChooser, SIGNAL(referenceChanged()), this, SLOT(referenceChanged()));
    Q_ASSERT(ok);

    m_bookKeyChooser = new BookKeyChooser(viewer, this);
    ok = connect(m_bookKeyChooser, SIGNAL(referenceChanged()), this, SLOT(referenceChanged()));
    Q_ASSERT(ok);
}

static QString getKeyText(CSwordKey* key) {
    QString keyText;
    if ( ! key)
        return keyText;

    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(key);
    if (verseKey) {
        keyText = verseKey->key();
        return keyText;
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(key);
    if (treeKey) {
        keyText = treeKey->key();
        return keyText;
    }
    return keyText;
}

void BtWindowInterface::updateModel() {
    QString moduleName= getModuleName();
    QStringList moduleList = QStringList() << moduleName;
    QList<const CSwordModuleInfo*> modules =
        CSwordBackend::instance()->getConstPointerList(moduleList);
    QString keyText = getKeyText(m_key);

    m_textModel->clear();
    if (modules.at(0)->type() == CSwordModuleInfo::Bible)
        m_bibleTextModelBuilder.updateModel(modules, keyText);
    else if (modules.at(0)->type() == CSwordModuleInfo::GenericBook)
        m_bookTextModelBuilder.updateModel(modules, keyText);

    emit currentModelIndexChanged();
}

int BtWindowInterface::getCurrentModelIndex() const {
    return m_bibleTextModelBuilder.getCurrentModelIndex();
}

static bool moduleIsBibleOrCommentary(const CSwordModuleInfo* module) {
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Bibles ||
            category == CSwordModuleInfo::Commentaries)
        return true;
    return false;
}

static bool moduleIsBook(const CSwordModuleInfo* module) {
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Books)
        return true;
    return false;
}

QString BtWindowInterface::getModuleName() const {
    QString moduleName;
    if (m_key)
        moduleName = m_key->module()->name();
    return moduleName;
}

void BtWindowInterface::setModuleName(const QString& moduleName) {
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

static void parseKey(CSwordTreeKey* currentKey, QStringList* keyPath, QStringList* children)
{
    if (currentKey == 0)
        return;

    CSwordTreeKey localKey(*currentKey);

    QString oldKey = localKey.key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        localKey.firstChild();
        oldKey = localKey.key();
    }
//    const int oldOffset = localKey.getOffset(); //backup key position

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

void BtWindowInterface::changeReference() {
    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
    if (verseKey != 0) {
        m_verseKeyChooser->open(verseKey);
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey != 0) {
        QStringList keyPath;
        QStringList children;
        parseKey(treeKey, &keyPath, &children);
        m_bookKeyChooser->open();
    }
}

void BtWindowInterface::referenceChanged() {
    emit referenceChange();
    updateModel();
}

const CSwordModuleInfo* BtWindowInterface::module() const {
    const CSwordModuleInfo* module = m_key->module();
    return module;
}

CSwordKey* BtWindowInterface::getKey() const {
    return m_key;
}

int BtWindowInterface::getFontSize() const {
    const CLanguageMgr::Language* lang = module()->language();
    BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
    QFont font = fontPair.second;
    return font.pointSize();
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
    var.setValue(m_textModel);
    return var;
}

} // end namespace
