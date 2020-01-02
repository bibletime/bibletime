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

#include "btmodulechooserbutton.h"

#include <QAction>
#include <QActionGroup>
#include <QSortFilterProxyModel>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../btmenuview.h"
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
            return m_popup->selectedModule().isEmpty()
                    ? CResMgr::modules::bible::icon_add()
                    : CResMgr::modules::bible::icon_unlocked();
        case CSwordModuleInfo::Commentary:
            return m_popup->selectedModule().isEmpty()
                   ? CResMgr::modules::commentary::icon_add()
                   : CResMgr::modules::commentary::icon_unlocked();
        case CSwordModuleInfo::Lexicon:
            return m_popup->selectedModule().isEmpty()
                   ? CResMgr::modules::lexicon::icon_add()
                   : CResMgr::modules::lexicon::icon_unlocked();
        case CSwordModuleInfo::GenericBook:
            return m_popup->selectedModule().isEmpty()
                   ? CResMgr::modules::book::icon_add()
                   : CResMgr::modules::book::icon_unlocked();
        default: //return as default the bible icon
            return CResMgr::modules::bible::icon_unlocked();
    }
}

void BtModuleChooserButton::updateMenu(QStringList newModulesToUse,
                                       QString newSelectedModule,
                                       int newButtonIndex,
                                       int newLeftLikeModules)
{
    if (newSelectedModule.isEmpty()) {
        setToolTip(tr("Select an additional work"));
    } else {
        setToolTip(QString(tr("Select a work [%1]")).arg(newSelectedModule));
    }
    m_popup->update(newModulesToUse,
                    newSelectedModule,
                    newButtonIndex,
                    newLeftLikeModules);
    if (auto const * const module =
        CSwordBackend::instance()->findModuleByName(newSelectedModule))
    {
        setIcon(module->moduleIcon());
    } else {
        setIcon(icon());
    }
}

/** Is called after a module was selected in the popup */
void BtModuleChooserButton::moduleChosen(
        CSwordModuleInfo const * const newModule)
{
    // If no module was previously selected:
    auto const & selectedModule = m_popup->selectedModule();
    if (selectedModule.isEmpty()) {
        if (newModule) {
            m_popup->setSelectedModule(newModule->name());
            setIcon(newModule->moduleIcon());
            emit sigModuleAdd(m_popup->buttonIndex(), newModule->name());
        }
    } else {
        if (newModule) {
            m_popup->setSelectedModule(newModule->name());
            setIcon(newModule->moduleIcon());
            emit sigModuleReplace(m_popup->buttonIndex(), newModule->name());
        } else {
            m_popup->setSelectedModule(QString());
            setIcon(icon());
            emit sigModuleRemove(m_popup->buttonIndex());
        }
    }
}
