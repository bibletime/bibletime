#include <QObject>
#include <QTest>
#include <QFile>
#include <QDebug>
#include <qglobal.h>
#include "backend/config/btconfig.h"

class BtConfigTest: public QObject
{
    Q_OBJECT

    QString m_configFileName;
    BtConfig* m_btConfig;

    void reloadConfig()
    {
        delete m_btConfig;
        m_configFileName = "testsettings.ini";
        m_btConfig = new BtConfig(m_configFileName);
    }
private slots:
    void initTestCase()
    {
        m_configFileName = "testsettings.ini";
        QFile(m_configFileName).remove();
        m_btConfig = new BtConfig(m_configFileName);
    }

    void cleanupTestCase()
    {
        delete m_btConfig;
        QFile(m_configFileName).remove();
    }

    void gettingNonSetOptionsTest()
    {
        QVERIFY(m_btConfig->getValue<QString>("settings/defaults/standardBible") == "KJV");
        QVERIFY(m_btConfig->getValue<bool>("presentation/verseNumbers") == false);
        QVERIFY(m_btConfig->getValue<Search::BtSearchOptionsArea::SearchType>("gui/windows/searchType") == Search::BtSearchOptionsArea::AndType);
    }

    void gettingSettingsOptionsTest()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        m_btConfig->setValue("gui/showTextWindowHeaders", false);
        m_btConfig->setValue<alignmentMode>("gui/alignmentMode", autoCascade);
        QVERIFY(m_btConfig->getValue<QString>("settings/defaults/standardBible") == "Lut1940");
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == false);
        QVERIFY(m_btConfig->getValue<alignmentMode>("gui/alignmentMode") == autoCascade);
    }

    void deletingOptionsTest()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        QVERIFY(m_btConfig->getValue<QString>("settings/defaults/standardBible") == "Lut1940");

        m_btConfig->deleteValue("settings/defaults/standardBible");
        QVERIFY(m_btConfig->getValue<QString>("settings/defaults/standardBible") == "KJV");
    }

    void stringMapTest()
    {
        BtConfig::StringMap map;
        map.insert("TestBla", "Teststring1");
        map.insert("TestBlu", "Teststring2");
        m_btConfig->setValue<BtConfig::StringMap>("properties/searchScopes", map);

        reloadConfig();

        BtConfig::StringMap newMap = m_btConfig->getValue<BtConfig::StringMap>("properties/searchScopes");
        QVERIFY(newMap.value("TestBla") == "Teststring1");
        QVERIFY(newMap.value("TestBlu") == "Teststring2");
    }

    void sessionNamesTest()
    {
        QVERIFY(m_btConfig->getCurrentSessionName() == "default session");
        QVERIFY(m_btConfig->getAllSessionNames().size() == 1);

        m_btConfig->switchToSession("One");
        m_btConfig->switchToSession("Two");
        m_btConfig->switchToSession("Three");
        QVERIFY(m_btConfig->getAllSessionNames().size() == 4);
        QVERIFY(m_btConfig->getCurrentSessionName() == "Three");
    }

    void switchingSessionTest()
    {
        m_btConfig->switchToSession("First session");
        m_btConfig->setValue("gui/showTextWindowHeaders", true);
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == true);

        m_btConfig->switchToSession("Second session");
        m_btConfig->setValue("gui/showTextWindowHeaders", false);
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == false);

        m_btConfig->switchToSession("First session");
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == true);
    }

    void deletingSessionTest()
    {
        // This test depends on the default value of gui/showTextWindowHeaders, I guess that's ok since that value will probably not change in the forseable future.
        m_btConfig->switchToSession("Third session");
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == true); // if this line fails the defaults have changed
        m_btConfig->setValue("gui/showTextWindowHeaders", false);
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == false);

        QVERIFY(m_btConfig->deleteSession("Third session") == false);
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == false);

        m_btConfig->switchToSession("Fourth session");
        QVERIFY(m_btConfig->deleteSession("Third session") == true);

        m_btConfig->switchToSession("Third session");
        QVERIFY(m_btConfig->getValue<bool>("gui/showTextWindowHeaders") == true);
    }

    void groupTest()
    {
        m_btConfig->getValue<bool>("presentation/lineBreaks");
        m_btConfig->beginGroup("presentation");
            m_btConfig->getValue<bool>("lineBreaks");
        m_btConfig->endGroup();
        m_btConfig->getValue<bool>("presentation/lineBreaks");
        m_btConfig->getValue<bool>("gui/logo");

        QVERIFY(m_btConfig->getGroup() == "");
            m_btConfig->beginGroup("asdf");
            m_btConfig->beginGroup("qwer");
            m_btConfig->beginGroup("zxcv");
            m_btConfig->beginGroup("yuio");
                QVERIFY(m_btConfig->getGroup() == "asdf/qwer/zxcv/yuio/");
            m_btConfig->endGroup();
            m_btConfig->endGroup();
            m_btConfig->endGroup();
            m_btConfig->endGroup();
        QVERIFY(m_btConfig->getGroup() == "");
    }
};

QTEST_MAIN(BtConfigTest)

#include "btconfig_test.moc"
