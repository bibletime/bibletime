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

#ifndef BT_MODULECHOOSERBUTTON
#define BT_MODULECHOOSERBUTTON

#include <QToolButton>

#include "../../backend/btmoduletreeitem.h"
#include "../../backend/drivers/cswordmoduleinfo.h"


class BtModuleChooserBar;
class QMenu;
class QAction;

/**
* A toolbar button for choosing a module in a window. When user selects a module,
* button sends a signal. This button needs to get a signal back after a window
* module list has been changed. Only then the button will be updated.
* See BtModuleChooserBar.
*/
class BtModuleChooserButton : public QToolButton  {
        Q_OBJECT
    public:

        /** Filter out modules of wrong type from buttons module list.
        * See populateMenu() and BTModuleTreeItem. */
        struct TypeFilter : public BTModuleTreeItem::Filter {
            TypeFilter(CSwordModuleInfo::ModuleType t) {
                m_mType = t;
            }
            bool filter(CSwordModuleInfo const & mi) const override
            { return ((mi.type() == m_mType) && !mi.isLocked()); }
            CSwordModuleInfo::ModuleType m_mType;
        };

        /**
        * A new empty button. updateMenu() is needed to update the icon, menu items etc.
        */
        BtModuleChooserButton(BtModuleChooserBar *parent, CSwordModuleInfo::ModuleType mtype);

        // /** Returns the module name this button represents. Not needed?*/
        //QString module();
        // /** Returns the index used for this button.*/
        //int getIndex() const;
        // /** Updates existing menu items, setting their states.*/
        //void updateMenuItems();

    public:
        /**
        * Called after the window module list has changed. Updates the existing menu items
        * but doesn't add or remove them if the menu exists.
        * If the menu doesn't exist, creates it first and then updates it.
        * Updates also the icon.
        */
        void updateMenu(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules);

        /** Creates the menu from scratch and updates the items using updateMenu().*/
        void recreateMenu(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules);

    signals:
        /** User selected a module from menu to replace another module*/
        void sigModuleReplace ( int index, QString newModule );
        /** User selected a module from menu to add */
        void sigModuleAdd ( int index, QString module );
        /** User selected a module from menu to be removed */
        void sigModuleRemove ( int index );

    private slots:
        /** Handle the action signal from the menu.*/
        void moduleChosen(QAction* action );

    private:

        /** Returns the icon used for the current status.*/
        QIcon const & icon();

        /**
        * Populates the menu with language submenus and module items without setting
        * their states.
        */
        void populateMenu();
        /** Adds items to the menu recursively. */
        void addItemToMenu(BTModuleTreeItem* item, QMenu* menu);

    private:
        bool m_hasModule;
        int m_id;
        QAction* m_noneAction;
        CSwordModuleInfo::ModuleType m_moduleType;
        QString m_module;

        QMenu* m_popup;
        QList<QMenu*> m_submenus;
};

#endif
