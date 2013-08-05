/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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

    Q_OBJECT

    public: /* Methods: */

        BibleTimeApp(int &argc, char **argv);
        ~BibleTimeApp();

        inline void startInit() { m_init = true; }
        bool initBtConfig();
        bool initDisplayTemplateManager();

    private: /* Fields: */

        bool m_init;

};

#define bApp (static_cast<BibleTimeApp *>(QCoreApplication::instance()))

#endif
