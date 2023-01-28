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

#include "btmodulechoosermenu.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QActionGroup>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QVariant>
#include <utility>
#include "../../backend/bookshelfmodel/btbookshelfmodel.h"
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../btmenuview.h"


namespace {

struct SortModel final: public QSortFilterProxyModel {

// Methods:

    SortModel(CSwordModuleInfo::ModuleType moduleType,
              QObject * parent = nullptr)
        : QSortFilterProxyModel(parent)
        , m_moduleType(moduleType)
        , m_showHidden(btConfig().value<bool>(
                           QStringLiteral("GUI/bookshelfShowHidden"),
                           false))
        , m_sourceModel(new BtBookshelfTreeModel(this))
    {
        m_sourceModel->setSourceModel(CSwordBackend::instance().model());
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

// Fields:

    CSwordModuleInfo::ModuleType const m_moduleType;
    bool const m_showHidden;
    BtBookshelfTreeModel * const m_sourceModel;

};

} // anonymous namespace


BtModuleChooserMenu::BtModuleChooserMenu(
        QString const & title,
        CSwordModuleInfo::ModuleType moduleType_,
        Flags flags,
        QWidget * parent)
    : BtMenuView(title, parent)
    , m_moduleType(moduleType_)
    , m_sortedModel(new SortModel(moduleType_, this))
    , m_flags(flags)
{
    setModel(static_cast<SortModel *>(m_sortedModel));
    BT_CONNECT(this, &BtMenuView::triggered,
               [this](QModelIndex const itemIndex) {
                    if (itemIndex.isValid()) {
                        auto const & sortedModel =
                                *static_cast<SortModel *>(m_sortedModel);
                        Q_EMIT sigModuleChosen(
                                    sortedModel.m_sourceModel->module(
                                        sortedModel.mapToSource(itemIndex)));
                    } else {
                        Q_EMIT sigModuleChosen(nullptr);
                    }
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

    auto const & sortedModel = *static_cast<SortModel *>(m_sortedModel);

    auto const sourceItemIndex = sortedModel.mapToSource(itemIndex);
    auto * const module =
            sortedModel.m_sourceModel->module(sourceItemIndex);
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
        && sortedModel.m_moduleType == CSwordModuleInfo::Bible
        && module->category() != CSwordModuleInfo::Bibles)
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

void BtModuleChooserMenu::setSelectedModule(
        CSwordModuleInfo * const newSelectedModule) noexcept
{
    m_selectedModule = newSelectedModule;
    rebuildMenu();
}
