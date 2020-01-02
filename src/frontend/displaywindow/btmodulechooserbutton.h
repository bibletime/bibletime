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

#ifndef BT_MODULECHOOSERBUTTON
#define BT_MODULECHOOSERBUTTON

#include <QToolButton>

#include <QString>
#include <QStringList>
#include "../../backend/drivers/cswordmoduleinfo.h"


class BtModuleChooserMenu;

/* A toolbar button for choosing a module in a window. When user selects a
   module, button sends a signal. This button needs to get a signal back after
   a window module list has been changed. Only then the button will be updated.
   \see BtModuleChooserBar.
*/
class BtModuleChooserButton: public QToolButton {

        Q_OBJECT

    public: /* Methods: */

        /** \brief Constructs an new empty button.
            \note updateMenu() is needed to update the icon, menu items etc.
        */
        BtModuleChooserButton(CSwordModuleInfo::ModuleType mtype,
                              QWidget * parent = nullptr);

        void updateMenu(QStringList newModulesToUse,
                        QString thisModule,
                        int newIndex,
                        int leftLikeModules);

    signals:

        /** User selected a module from menu to replace another module. */
        void sigModuleReplace ( int index, QString newModule );

        /** User selected a module from menu to add. */
        void sigModuleAdd ( int index, QString module );

        /** User selected a module from menu to be removed. */
        void sigModuleRemove ( int index );

    private slots:

        /** Handle the action signal from the menu. */
        void moduleChosen(CSwordModuleInfo const * const module);

    private:

        /** Returns the icon used for the current status. */
        QIcon const & icon();

    private:

        BtModuleChooserMenu * m_popup;
};

#endif
