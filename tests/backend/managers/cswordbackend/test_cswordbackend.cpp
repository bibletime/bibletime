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

#include "test_cswordbackend.h"
#include <QtTest/QtTest>
#include <swbuf.h>
#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordversekey.h"
#include "backend/config/btconfig.h"
#include "util/directory.h"

Q_DECLARE_METATYPE(CSwordModuleInfo::FilterTypes)

class BibleTimeApp {
public:
    BibleTimeApp() {
    }
    static void init() {
        QVERIFY(BtConfig::initBtConfig() ==
                BtConfig::INIT_OK);
    }

private:
};

void test_CSwordBackend::initTestCase() {
    QVERIFY(util::directory::initDirectoryCache());
    CSwordBackend* backend = CSwordBackend::createInstance();
    QVERIFY(backend != 0);
    BibleTimeApp::init();
    backend->initModules(CSwordBackend::OtherChange);
}

void test_CSwordBackend::instance() {
     QVERIFY(CSwordBackend::instance() != 0);    
}

void test_CSwordBackend::moduleList() {
    QVERIFY(CSwordBackend::instance()->moduleList().count() > 0);
}

// Should CSwordBackend::shutdownModules be private?

// This also tests setOption
void test_CSwordBackend::setFilterOptions() {
    CSwordBackend* backend = CSwordBackend::instance();
    backend->setFilterOptions(FilterOptions());
    FilterOptions fOpt;

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::footnotes).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.footnotes = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.footnotes = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::strongNumbers).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.strongNumbers = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.strongNumbers = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::headings).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.headings = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.headings = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::morphTags).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.morphTags = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.morphTags = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::lemmas).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.lemmas = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.lemmas = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::hebrewPoints).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.hebrewPoints = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.hebrewPoints = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::hebrewCantillation).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.hebrewCantillation = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.hebrewCantillation = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::greekAccents).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.greekAccents = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.greekAccents = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::scriptureReferences).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.scriptureReferences = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.scriptureReferences = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::redLetterWords).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");

        fOpt.redLetterWords = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "On");

        fOpt.redLetterWords = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Off");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::textualVariants).toUtf8();
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Primary Reading");

        fOpt.textualVariants = 1;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Secondary Reading");

        fOpt.textualVariants = 0;
        backend->setFilterOptions(fOpt);
        QVERIFY(QString(backend->getGlobalOption(optName.constData())) == "Primary Reading");
    }

    {
        QByteArray optName = backend->optionName(CSwordModuleInfo::morphSegmentation).toUtf8();

        // Test only if morph option is available
        if (backend->getGlobalOption(optName.constData()) != 0) {

            fOpt.morphSegmentation = 1;
            backend->setFilterOptions(fOpt);
            QCOMPARE(backend->getGlobalOption(optName.constData()), "On");

            fOpt.morphSegmentation = 0;
            backend->setFilterOptions(fOpt);
            QCOMPARE(backend->getGlobalOption(optName.constData()), "Off");
        }
    }
}

void test_CSwordBackend::booknameLanguage_data() {
        QTest::addColumn<QString>("language");
        QTest::addColumn<QString>("reference");
        QTest::addColumn<QString>("bookName");

        QTest::newRow("L1") << "en_US" << "John 4:5" << "John";
        QTest::newRow("L2") << "de"    << "John 4:5" << "Johannes";
        QTest::newRow("L3") << "en_US" << "Genesis 1:1" << "Genesis";
        QTest::newRow("L4") << "de"    << "Genesis 1:1" << "1. Mose";
}

void test_CSwordBackend::booknameLanguage() {
    QFETCH(QString, language);
    QFETCH(QString, reference);
    QFETCH(QString, bookName);

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* module = backend->findModuleByName("KJV");
    QVERIFY(module != 0);
    CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(CSwordKey::createInstance(module));

    backend->booknameLanguage(language);
    vKey->setKey(reference);
    QCOMPARE(QString(vKey->getBookName()), bookName);
}

void test_CSwordBackend::findModuleByDescription() {
    CSwordBackend* backend = CSwordBackend::instance();
    QVERIFY(backend->findModuleByDescription("Josephus: The Complete Works") != 0);
}

void test_CSwordBackend::findModuleByName() {
    CSwordBackend* backend = CSwordBackend::instance();
    QVERIFY(backend->findModuleByName("KJV") != 0);
    QVERIFY(backend->findModuleByName("KJVA") != 0);
    QVERIFY(backend->findModuleByName("Scofield") != 0);
    QVERIFY(backend->findModuleByName("Josephus") != 0);
    QVERIFY(backend->findModuleByName("StrongsGreek") != 0);
    QVERIFY(backend->findModuleByName("ABC") == 0);
}

void test_CSwordBackend::findModuleByPointer() {
    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* module1 = backend->findModuleByName("KJV");
    sword::SWModule& swModule = module1->module();

    CSwordModuleInfo* module2 = backend->findSwordModuleByPointer(&swModule);
    QCOMPARE(module1,module2);
}

void test_CSwordBackend::getConfig() {
    CSwordBackend* backend = CSwordBackend::instance();
    sword::SWConfig* config = backend->getConfig();
    QVERIFY(config != 0);
}

