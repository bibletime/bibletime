#include <QObject>
#include <QTest>
#include <QFile>
#include <QDebug>
#include <qglobal.h>
#include "backend/config/btconfig.h"

namespace btconfiguration
{
class BtConfigTest: public QObject
{
    Q_OBJECT

    QString m_configFileName;
    BtConfig* m_btConfig;
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
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
        QVERIFY(m_btConfig->getValue("presentation/verseNumbers").toBool() == false);
        QVERIFY(m_btConfig->getValue("gui/windows/searchType").value<Search::BtSearchOptionsArea::SearchType>() == Search::BtSearchOptionsArea::AndType);
    }

    void gettingSettingsOptionsTest()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        m_btConfig->setValue("gui/showTextWindowHeaders", false);
        m_btConfig->setValue<alignmentMode>("gui/alignmentMode", autoCascade);
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "Lut1940");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == false);
        QVERIFY(m_btConfig->getValue("gui/alignmentMode").value<alignmentMode>() == autoCascade);
    }

    void deletingOptionsTest()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "Lut1940");

        m_btConfig->deleteValue("settings/defaults/standardBible");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
    }

    void stringMapTest()
    {
        StringMap map;
        map.insert(QObject::tr("Old testament"),          QString("Gen - Mal"));
        map.insert(QObject::tr("Moses/Pentateuch/Torah"), QString("Gen - Deut"));
        m_btConfig->setValue<StringMap>("test/map", map);

        StringMap newMap = m_btConfig->getValue("test/map").value<StringMap>();
        QVERIFY(newMap.value("Old testament") == "Gen - Mal");
        QVERIFY(newMap.value("Moses/Pentateuch/Torah") == "Gen - Deut");
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
        m_btConfig->setValue("settings/defaults/standardBible", "neUe");
        m_btConfig->setValue("gui/showTextWindowHeaders", true);
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "neUe");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == true);

        m_btConfig->switchToSession("Second session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == false);

        m_btConfig->switchToSession("First session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "neUe");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == true);
    }

    void deletingSessionTest()
    {
        m_btConfig->switchToSession("Third session");
        m_btConfig->setValue("settings/defaults/standardBible", "neUe");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "neUe");

        QVERIFY(m_btConfig->deleteSession("Third Session") == false);

        m_btConfig->switchToSession("Fourth session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");

        QVERIFY(m_btConfig->deleteSession("Third Session") == true);

        m_btConfig->switchToSession("Third session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
    }
};
} //btconfiguration

QTEST_MAIN(btconfiguration::BtConfigTest)

#include "btconfig_test.moc"
