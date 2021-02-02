/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "test_cswordversekey.h"

#include <memory>
#include <qdebug.h>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/config/btconfig.h"
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

private:
};

void test_CSwordVerseKey::initTestCase() {
    QVERIFY(util::directory::initDirectoryCache());
    CSwordBackend* backend = CSwordBackend::createInstance();
    QVERIFY(backend != 0);
    BibleTimeApp::init();
    backend->initModules(CSwordBackend::OtherChange);

    m_moduleKJV = backend->findModuleByName("KJV");
    QVERIFY(m_moduleKJV != 0);
    m_moduleKJVA = backend->findModuleByName("KJVA");
    QVERIFY(m_moduleKJVA != 0);
    m_moduleJosephus = backend->findModuleByName("Josephus");
    QVERIFY(m_moduleKJVA != 0);
}

void test_CSwordVerseKey::CSwordVerseKey_module_constructor() {
    try {
        std::unique_ptr<CSwordVerseKey>(new CSwordVerseKey(m_moduleKJVA));
    }
    catch (...) {
        QFAIL("constructor failed");
    }

    // QUESTION: Should a null module cause a throw?
    try {
        std::unique_ptr<CSwordVerseKey>(new CSwordVerseKey(nullptr));
    }
    catch (...) {
        QFAIL("constructor failed");
    }
}

void test_CSwordVerseKey::CSwordVerseKey_copy_constructor() {

    CSwordVerseKey* key = nullptr;
    try {
        key = new CSwordVerseKey(m_moduleKJVA);
        key->setKey("II Maccabees 15:39");
    }
    catch (...) {
        QFAIL("constructor failed");
    }

    const CSwordVerseKey* copyKey = new CSwordVerseKey(*key);
    QCOMPARE(QString(copyKey->getBookName()), QString(key->getBookName()));
    QCOMPARE(QString(copyKey->getChapter()), QString(key->getChapter()));
    QCOMPARE(QString(copyKey->getVerse()), QString(key->getVerse()));
}

void test_CSwordVerseKey::CSwordVerseKey_versekey_module_constructor() {

    CSwordVerseKey* key = nullptr;
    try {
        key = new CSwordVerseKey(m_moduleKJVA);
        key->setKey("Matthew 15:39");
    }
    catch (...) {
        QFAIL("constructor failed");
    }

    CSwordVerseKey* key2 = nullptr;
    try {
        key2 = new CSwordVerseKey(key,m_moduleKJV);
        key2->setKey("Matthew 15:39");
    }
    catch (...) {
        QFAIL("constructor failed");
    }
    QCOMPARE(QString(key2->getBookName()), QString(key->getBookName()));
    QCOMPARE(QString(key2->getChapter()), QString(key->getChapter()));
    QCOMPARE(QString(key2->getVerse()), QString(key->getVerse()));


}

void test_CSwordVerseKey::setKey_data() {
        QTest::addColumn<QString>("moduleName");
        QTest::addColumn<QString>("reference");

        QTest::newRow("ref1") << "KJV" << "Genesis 1:1";
        QTest::newRow("ref2") << "KJV" << "Zechariah 13:9";
        QTest::newRow("ref3") << "KJV" << "Matthew 1:1";
        QTest::newRow("ref4") << "KJV" << "Revelation of John 22:21";

        QTest::newRow("ref1A") << "KJVA" << "Genesis 1:1";
        QTest::newRow("ref2A") << "KJVA" << "Zechariah 13:9";
        QTest::newRow("ref3A") << "KJVA" << "Matthew 1:1";
        QTest::newRow("ref4A") << "KJVA" << "Revelation of John 22:21";

        QTest::newRow("ref5A") << "KJVA" << "II Esdras 16:78";
}


// Also tests key()
void test_CSwordVerseKey::setKey() {
    QFETCH(QString, moduleName);
    QFETCH(QString, reference);

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* module = backend->findModuleByName(moduleName);
    QVERIFY(module != 0);
    CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(CSwordKey::createInstance(module));

    vKey->setKey(reference);
    QString newKey = vKey->key();
    QCOMPARE(newKey, reference);
}


void test_CSwordVerseKey::next_data() {
        QTest::addColumn<QString>("moduleName");
        QTest::addColumn<QString>("reference");
        QTest::addColumn<QString>("reference2");

        QTest::newRow("r1") << "KJV" << "Genesis 1:1"   << "Genesis 1:2";
        QTest::newRow("r2") << "KJV" << "Malachi 4:5"   << "Malachi 4:6";
        QTest::newRow("r3") << "KJV" << "Matthew 1:1"   << "Matthew 1:2";
        QTest::newRow("r4") << "KJV" << "Revelation of John 22:20" << "Revelation of John 22:21";
}

// Does not work correctly over various chapter/book boundaries
// Implementation is in sword.
void test_CSwordVerseKey::next() {
    QFETCH(QString, moduleName);
    QFETCH(QString, reference);
    QFETCH(QString, reference2);

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* module = backend->findModuleByName(moduleName);
    QVERIFY(module != 0);
    CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(CSwordKey::createInstance(module));

    vKey->setKey(reference);
    vKey->next(CSwordVerseKey::UseVerse);
    QString newKey = vKey->key();
    QCOMPARE(newKey, reference2);
}


void test_CSwordVerseKey::previous_data() {
        QTest::addColumn<QString>("moduleName");
        QTest::addColumn<QString>("reference2");
        QTest::addColumn<QString>("reference");

        QTest::newRow("r1") << "KJVA" << "Genesis 1:1"   << "Genesis 1:2";
        QTest::newRow("r2") << "KJVA" << "Malachi 4:5"   << "Malachi 4:6";
        QTest::newRow("r3") << "KJVA" << "Matthew 1:1"   << "Matthew 1:2";
        QTest::newRow("r4") << "KJVA" << "Revelation of John 22:20" << "Revelation of John 22:21";
}

void test_CSwordVerseKey::previous() {
    QFETCH(QString, moduleName);
    QFETCH(QString, reference);
    QFETCH(QString, reference2);

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* module = backend->findModuleByName(moduleName);
    QVERIFY(module != 0);
    CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(CSwordKey::createInstance(module));

    vKey->setKey(reference);
    vKey->previous(CSwordVerseKey::UseVerse);
    QString newKey = vKey->key();
    QCOMPARE(newKey, reference2);
}


void test_CSwordVerseKey::setModule() {
    CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(CSwordKey::createInstance(m_moduleKJV));
    QVERIFY(vKey != 0);

    QSignalSpy beforeSpy(vKey->beforeChangedSignaller(), &BtSignal::signal);
    QSignalSpy afterSpy(vKey->afterChangedSignaller(), &BtSignal::signal);

    vKey->setModule(m_moduleKJVA);

    QCOMPARE(beforeSpy.count(), 1);
    QCOMPARE(afterSpy.count(), 1);

    const CSwordModuleInfo* module = vKey->module();
    QVERIFY(module != 0);
    QCOMPARE(module->name(), QString("KJVA"));
}



void test_CSwordVerseKey::cleanupTestCase() {
     CSwordBackend::destroyInstance();
     QVERIFY(CSwordBackend::instance() == 0);
}

QTEST_MAIN(test_CSwordVerseKey)

