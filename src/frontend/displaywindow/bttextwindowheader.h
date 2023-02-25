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

#include <QList>
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/drivers/btmodulelist.h"


class BtTextWindowHeaderWidget;

class BtTextWindowHeader final: public QWidget {

    Q_OBJECT

public: /* Methods: */

    BtTextWindowHeader(CSwordModuleInfo::ModuleType modtype,
                       BtModuleList modules,
                       QWidget * window = nullptr);

public Q_SLOTS:

    void setModules(BtModuleList newModules);

Q_SIGNALS:

    void moduleAdded(int index, CSwordModuleInfo * module);
    void moduleReplaced(int index, CSwordModuleInfo * newModule);
    void moduleRemoved(int index);

private: /* Methods: */

    /** Called when backend has changed and menus must be created from scratch.*/
    void initMenus();

    /** Updates all widgets without recreating them. */
    void updateWidgets(BtModuleList const & modules);

    /**
       \brief Ensures exactly numModules widgets.
       \param[in] numWidgets The exact number of widgets required.
    */
    void adjustWidgetCount(int const numWidgets);

    /** Adds an empty widget to the header. */
    BtTextWindowHeaderWidget * addWidget();

private: /* Fields: */

    QList<BtTextWindowHeaderWidget *> m_widgetList;
    CSwordModuleInfo::ModuleType const m_moduleType;

};
