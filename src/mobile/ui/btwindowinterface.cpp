
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
#include <QStringList>


namespace btm {

BtWindowInterface::BtWindowInterface(QObject* parent)
    : QObject(parent),
      m_key(0) {
}

static void outputText(const QString& text, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << text;
    file.close();
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

QString BtWindowInterface::getText() const {
    QString moduleName= getModuleName();
    QStringList moduleList = QStringList() << moduleName;

    QList<const CSwordModuleInfo*> modules =
        CSwordBackend::instance()->getConstPointerList(moduleList);
    Rendering::CEntryDisplay* display = modules.first()->getDisplay();

    QString key = getKeyText(m_key);

    QString anchor = Rendering::CDisplayRendering::keyToHTMLAnchor(key);

    QString gotoFunction = "function gotoAnchor(anchor) {document.location=document.location + \"#\" + anchor;}";
    QString beginScript = "<script  type=\"text/javascript\">";
    QString endScript = "</script>";
    QString script = beginScript + gotoFunction + endScript;
    QString endHeader = "</head>";
    QString body = "<body";
    QString bodyGoto = "<body onload=\"gotoAnchor('" +anchor + "')\"  ";

//    BtConfig & conf = btConfig();
//    conf.beginGroup(windowGroup);
    FilterOptions filterOptions;// = conf.getFilterOptions();
    DisplayOptions displayOptions;// = conf.getDisplayOptions();
    displayOptions.verseNumbers =1;
    displayOptions.lineBreaks = 1;
//    conf.endGroup();

    QString text;
    if (display) {
        text = display->text(modules, key, displayOptions, filterOptions);
    }

    text.replace(endHeader, script + endHeader);
    text.replace(body, bodyGoto);

    outputText(text, "out.html");

//    qDebug() << text;
    return text;
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

////    BtConfig & conf = btConfig();
////    conf.beginGroup(windowGroup);
//    FilterOptions filterOptions;// = conf.getFilterOptions();
//    DisplayOptions displayOptions;// = conf.getDisplayOptions();
////    conf.endGroup();

    emit moduleChanged();
    emit displayedChanged();
    emit textChanged();
}

QString BtWindowInterface::getDisplayed() const {
    QString displayed;
    if (m_key)
        displayed = m_key->key();
    return displayed;
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
    const int oldOffset = localKey.getOffset(); //backup key position

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
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();

    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(m_key);
    if (verseKey != 0) {
        VerseChooser* dlg = new VerseChooser(viewer, this);
        bool ok = connect(dlg, SIGNAL(referenceChanged()), this, SLOT(referenceChanged()));
        Q_ASSERT(ok);
        dlg->open(verseKey);
    }

    CSwordTreeKey* treeKey = dynamic_cast<CSwordTreeKey*>(m_key);
    if (treeKey != 0) {
        QStringList keyPath;
        QStringList children;
        parseKey(treeKey, &keyPath, &children);
        int x = 0;
        BookKeyChooser* dlg = new BookKeyChooser(viewer, this);
        dlg->open();
    }
}

void BtWindowInterface::referenceChanged() {
    emit displayedChanged();
    emit textChanged();
}

void BtWindowInterface::setDisplayed(const QString& text) {
    emit displayedChanged();
    emit textChanged();
}

const CSwordModuleInfo* BtWindowInterface::module() const {
    const CSwordModuleInfo* module = m_key->module();
    return module;
}

CSwordKey* BtWindowInterface::getKey() const {
    return m_key;
}

} // end namespace
