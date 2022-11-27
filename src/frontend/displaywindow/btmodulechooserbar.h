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
#include "btwindowmodulechooser.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include "../../backend/drivers/cswordmoduleinfo.h"


class CDisplayWindow;
class QWidget;
class BtModuleChooserButton;

class BtModuleChooserBar: public QToolBar, public BtWindowModuleChooser {
        Q_OBJECT
    public:
        BtModuleChooserBar(QWidget* parent);

        /** Initialize with module list.*/
        void setModules(BtModuleList useModules,
                        CSwordModuleInfo::ModuleType type,
                        CDisplayWindow * window);

    public Q_SLOTS:
        /**
        * The backend module list was updated, module list and widgets must be updated.
        * The signal comes from the window, not from the backend. The new list can
        * be shorter but not longer than the old list.
        */
        void slotBackendModulesChanged(BtModuleList newModules) override;

        /**
        * The window module list was changed, i.e. 1 module added, removed or replaced.
        */
        void slotWindowModulesChanged(BtModuleList newModules) override;

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
        int m_idCounter;
        CDisplayWindow * m_window;
        CSwordModuleInfo::ModuleType m_moduleType;
        QList<BtModuleChooserButton*> m_buttonList;
};
