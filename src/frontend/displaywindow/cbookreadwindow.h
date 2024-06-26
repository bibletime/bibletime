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

#include "cdisplaywindow.h"

#include <QList>
#include <QObject>
#include <QString>
#include "../../backend/drivers/cswordmoduleinfo.h"


class BtActionCollection;
class BtConfigCore;
class CBookTreeChooser;
class CMDIArea;
class QAction;

class CBookReadWindow: public CDisplayWindow {

    Q_OBJECT

public: // methods:

    CBookReadWindow(QList<CSwordModuleInfo *> const & modules,
                    CMDIArea * parent);

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::GenericBook; }

    void storeProfileSettings(BtConfigCore & windowConf) const override;
    void applyProfileSettings(BtConfigCore const & windowConf) override;
    static void insertKeyboardActions(BtActionCollection * const a);

protected: // methods:

    void initActions() override;
    void initToolbars() override;
    void initConnections() override;
    void setupMainWindowToolBars() override;

protected Q_SLOTS:

    void modulesChanged() override;

private: // fields:

    QAction * m_treeAction = nullptr;
    CBookTreeChooser * m_treeChooser;
    QDockWidget * m_treeChooserDock = nullptr;
};
