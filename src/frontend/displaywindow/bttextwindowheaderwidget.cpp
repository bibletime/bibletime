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

#include "bttextwindowheaderwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QToolTip>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../btmenuview.h"
#include "bttextwindowheader.h"



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

    MenuView(QString const & title,
             CSwordModuleInfo::ModuleType moduleType,
             bool disableNonBiblesOnFirstButton,
             QWidget * parent = nullptr)
        : BtMenuView(title, parent)
        , m_sortedModel(new SortModel(moduleType, this))
        , m_disableNonBiblesOnFirstButton(disableNonBiblesOnFirstButton)
    {
        setModel(m_sortedModel);
    }

    void preBuildMenu(QActionGroup * actionGroup) final override {
        BT_ASSERT(actionGroup);
        actionGroup->setExclusive(true);
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
        if (m_disableNonBiblesOnFirstButton
            && m_buttonIndex <= 0
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
    bool const m_disableNonBiblesOnFirstButton;
    QAction * m_noneAction;
    QStringList m_newModulesToUse;
    QString m_selectedModule;
    int m_buttonIndex;
    int m_leftLikeModules;

};

} // anonymous namespace

BtTextWindowHeaderWidget::BtTextWindowHeaderWidget(
        CSwordModuleInfo::ModuleType mtype,
        QWidget * parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_label = new QLabel("", this);
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    m_label->setSizePolicy(sizePolicy);
    m_label->setStyleSheet("QLabel{font-weight:bold}");
    layout->addWidget(m_label, 0, Qt::AlignRight);

    m_button = new QToolButton( this );
    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_button->setPopupMode( QToolButton::InstantPopup );
    m_button->setArrowType(Qt::NoArrow);
    m_button->setStyleSheet("QToolButton{margin:0px;}QToolButton::menu-indicator{subcontrol-position: center center;}");
    m_button->setToolTip( tr("Add/remove/replace") );

    auto * const popup  = new QMenu(m_button);
    m_button->setMenu(popup);

    m_removeAction = new QAction(tr("Remove"), popup);
    m_removeAction->setIcon(CResMgr::displaywindows::general::icon_removeModule());
    BT_CONNECT(m_removeAction, &QAction::triggered,
               [this] { emit sigModuleRemove(m_id); });
    popup->addAction(m_removeAction);

    // Add Replace and Add menus, both have all modules in them
    auto * const replaceMenu = new MenuView(tr("Replace"), mtype, true, popup);
    replaceMenu->setIcon(CResMgr::displaywindows::general::icon_replaceModule());
    BT_CONNECT(replaceMenu, qOverload<QModelIndex>(&BtMenuView::triggered),
               [this](QModelIndex index) {
                    auto const * menuView =
                            static_cast<MenuView *>(m_replaceMenu);
                    emit sigModuleReplace(
                            m_id,
                            menuView->m_sortedModel->data(index).toString());
                });
    popup->addMenu(replaceMenu);
    m_replaceMenu = replaceMenu;

    auto * const addMenu = new MenuView(tr("Add"), mtype, false, popup);
    addMenu->setIcon(CResMgr::displaywindows::general::icon_addModule());
    BT_CONNECT(addMenu, qOverload<QModelIndex>(&BtMenuView::triggered),
               [this](QModelIndex index) {
                    auto const * menuView =
                            static_cast<MenuView *>(m_addMenu);
                    emit sigModuleAdd(
                            m_id + 1,
                            menuView->m_sortedModel->data(index).toString());
                });
    popup->addMenu(addMenu);
    m_addMenu = addMenu;

    layout->addWidget(m_button, 0, Qt::AlignLeft);

    auto * const separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    layout->addWidget(separator);
}

void BtTextWindowHeaderWidget::updateWidget(QStringList newModulesToUse,
                                            QString thisModule,
                                            int newIndex,
                                            int leftLikeModules)
{
    m_label->setText(thisModule);
    m_id = newIndex;

    bool disableRemove = false;
    if (newModulesToUse.count() == 1 ||
        (newIndex == 0 && leftLikeModules == 1))
        disableRemove = true;
    m_removeAction->setDisabled(disableRemove);

    static_cast<MenuView *>(m_replaceMenu)->update(newModulesToUse,
                                                   thisModule,
                                                   newIndex,
                                                   leftLikeModules);
    static_cast<MenuView *>(m_addMenu)->update(newModulesToUse,
                                               thisModule,
                                               newIndex,
                                               leftLikeModules);
}
