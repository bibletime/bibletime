/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BIBLETIMEAPP_H
#define BIBLETIMEAPP_H

#include <QApplication>


class BtIcons;

/**
  The BibleTimeApp class is used to clean up all instances of the backend and to
  delete all created module objects.
*/
class BibleTimeApp : public QApplication {

    Q_OBJECT

    public: /* Methods: */

        BibleTimeApp(int &argc, char **argv);
        ~BibleTimeApp();

        inline void startInit(bool const debugMode = false) {
            m_init = true;
            m_debugMode = debugMode;
        }

        bool initBtConfig();
        void initColorManager();
        bool initDisplayTemplateManager();
        void initIcons();

        bool debugMode() const { return m_debugMode; }

    private: /* Fields: */

        bool m_init;
        bool m_debugMode;
        BtIcons * m_icons;

};

#define btApp (static_cast<BibleTimeApp *>(BibleTimeApp::instance()))

#endif
