

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

enum WorksRoles {
    TitleRole = Qt::UserRole + 1,
    DescriptionRole = Qt::UserRole + 2
};

static void setupTextModel(const QStringList& modelList, RoleItemModel* model) {
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

static void setupWorksModel(const QStringList& titleList,
                            const QStringList& descriptionList,
                            RoleItemModel* model) {
    Q_ASSERT(titleList.count() == descriptionList.count());

    QHash<int, QByteArray> roleNames;
    roleNames[TitleRole] =  "title";
    roleNames[DescriptionRole] = "desc";
    model->setRoleNames(roleNames);

    model->clear();
    for (int i=0; i< titleList.count(); ++i) {
        QStandardItem* item = new QStandardItem();
        QString title = titleList.at(i);
        item->setData(title, TitleRole);
        QString description = descriptionList.at(i);
        item->setData(description, DescriptionRole);
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
    setupTextModel(m_sourceList, &m_sourceModel);
}

void InstallManager::makeConnections()
{
    installManagerChooserObject_->disconnect();

    bool ok = connect(installManagerChooserObject_, SIGNAL(sourceChanged(int)),
                      this, SLOT(sourceIndexChanged(int)));
    Q_ASSERT(ok);

    ok = connect(installManagerChooserObject_, SIGNAL(categoryChanged(int)),
                      this, SLOT(categoryIndexChanged(int)));
    Q_ASSERT(ok);

    ok = connect(installManagerChooserObject_, SIGNAL(languageChanged(int)),
                      this, SLOT(languageIndexChanged(int)));
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
    installManagerChooserObject_->setProperty("worksModel", QVariant::fromValue(&m_worksModel));
    installManagerChooserObject_->setProperty("sourceIndex", 0);
    installManagerChooserObject_->setProperty("visible", true);
}

void InstallManager::sourceIndexChanged(int index)
{
    if (index < 0 || index >= m_sourceList.count())
        return;

    updateCategoryAndLanguageModels();
    updateWorksModel();
}

void InstallManager::categoryIndexChanged(int index)
{
    if (index < 0 || index >= m_categoryList.count())
        return;
    updateWorksModel();
}

void InstallManager::languageIndexChanged(int index)
{
    if (index < 0 || index >= m_languageList.count())
        return;
    updateWorksModel();
}

void InstallManager::updateCategoryAndLanguageModels()
{
    QString sourceName = getCurrentListItem("sourceIndex", m_sourceList);
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
    setupTextModel(m_categoryList, &m_categoryModel);
    setCurrentListItem("categoryIndex", m_categoryList, currentCategory);

    QString currentLanguage = getCurrentListItem("languageIndex", m_languageList);
    m_languageList = languages.toList();
    m_languageList.sort();
    setupTextModel(m_languageList, &m_languageModel);
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

void InstallManager::updateWorksModel()
{
    QString sourceName = getCurrentListItem("sourceIndex", m_sourceList);
    QString categoryName = getCurrentListItem("categoryIndex", m_categoryList);
    QString languageName = getCurrentListItem("languageIndex", m_languageList);

    sword::InstallSource source = BtInstallBackend::source(sourceName);
    CSwordBackend* backend = BtInstallBackend::backend(source);
    const QList<CSwordModuleInfo*> modules = backend->moduleList();

    m_worksTitleList.clear();
    m_worksDescList.clear();
    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        CSwordModuleInfo::Category category = module->category();
        //QString name = module->name();
        QString moduleCategoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString moduleLanguageName = language->englishName();
        if (moduleCategoryName == categoryName &&
            moduleLanguageName == languageName ) {
            m_worksTitleList.append(module->name());
            QString description = module->config(CSwordModuleInfo::Description);
            QString version = module->config(CSwordModuleInfo::ModuleVersion);
            QString info = description + ": " + version;
            m_worksDescList.append(info);
        }
    }
    setupWorksModel(m_worksTitleList, m_worksDescList, &m_worksModel);
}

} // end namespace
