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

    void addingConfigOptions()
    {
        m_btConfig->setValue("settings/defaults/standardBible", "Lut1940");
        m_btConfig->setValue("gui/showTextWindowHeaders", false);
        m_btConfig->setValue<btconfigtypes::alignmentMode>("gui/alignmentMode", btconfigtypes::autoCascade);
        QVERIFY(true);
    }

    void retrievingSetOptions()
    {
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "Lut1940");
        QVERIFY(m_btConfig->getValue("gui/showTextWindowHeaders").toBool() == false);
        QVERIFY(m_btConfig->getValue("gui/alignmentMode").value<btconfigtypes::alignmentMode>() == btconfigtypes::autoCascade);
    }

    void retrievingNonSetOptions()
    {
        QVERIFY(m_btConfig->getValue("settings/defaults/standardCommentary").toString() == "MHC");
        QVERIFY(m_btConfig->getValue("presentation/verseNumbers").toBool() == false);
        QVERIFY(m_btConfig->getValue("gui/windows/searchType").value<Search::BtSearchOptionsArea::SearchType>() == Search::BtSearchOptionsArea::AndType);
    }

    void deletingOptions()
    {
        m_btConfig->deleteValue("settings/defaults/standardBible");
        QVERIFY(m_btConfig->getValue("settings/defaults/standardBible").toString() == "KJV");
    }

    void cleanupTestCase()
    {
        delete m_btConfig;
        QFile(m_configFileName).remove();
    }
};

QTEST_MAIN(BtConfigTest)

#include "btconfig_test.moc"
