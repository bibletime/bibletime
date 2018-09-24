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

#include "btsearchinterface.h"

#include <QDebug>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "mobile/ui/indexthread.h"
#include "util/btconnect.h"


namespace btm {
using CSMI = QList<CSwordModuleInfo const *>;

BtSearchInterface::BtSearchInterface(QObject* parent) :
    QObject(parent),
    m_searchType(AndType),
    m_wasCancelled(false) {
}

BtSearchInterface::~BtSearchInterface() {
}

static const CSwordModuleInfo* getModuleFromResults(const CSwordModuleSearch::Results& results, int index) {

    int moduleIndex = 0;
    Q_FOREACH(CSwordModuleInfo const * const module, results.keys()) {
        if (moduleIndex == index)
            return module;
        ++moduleIndex;
    }
    return nullptr;
}

bool BtSearchInterface::modulesAreIndexed() {
    QStringList moduleList =  m_moduleList.split(", ");
    CSMI modules = CSwordBackend::instance()->getConstPointerList(moduleList);
    CSMI unindexedModules = CSwordModuleSearch::unindexedModules(modules);
    if (unindexedModules.size() > 0)
        return false;
    return true;
}

void BtSearchInterface::slotModuleProgress(int value) {
    m_progressValue = value;
    emit progressValueChanged();
}

void BtSearchInterface::slotBeginModuleIndexing(const QString& moduleName) {
    m_progressText = tr("Indexing %1").arg(moduleName);
    emit progressTextChanged();
}
void BtSearchInterface::slotIndexingFinished() {
    emit indexingFinished();
}

void BtSearchInterface::cancel() {
    m_thread->stopIndex();
    m_wasCancelled = true;
}

bool BtSearchInterface::wasCanceled() {
    return m_wasCancelled;
}

enum TextRoles {
    TextRole = Qt::UserRole + 1,
    ValueRole = Qt::UserRole + 2
};

bool BtSearchInterface::indexModules() {
    QStringList moduleList =  m_moduleList.split(", ");
    CSMI modules = CSwordBackend::instance()->getConstPointerList(moduleList);
    bool success = true;
    m_wasCancelled = false;

    QList<CSwordModuleInfo *> nonIndexedModules;
    Q_FOREACH(CSwordModuleInfo const * const cm, modules) {
        if (cm->hasIndex())
            continue;
        CSwordModuleInfo *m = const_cast<CSwordModuleInfo*>(cm);
        nonIndexedModules.append(m);
    }
    m_thread = new IndexThread(nonIndexedModules);
    BT_CONNECT(m_thread, SIGNAL(indexingProgress(int)),
               this,      SLOT(slotModuleProgress(int)));
    BT_CONNECT(m_thread, SIGNAL(beginIndexingModule(QString const &)),
               this,     SLOT(slotBeginModuleIndexing(QString const &)));
    BT_CONNECT(m_thread, SIGNAL(indexingFinished()),
               this, SLOT(slotIndexingFinished()));

    m_thread->start();
    return success;
}

bool BtSearchInterface::performSearch() {

    setupSearchType();
    QString searchText = prepareSearchText(m_searchText);

    // Check that we have the indices we need for searching
    QStringList moduleList =  m_moduleList.split(", ");
    CSMI modules = CSwordBackend::instance()->getConstPointerList(moduleList);

    // Set the search options:
    CSwordModuleSearch searcher;
    searcher.setSearchedText(searchText);
    searcher.setModules(modules);
    searcher.resetSearchScope();

    searcher.startSearch();

    m_results = searcher.results();
    setupModuleModel(m_results);
    const CSwordModuleInfo* module = getModuleFromResults(m_results,0);
    setupReferenceModel(module, m_results.value(module));
    return true;
}

bool BtSearchInterface::haveReferences() {
    return m_referencesModel.rowCount() != 0;
}

void BtSearchInterface::setupSearchType() {
    if (m_findChoice == "and")
        m_searchType = AndType;
    else if (m_findChoice == "or")
        m_searchType = OrType;
    else
        m_searchType = FullType;
}

QString BtSearchInterface::prepareSearchText(const QString& orig) {
    static const QRegExp syntaxCharacters("[+\\-()!\"~]");
    static const QRegExp andWords("\\band\\b", Qt::CaseInsensitive);
    static const QRegExp orWords("\\bor\\b", Qt::CaseInsensitive);
    QString text("");
    if (m_searchType == AndType) {
        text = orig.simplified();
        text.remove(syntaxCharacters);
        text.replace(andWords, "\"and\"");
        text.replace(orWords, "\"or\"");
        text.replace(" ", " AND ");
    }
    if (m_searchType == OrType) {
        text = orig.simplified();
        text.remove(syntaxCharacters);
        text.replace(andWords, "\"and\"");
        text.replace(orWords, "\"or\"");
    }
    if (m_searchType == FullType) {
        text = orig;
    }
    return text;
}

void BtSearchInterface::setupModuleModel(const CSwordModuleSearch::Results & results) {
    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "text";
    roleNames[ValueRole] = "value";
    m_modulesModel.setRoleNames(roleNames);

    m_modulesModel.clear();
    Q_FOREACH(CSwordModuleInfo const * const m, results.keys()) {
        const int count = swordxx::ListKey(results.value(m)).getCount();
        QString moduleName = m->name();
        QString moduleEntry = moduleName + "(" +QString::number(count) + ")";

        QStandardItem* item = new QStandardItem();
        item->setData(moduleEntry, TextRole);
        item->setData(moduleName, ValueRole);
        m_modulesModel.appendRow(item);
    }
    modulesModelChanged();
}

/** Setups the list with the given module. */
void BtSearchInterface::setupReferenceModel(const CSwordModuleInfo *m,
                                            const swordxx::ListKey & results)
{
    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "text";
    roleNames[ValueRole] = "value";
    m_referencesModel.setRoleNames(roleNames);

    m_referencesModel.clear();
    if (!m) {
        haveReferencesChanged();
        return;
    }
    const int count = results.getCount();
    if (!count)
        return;

    for (int index = 0; index < count; index++) {
        QString reference = QString::fromUtf8(results.getElement(index)->getText());
        QStandardItem* item = new QStandardItem();
        item->setData(reference, TextRole);
        item->setData(reference, ValueRole);
        m_referencesModel.appendRow(item);
    }
    referencesModelChanged();
    haveReferencesChanged();
}

QString BtSearchInterface::getSearchText() const {
    return m_searchText;
}

QString BtSearchInterface::getFindChoice() const {
    return m_findChoice;
}

QString BtSearchInterface::getModuleList() const {
    return m_moduleList;
}

void BtSearchInterface::setSearchText(const QString& searchText) {
    m_searchText = searchText;
}

void BtSearchInterface::setFindChoice(const QString& findChoice) {
    m_findChoice = findChoice;
}

void BtSearchInterface::setModuleList(const QString& moduleList) {
    m_moduleList = moduleList;
}

QVariant BtSearchInterface::getModulesModel() {
    QVariant var;
    var.setValue(&m_modulesModel);
    return var;
}

QVariant BtSearchInterface::getReferencesModel() {
    QVariant var;
    var.setValue(&m_referencesModel);
    return var;
}

void BtSearchInterface::selectReferences(int moduleIndex) {
    const int count = m_results.count();
    if ( moduleIndex < 0 || moduleIndex >= count) {
        m_referencesModel.clear();
        haveReferencesChanged();
        return;
    }
    const CSwordModuleInfo* module = getModuleFromResults(m_results, moduleIndex);
    setupReferenceModel(module, m_results.value(module));
}

QString BtSearchInterface::getReference(int index) {
    const int count = m_referencesModel.rowCount();
    if ( index < 0 || index >= count)
        return QString();
    QString value = m_referencesModel.item(index,0)->data(ValueRole).toString();
    return value;
}

QString BtSearchInterface::getModuleName(int index) {
    const int count = m_modulesModel.rowCount();
    if ( index < 0 || index >= count)
        return QString();
    QString value = m_modulesModel.item(index,0)->data(ValueRole).toString();
    return value;
}

void BtSearchInterface::setSearchType(int searchType) {
    m_searchType = searchType;
}

qreal BtSearchInterface::progressValue() const {
    return m_progressValue;
}

QString BtSearchInterface::progressText() const {
    return m_progressText;
}


} // end namespace
