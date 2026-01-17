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

#include "btmodulechoosermenu.h"

#include <QAction>
#include <QActionGroup>
#include "../../backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../btbookshelfdockwidget.h"
#include "../btmenuview.h"


BtModuleChooserMenu::BtModuleChooserMenu(
        QString const & title,
        CSwordModuleInfo::ModuleType moduleType_,
        Flags flags,
        QWidget * parent)
    : BtMenuView(title, parent)
    , m_filterModel(new BtBookshelfFilterModel(this))
    , m_flags(flags)
{
    m_filterModel->setModuleChooserType(moduleType_);
    m_filterModel->setSourceModel(
                BtBookshelfDockWidget::getInstance()->treeModel());
    setModel(m_filterModel);

    BT_CONNECT(this, &BtMenuView::triggered,
               [this](QModelIndex const itemIndex) {
                    Q_EMIT sigModuleChosen(m_filterModel->module(itemIndex));
               });
}

void BtModuleChooserMenu::preBuildMenu(QActionGroup * actionGroup) {
    BT_ASSERT(actionGroup);
    actionGroup->setExclusive(true);

    if (m_flags & AddNoneButton) {
        QAction * noneAction = new QAction(this);
        noneAction->setCheckable(true);
        noneAction->setText(tr("NONE"));
        noneAction->setChecked(!m_selectedModule);
        noneAction->setDisabled(
                    m_newModulesToUse.size() <= 1
                    || (m_buttonIndex <= 0 && m_leftLikeModules <= 1));
        noneAction->setActionGroup(actionGroup);
        BT_CONNECT(noneAction, &QAction::triggered,
                   [this]{ Q_EMIT triggered(QModelIndex()); });
        addAction(noneAction);

        addSeparator();
    }
}

QAction * BtModuleChooserMenu::newAction(QMenu * parentMenu,
                                         QModelIndex const & itemIndex)
{
    auto * const action = BtMenuView::newAction(parentMenu, itemIndex);
    auto * const module = m_filterModel->module(itemIndex);
    BT_ASSERT(module);
    action->setCheckable(true);
    action->setChecked(module == m_selectedModule);

    if (m_flags & DisableSelectedModule) {
        action->setDisabled(module == m_selectedModule
                            || m_newModulesToUse.contains(module)
                            || module->isLocked());
    } else {
        action->setDisabled((module != m_selectedModule
                             && m_newModulesToUse.contains(module))
                            || module->isLocked());
    }

    // Disable non-Bible modules on first button:
    if ((m_flags & DisableNonBiblesOnFirstButton)
        && m_buttonIndex <= 0
        && *m_filterModel->moduleChooserType() == CSwordModuleInfo::Bible
        && module->category() != CSwordModuleInfo::Category::Bibles)
        action->setDisabled(true);

    return action;
}

void BtModuleChooserMenu::update(BtModuleList newModulesToUse,
                                 CSwordModuleInfo * newSelectedModule,
                                 int newButtonIndexIndex,
                                 int newLeftLikeModules)
{
    m_newModulesToUse = newModulesToUse;
    m_selectedModule = newSelectedModule;
    m_buttonIndex = newButtonIndexIndex;
    m_leftLikeModules = newLeftLikeModules;
    rebuildMenu();
}

QIcon BtModuleChooserMenu::buttonIcon() const {
    if (m_selectedModule)
        return CSwordModuleInfo::categoryIcon(m_selectedModule->category());
    return CSwordModuleInfo::categoryIconAdd(
                *m_filterModel->moduleChooserType());
}

void BtModuleChooserMenu::setSelectedModule(
        CSwordModuleInfo * const newSelectedModule) noexcept
{
    m_selectedModule = newSelectedModule;
    rebuildMenu();
}
