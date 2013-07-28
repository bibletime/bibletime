
#include "moduleinterface.h"

#include "qtquick2applicationviewer.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/managers/cswordbackend.h"
#include "mobile/util/findqmlobject.h"
#include <cmath>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include <QQmlProperty>
#include <QCoreApplication>
#include "btwindowinterface.h"
#include "gridchooser.h"

namespace btm {

ModuleInterface::ModuleInterface() {
}

enum TextRoles {
    TextRole = Qt::UserRole + 1
};

typedef BtBookshelfModel::ModuleRole MRole;
static const MRole HR(BtBookshelfModel::ModuleHiddenRole);
static const MRole PR(BtBookshelfModel::ModulePointerRole);
static const MRole IR(BtBookshelfModel::ModuleHasIndexRole);
static const MRole CR(BtBookshelfModel::ModuleCategoryRole);

static void setupTextModel(const QSet<QString>& modelSet, RoleItemModel* model) {

    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "modelText";
    model->setRoleNames(roleNames);

    QStringList modelList = modelSet.toList();
    modelList.sort();

    model->clear();
    for (int i=0; i< modelList.count(); ++i) {
        QString source = modelList.at(i);
        QStandardItem* item = new QStandardItem();
        item->setData(source, TextRole);
        model->appendRow(item);
    }
}

static CSwordModuleInfo* getModule(BtBookshelfModel* bookshelfModel, const QModelIndex& index) {
    QVariant var = bookshelfModel->data(index, PR);
    CSwordModuleInfo* module = static_cast<CSwordModuleInfo*>(var.value<void*>());
    return module;
}

void ModuleInterface::updateCategoryAndLanguageModels() {
    QQuickItem* object = findQmlObject("moduleChooser");
    if (object == 0)
        return;

    getCategoriesAndLanguages();
    setupTextModel(m_categories, &m_categoryModel);
    setupTextModel(m_languages, &m_languageModel);
    object->setProperty("categoryModel", QVariant::fromValue(&m_categoryModel));
    object->setProperty("languageModel", QVariant::fromValue(&m_languageModel));
}

void ModuleInterface::getCategoriesAndLanguages() {

    m_categories.clear();
    m_languages.clear();

    QQuickItem* object = findQmlObject("moduleChooser");
    if (object == 0)
        return;
    QString categoryLimit = object->property("categoryLimit").toString();

    BtBookshelfModel* bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel == 0)
        return;
    int count = bookshelfModel->rowCount();
    for (int row=0; row<count; ++row) {
        QModelIndex index = bookshelfModel->index(row);
        CSwordModuleInfo* module = getModule(bookshelfModel, index);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->translatedName();

        QStringList categoryLimitList = categoryLimit.split(",");

        if ( ! categoryLimit.isEmpty() && ! categoryLimitList.contains(categoryName))
            continue;

        m_categories.insert(categoryName);
        m_languages.insert(languageName);
    }
}

void ModuleInterface::updateWorksModel() {
    m_worksModel.clear();
    m_modules.clear();

    QString currentLang = currentLanguage();
    QString currentCat = currentCategory();

    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "modelText";
    m_worksModel.setRoleNames(roleNames);

    BtBookshelfModel* bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel == 0)
        return;
    int count = bookshelfModel->rowCount();
    for (int row=0; row<count; ++row) {
        QModelIndex index = bookshelfModel->index(row);
        CSwordModuleInfo* module = getModule(bookshelfModel, index);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->translatedName();
        if (languageName == currentLang &&
                categoryName == currentCat) {
            m_modules << module;
            QString moduleName = module->name();
            QStandardItem* item = new QStandardItem();
            item->setData(moduleName, TextRole);
            m_worksModel.appendRow(item);
        }
    }

    QQuickItem* object = findQmlObject("moduleChooser");
    if (object == 0)
        return;
    object->setProperty("worksModel", QVariant::fromValue(&m_worksModel));
}

QString ModuleInterface::currentLanguage() const {
    QQuickItem* object = findQmlObject("moduleChooser");
    if (object == 0)
        return "";
    int row = object->property("languageIndex").toInt();
    QModelIndex modelIndex = m_languageModel.index(row,0);
    QString language = modelIndex.data(TextRole).toString();
    return language;

}

QString ModuleInterface::currentCategory() const {
    QQuickItem* object = findQmlObject("moduleChooser");
    if (object == 0)
        return "";
    int row = object->property("categoryIndex").toInt();
    QModelIndex modelIndex = m_categoryModel.index(row,0);
    QString category = modelIndex.data(TextRole).toString();
    return category;
}

QString ModuleInterface::category(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == 0)
        return "";
    CSwordModuleInfo::Category category = module->category();
    if (category == 0)
        return "";
    return module->categoryName(category);
}

QString ModuleInterface::language(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == 0)
        return "";
    const CLanguageMgr::Language* language = module->language();
    if (language == 0)
        return "";
    return language->translatedName();
}

QString ModuleInterface::module(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == 0)
        return "";
    return module->name();
}

} // end namespace
