/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef INSTALL_MODULE_CHOOSER_H
#define INSTALL_MODULE_CHOOSER_H

#include "installsourcesmanager.h"
#include "mobile/models/roleitemmodel.h"
#include "installprogress.h"
#include <QList>
#include <QMap>
#include <QObject>

class QQuickItem;
class CSwordModuleInfo;
class CSwordBackend;
class BtInstallMgr;

namespace btm {

class InstallManager :public QObject {
    Q_OBJECT

public:
    InstallManager(QObject* parent = nullptr);

    Q_PROPERTY(int installedModuleCount READ getInstalledModuleCount)

    Q_INVOKABLE void openChooser();

    int getInstalledModuleCount() const;

private slots:
    void cancel();
    void categoryIndexChanged(int index);
    void installRemove();
    void languageIndexChanged(int index);
    void refreshLists();
    void sourceIndexChanged(int index);
    void updateModels();
    void workSelected(int index);

private:
    QString getCurrentListItem(const char* propertyName,
                               const QStringList& list);
    void findInstallManagerObject();
    void findProgressObject();
    void installModules(const QList<CSwordModuleInfo*>& modules);
    void makeConnections();
    void removeModules(const QList<CSwordModuleInfo*>& modules);
    void setProperties();
    void setupSourceModel();
    void setCurrentListItem(const char* propertyName,
                            const QStringList& list,
                            const QString& itemName);
    void updateCategoryAndLanguageModels();
    void updateSwordBackend();
    void updateWorksModel();

    QQuickItem* m_installManagerChooserObject;
    QQuickItem* m_progressObject;
    BtInstallMgr* m_btInstallMgr;
    InstallSourcesManager m_installSourcesManager;
    CSwordBackend* m_backend;


    InstallProgress m_installProgress;
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
    QList<CSwordModuleInfo*> m_modulesToRemove;
    QList<CSwordModuleInfo*> m_modulesToInstall;
};

} // end namespace
#endif
