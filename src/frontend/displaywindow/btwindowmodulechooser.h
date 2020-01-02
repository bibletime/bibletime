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

#ifndef BT_WINDOWMODULECHOOSER
#define BT_WINDOWMODULECHOOSER

#include "../../backend/drivers/cswordmoduleinfo.h"
#include <QStringList>

class CDisplayWindow;

/**
* This is an abstract base class for buttonbar and text window header
* which work as module choosers in a text window.
*
* Signals or slots can't be inherited from this class for Qt's
* technical reasons. Add corresponding slots for the protected
* modulesChanged methods and delegate the message.
*
* Add these signals to the subclass or to some part of it:
*    User selected a module from menu to replace another module:
*    void sigModuleReplace ( int index, QString newModule );
*    User selected a module from menu to add:
*    void sigModuleAdd ( int index, QString module );
*    User selected a module from menu to be removed:
*    void sigModuleRemove ( int index );
*
* Connect slots and signals to the window's corresponding signals and slots.
*/
class BtWindowModuleChooser {
    public:
        BtWindowModuleChooser(CSwordModuleInfo::ModuleType moduleType,
                              CDisplayWindow *parentWindow)
                : m_window(parentWindow), m_moduleType (moduleType) {}

        /**
        * The backend module list was updated, module list and widgets must be updated.
        * This expects that the window module list has already been updated, so
        * the corresponding slot should be connected to the window, not to the backend.
        */
        virtual void slotBackendModulesChanged() = 0;
        /** Modules have been added, replaced or removed in the window without backend changing.*/
        virtual void slotWindowModulesChanged() = 0;


    protected:
        CDisplayWindow* m_window;
        CSwordModuleInfo::ModuleType m_moduleType;
        /** The cache of the window module list. Kept for convenience.*/
        QStringList m_modules;
};

#endif
