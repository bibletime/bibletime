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

public:
    ModuleInterface();
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

private:
    void getCategoriesAndLanguages();
    void setProperties(const QStringList& list);
    QString currentLanguage() const;
    QString currentCategory() const;

    QSet<QString> m_categories;
    QSet<QString> m_languages;
    RoleItemModel m_categoryModel;
    RoleItemModel m_languageModel;
    RoleItemModel m_worksModel;
    QList<CSwordModuleInfo*> m_modules;

};

} // end namespace

#endif
