#ifndef INSTALL_MODULE_CHOOSER_H
#define INSTALL_MODULE_CHOOSER_H

#include <QObject>
#include "mobile/ui/roleitemmodel.h"

class QQuickItem;

namespace btm {

class InstallManager :public QObject {
    Q_OBJECT

public:
    InstallManager(QObject* parent = 0);

    Q_INVOKABLE void openChooser();

private slots:
    void categoryIndexChanged(int index);
    void languageIndexChanged(int index);
    void sourceIndexChanged(int index);

private:
    QString getCurrentListItem(const char* propertyName,
                               const QStringList& list);
    void findInstallManagerObject();
    void makeConnections();
    void setProperties();
    void setupSourceModel();
    void setCurrentListItem(const char* propertyName,
                            const QStringList& list,
                            const QString& itemName);
    void updateCategoryAndLanguageModels();
    void updateWorksModel();

    QQuickItem* installManagerChooserObject_;

    QStringList m_sourceList;
    QStringList m_categoryList;
    QStringList m_languageList;
    QStringList m_worksTitleList;
    QStringList m_worksDescList;

    RoleItemModel m_sourceModel;
    RoleItemModel m_categoryModel;
    RoleItemModel m_languageModel;
    RoleItemModel m_worksModel;
};

} // end namespace
#endif
