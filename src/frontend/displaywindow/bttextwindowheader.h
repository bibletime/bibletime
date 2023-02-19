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

#include "btwindowmodulechooser.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>
#include "../../backend/drivers/cswordmoduleinfo.h"


class CDisplayWindow;
class BtTextWindowHeaderWidget;

class BtTextWindowHeader: public QWidget, public BtWindowModuleChooser {
        Q_OBJECT
    public:
        BtTextWindowHeader(CSwordModuleInfo::ModuleType modtype,
                           BtModuleList modules,
                           CDisplayWindow * window);

    public Q_SLOTS:
        /**
          The backend module list was updated, module list and widgets must be updated from
          scratch.
        */
        void slotBackendModulesChanged(BtModuleList newModules) override;

        /**
          The window module list was updated, module list and widgets must be updated.
        */
        void slotWindowModulesChanged(BtModuleList newModules) override;

    Q_SIGNALS:

        void moduleAdded(int index, CSwordModuleInfo * module);

        void moduleReplaced(int index, CSwordModuleInfo * newModule);

        void moduleRemoved(int index);

    private:
        /** Called when backend has changed and menus must be created from scratch.*/
        void initMenus();
        /** Updates all widgets without recreating them. */
        void updateWidgets();
        /**
        * Removes or adds widgets so that the count matches the limit.
        * The module count is the limit unless adjustToZero is true
        * when limit is 0 and list is emptied.
        */
        void adjustWidgetCount();
        /** Adds an empty widget to the header.*/
        BtTextWindowHeaderWidget* addWidget();

    private:
        QList<BtTextWindowHeaderWidget*> m_widgetList;
};
