#ifndef BTCONFIG_TEST_H
#define BTCONFIG_TEST_H

#include <QObject>
#include <QTest>
#include <QFile>
#include "backend/config/btconfig.h"

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

    void myFirstTest()
    {
        QVERIFY(1 == 1);
    }

    void mySecondTest()
    {
        QVERIFY(1 != 2);
    }

    void cleanupTestCase()
    {
        QFile(m_configFileName).remove();
    }
};

#endif //BTCONFIG_TEST_H
