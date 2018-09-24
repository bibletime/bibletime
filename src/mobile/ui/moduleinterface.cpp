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

#include "moduleinterface.h"

#include "backend/config/btconfig.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordversekey.h"
#include "mobile/btmmain.h"
#include <cmath>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include <QQmlProperty>
#include <QCoreApplication>
#include "btwindowinterface.h"

namespace btm {

struct FontSettings {
    QString language;
    QString fontName;
    qreal fontSize;
};

static QList<FontSettings> savedFontSettings;

ModuleInterface::ModuleInterface() :
    m_bibleCommentaryOnly(false) {
}

enum TextRoles {
    TextRole = Qt::UserRole + 1
};

using MRole = BtBookshelfModel::ModuleRole;
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

void ModuleInterface::setBibleCommentaryOnly(bool value) {
    m_bibleCommentaryOnly = value;
}

void ModuleInterface::updateCategoryAndLanguageModels() {
    getCategoriesAndLanguages();
    setupTextModel(m_categories, &m_categoryModel);
    setupTextModel(m_languages, &m_languageModel);
    updateWorksModel();
}

void ModuleInterface::getCategoriesAndLanguages() {

    m_categories.clear();
    m_languages.clear();

    BtBookshelfModel* bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel == nullptr)
        return;
    int count = bookshelfModel->rowCount();
    for (int row=0; row<count; ++row) {
        QModelIndex index = bookshelfModel->index(row);
        CSwordModuleInfo* module = getModule(bookshelfModel, index);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        if (m_bibleCommentaryOnly) {
            if ((categoryName != "Bibles") && (categoryName != "Commentaries"))
                continue;
        }
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->translatedName();
        m_categories.insert(categoryName);
        m_languages.insert(languageName);
    }
}

QStringList ModuleInterface::installedModuleLanguages() {
    QStringList languages;
    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();
    for (auto lang: langMap) {
        languages << lang->englishName();
    }
    return languages;
}


void ModuleInterface::updateWorksModel() {
    m_worksModel.clear();
    m_modules.clear();
    requestCurrentCategory();
    requestCurrentLanguage();
    QHash<int, QByteArray> roleNames;
    roleNames[TextRole] =  "modelText";
    m_worksModel.setRoleNames(roleNames);
    BtBookshelfModel* bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel == nullptr)
        return;
    int count = bookshelfModel->rowCount();
    for (int row=0; row<count; ++row) {
        QModelIndex index = bookshelfModel->index(row);
        CSwordModuleInfo* module = getModule(bookshelfModel, index);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->translatedName();
        if (languageName == m_currentLanguage &&
                categoryName == m_currentCategory) {
            m_modules << module;
            QString moduleName = module->name();
            QStandardItem* item = new QStandardItem();
            item->setData(moduleName, TextRole);
            m_worksModel.appendRow(item);
        }
    }
}

QString ModuleInterface::category(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == nullptr)
        return "";
    CSwordModuleInfo::Category category = module->category();
    if (category == 0)
        return "";
    return module->categoryName(category);
}

QString ModuleInterface::englishCategory(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == nullptr)
        return "";
    CSwordModuleInfo::Category category = module->category();
    if (category == 0)
        return "";
    return module->englishCategoryName(category);
}

QString ModuleInterface::language(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == nullptr)
        return "";
    const CLanguageMgr::Language* language = module->language();
    if (language == nullptr)
        return "";
    return language->translatedName();
}

QString ModuleInterface::module(int index) {
    if (index < 0 || index >= m_modules.count())
        return "";
    CSwordModuleInfo* module = m_modules.at(index);
    if (module == nullptr)
        return "";
    return module->name();
}

bool ModuleInterface::isLocked(const QString& moduleName) {
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (module) {

        // Verse intros must be false for checking lock
        if (module->type() == CSwordModuleInfo::Bible ||
                module->type() == CSwordModuleInfo::Commentary) {
                ((swordxx::VerseKey*)(module->module().getKey()))->setIntros(false);
        }

        bool locked = module->isLocked();
        return locked;
    }
    return false;
}

void ModuleInterface::unlock(const QString& moduleName, const QString& unlockKey) {
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (module) {
        module->unlock(unlockKey);

        // Re-initialize module pointers:
        CSwordBackend *backend = CSwordBackend::instance();
        backend->reloadModules(CSwordBackend::OtherChange);
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        updateWorksModel();
    }
}

static const CLanguageMgr::Language* getLanguageFromEnglishName(const QString& name) {
    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();
    for (auto l: langMap) {
        if (l->englishName() == name)
            return l;
    }
    return nullptr;
 }

QString ModuleInterface::getFontNameForLanguage(const QString& language)
{
    auto lang = getLanguageFromEnglishName(language);
    if (lang) {
        BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
        if (fontPair.first) {
            QFont font = fontPair.second;
            QString fontFamily = font.family();
            return fontFamily;
        }
    }
    QString fontFamily = getDefaultFont().family();
    return fontFamily;
}

qreal ModuleInterface::getFontSizeForLanguage(const QString& language)
{
    auto lang = getLanguageFromEnglishName(language);
    if (lang) {
        BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
        if (fontPair.first) {
            QFont font = fontPair.second;
            int fontPointSize = font.pointSize();
            return fontPointSize;
        }
    }
    qreal pointSize = getDefaultFont().pointSizeF();
    return pointSize;
}

void ModuleInterface::setFontForLanguage(const QString& language, const QString& fontName, qreal fontSize) {
    if (CLanguageMgr::instance() == nullptr)
        return;
    auto lang = getLanguageFromEnglishName(language);
    if (lang) {

        QFont font;
        font.setFamily(fontName);
        font.setPointSizeF(fontSize);
        BtConfig::FontSettingsPair fontPair;
        fontPair.first = true;
        fontPair.second = font;
        btConfig().setFontForLanguage(*lang, fontPair);
    }
}

void ModuleInterface::saveCurrentFonts() {
    savedFontSettings.clear();
    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();
    for (auto lang: langMap) {
        BtConfig::FontSettingsPair fontPair = btConfig().getFontForLanguage(*lang);
        if (fontPair.first) {
            FontSettings fontSettings;
            QFont font = fontPair.second;
            fontSettings.language = lang->englishName();
            fontSettings.fontName = font.family();
            fontSettings.fontSize = font.pointSize();
            savedFontSettings.append(fontSettings);
        }
    }
}

void ModuleInterface::restoreSavedFonts() {
    for (auto fontSettings : savedFontSettings) {
        setFontForLanguage(fontSettings.language, fontSettings.fontName, fontSettings.fontSize);
    }
}

QVariant ModuleInterface::categoryModel() {
    QVariant var;
    var.setValue(&m_categoryModel);
    return var;
}

QVariant ModuleInterface::languageModel() {
    QVariant var;
    var.setValue(&m_languageModel);
    return var;
}

QVariant ModuleInterface::worksModel() {
    QVariant var;
    var.setValue(&m_worksModel);
    return var;
}

void ModuleInterface::setCurrentLanguage(int row) {
    QModelIndex index = m_languageModel.index(row,0);
    QVariant v = index.data(TextRole);
    m_currentLanguage = v.toString();
}

void ModuleInterface::setCurrentCategory(int row) {
    QModelIndex index = m_categoryModel.index(row, 0);
    QVariant v = index.data(TextRole);
    m_currentCategory = v.toString();
}

} // end namespace
