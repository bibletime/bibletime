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


class BtActionCollection;
class CBookTreeChooser;
class QAction;

class CBookReadWindow: public CDisplayWindow {

    Q_OBJECT

public: /* Methods: */

    CBookReadWindow(QList<CSwordModuleInfo *> const & modules,
                    CMDIArea * parent)
        : CDisplayWindow(modules, parent)
        , m_treeAction(nullptr)
        , m_treeChooser(nullptr) {}

    CSwordModuleInfo::ModuleType moduleType() const override
    { return CSwordModuleInfo::GenericBook; }

    void storeProfileSettings(BtConfigCore & windowConf) const override;
    void applyProfileSettings(BtConfigCore const & windowConf) override;
    static void insertKeyboardActions(BtActionCollection * const a);

protected: /* Methods: */

    void initActions() override;
    void initToolbars() override;
    void initConnections() override;
    void initView() override;
    void setupMainWindowToolBars() override;

protected Q_SLOTS:

    void modulesChanged() override;

private Q_SLOTS:

    /**
        * Is called when the action was executed to toggle the tree view.
        */
    void treeToggled();

private: /* Fields: */

    QAction * m_treeAction;
    CBookTreeChooser * m_treeChooser;
};
