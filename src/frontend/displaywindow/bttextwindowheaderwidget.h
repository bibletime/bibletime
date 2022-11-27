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

#include <QWidget>

#include <QObject>
#include <QString>
#include <QStringList>
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/drivers/btmodulelist.h"


class BtModuleChooserMenu;
class QAction;
class QLabel;
class QToolButton;

/**
* A widget for choosing a module in a window. Consists of a label and a button.
* When user selects a module,
* button sends a signal. This widget needs to get a message back after a window
* module list has been changed. Only then it will be updated.
* See BtTextWindowHeader.
*/
class BtTextWindowHeaderWidget : public QWidget {
        Q_OBJECT

    public:

        /**
        * A new empty widget. updateMenu() is needed to update the label, menu items etc.
        */
        BtTextWindowHeaderWidget(CSwordModuleInfo::ModuleType mtype,
                                 QWidget * parent = nullptr);

    public:
        /**
        * Called after the window module list has changed. Updates the module name and
        * the existing menu items but doesn't add or remove them if the menu exists.
        * If the menu doesn't exist, creates it first and then updates it.
        */
        void updateWidget(BtModuleList newModulesToUse,
                          CSwordModuleInfo * module,
                          int newIndex,
                          int leftLikeModules);

    Q_SIGNALS:
        /** User selected a module from menu to replace an existing module.*/
        void sigModuleReplace(int index, CSwordModuleInfo * newModule);
        /** User selected a module from menu to add. */
        void sigModuleAdd(int index, CSwordModuleInfo * module);
        /** User selected a module from menu to be removed. */
        void sigModuleRemove ( int index );

    private:

        int m_id;
        QAction* m_removeAction;
        BtModuleChooserMenu * m_replaceMenu;
        BtModuleChooserMenu * m_addMenu;
        QLabel* m_iconLabel;
        QLabel* m_label;
        QToolButton* m_button;
};
