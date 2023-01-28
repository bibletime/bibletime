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

#include "../backend/managers/cswordbackend.h"
#include <optional>
#include <QObject>
#include <QString>


class BtIcons;

/**
  The BibleTimeApp class is used to clean up all instances of the backend and to
  delete all created module objects.
*/
class BibleTimeApp : public QApplication {

    Q_OBJECT

public: // methods:

    BibleTimeApp(int &argc, char **argv);
    ~BibleTimeApp() override;

    void startInit() { m_init = true; }

    bool initBtConfig();
    void initLightDarkPalette();
    bool initDisplayTemplateManager();
    void initIcons();
    void initBackends();

    bool debugMode() const noexcept { return m_debugMode; }
    void setDebugMode(bool const debugMode) noexcept
    { m_debugMode = debugMode; }

private: // fields:

    bool m_init;
    bool m_debugMode;
    BtIcons * m_icons;
    std::optional<CSwordBackend> m_backend;

};

#define btApp (static_cast<BibleTimeApp *>(BibleTimeApp::instance()))
