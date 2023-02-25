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
#include <QObject>
#include <QString>
#include <QStringList>
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"


class CDisplayWindow;
class QWidget;
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
    void updateButtonMenus();

    void adjustButtonCount();

private: /* Fields: */

    BtModuleList m_modules; /**< The cache of the window module list. */
    CDisplayWindow * m_window = nullptr;
    CSwordModuleInfo::ModuleType m_moduleType = CSwordModuleInfo::Unknown;
    QList<BtModuleChooserButton *> m_buttonList;

};
