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

#ifndef BT_TEXTWINDOWHEADER
#define BT_TEXTWINDOWHEADER

#include "btwindowmodulechooser.h"
#include <QStringList>
#include <QWidget>

class QAction;
class QToolButton;
class CDisplayWindow;
class BtTextWindowHeaderWidget;

class BtTextWindowHeader: public QWidget, public BtWindowModuleChooser {
        Q_OBJECT
    public:
        BtTextWindowHeader(CSwordModuleInfo::ModuleType modtype, QStringList modules, CDisplayWindow *window);

    public slots:
        /**
          The backend module list was updated, module list and widgets must be updated from
          scratch.
        */
        void slotBackendModulesChanged() override;

        /**
          The window module list was updated, module list and widgets must be updated.
        */
        void slotWindowModulesChanged() override;

    signals:
        /** User selected a module from menu to replace another module*/
        void sigModuleReplace ( int index, QString newModule );
        /** User selected a module from menu to add */
        void sigModuleAdd ( int index, QString module );
        /** User selected a module from menu to be removed */
        void sigModuleRemove ( int index );

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
        void adjustWidgetCount(bool adjustToZero = false);
        /** Adds an empty widget to the header.*/
        BtTextWindowHeaderWidget* addWidget();
        /** Sets the initial modules.*/
        void setModules( QStringList useModules );

    private:
        QList<BtTextWindowHeaderWidget*> m_widgetList;
};

#endif
