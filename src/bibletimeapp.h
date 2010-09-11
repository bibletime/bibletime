/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BIBLETIMEAPP_H
#define BIBLETIMEAPP_H

#include <QApplication>


/**
  The BibleTimeApp class is used to clean up all instances of the backend and to
  delete all created module objects.
*/
class BibleTimeApp : public QApplication {
    public:
        inline BibleTimeApp(int &argc, char **argv) : QApplication(argc, argv), m_init(false) {}
        ~BibleTimeApp();

        inline void startInit() { m_init = true; }

    private:
        bool m_init;
};

#endif
