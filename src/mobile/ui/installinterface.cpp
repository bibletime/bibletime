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

#include "installinterface.h"

#include "backend/btinstallbackend.h"
#include "backend/btinstallthread.h"
#include "backend/config/btconfig.h"
#include "mobile/bookshelfmanager/installsources.h"
#include <QThread>

namespace btm {

enum TextRoles {
    TextRole = Qt::UserRole + 1
};

enum WorksRoles {
    TitleRole = Qt::UserRole + 1,
    DescriptionRole = Qt::UserRole + 2,
    SelectedRole = Qt::UserRole + 3,
    InstalledRole = Qt::UserRole + 4
};

QString const SourceKey = "GUI/InstallManager/source";
QString const CategoryKey = "GUI/InstallManager/category";
QString const LanguageKey = "GUI/InstallManager/language";

InstallInterface::InstallInterface() :
    QObject(),
    m_backend(0),
    m_thread(0),
    m_worker(0),
    m_nextInstallIndex(0),
    m_progressVisible(false),
    m_progressMin(0),
    m_progressMax(0),
    m_progressValue(0),
    m_wasCanceled(false) {
}

void InstallInterface::setup() {
    m_modulesToInstallRemove.clear();
    setupSourceModel();
    if (m_sourceModel.rowCount() > 0) {
        QModelIndex index = m_sourceModel.index(0,0);
        QString newSource = m_sourceModel.data(index, TextRole).toString();
        updateSwordBackend(newSource);
    }
}

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

void InstallInterface::setupSourceModel() {
    m_sourceList = BtInstallBackend::sourceNameList();
    setupTextModel(m_sourceList, &m_sourceModel);
}

void InstallInterface::updateCategoryModel() {
    if (m_backend == nullptr)
        return;
    const QList<CSwordModuleInfo*> modules = m_backend->moduleList();
    QSet<QString> categories;
    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        categories.insert(categoryName);
    }
    m_categoryList = categories.toList();
    m_categoryList.sort();
    setupTextModel(m_categoryList, &m_categoryModel);
}

void InstallInterface::updateLanguageModel(const QString& currentCategory) {
    if (m_backend == nullptr)
        return;
    const QList<CSwordModuleInfo*> modules = m_backend->moduleList();
    QSet<QString> languages;
    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        CSwordModuleInfo::Category category = module->category();
        QString categoryName = module->categoryName(category);
        if (!currentCategory.isEmpty() && currentCategory != categoryName)
            continue;
        const CLanguageMgr::Language* language = module->language();
        QString languageName = language->translatedName();
        languages.insert(languageName);
    }
    m_languageList = languages.toList();
    m_languageList.sort();
    setupTextModel(m_languageList, &m_languageModel);
}


void InstallInterface::updateSwordBackend(const QString& sourceName) {
    if (sourceName.isEmpty())
        return;
    sword::InstallSource source = BtInstallBackend::source(sourceName);
    m_backend = BtInstallBackend::backend(source);
}

int InstallInterface::installedModulesCount() {
    return CSwordBackend::instance()->moduleList().count();
}

static CSwordModuleInfo* moduleInstalled(const CSwordModuleInfo& moduleInfo) {
    CSwordModuleInfo *installedModule = CSwordBackend::instance()->findModuleByName(moduleInfo.name());
    return installedModule;
}

static void setupWorksModel(const QStringList& titleList,
                            const QStringList& descriptionList,
                            const QList<int>& installedList,
                            RoleItemModel* model) {
    BT_ASSERT(titleList.count() == descriptionList.count());
    BT_ASSERT(titleList.count() == installedList.count());

    QHash<int, QByteArray> roleNames;
    roleNames[TitleRole] =  "title";
    roleNames[DescriptionRole] = "desc";
    roleNames[SelectedRole] = "selected";
    roleNames[InstalledRole] = "installed";
    model->setRoleNames(roleNames);

    model->clear();
    for (int i=0; i< titleList.count(); ++i) {
        QStandardItem* item = new QStandardItem();
        QString title = titleList.at(i);
        item->setData(title, TitleRole);
        QString description = descriptionList.at(i);
        item->setData(description, DescriptionRole);
        int installed = installedList.at(i);
        item->setData(installed, InstalledRole);
        item->setData(0, SelectedRole);
        model->appendRow(item);
    }
}

