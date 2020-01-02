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

#include "btmodulechooserbar.h"

#include <QAction>
#include <QDebug>
#include <QList>
#include <QToolBar>
#include "../../util/btconnect.h"
#include "../../util/btmodules.h"
#include "btmodulechooserbutton.h"
#include "creadwindow.h"


BtModuleChooserBar::BtModuleChooserBar(QWidget *parent)
        : QToolBar(parent),
        BtWindowModuleChooser(CSwordModuleInfo::Unknown, nullptr),
        m_idCounter(0),
        m_window(nullptr) {
    setAllowedAreas(Qt::TopToolBarArea);
    setFloatable(false);
}

void BtModuleChooserBar::slotBackendModulesChanged() {
    m_modules = m_window->getModuleList();
    adjustButtonCount();

    // Recreate all menus from scratch:
    int const leftLikeModules = leftLikeParallelModules(m_modules);
    for (int i = 0; i < m_buttonList.count(); i++)
        m_buttonList.at(i)->updateMenu(m_modules,
                                       (i >= m_modules.count())
                                       ? QString()
                                       : m_modules.at(i),
                                       i,
                                       leftLikeModules);
}

void BtModuleChooserBar::adjustButtonCount(bool adjustToZero) {
    int buttonCountDifference = 0;
    if (adjustToZero) {
        buttonCountDifference = m_buttonList.count();
    }
    else {
        buttonCountDifference = m_buttonList.count() - (m_modules.count() + 1);
    }
    if (m_moduleType == CSwordModuleInfo::GenericBook && !adjustToZero) {
        buttonCountDifference = (1 - m_buttonList.count()) * -1;
    }
    //if there are more buttons than modules, delete buttons
    if (buttonCountDifference > 0) {
        for (int j = 0; j < buttonCountDifference; j++) {
            delete m_buttonList.takeFirst();
        }
    }
    // if there are more modules than buttons, add buttons
    if (buttonCountDifference < 0) {
        for (int i = (buttonCountDifference * (-1)); i > 0; i--) {
            addButton();
        }
    }
}

void BtModuleChooserBar::slotWindowModulesChanged() {
    m_modules = m_window->getModuleList();
    adjustButtonCount();
    updateButtonMenus();
}

BtModuleChooserButton* BtModuleChooserBar::addButton() {
    BtModuleChooserButton* b = new BtModuleChooserButton(m_moduleType, this);
    QAction* a = addWidget(b);
    m_buttonList.append(b);

    /* The button sends signals directly to the window which then signals back
       when the module list has changed. Changes to the module list may mean
       deletion of the buttons, hence we must queue these signals. Otherwise,
       when triggered via QAction::triggered(), a mouse release event on the
       widget with that QAction may follow, but the widget might already have
       been deleted. */
    BT_CONNECT(b,        &BtModuleChooserButton::sigModuleAdd,
               m_window, &CReadWindow::slotAddModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,        &BtModuleChooserButton::sigModuleReplace,
               m_window, &CReadWindow::slotReplaceModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,        &BtModuleChooserButton::sigModuleRemove,
               m_window, &CReadWindow::slotRemoveModule,
               Qt::QueuedConnection);

    a->setVisible(true);
    return b;
}


/** Sets the modules which are chosen in this module chooser bar. */
void BtModuleChooserBar::setModules( QStringList useModules,CSwordModuleInfo::ModuleType type, CReadWindow* window) {
    m_modules = useModules;
    m_window = window;
    m_moduleType = type;

    clear();

    adjustButtonCount(true);

    //if (!useModules.count()) return;
    for (int i = 0; i < useModules.count(); i++) {
        addButton();
    }
    if (!(m_moduleType == CSwordModuleInfo::GenericBook)) {
        addButton(); // for ADD button
    }
    updateButtonMenus();

    BT_CONNECT(m_window, &CReadWindow::sigModuleListSet,
               this,     &BtModuleChooserBar::slotBackendModulesChanged);
    BT_CONNECT(m_window, &CReadWindow::sigModuleListChanged,
               this,     &BtModuleChooserBar::slotWindowModulesChanged);
}

void BtModuleChooserBar::updateButtonMenus() {
    int const leftLikeModules = leftLikeParallelModules(m_modules);
    for (int i = 0; i < m_buttonList.count(); i++)
        m_buttonList.at(i)->updateMenu(m_modules,
                                       (i >= m_modules.count())
                                       ? QString()
                                       : m_modules.at(i),
                                       i,
                                       leftLikeModules);
}
