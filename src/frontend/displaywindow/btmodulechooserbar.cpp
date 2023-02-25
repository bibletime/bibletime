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

void BtModuleChooserBar::setModules(BtModuleList modules) {
    if (m_moduleType != CSwordModuleInfo::GenericBook)
        adjustButtonCount(modules.size() + 1,
                          static_cast<CDisplayWindow *>(sender()));
    updateButtonMenus(modules);
}

void BtModuleChooserBar::adjustButtonCount(int const numButtons,
                                           CDisplayWindow * const window)
{
    if (m_buttonList.size() > numButtons) {
        auto toRemove = m_buttonList.size() - numButtons;
        do {
            delete m_buttonList.takeFirst();
        } while (--toRemove);
    } else if (numButtons > m_buttonList.size()) {
        auto toAdd = numButtons - m_buttonList.size();
        do {
            addButton(window);
        } while (--toAdd);
    }
}

BtModuleChooserButton *
BtModuleChooserBar::addButton(CDisplayWindow * const window) {
    BtModuleChooserButton* b = new BtModuleChooserButton(m_moduleType, this);
    QAction* a = addWidget(b);
    m_buttonList.append(b);

    /* The button sends signals directly to the window which then signals back
       when the module list has changed. Changes to the module list may mean
       deletion of the buttons, hence we must queue these signals. Otherwise,
       when triggered via QAction::triggered(), a mouse release event on the
       widget with that QAction may follow, but the widget might already have
       been deleted. */
    BT_CONNECT(b,      &BtModuleChooserButton::sigModuleAdd,
               window, &CDisplayWindow::slotAddModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,      &BtModuleChooserButton::sigModuleReplace,
               window, &CDisplayWindow::slotReplaceModule,
               Qt::QueuedConnection);
    BT_CONNECT(b,      &BtModuleChooserButton::sigModuleRemove,
               window, &CDisplayWindow::slotRemoveModule,
               Qt::QueuedConnection);

    a->setVisible(true);
    return b;
}


/** Sets the modules which are chosen in this module chooser bar. */
void BtModuleChooserBar::associateWithWindow(CDisplayWindow * const window) {
    auto const & modules = window->modules();
    m_moduleType = modules.first()->type();

    clear();
    qDeleteAll(m_buttonList);
    m_buttonList.clear();

    //if (!useModules.count()) return;
    for (int i = 0; i < modules.size(); i++) {
        addButton(window);
    }
    if (!(m_moduleType == CSwordModuleInfo::GenericBook)) {
        addButton(window); // for ADD button
    } else {
        BT_ASSERT(modules.size() == 1);
    }
    updateButtonMenus(modules);

    BT_CONNECT(window, &CDisplayWindow::sigModuleListChanged,
               this, qOverload<BtModuleList>(&BtModuleChooserBar::setModules));
}

void BtModuleChooserBar::updateButtonMenus(BtModuleList const & modules) {
    int const leftLikeModules = leftLikeParallelModules(modules);
    for (int i = 0; i < m_buttonList.count(); i++)
        m_buttonList.at(i)->updateMenu(modules,
                                       (i >= modules.size())
                                       ? nullptr
                                       : modules.at(i),
                                       i,
                                       leftLikeModules);
}
