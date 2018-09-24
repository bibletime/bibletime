
#include "test_btinstallthread.h"

// This class serves 2 purposes. It is a test of the BtInstallThread class.
// It also installs several modules that will be used by other tests.
// It should be the second test ran (after test_btsourcesthread).

#include <QEventLoop>

#include <stdio.h>

#include "backend/btinstallbackend.h"
#include "backend/btinstallthread.h"
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "util/btconnect.h"
#include "util/directory.h"

static QString s_sourceName = "CrossWire";
static QStringList s_moduleList {"KJV", "KJVA","Scofield", "Josephus", "StrongsGreek" };


class BibleTimeApp
{
public:
    BibleTimeApp() {
    }
    static void init() {
        QVERIFY(BtConfig::initBtConfig() ==
                BtConfig::INIT_OK);
    }
private:
};


static bool installPathIsUsable(QString const & path) {
    if (path.isEmpty())
        return false;
    QDir const dir(path);
    if (!dir.exists() || !dir.isReadable())
        return false;
    return QFileInfo(dir.canonicalPath()).isWritable();
}


void test_BtInstallThread::initTestCase() {
    initBackend();
    getInstallPath();
    removeModulesIfInstalled();
    findModulesToInstall();
}

void test_BtInstallThread::initBackend() {
    QVERIFY(util::directory::initDirectoryCache());
    CSwordBackend* backend = CSwordBackend::createInstance();
    QVERIFY(backend != 0);
    BibleTimeApp::init();
    backend->initModules(CSwordBackend::OtherChange);
}

void test_BtInstallThread::getInstallPath() {
    QStringList const targets(BtInstallBackend::targetList());
    for (auto const & target : targets) {
        if (installPathIsUsable(target)) {
            m_destination = target;
            break;
        }
    }
}

void test_BtInstallThread::findModulesToInstall() {
    swordxx::InstallSource const source = BtInstallBackend::source(s_sourceName);
    CSwordBackend * const installBackend = BtInstallBackend::backend(source);

    for (auto moduleName : s_moduleList) {
        CSwordModuleInfo * module = installBackend->findModuleByName(moduleName);
        QVERIFY(module != 0);
        module->setProperty("installSourceName", s_sourceName);
        m_modules.append(module);
    }
}

void test_BtInstallThread::removeModulesIfInstalled() {
    QList<CSwordModuleInfo *> modules;
    for (auto moduleName : s_moduleList) {
        CSwordModuleInfo * module = CSwordBackend::instance()->findModuleByName(moduleName);
        if (module) {
            modules.append(module);
        }
    }
    BtConstModuleSet moduleSet = modules.toSet();
    CSwordBackend::instance()->uninstallModules(moduleSet);
}


void test_BtInstallThread::installModules() {
    m_thread = new BtInstallThread(m_modules, m_destination, this);

    BT_CONNECT(m_thread, &BtInstallThread::installCompleted,
               this, &test_BtInstallThread::slotOneItemCompleted);
    BT_CONNECT(m_thread, &BtInstallThread::finished,
               this,     &test_BtInstallThread::slotThreadFinished);

    m_thread->start();
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();
}

void test_BtInstallThread::slotOneItemCompleted(
        int moduleIndex,
        bool successful) {
    CSwordModuleInfo * module = m_modules.at(moduleIndex);
    QString moduleName = module->name();
    fprintf(stderr, "        Installing %s\n",
            moduleName.toLocal8Bit().constData());
    QVERIFY(successful);
}

void test_BtInstallThread::slotThreadFinished() {
    m_eventLoop->exit();
}

void test_BtInstallThread::cleanupTestCase() {
    CSwordBackend::destroyInstance();
    QVERIFY(CSwordBackend::instance() == 0);
}

QTEST_MAIN(test_BtInstallThread)
