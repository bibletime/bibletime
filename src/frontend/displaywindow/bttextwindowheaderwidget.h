/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_MODULECHOOSERHEADERWIDGET
#define BT_MODULECHOOSERHEADERWIDGET

#include <QWidget>

#include <QString>
#include <QList>
#include "../../backend/drivers/cswordmoduleinfo.h"


class BTModuleTreeItem;
class BtTextWindowHeader;
class QMenu;
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
        /** For internal use to mark the menu items */
        enum TypeOfAction {RemoveAction, AddAction, ReplaceAction};

        /**
        * A new empty widget. updateMenu() is needed to update the label, menu items etc.
        */
        BtTextWindowHeaderWidget(BtTextWindowHeader *parent, CSwordModuleInfo::ModuleType mtype);

    public:
        /**
        * Called after the window module list has changed. Updates the module name and
        * the existing menu items but doesn't add or remove them if the menu exists.
        * If the menu doesn't exist, creates it first and then updates it.
        */
        void updateWidget(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules);

        /** Creates the menu from scratch and updates the items using updateMenu().*/
        void recreateWidget(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules);

    signals:
        /** User selected a module from menu to replace an existing module.*/
        void sigModuleReplace ( int index, QString newModule );
        /** User selected a module from menu to add. */
        void sigModuleAdd ( int index, QString module );
        /** User selected a module from menu to be removed. */
        void sigModuleRemove ( int index );

    private slots:
        /** Handle the action signal from the menu.*/
        void moduleChosen(QAction* action );

    private:

        /**
        * Populates the menu with language submenus and module items without setting
        * their states.
        */
        void populateMenu();
        /** Adds items to the menu recursively. */
        void addItemToMenu(BTModuleTreeItem* item, QMenu* menu, TypeOfAction actionType);

    private:

        int m_id;
        QAction* m_removeAction;
        CSwordModuleInfo::ModuleType m_moduleType;
        QString m_module;
        QLabel* m_label;
        QToolButton* m_button;
        QMenu* m_popup;
        QList<QMenu*> m_submenus;
};

#endif
