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

#include <QToolBar>

#include <QList>
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"


class CDisplayWindow;
class BtModuleChooserButton;

class BtModuleChooserBar final: public QToolBar {

    Q_OBJECT

public:

    BtModuleChooserBar(QWidget * parent);

    /** Initialize with module list.*/
    void setModules(BtModuleList useModules,
                    CSwordModuleInfo::ModuleType type,
                    CDisplayWindow * window);

private Q_SLOTS:

    void setModules(BtModuleList newModules);

private: /* Methods: */

    /** Adds an empty button to the toolbar.*/
    BtModuleChooserButton * addButton();

    /** Updates every button's menu without recreating it.*/
    void updateButtonMenus(BtModuleList const & modules);

    /**
       \brief Ensures exactly numButtons buttons..
       \param[in] numButtons The exact number of buttons required.
    */
    void adjustButtonCount(int const numButtons);

private: /* Fields: */

    CDisplayWindow * m_window = nullptr;
    CSwordModuleInfo::ModuleType m_moduleType = CSwordModuleInfo::Unknown;
    QList<BtModuleChooserButton *> m_buttonList;

};
