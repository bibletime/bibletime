
#include "test_btsourcesthread.h"

// This class serves 2 purposes. It is a test of the BtSourcesThread class.
// This installs the lists of available modules.
// It should be the first test ran.

#include <QEventLoop>

#include <stdio.h>

#include "backend/managers/cswordbackend.h"
#include "backend/config/btconfig.h"
#include "backend/btsourcesthread.h"
#include "util/btconnect.h"
#include "util/directory.h"

class BibleTimeApp
{
public:
    BibleTimeApp() {
    }
    static void init() {
        QVERIFY(BtConfig::initBtConfig() ==
                BtConfig::INIT_OK);
    }
};

void test_BtSourcesThread::initTestCase() {
    QVERIFY(util::directory::initDirectoryCache());
    CSwordBackend* backend = CSwordBackend::createInstance();
    QVERIFY(backend != 0);
    BibleTimeApp::init();
    backend->initModules(CSwordBackend::OtherChange);
}

void test_BtSourcesThread::installRemoteSources() {
    m_thread = new BtSourcesThread(this);
    BT_CONNECT(m_thread,   &BtSourcesThread::showMessage,
               this, &test_BtSourcesThread::slotShowMessage);
    BT_CONNECT(m_thread, &BtSourcesThread::finished,
               this,     &test_BtSourcesThread::slotThreadFinished);
    m_thread->start();
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();
}

void test_BtSourcesThread::slotThreadFinished() {
    m_eventLoop->exit();
}

void test_BtSourcesThread::slotShowMessage(const QString & msg) {
    fprintf(stderr, "        %s\n", msg.toLocal8Bit().constData());
}

void test_BtSourcesThread::cleanupTestCase() {
    CSwordBackend::destroyInstance();
    QVERIFY(CSwordBackend::instance() == 0);
}

QTEST_MAIN(test_BtSourcesThread)


//void test_BtSourcesThread::loadModule_data() {

//        QTest::addColumn<QString>("moduleName");

//        QTest::newRow("ref1") << "KJV";
//        QTest::newRow("ref1") << "KJVA";
//}

//void test_BtSourcesThread::loadModule() {
//    QFETCH(QString, moduleName);

//    CSwordBackend* backend = CSwordBackend::instance();
//    CSwordModuleInfo* module = backend->findModuleByName(moduleName);
//    if (module) {
//        qDebug() << moduleName << " is already loaded.";
//    } else {
//        qDebug() << moduleName << " is not loaded.";
//    }
//}

