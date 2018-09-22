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

#ifndef INSTALL_INTERFACE_H
#define INSTALL_INTERFACE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include "mobile/models/roleitemmodel.h"

class BtInstallThread;
class CSwordBackend;
class CSwordModuleInfo;

namespace btm {

class InstallSources;

class InstallInterface : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariant sourceModel   READ sourceModel   NOTIFY sourceModelChanged)
    Q_PROPERTY(QVariant categoryModel READ categoryModel NOTIFY categoryModelChanged)
    Q_PROPERTY(QVariant languageModel READ languageModel NOTIFY languageModelChanged)
    Q_PROPERTY(QVariant worksModel    READ worksModel    NOTIFY worksModelChanged)

    Q_PROPERTY(bool progressVisible READ progressVisible NOTIFY progressVisibleChanged)
    Q_PROPERTY(qreal progressMin    READ progressMin     NOTIFY progressMinChanged)
    Q_PROPERTY(qreal progressMax    READ progressMax     NOTIFY progressMaxChanged)
    Q_PROPERTY(qreal progressValue  READ progressValue   NOTIFY progressValueChanged)
    Q_PROPERTY(QString progressText READ progressText    NOTIFY progressTextChanged)

    Q_PROPERTY(bool wasCanceled READ getWasCanceled NOTIFY wasCanceledChanged)

public:
    InstallInterface();

    Q_INVOKABLE void setup();

    Q_INVOKABLE void updateCategoryModel();
    Q_INVOKABLE void updateLanguageModel(const QString& currentCategory);
    Q_INVOKABLE void updateWorksModel(
            const QString& sourceName,
            const QString& categoryName,
            const QString& languageName);

    Q_INVOKABLE void updateSwordBackend(const QString& sourceName);
    Q_INVOKABLE int installedModulesCount();

    Q_INVOKABLE QString getSource(int index);
    Q_INVOKABLE QString getCategory(int index);
    Q_INVOKABLE QString getLanguage(int index);

    Q_INVOKABLE QString getSourceSetting();
    Q_INVOKABLE QString getCategorySetting();
    Q_INVOKABLE QString getLanguageSetting();

    Q_INVOKABLE void setSourceSetting(const QString& source);
    Q_INVOKABLE void setCategorySetting(const QString& category);
    Q_INVOKABLE void setLanguageSetting(const QString& language);

    Q_INVOKABLE int searchSource(const QString& value);
    Q_INVOKABLE int searchCategory(const QString& value);
    Q_INVOKABLE int searchLanguage(const QString& value);

    Q_INVOKABLE void installRemove();
    Q_INVOKABLE void refreshLists(
            const QString& source,
            const QString& category,
            const QString& language);

    Q_INVOKABLE void clearModules();
    Q_INVOKABLE void addModule(const QString& sourceName, const QString& moduleName);
    Q_INVOKABLE void installModulesAuto();

    Q_INVOKABLE void workSelected(int index);
    Q_INVOKABLE void cancel();

    QVariant sourceModel();
    QVariant categoryModel();
    QVariant languageModel();
    QVariant worksModel();

    bool progressVisible() const;
    qreal progressMin() const;
    qreal progressMax() const;
    qreal progressValue() const;
    QString progressText() const;

    void setProgressVisible(bool value);
    void setProgressMin(qreal value);
    void setProgressMax(qreal value);
    void setProgressValue(qreal value);
    void setProgressText(const QString& value);

    bool getWasCanceled();

signals:
    void sourceModelChanged();
    void categoryModelChanged();
    void languageModelChanged();
    void worksModelChanged();

    void progressVisibleChanged();
    void progressMinChanged();
    void progressMaxChanged();
    void progressValueChanged();
    void progressTextChanged();
    void updateCurrentViews(
            const QString& source,
            const QString& category,
            const QString& language);
    void progressFinished();
    void modulesDownloadFinished();
    void wasCanceledChanged();

private slots:
    void slotStopInstall();
    void slotInstallStarted(int moduleIndex);
    void slotDownloadStarted(int moduleIndex);
    void slotStatusUpdated(int moduleIndex, int status);
    void slotOneItemCompleted(int moduleIndex, bool status);
    void slotThreadFinished();
    void slotPercentComplete(int percent, const QString& title);


private:
    QString getModuleName(int moduleIndex);
    QString getSourcePath();
    void installModules();
    void removeModules();
    void runThread();
    void setupSourceModel();

    CSwordBackend* m_backend;
    BtInstallThread* m_thread;
    InstallSources* m_worker;
    bool m_wasCanceled;
    int m_nextInstallIndex;
    bool m_progressVisible;
    qreal m_progressMin;
    qreal m_progressMax;
    qreal m_progressValue;

    QString m_progressText;

    QString m_tempSource;
    QString m_tempCategory;
    QString m_tempLanguage;

    QStringList m_sourceList;
    QStringList m_categoryList;
    QStringList m_languageList;
    QStringList m_worksTitleList;
    QStringList m_worksDescList;

    QList<CSwordModuleInfo*> m_worksList;
    QList<int> m_worksInstalledList;

    RoleItemModel m_sourceModel;
    RoleItemModel m_categoryModel;
    RoleItemModel m_languageModel;
    RoleItemModel m_worksModel;

    QMap<CSwordModuleInfo*, bool> m_modulesToInstallRemove;
    QList<CSwordModuleInfo*> m_modulesToRemove;
    QList<CSwordModuleInfo*> m_modulesToInstall;

};

}
#endif
