/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BIBLETIMEAPP_H
#define BIBLETIMEAPP_H

//BibleTime
#include "util/cpointers.h"

#include <QApplication>

/** The BibleTimeApp class is used to clean up all instances of the backend and to delete all created module objects.
  * @author The BibleTime team
  */
class BibleTimeApp : public QApplication, public CPointers {
public:
    BibleTimeApp(int &argc, char **argv);
    ~BibleTimeApp();

protected:
//	void initDCOP();
};

#endif
