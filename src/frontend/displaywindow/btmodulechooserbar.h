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

#ifndef BT_MODULECHOOSERBAR
#define BT_MODULECHOOSERBAR

#include "../../backend/drivers/cswordmoduleinfo.h"
#include "btwindowmodulechooser.h"
#include <QToolBar>
#include <QStringList>

class CReadWindow;
class BtModuleChooserButton;

class BtModuleChooserBar: public QToolBar, public BtWindowModuleChooser {
        Q_OBJECT
    public:
        BtModuleChooserBar(QWidget* parent);
        /** Initialize with module list.*/
        void setModules( QStringList useModules,CSwordModuleInfo::ModuleType type, CReadWindow* window);

    public slots:
        /**
        * The backend module list was updated, module list and widgets must be updated.
        * The signal comes from the window, not from the backend. The new list can
        * be shorter but not longer than the old list.
        */
        void slotBackendModulesChanged() override;

        /**
        * The window module list was changed, i.e. 1 module added, removed or replaced.
        */
        void slotWindowModulesChanged() override;

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
        CReadWindow* m_window;
        CSwordModuleInfo::ModuleType m_moduleType;
        QList<BtModuleChooserButton*> m_buttonList;
};

#endif
