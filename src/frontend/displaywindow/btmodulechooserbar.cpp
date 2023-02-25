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

#include "btmodulechooserbar.h"

#include <QAction>
#include <Qt>
#include <utility>
#include "../../util/btconnect.h"
#include "../../util/btmodules.h"
#include "../displaywindow/cdisplaywindow.h"
#include "btmodulechooserbutton.h"


BtModuleChooserBar::BtModuleChooserBar(QWidget * parent)
    : QToolBar(parent)
{
    setAllowedAreas(Qt::TopToolBarArea);
    setFloatable(false);
}

void BtModuleChooserBar::setModules(BtModuleList newModules) {
    m_modules = std::move(newModules);
    if (m_moduleType != CSwordModuleInfo::GenericBook)
        adjustButtonCount();
    updateButtonMenus();
}

void BtModuleChooserBar::adjustButtonCount() {
    int buttonCountDifference = m_buttonList.count() - (m_modules.count() + 1);
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

BtModuleChooserButton * BtModuleChooserBar::addButton() {
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
               m_window, &CDisplayWindow::slotAddModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,        &BtModuleChooserButton::sigModuleReplace,
               m_window, &CDisplayWindow::slotReplaceModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,        &BtModuleChooserButton::sigModuleRemove,
               m_window, &CDisplayWindow::slotRemoveModule,
               Qt::QueuedConnection);

    a->setVisible(true);
    return b;
}


/** Sets the modules which are chosen in this module chooser bar. */
void BtModuleChooserBar::setModules(BtModuleList useModules,
                                    CSwordModuleInfo::ModuleType type,
                                    CDisplayWindow * window)
{
    m_modules = std::move(useModules);
    m_window = window;
    m_moduleType = type;

    clear();
    qDeleteAll(m_buttonList);
    m_buttonList.clear();

    //if (!useModules.count()) return;
    for (int i = 0; i < m_modules.size(); i++) {
        addButton();
    }
    if (!(m_moduleType == CSwordModuleInfo::GenericBook)) {
        addButton(); // for ADD button
    } else {
        BT_ASSERT(m_modules.size() == 1);
    }
    updateButtonMenus();

    BT_CONNECT(m_window, &CDisplayWindow::sigModuleListSet,
               this, qOverload<BtModuleList>(&BtModuleChooserBar::setModules));
    BT_CONNECT(m_window, &CDisplayWindow::sigModuleListChanged,
               this, qOverload<BtModuleList>(&BtModuleChooserBar::setModules));
}

void BtModuleChooserBar::updateButtonMenus() {
    int const leftLikeModules = leftLikeParallelModules(m_modules);
    for (int i = 0; i < m_buttonList.count(); i++)
        m_buttonList.at(i)->updateMenu(m_modules,
                                       (i >= m_modules.count())
                                       ? nullptr
                                       : m_modules.at(i),
                                       i,
                                       leftLikeModules);
}
