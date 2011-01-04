#ifndef BTCONFIG_TEST_H
#define BTCONFIG_TEST_H

#include <QObject>
#include <QTest>
#include "backend/config/btconfig.h"

class BtConfigTest: public QObject
{
Q_OBJECT

    BtConfig* m_btConfig;
private slots:
    void initTestCase()
    {
        m_btConfig->getInstance();
        qDebug("called before everything else");
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
        qDebug("called after myFirstTest and mySecondTest");
    }
};

#endif //BTCONFIG_TEST_H
