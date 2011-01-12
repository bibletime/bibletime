#include <QObject>
#include <QTest>
#include <QFile>
#include <qglobal.h>
#include "backend/config/btconfig.h"
#include "backend/config/btconfigtypes.h"

#include <QtDebug>
class BtConfigTest: public QObject
{
    Q_OBJECT

    QString m_configFileName;
    BtConfig* m_btConfig;
private slots:
    void initTestCase()
    {
        m_configFileName = "testsettings.ini";
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
        m_btConfig->setValue<btconfigtypes::alignmentMode>("gui/alignmentMode", btconfigtypes::autoCascade);
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "Lut1940");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == false);
        QVERIFY(m_btConfig->getValue("gui/alignmentMode").value<btconfigtypes::alignmentMode>() == btconfigtypes::autoCascade);
    }

    void deletingOptionsTest()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "Lut1940");

        m_btConfig->deleteValue("settings/defaults/standardBible");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
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

        QVERIFY(m_btConfig->deleteSession("First Session") == false);

        m_btConfig->switchToSession("Fourth session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");

        QVERIFY(m_btConfig->deleteSession("First Session") == true);

        m_btConfig->switchToSession("Third session");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
    }
};

QTEST_MAIN(BtConfigTest)

#include "btconfig_test.moc"
