/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMODULECHOOSERBUTTON_H
#define CMODULECHOOSERBUTTON_H

#include <QToolButton>
#include "util/cpointers.h"

#include <QList>
#include "backend/btmoduletreeitem.h"


class QMenu;
class CModuleChooserBar;
class CSwordModuleInfo;

/** The CModuleChooserButton displays a list of submenus sorted by language which contain the possible modules
  * which can be displayed together with the first one.
  * @author The BibleTime team
  */
class CModuleChooserButton : public QToolButton, public CPointers  {
        Q_OBJECT
    public:

        /** Filter out modules of wrong type. See populateMenu() and BTModuleTreeItem. */
        struct TypeFilter : public BTModuleTreeItem::Filter {
            TypeFilter(CSwordModuleInfo::ModuleType t) {
                m_mType = t;
            }
            bool filter(CSwordModuleInfo* mi) {
                return (mi->type() == m_mType);
            }
            CSwordModuleInfo::ModuleType m_mType;
        };

        CModuleChooserButton(CSwordModuleInfo* useModule, CSwordModuleInfo::ModuleType type, const int id, CModuleChooserBar *parent);
        ~CModuleChooserButton();

        CSwordModuleInfo* module();
        /**
        * Returns the id used for this button.
        */
        int getId() const;
        void updateMenuItems();

    protected:
        /** Populates the menu with language submenus and module items. */
        void populateMenu();
        /** Adds items to the menu recursively. */
        void addItemToMenu(BTModuleTreeItem* item, QMenu* menu);

    private:
        /**
        * Returns the icon used for the current status.
        */
        const QString iconName();

        bool m_hasModule;
        int m_id;
        QAction* m_noneAction;
        CSwordModuleInfo::ModuleType m_moduleType;
        CSwordModuleInfo* m_module;

        QMenu* m_popup;
        QList<QMenu*> m_submenus;

        CModuleChooserBar* m_moduleChooserBar;


    private slots:
        void moduleChosen(QAction* action );

    signals:
        void sigRemoveButton(const int ID);
        void sigAddButton();
        void sigChanged();
};
#endif