void InstallInterface::updateWorksModel(
        const QString& sourceName,
        const QString& categoryName,
        const QString& languageName)
{
    if (m_backend == nullptr)
        return;
    const QList<CSwordModuleInfo*> modules = m_backend->moduleList();

    m_worksTitleList.clear();
    m_worksDescList.clear();
    m_worksList.clear();
    m_worksInstalledList.clear();

    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        module->setProperty("installSourceName", sourceName);
        CSwordModuleInfo::Category category = module->category();
        QString moduleCategoryName = module->categoryName(category);
        const CLanguageMgr::Language* language = module->language();
        QString moduleLanguageName = language->translatedName();
        if (moduleCategoryName == categoryName &&
                moduleLanguageName == languageName ) {
            QString name = module->name();
            QString description = module->config(CSwordModuleInfo::Description);
            QString version = module->config(CSwordModuleInfo::ModuleVersion);
            QString info = description + ": " + version;\
            int installed = moduleInstalled(*module) ? 1 : 0;
            m_worksTitleList.append(name);
            m_worksDescList.append(info);
            m_worksList.append(module);
            m_worksInstalledList.append(installed);
        }
    }
    setupWorksModel(m_worksTitleList, m_worksDescList, m_worksInstalledList, &m_worksModel);
}

QString InstallInterface::getSource(int index) {
    if (m_sourceModel.rowCount() == 0)
        return QString();
    QModelIndex mIndex = m_sourceModel.index(index, 0);
    return mIndex.data(TextRole).toString();
}

QString InstallInterface::getCategory(int index) {
    if (m_categoryModel.rowCount() == 0)
        return QString();
    QModelIndex mIndex = m_categoryModel.index(index, 0);
    return mIndex.data(TextRole).toString();
}

QString InstallInterface::getLanguage(int index) {
    if (m_languageModel.rowCount() == 0)
        return QString();
    QModelIndex mIndex = m_languageModel.index(index, 0);
    return mIndex.data(TextRole).toString();
}

QString InstallInterface::getSourceSetting() {
    return btConfig().value<QString>(SourceKey, QString());
}

QString InstallInterface::getCategorySetting() {
    return btConfig().value<QString>(CategoryKey, QString());
}

QString InstallInterface::getLanguageSetting() {
    return btConfig().value<QString>(LanguageKey, QString());
}

void InstallInterface::setSourceSetting(const QString& source) {
    btConfig().setValue(SourceKey, source);
}

void InstallInterface::setCategorySetting(const QString& category) {
    btConfig().setValue(CategoryKey, category);
}

void InstallInterface::setLanguageSetting(const QString& language) {
    btConfig().setValue(LanguageKey, language);
}

static int findModelIndex(RoleItemModel* model, const QString& value) {
    if (value.isEmpty())
        return -1;
    int count = model->rowCount();
    for (int row=0; row<count; ++row) {
        QModelIndex index = model->index(row, 0);
        QString text = index.data(TextRole).toString();
        if (text == value)
            return row;
    }
    return -1;
}

int InstallInterface::searchSource(const QString& value) {
    return findModelIndex(&m_sourceModel, value);
}

int InstallInterface::searchCategory(const QString& value) {
    return findModelIndex(&m_categoryModel, value);
}

int InstallInterface::searchLanguage(const QString& value) {
    return findModelIndex(&m_languageModel, value);
}

void InstallInterface::installRemove() {
    m_modulesToRemove.clear();
    m_modulesToInstall.clear();
    QMap<CSwordModuleInfo*, bool>::const_iterator it;
    for(it=m_modulesToInstallRemove.constBegin();
        it!=m_modulesToInstallRemove.constEnd();
        ++it) {
        CSwordModuleInfo* moduleInfo = it.key();
        bool selected = it.value();
        if ( ! selected)
            continue;
        CSwordModuleInfo* installedModule = moduleInstalled(*moduleInfo);
        if (installedModule) {
            m_modulesToRemove.append(installedModule);
        }
        else if ( ! moduleInstalled(*moduleInfo)) {
            m_modulesToInstall.append(moduleInfo);
        }
    }
    installModules();
    removeModules();
}

