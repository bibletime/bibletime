
#include "btwindowinterface.h"

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cdisplayrendering.h"
#include "mobile/btmmain.h"
#include "mobile/keychooser/versechooser.h"
#include "mobile/ui/modulechooser.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QFile>
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

QString BtWindowInterface::getText() const {
    QString moduleName= getModuleName();
    QStringList moduleList = QStringList() << moduleName;

    QList<const CSwordModuleInfo*> modules =
        CSwordBackend::instance()->getConstPointerList(moduleList);
    Rendering::CEntryDisplay* display = modules.first()->getDisplay();

    QString key;
    if (m_key)
        key = m_key->key();

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

QString BtWindowInterface::getModuleName() const {
    QString moduleName;
    if (m_key)
        moduleName = m_key->module()->name();
    return moduleName;
}

void BtWindowInterface::setModuleName(const QString& moduleName) {
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!m_key) {
        CSwordKey* key = CSwordKey::createInstance(m);
        m_key = dynamic_cast<CSwordVerseKey*>(key);
    }
    else {
        m_key->setModule(m);
    }
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

void BtWindowInterface::changeReference() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();

    CSwordVerseKey* key = dynamic_cast<CSwordVerseKey*>(m_key);
    if (key != 0) {
        VerseChooser* dlg = new VerseChooser(viewer, this);
        bool ok = connect(dlg, SIGNAL(referenceChanged()), this, SLOT(referenceChanged()));
        Q_ASSERT(ok);
        dlg->open(key);
    }

}

void BtWindowInterface::referenceChanged() {
    emit displayedChanged();
    emit textChanged();
}

void BtWindowInterface::setDisplayed(const QString& text) {
    emit displayedChanged();
}

const CSwordBibleModuleInfo* BtWindowInterface::module() const {
    const CSwordModuleInfo* module = m_key->module();
    const CSwordBibleModuleInfo* bibleModule = qobject_cast<const CSwordBibleModuleInfo*>(module);
    return bibleModule;
}

} // end namespace
