

#include "installmanager.h"

#include "backend/btinstallbackend.h"
#include "backend/managers/clanguagemgr.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QQuickItem>

namespace btm {

enum TextRoles {
    TextRole = Qt::UserRole + 1
};

static void setupModel(const QStringList& modelList, RoleItemModel* model) {
    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "modelText";
    model->setRoleNames(roleNames);

    model->clear();
    for (int i=0; i< modelList.count(); ++i) {
        QString source = modelList.at(i);
        QStandardItem* item = new QStandardItem();
        item->setData(source, TextRole);
        model->appendRow(item);
    }
}

InstallManager::InstallManager(QObject* parent)
    : installManagerChooserObject_(0) {
}

void InstallManager::openChooser() {

    if (installManagerChooserObject_ == 0)
        findInstallManagerObject();
    if (installManagerChooserObject_ == 0)
        return;

    setupSourceModel();
    makeConnections();
    setProperties();
    sourceIndexChanged(0);
}

void InstallManager::findInstallManagerObject() {

    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = 0;
    if (viewer != 0)
        rootObject = viewer->rootObject();
    if (rootObject != 0)
        installManagerChooserObject_ = rootObject->findChild<QQuickItem*>("installManagerChooser");
}

void InstallManager::setupSourceModel() {
    m_sourceList = BtInstallBackend::sourceNameList();
    setupModel(m_sourceList, &m_sourceModel);
}

void InstallManager::makeConnections()
{
    installManagerChooserObject_->disconnect();
    bool ok = connect(installManagerChooserObject_, SIGNAL(sourceChanged(int)),
                      this, SLOT(sourceIndexChanged(int)));
    Q_ASSERT(ok);
}

void InstallManager::setProperties() {

    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    if (viewer == 0 || installManagerChooserObject_ == 0)
        return;

    QQmlContext* ctx = viewer->rootContext();
    installManagerChooserObject_->setProperty("sourceModel", QVariant::fromValue(&m_sourceModel));
    installManagerChooserObject_->setProperty("categoryModel", QVariant::fromValue(&m_categoryModel));
    installManagerChooserObject_->setProperty("languageModel", QVariant::fromValue(&m_languageModel));
    installManagerChooserObject_->setProperty("sourceIndex", 0);
    installManagerChooserObject_->setProperty("visible", true);
}

void InstallManager::sourceIndexChanged(int index)
{
    if (index < 0 || index >= m_sourceList.count())
        return;

    QString sourceName = m_sourceList.at(index);
    updateCategoryAndLanguageModels(sourceName);
}

void InstallManager::updateCategoryAndLanguageModels(const QString& sourceName)
{
    sword::InstallSource source = BtInstallBackend::source(sourceName);
    CSwordBackend* backend = BtInstallBackend::backend(source);
    const QList<CSwordModuleInfo*> modules = backend->moduleList();

    QSet<QString> categories;
    QSet<QString> languages;
    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        CSwordModuleInfo::Category category = module->category();
        //        QString name = module->name();
        QString categoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->englishName();
        categories.insert(categoryName);
        languages.insert(languageName);
    }

    QString currentCategory = getCurrentListItem("categoryIndex", m_categoryList);
        m_categoryList = categories.toList();
    m_categoryList.sort();
    setupModel(m_categoryList, &m_categoryModel);
    setCurrentListItem("categoryIndex", m_categoryList, currentCategory);

    QString currentLanguage = getCurrentListItem("languageIndex", m_languageList);
    m_languageList = languages.toList();
    m_languageList.sort();
    setupModel(m_languageList, &m_languageModel);
    setCurrentListItem("languageIndex", m_languageList, currentLanguage);
}

QString InstallManager::getCurrentListItem(const char* propertyName, const QStringList& list) {
    QString value;
    QVariant vIndex = installManagerChooserObject_->property(propertyName);
    bool ok;
    int index = vIndex.toInt(&ok);
    if (ok) {
        if (index >= 0 && index < list.count())
            value = list.at(index);
    }
    return value;
}

void InstallManager::setCurrentListItem(const char* propertyName,
                                              const QStringList& list,
                                              const QString& itemName) {
    int  index = list.indexOf(itemName);
    if (index < 0)
        index = 0;
    installManagerChooserObject_->setProperty(propertyName, index);
}

} // end namespace