void InstallInterface::clearModules() {
    m_modulesToInstall.clear();
    m_modulesToInstallRemove.clear();
}

void InstallInterface::addModule(const QString& sourceName, const QString& moduleName) {
    updateSwordBackend(sourceName);
    const QList<CSwordModuleInfo*> modules = m_backend->moduleList();
    for (int moduleIndex=0; moduleIndex<modules.count(); ++moduleIndex) {
        CSwordModuleInfo* module = modules.at(moduleIndex);
        module->setProperty("installSourceName", sourceName);
        QString name = module->name();
        if (name == moduleName) {
            m_modulesToInstall.append(module);
        }
    }
}

void InstallInterface::installModulesAuto() {
    installModules();
}

void InstallInterface::workSelected(int index) {
    QStandardItem* item = m_worksModel.item(index,0);
    QVariant vSelected = item->data(SelectedRole);
    int selected = vSelected.toInt();
    selected = selected == 0 ? 1 : 0;
    item->setData(selected, SelectedRole);

    CSwordModuleInfo* moduleInfo = m_worksList.at(index);
    m_modulesToInstallRemove[moduleInfo] = selected == 1;
}

void InstallInterface::refreshLists(
        const QString& source,
        const QString& category,
        const QString& language) {
    m_wasCanceled = false;
    emit wasCanceledChanged();
    m_tempSource = source;
    m_tempCategory = category;
    m_tempLanguage = language;
    setProgressMin(0);
    setProgressMax(100);
    setProgressValue(0);
    setProgressText(tr("Refreshing Source List"));
    setProgressVisible(true);
    runThread();
}

