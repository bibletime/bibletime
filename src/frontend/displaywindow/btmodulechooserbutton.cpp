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

#include "btmodulechooserbutton.h"

#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
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
    setIcon(icon());
    setMenu(m_popup);
    setPopupMode(QToolButton::InstantPopup);
    BT_CONNECT(m_popup, &BtModuleChooserMenu::sigModuleChosen,
               this,    &BtModuleChooserButton::moduleChosen);
}

QIcon const & BtModuleChooserButton::icon() {
    switch (m_popup->moduleType()) {
        case CSwordModuleInfo::Bible:
            return m_popup->selectedModule()
                    ? CResMgr::modules::bible::icon_unlocked()
                    : CResMgr::modules::bible::icon_add();
        case CSwordModuleInfo::Commentary:
            return m_popup->selectedModule()
                   ? CResMgr::modules::commentary::icon_unlocked()
                   : CResMgr::modules::commentary::icon_add();
        case CSwordModuleInfo::Lexicon:
            return m_popup->selectedModule()
                   ? CResMgr::modules::lexicon::icon_unlocked()
                   : CResMgr::modules::lexicon::icon_add();
        case CSwordModuleInfo::GenericBook:
            return m_popup->selectedModule()
                   ? CResMgr::modules::book::icon_unlocked()
                   : CResMgr::modules::book::icon_add();
        default: //return as default the bible icon
            return CResMgr::modules::bible::icon_unlocked();
    }
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
    if (newSelectedModule) {
        setIcon(newSelectedModule->moduleIcon());
    } else {
        setIcon(icon());
    }
}

/** Is called after a module was selected in the popup */
void BtModuleChooserButton::moduleChosen(
        CSwordModuleInfo * const newModule)
{
    if (m_popup->selectedModule()) { // If module was previously selected:
        if (newModule) {
            m_popup->setSelectedModule(newModule);
            setIcon(newModule->moduleIcon());
            Q_EMIT sigModuleReplace(m_popup->buttonIndex(), newModule);
        } else {
            m_popup->setSelectedModule(nullptr);
            setIcon(icon());
            Q_EMIT sigModuleRemove(m_popup->buttonIndex());
        }
    } else if (newModule) {
        m_popup->setSelectedModule(newModule);
        setIcon(newModule->moduleIcon());
        Q_EMIT sigModuleAdd(m_popup->buttonIndex(), newModule);
    }
}
