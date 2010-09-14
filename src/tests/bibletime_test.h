/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BIBLETIME_TEST_H
#define BIBLETIME_TEST_H

#include <QObject>

class BibleTimeTest: public QObject
{
	Q_OBJECT

private slots:
	//frontend
    void frontend_cbtconfig_test();
    
    //dummy test
    void dummyTest();
};


#endif
