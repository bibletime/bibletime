/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "test_btsourcesthread.h"

// This class serves 2 purposes. It is a test of the BtSourcesThread class.
// This installs the lists of available modules.
// It should be the first test ran.

#include <condition_variable>
#include <cstdio>
#include <memory>
#include <mutex>
#include "backend/managers/cswordbackend.h"
#include "backend/config/btconfig.h"
#include "backend/btsourcesthread.h"
#include "util/btconnect.h"
#include "util/directory.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swbuf.h>
#pragma GCC diagnostic pop

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
    QVERIFY(backend);
    BibleTimeApp::init();
    backend->initModules(CSwordBackend::OtherChange);
}

void test_BtSourcesThread::installRemoteSources() {
    auto const thread = std::make_unique<BtSourcesThread>();

    std::mutex mutex;
    std::condition_variable cond;
    bool done = false;

    BT_CONNECT(
            thread.get(), &BtSourcesThread::showMessage,
            [](QString const & m)
            { fprintf(stderr, "        %s\n", m.toLocal8Bit().constData()); });
    BT_CONNECT(
            thread.get(), &BtSourcesThread::finished,
            [&mutex, &cond, &done]() {
                std::lock_guard const guard(mutex);
                done = true;
                cond.notify_one();
            });
    thread->start();

    std::unique_lock lock(mutex);
    cond.wait(lock, [&done]() noexcept { return done; });
}

void test_BtSourcesThread::cleanupTestCase() {
    CSwordBackend::destroyInstance();
    QVERIFY(!CSwordBackend::instance());
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

