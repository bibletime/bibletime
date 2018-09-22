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

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include "mobile/models/roleitemmodel.h"
#include <QObject>
#include <QList>
#include <QSet>
#include <QString>

class QQuickItem;
class QStringList;
class CSwordModuleInfo;

namespace btm {

class GridChooser;

class BtWindowInterface;

class ModuleInterface : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariant categoryModel READ categoryModel NOTIFY categoryModelChanged)
    Q_PROPERTY(QVariant languageModel READ languageModel NOTIFY languageModelChanged)
    Q_PROPERTY(QVariant worksModel    READ worksModel    NOTIFY worksModelChanged)

public:
    ModuleInterface();
    Q_INVOKABLE void setBibleCommentaryOnly(bool value);
    Q_INVOKABLE void updateCategoryAndLanguageModels();
    Q_INVOKABLE void updateWorksModel();
    Q_INVOKABLE QString category(int index);
    Q_INVOKABLE QString englishCategory(int index);
    Q_INVOKABLE QString language(int index);
    Q_INVOKABLE QString module(int index);
    Q_INVOKABLE bool isLocked(const QString& moduleName);
    Q_INVOKABLE void unlock(const QString& moduleName, const QString& unlockKey);
    Q_INVOKABLE QStringList installedModuleLanguages();
    Q_INVOKABLE QString getFontNameForLanguage(const QString& language);
    Q_INVOKABLE qreal getFontSizeForLanguage(const QString& language);
    Q_INVOKABLE void setFontForLanguage(const QString& language, const QString& fontName, qreal fontSize);
    Q_INVOKABLE void saveCurrentFonts();
    Q_INVOKABLE void restoreSavedFonts();
    Q_INVOKABLE void setCurrentLanguage(int index);
    Q_INVOKABLE void setCurrentCategory(int index);

    QVariant categoryModel();
    QVariant languageModel();
    QVariant worksModel();

signals:
    void categoryModelChanged();
    void languageModelChanged();
    void worksModelChanged();
    void requestCurrentLanguage();
    void requestCurrentCategory();

private:
    void getCategoriesAndLanguages();
    void setProperties(const QStringList& list);

    bool m_bibleCommentaryOnly;
    QSet<QString> m_categories;
    QSet<QString> m_languages;
    RoleItemModel m_categoryModel;
    RoleItemModel m_languageModel;
    RoleItemModel m_worksModel;
    QList<CSwordModuleInfo*> m_modules;
    QString m_currentLanguage;
    QString m_currentCategory;

};

} // end namespace

#endif
