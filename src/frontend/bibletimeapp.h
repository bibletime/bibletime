/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

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

        void startInit() { m_init = true; }

        bool initBtConfig();
        void initColorManager();
        bool initDisplayTemplateManager();
        void initIcons();

        bool debugMode() const noexcept { return m_debugMode; }
        void setDebugMode(bool const debugMode) noexcept
        { m_debugMode = debugMode; }

    private: /* Fields: */

        bool m_init;
        bool m_debugMode;
        BtIcons * m_icons;

};

#define btApp (static_cast<BibleTimeApp *>(BibleTimeApp::instance()))