void test_CSwordBackend::optionName_data() {
    QTest::addColumn<CSwordModuleInfo::FilterTypes>("type");
    QTest::addColumn<QString>("name");

    QTest::newRow("Opt1") << CSwordModuleInfo::footnotes << "Footnotes";
    QTest::newRow("Opt2") << CSwordModuleInfo::filterTypesMIN << "Footnotes";
    QTest::newRow("Opt3") << CSwordModuleInfo::morphSegmentation << "Morph Segmentation";
    QTest::newRow("Opt4") << CSwordModuleInfo::filterTypesMAX << "Morph Segmentation";
}

void test_CSwordBackend::optionName() {
    QFETCH(QString, name);
    QFETCH(CSwordModuleInfo::FilterTypes, type);

    CSwordBackend* backend = CSwordBackend::instance();
    QCOMPARE(backend->optionName(type), name);
}

void test_CSwordBackend::configOptionName_data() {
    QTest::addColumn<CSwordModuleInfo::FilterTypes>("type");
    QTest::addColumn<QString>("name");

    QTest::newRow("configOpt1") << CSwordModuleInfo::footnotes << "Footnotes";
    QTest::newRow("configOpt2") << CSwordModuleInfo::filterTypesMIN << "Footnotes";
    QTest::newRow("configOpt3") << CSwordModuleInfo::morphSegmentation << "MorphSegmentation";
    QTest::newRow("configOpt4") << CSwordModuleInfo::filterTypesMAX << "MorphSegmentation";
}

void test_CSwordBackend::configOptionName() {
    QFETCH(QString, name);
    QFETCH(CSwordModuleInfo::FilterTypes, type);

    CSwordBackend* backend = CSwordBackend::instance();
    QCOMPARE(backend->configOptionName(type), name);
}

void test_CSwordBackend::translatedOptionName_data() {
    QTest::addColumn<CSwordModuleInfo::FilterTypes>("type");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("transFile");

    QTest::newRow("transOpt1") << CSwordModuleInfo::footnotes         << "Footnotes"          << "bibletime_ui_en_US";
    QTest::newRow("transOpt2") << CSwordModuleInfo::filterTypesMIN    << "Footnotes"          << "bibletime_ui_en_US";
    QTest::newRow("transOpt3") << CSwordModuleInfo::morphSegmentation << "Morph segmentation" << "bibletime_ui_en_US";
    QTest::newRow("transOpt4") << CSwordModuleInfo::filterTypesMAX    << "Morph segmentation" << "bibletime_ui_en_US";

    QTest::newRow("transOpt5") << CSwordModuleInfo::footnotes         << "Fu\u00DFnoten"       << "bibletime_ui_de";
    QTest::newRow("transOpt6") << CSwordModuleInfo::morphSegmentation << "Morphologische Segmentierung" << "bibletime_ui_de";
}

void test_CSwordBackend::translatedOptionName() {
    QFETCH(QString, name);
    QFETCH(CSwordModuleInfo::FilterTypes, type);
    QFETCH(QString, transFile);

    CSwordBackend* backend = CSwordBackend::instance();
    QCoreApplication* app = QCoreApplication::instance();
    QTranslator qtTranslator;
    qtTranslator.load(transFile);
    app->installTranslator(&qtTranslator);

    QCOMPARE(backend->translatedOptionName(type), name);
}

#if 0
// test reloadModules - see takeModulesFromList

void test_CSwordBackend::takeModulesFromList() {
    QStringList modules;
    modules << "KJV";
    modules << "abc";
    modules << "KJVA";
    modules << "def";

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* kjv = backend->findModuleByName("KJV");
    CSwordModuleInfo* kjva = backend->findModuleByName("KJVA");

    QList<CSwordModuleInfo*> moduleList = backend->takeModulesFromList(modules);
    QVERIFY(moduleList.count() == 2);
    QVERIFY(moduleList.contains(kjv));
    QVERIFY(moduleList.contains(kjva));

    QVERIFY(backend->findModuleByName("KJV") == nullptr);
    QVERIFY(backend->findModuleByName("KJVA") == nullptr);

    // Tests reloadModules
    backend->reloadModules(CSwordBackend::OtherChange);
    QVERIFY(backend->findModuleByName("KJV") != nullptr);
    QVERIFY(backend->findModuleByName("KJVA") != nullptr);
}
#endif

void test_CSwordBackend::getPointerList() {

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* kjv = backend->findModuleByName("KJV");
    CSwordModuleInfo* kjva = backend->findModuleByName("KJVA");

    QStringList modules;
    modules << "KJV";
    modules << "KJVA";

    QList<CSwordModuleInfo*> moduleList = backend->getPointerList(modules);
    QVERIFY(moduleList.count() == 2);
    QVERIFY(moduleList.contains(kjv));
    QVERIFY(moduleList.contains(kjva));
}

void test_CSwordBackend::getConstPointerList() {

    CSwordBackend* backend = CSwordBackend::instance();
    CSwordModuleInfo* kjv = backend->findModuleByName("KJV");
    CSwordModuleInfo* kjva = backend->findModuleByName("KJVA");

    QStringList modules;
    modules << "KJV";
    modules << "KJVA";

    BtConstModuleList moduleList = backend->getConstPointerList(modules);
    QVERIFY(moduleList.count() == 2);
    QVERIFY(moduleList.contains(kjv));
    QVERIFY(moduleList.contains(kjva));
}

// TODO test swordDirList

// TODO test deleteOrphanedIndices

void test_CSwordBackend::cleanupTestCase() {
     CSwordBackend::destroyInstance();
     QVERIFY(CSwordBackend::instance() == 0);    
}

QTEST_MAIN(test_CSwordBackend)

