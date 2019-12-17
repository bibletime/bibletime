/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
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


namespace {

struct SortModel final: public QSortFilterProxyModel {

/* Methods: */

    SortModel(CSwordModuleInfo::ModuleType moduleType,
              QObject * parent = nullptr)
        : QSortFilterProxyModel(parent)
        , m_moduleType(moduleType)
        , m_showHidden(btConfig().value<bool>("GUI/bookshelfShowHidden", false))
        , m_sourceModel(new BtBookshelfTreeModel(this))
    {
        m_sourceModel->setSourceModel(CSwordBackend::instance()->model());
        setSourceModel(m_sourceModel);
    }

    bool filterAcceptsRow(int sourceRow,
                          QModelIndex const & sourceParentIndex)
            const final override
    {
        auto const & model = *sourceModel();
        auto const itemIndex = model.index(sourceRow, 0, sourceParentIndex);

        // Do recursive hidden check, if configured:
        if (!m_showHidden
            && model.data(itemIndex,
                          BtBookshelfModel::ModuleHiddenRole).toBool())
            return false;

        // Do other recursive checks:
        return filterAcceptsRowNoHiddenCheck(sourceRow, sourceParentIndex);
    }

    bool filterAcceptsRowNoHiddenCheck(int sourceRow,
                                       QModelIndex const & sourceParentIndex)
            const
    {
        auto const & model =
                *static_cast<BtBookshelfTreeModel *>(sourceModel());
        auto const itemIndex = model.index(sourceRow, 0, sourceParentIndex);

        // Accept subtrees if it has any accepted children:
        if (auto const numRows = model.rowCount(itemIndex)) {
            for (int i = 0; i < numRows; ++i)
                if (filterAcceptsRowNoHiddenCheck(i, itemIndex))
                    return true;
            return false;
        }

        auto const * const module = model.module(itemIndex);
        BT_ASSERT(module);
        return (module->type() == m_moduleType)
               || ((m_moduleType == CSwordModuleInfo::Bible)
                   && (module->type() == CSwordModuleInfo::Commentary));
    }

/* Fields: */

    CSwordModuleInfo::ModuleType const m_moduleType;
    bool const m_showHidden;
    BtBookshelfTreeModel * const m_sourceModel;

};

class MenuView final: public BtMenuView {

public: /* Methods: */

    MenuView(CSwordModuleInfo::ModuleType moduleType,
             QWidget * parent = nullptr)
        : BtMenuView(parent)
        , m_sortedModel(new SortModel(moduleType, this))
    {
        setModel(m_sortedModel);
    }

    void preBuildMenu(QActionGroup * actionGroup) final override {
        BT_ASSERT(actionGroup);
        actionGroup->setExclusive(true);

        m_noneAction = new QAction(this);
        m_noneAction->setCheckable(true);
        m_noneAction->setText(BtModuleChooserButton::tr("NONE"));
        m_noneAction->setChecked(m_selectedModule.isEmpty());
        m_noneAction->setDisabled(
                    m_newModulesToUse.size() <= 1
                    || (m_buttonIndex <= 0 && m_leftLikeModules <= 1));
        m_noneAction->setActionGroup(actionGroup);
        BT_CONNECT(m_noneAction, &QAction::triggered,
                   [this]{ emit triggered(QModelIndex()); });
        addAction(m_noneAction);

        addSeparator();
    }

    QAction * newAction(QMenu * parentMenu,
                        QModelIndex const & itemIndex) final override
    {
        auto * const action = BtMenuView::newAction(parentMenu, itemIndex);
        auto const sourceItemIndex = m_sortedModel->mapToSource(itemIndex);
        auto const * const module =
                m_sortedModel->m_sourceModel->module(sourceItemIndex);
        BT_ASSERT(module);
        action->setCheckable(true);
        auto const & moduleName = module->name();
        action->setChecked(moduleName == m_selectedModule);
        action->setDisabled((moduleName != m_selectedModule
                             && m_newModulesToUse.contains(moduleName))
                            || module->isLocked());

        // Disable non-Bible modules on first button:
        if (m_buttonIndex <= 0
            && m_sortedModel->m_moduleType == CSwordModuleInfo::Bible
            && module->category() != CSwordModuleInfo::Bibles)
            action->setDisabled(true);

        return action;
    }

    void update(QStringList newModulesToUse,
                QString newSelectedModule,
                int newButtonIndexIndex,
                int newLeftLikeModules)
    {
        m_newModulesToUse = newModulesToUse;
        m_selectedModule = newSelectedModule;
        m_buttonIndex = newButtonIndexIndex;
        m_leftLikeModules = newLeftLikeModules;
    }

public: /* Fields: */

    SortModel * const m_sortedModel;
    QAction * m_noneAction;
    QStringList m_newModulesToUse;
    QString m_selectedModule;
    int m_buttonIndex;
    int m_leftLikeModules;

};

} // anonymous namespace

BtModuleChooserButton::BtModuleChooserButton(CSwordModuleInfo::ModuleType mtype,
                                             QWidget * parent)
    : QToolButton(parent)
    , m_popup(new MenuView(mtype, this))
{
    setIcon(icon());
    setMenu(static_cast<MenuView *>(m_popup));
    setPopupMode(QToolButton::InstantPopup);
    BT_CONNECT(static_cast<MenuView *>(m_popup),
               &BtMenuView::triggered,
               [this](QModelIndex const itemIndex) {
                   auto const & sortedModel =
                           *static_cast<MenuView const *>(
                               m_popup)->m_sortedModel;
                   moduleChosen(sortedModel.m_sourceModel->module(
                                    sortedModel.mapToSource(itemIndex)));
               });
}

QIcon const & BtModuleChooserButton::icon() {
    auto const & popup = *static_cast<MenuView const *>(m_popup);
    switch (popup.m_sortedModel->m_moduleType) {
        case CSwordModuleInfo::Bible:
            return popup.m_selectedModule.isEmpty()
                    ? CResMgr::modules::bible::icon_add()
                    : CResMgr::modules::bible::icon_unlocked();
        case CSwordModuleInfo::Commentary:
            return popup.m_selectedModule.isEmpty()
                   ? CResMgr::modules::commentary::icon_add()
                   : CResMgr::modules::commentary::icon_unlocked();
        case CSwordModuleInfo::Lexicon:
            return popup.m_selectedModule.isEmpty()
                   ? CResMgr::modules::lexicon::icon_add()
                   : CResMgr::modules::lexicon::icon_unlocked();
        case CSwordModuleInfo::GenericBook:
            return popup.m_selectedModule.isEmpty()
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
    static_cast<MenuView *>(m_popup)->update(newModulesToUse,
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
    auto & popup = *static_cast<MenuView *>(m_popup);
    // If no module was previously selected:
    if (popup.m_selectedModule.isEmpty()) {
        if (newModule) {
            popup.m_selectedModule = newModule->name();
            setIcon(newModule->moduleIcon());
            emit sigModuleAdd(popup.m_buttonIndex, newModule->name());
        }
    } else {
        if (newModule) {
            popup.m_selectedModule = newModule->name();
            setIcon(newModule->moduleIcon());
            emit sigModuleReplace(popup.m_buttonIndex, newModule->name());
        } else {
            popup.m_selectedModule.clear();
            setIcon(icon());
            emit sigModuleRemove(popup.m_buttonIndex);
        }
    }
}
