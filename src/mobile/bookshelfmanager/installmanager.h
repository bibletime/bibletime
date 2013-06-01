#ifndef INSTALL_MODULE_CHOOSER_H
#define INSTALL_MODULE_CHOOSER_H

#include "mobile/ui/roleitemmodel.h"
#include "installprogress.h"
#include <QMap>
#include <QObject>

class QQuickItem;
class CSwordModuleInfo;

namespace btm {

class InstallManager :public QObject {
    Q_OBJECT

public:
    InstallManager(QObject* parent = 0);

    Q_INVOKABLE void openChooser();

private slots:
    void cancel();
    void categoryIndexChanged(int index);
    void installRemove();
    void languageIndexChanged(int index);
    void sourceIndexChanged(int index);
    void workSelected(int index);

private:
    QString getCurrentListItem(const char* propertyName,
                               const QStringList& list);
    void findInstallManagerObject();
    void installModules(const QList<CSwordModuleInfo*>& modules);
    void makeConnections();
    void removeModules(const QList<CSwordModuleInfo*>& modules);
    void setProperties();
    void setupSourceModel();
    void setCurrentListItem(const char* propertyName,
                            const QStringList& list,
                            const QString& itemName);
    void updateCategoryAndLanguageModels();
    void updateWorksModel();

    QQuickItem* installManagerChooserObject_;
    InstallProgress installProgress_;

    QStringList m_sourceList;
    QStringList m_categoryList;
    QStringList m_languageList;
    QStringList m_worksTitleList;
    QStringList m_worksDescList;
    QList<CSwordModuleInfo*> m_worksList;
    QList<int> m_worksInstalledList;
    QMap<CSwordModuleInfo*, bool> m_modulesToInstallRemove;

    RoleItemModel m_sourceModel;
    RoleItemModel m_categoryModel;
    RoleItemModel m_languageModel;
    RoleItemModel m_worksModel;
};

} // end namespace
#endif
