/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btmodulechooserbutton.h"

#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btconnect.h"
#include "btmodulechoosermenu.h"


BtModuleChooserButton::BtModuleChooserButton(CSwordModuleInfo::ModuleType mtype,
                                             QWidget * parent)
    : QToolButton(parent)
    , m_popup(
          new BtModuleChooserMenu(
              QString(),
              mtype,
              BtModuleChooserMenu::DisableNonBiblesOnFirstButton
              | BtModuleChooserMenu::AddNoneButton,
              this))
{
    setIcon(m_popup->buttonIcon());
    setMenu(m_popup);
    setPopupMode(QToolButton::InstantPopup);
    BT_CONNECT(m_popup, &BtModuleChooserMenu::sigModuleChosen,
               this,    &BtModuleChooserButton::moduleChosen);
}

void BtModuleChooserButton::updateMenu(BtModuleList newModulesToUse,
                                       CSwordModuleInfo * newSelectedModule,
                                       int newButtonIndex,
                                       int newLeftLikeModules)
{
    if (newSelectedModule) {
        setToolTip(tr("Select a work [%1]").arg(newSelectedModule->name()));
    } else {
        setToolTip(tr("Select an additional work"));
    }
    m_popup->update(std::move(newModulesToUse),
                    newSelectedModule,
                    newButtonIndex,
                    newLeftLikeModules);
    setIcon(m_popup->buttonIcon());
}

/** Is called after a module was selected in the popup */
void BtModuleChooserButton::moduleChosen(
        CSwordModuleInfo * const newModule)
{
    if (m_popup->selectedModule()) { // If module was previously selected:
        if (newModule) {
            m_popup->setSelectedModule(newModule);
            setIcon(m_popup->buttonIcon());
            Q_EMIT sigModuleReplace(m_popup->buttonIndex(), newModule);
        } else {
            m_popup->setSelectedModule(nullptr);
            setIcon(m_popup->buttonIcon());
            Q_EMIT sigModuleRemove(m_popup->buttonIndex());
        }
    } else if (newModule) {
        m_popup->setSelectedModule(newModule);
        setIcon(m_popup->buttonIcon());
        Q_EMIT sigModuleAdd(m_popup->buttonIndex(), newModule);
    }
}
