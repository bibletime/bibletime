/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "test_btlistmodel.h"
#include "backend/models/btlistmodel.h"
#include <QtTest/QtTest>

void test_BtListModel::initTestCase() {
}

void test_BtListModel::appendItem_data() {
    QTest::addColumn<bool>("checkable");
    QTest::addColumn<QString>("title1");
    QTest::addColumn<QString>("title2");
    QTest::addColumn<QString>("tip1");
    QTest::addColumn<QString>("tip2");
    QTest::addColumn<int>("columns");
    QTest::newRow("T1") << false << "a" << "b" << "c" << "d" << 1;
    QTest::newRow("T2") << false << "a" << "b" << "c" << "d" << 2;
    QTest::newRow("T3") << true  << "a" << "b" << "c" << "d" << 1;
    QTest::newRow("T4") << true  << "a" << "b" << "c" << "d" << 2;
}

void test_BtListModel::appendItem() {
    QFETCH(bool, checkable);
    QFETCH(QString, title1);
    QFETCH(QString, title2);
    QFETCH(QString, tip1);
    QFETCH(QString, tip2);
    QFETCH(int, columns);

    BtListModel model(checkable, this, columns);
    QCOMPARE(model.columnCount(), columns);
    model.appendItem(title1, tip1);
    model.appendItem(title2, tip2);

    QStandardItem* item0 = model.item(0,0);
    QCOMPARE(item0->text(), title1);
    QCOMPARE(item0->toolTip(),tip1);
    QStandardItem* item1 = model.item(1,0);
    QCOMPARE(item1->text(), title2);
    QCOMPARE(item1->toolTip(),tip2);
    QCOMPARE(item0->isCheckable(),checkable);
}

QTEST_MAIN(test_BtListModel)

