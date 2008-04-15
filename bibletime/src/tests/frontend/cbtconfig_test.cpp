/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "tests/bibletime_test.h"

#include "frontend/cbtconfig.h"

#include <QtTest/QtTest>


void BibleTimeTest::frontend_cbtconfig_test() 
{
	QList<int> value1;
	value1 << -1 << 0 << 1 << 993738;
	QString value1String("-1,0,1,993738"); 
	QList<int> emptyValue;
	QString emptyValueString("");

	//conversion checks int-string
	QCOMPARE(CBTConfig::IntListToString(value1), value1String);
	QCOMPARE(CBTConfig::IntListToString(emptyValue), emptyValueString);

	//conversion checks string-int
	QCOMPARE(CBTConfig::StringToIntList(value1String), value1);
	QCOMPARE(CBTConfig::StringToIntList(emptyValueString), emptyValue);
	
	//roundtrip checks
	QCOMPARE(CBTConfig::StringToIntList(CBTConfig::IntListToString(value1)), value1);
	QCOMPARE(CBTConfig::StringToIntList(CBTConfig::IntListToString(emptyValue)), emptyValue);
}
