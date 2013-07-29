#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include "mobile/ui/roleitemmodel.h"
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
    Q_INVOKABLE QString language(int index);
    Q_INVOKABLE QString module(int index);

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
