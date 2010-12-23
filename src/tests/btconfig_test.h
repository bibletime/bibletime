#ifndef BTCONFIG_TEST_H
#define BTCONFIG_TEST_H

#include <qglobal.h>
#include <QObject>
#include <QTest>

class BtConfigTest: public QObject
{
Q_OBJECT
private slots:
    void initTestCase()
    {
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
