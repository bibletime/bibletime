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
        BtModuleChooserBar(QWidget* parent);

        /** Initialize with module list.*/
        void setModules(BtModuleList useModules,
                        CSwordModuleInfo::ModuleType type,
                        CDisplayWindow * window);

    private Q_SLOTS:

        void setModules(BtModuleList newModules);

    private:
        /** Adds an empty button to the toolbar.*/
        BtModuleChooserButton* addButton();
        /** Updates every button's menu without recreating it.*/
        void updateButtonMenus();
        /**
        * Removes or adds buttons so that the count matches the limit.
        * If given limit is -1 the module count is the limit.
        */
        void adjustButtonCount(bool adjustToZero = false);

    private:

        BtModuleList m_modules; /**< The cache of the window module list. */
        int m_idCounter = 0;
        CDisplayWindow * m_window = nullptr;
        CSwordModuleInfo::ModuleType m_moduleType = CSwordModuleInfo::Unknown;
        QList<BtModuleChooserButton*> m_buttonList;
};