void InstallInterface::runThread() {
    QThread* thread = new QThread;
    m_worker = new InstallSources();
    m_worker->moveToThread(thread);
    //    BT_CONNECT(m_worker, SIGNAL(error(QString)),
    //               this,     SLOT(errorString(QString)));
    BT_CONNECT(thread, SIGNAL(started()), m_worker, SLOT(process()));
    BT_CONNECT(m_worker, SIGNAL(finished()), thread, SLOT(quit()));
    BT_CONNECT(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    BT_CONNECT(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    BT_CONNECT(thread, SIGNAL(finished()), this, SIGNAL(progressFinished()));
    BT_CONNECT(m_worker, SIGNAL(percentComplete(int, QString const &)),
               this,     SLOT(slotPercentComplete(int, QString const &)));
    thread->start();
}

void InstallInterface::cancel() {
    m_wasCanceled = true;
    emit wasCanceledChanged();
    if (m_thread) {
        m_thread->stopInstall();
        while (!m_thread->wait()) /* join */;
    }
    if (m_worker)
        m_worker->cancel();
    setProgressVisible(false);
}

void InstallInterface::installModules() {
    if (m_modulesToInstall.count() == 0)
        return;
    m_nextInstallIndex = 0;
    QString destination = getSourcePath();
    if (destination.isEmpty())
        return;
    m_wasCanceled = false;
    emit wasCanceledChanged();
    setProgressVisible(true);
    setProgressMin(0.0);
    setProgressMax(100.0);
    setProgressValue(0.0);

    m_thread = new BtInstallThread(m_modulesToInstall, destination, this);
    // Connect the signals between the dialog, items and threads
    BT_CONNECT(m_thread, SIGNAL(preparingInstall(int)),
               this,     SLOT(slotInstallStarted(int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(downloadStarted(int)),
               this,     SLOT(slotDownloadStarted(int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(statusUpdated(int, int)),
               this,     SLOT(slotStatusUpdated(int, int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(installCompleted(int, bool)),
               this,     SLOT(slotOneItemCompleted(int, bool)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(finished()),
               this,     SLOT(slotThreadFinished()),
               Qt::QueuedConnection);
    m_thread->start();
}

void InstallInterface::removeModules() {
    CSwordBackend::instance()->uninstallModules(m_modulesToRemove.toSet());
}

QString InstallInterface::getSourcePath() {
    QStringList targets = BtInstallBackend::targetList();
    for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
        // Add the path only if it's writable
        QString sourcePath = *it;
        if (sourcePath.isEmpty())
            continue;
        QDir dir(sourcePath);
        if (!dir.exists())
            continue;
        if (!dir.isReadable())
            continue;
        QFileInfo fi( dir.canonicalPath());
        if (!fi.isWritable())
            continue;
        return sourcePath;
    }
    return QString();
}

void InstallInterface::slotStopInstall() {
    m_thread->stopInstall();
}

void InstallInterface::slotInstallStarted(int moduleIndex) {
    BT_ASSERT(moduleIndex == m_nextInstallIndex);
    m_nextInstallIndex++;
    setProgressText(tr("Installing %1").arg(getModuleName(moduleIndex)));
}

void InstallInterface::slotDownloadStarted(int moduleIndex) {
    setProgressValue(0);
    setProgressText(tr("Downloading %1").arg(getModuleName(moduleIndex)));
}

void InstallInterface::slotStatusUpdated(int /*moduleIndex*/, int status) {
    setProgressValue(status);
}

void InstallInterface::slotOneItemCompleted(int /* moduleIndex */, bool /* successful */) {
    // TODO show failed status
}

void InstallInterface::slotThreadFinished() {
    setProgressVisible(false);
    CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);
    if (m_wasCanceled) {
        return;
    }
    emit modulesDownloadFinished();
}

void InstallInterface::slotPercentComplete(int percent, const QString& title) {
    setProgressValue(percent);
    setProgressText(title);
    if (percent == 100) {
        setProgressVisible(false);
        setupSourceModel();
        updateSwordBackend(m_tempSource);
        updateCategoryModel();
        updateLanguageModel("");
        updateWorksModel(m_tempSource, "", "");
        updateCurrentViews(m_tempSource, m_tempCategory, m_tempLanguage);
    }
}


QString InstallInterface::getModuleName(int moduleIndex) {
    BT_ASSERT(moduleIndex < m_modulesToInstall.count());
    CSwordModuleInfo * module = m_modulesToInstall.at(moduleIndex);
    return module->name();
}




// *************************************************************

QVariant InstallInterface::sourceModel() {
    QVariant var;
    var.setValue(&m_sourceModel);
    return var;
}

QVariant InstallInterface::categoryModel() {
    QVariant var;
    var.setValue(&m_categoryModel);
    return var;
}

QVariant InstallInterface::languageModel() {
    QVariant var;
    var.setValue(&m_languageModel);
    return var;
}

QVariant InstallInterface::worksModel() {
    QVariant var;
    var.setValue(&m_worksModel);
    return var;
}

bool InstallInterface::progressVisible() const {
    return m_progressVisible;
}

qreal InstallInterface::progressMin() const {
    return m_progressMin;
}

qreal InstallInterface::progressMax() const {
    return m_progressMax;
}

qreal InstallInterface::progressValue() const {
    return m_progressValue;
}

QString InstallInterface::progressText() const {
    return m_progressText;
}

void InstallInterface::setProgressVisible(bool value) {
    m_progressVisible = value;
    progressVisibleChanged();
}

void InstallInterface::setProgressMin(qreal value) {
    m_progressMin = value;
    progressMinChanged();
}

void InstallInterface::setProgressMax(qreal value) {
    m_progressMax = value;
    progressMaxChanged();
}

void InstallInterface::setProgressValue(qreal value) {
    if (m_progressValue != value) {
        m_progressValue = value;
        progressValueChanged();
    }
}

void InstallInterface::setProgressText(const QString& value) {
    m_progressText = value;
    progressTextChanged();
}

bool InstallInterface::getWasCanceled() {
    return m_wasCanceled;
}

}

