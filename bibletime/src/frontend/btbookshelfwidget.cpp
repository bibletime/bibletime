/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/btbookshelfwidget.h"

#include <QActionGroup>
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSettings>
#include <QToolButton>
#include <QVBoxLayout>
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/config/cbtconfig.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h"
#include "frontend/bookshelfmanager/removepage/btremovepagetreemodel.h"
#include "frontend/btbookshelfview.h"
#include "util/cresmgr.h"
#include "util/directory.h"


#define ISGROUPING(v) (v).canConvert<BtBookshelfTreeModel::Grouping>()
#define TOGROUPING(v) (v).value<BtBookshelfTreeModel::Grouping>()

BtBookshelfWidget *BtBookshelfWidget::m_primaryWidget = 0;

BtBookshelfWidget::BtBookshelfWidget(WidgetTypeHint typeHint, QWidget *parent,
                                     Qt::WindowFlags flags)
        : QWidget(parent, flags), m_typeHint(typeHint), m_ownGrouping(false)
{
    // Register the only bookshelf dock as the primary widget
    if (m_primaryWidget == 0) {
        Q_ASSERT(typeHint == HintBookshelfDock);
        m_primaryWidget = this;
    } else {
        Q_ASSERT(m_primaryWidget->m_typeHint == HintBookshelfDock);
    }

    // Setup main tree model:
    switch (typeHint) {
        case HintBookshelfManagerRemovePage:
            m_treeModel = new BtRemovePageTreeModel(loadGroupingSetting(), this);
            break;
        case HintInstallModuleChooserDialog:
            m_treeModel = new BtInstallModuleChooserDialogModel(loadGroupingSetting(), this);
            break;
        case HintBookshelfDock:
            m_treeModel = new BtBookshelfTreeModel(loadGroupingSetting(), this);
            m_treeModel->setDefaultChecked(BtBookshelfTreeModel::MODULE_HIDDEN);
            break;
        case HintSearchModuleChooserDialog:
            m_treeModel = new BtBookshelfTreeModel(loadGroupingSetting(), this);
            m_treeModel->setDefaultChecked(BtBookshelfTreeModel::UNCHECKED);
            m_treeModel->setCheckable(true);
            break;
    }

    // Setup post-filter:
    m_postFilterModel = new BtBookshelfFilterModel(this);
    m_postFilterModel->setSourceModel(m_treeModel);

    // Init widgets and such:
    initActions();
    initMenus();
    initWidgets();

    // Connect treeview to model:
    m_treeView->setModel(m_postFilterModel);

    retranslateUi();

    connect(m_treeModel, SIGNAL(groupingOrderChanged()),
            this,        SIGNAL(groupingOrderChanged()));
    connect(m_nameFilterEdit,  SIGNAL(textEdited(QString)),
            m_postFilterModel, SLOT(setNameFilterFixedString(QString)));
    connect(m_treeView, SIGNAL(contextMenuActivated(QPoint)),
            this,       SLOT(slotShowContextMenu(QPoint)));
    connect(m_treeView, SIGNAL(moduleContextMenuActivated(CSwordModuleInfo*, QPoint)),
            this,       SLOT(slotShowItemContextMenu(CSwordModuleInfo*, QPoint)));

    // Start following grouping changes of the primary bookshelf widget:
    if (m_primaryWidget != this) {
        connect(m_primaryWidget, SIGNAL(groupingOrderChanged()),
                this,            SLOT(slotPrimaryGroupingOrderChanged()));
    }
}

BtBookshelfWidget::~BtBookshelfWidget() {
    // Intentionally empty
}

void BtBookshelfWidget::setSourceModel(QAbstractItemModel *model) {
    m_treeModel->setSourceModel(model);
}

void BtBookshelfWidget::initActions() {
    namespace DU = util::directory;
    namespace RM = CResMgr::mainIndex;

    // Grouping action group and actions:
    m_groupingActionGroup = new QActionGroup(this);
    connect(m_groupingActionGroup, SIGNAL(triggered(QAction*)),
            this, SLOT(slotGroupingActionTriggered(QAction*)));

    m_groupingCatLangAction = new QAction(this);
    m_groupingCatLangAction->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingCatLangAction->setChecked(true);
    m_groupingActionGroup->addAction(m_groupingCatLangAction);

    m_groupingCatAction = new QAction(this);
    m_groupingCatAction->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingActionGroup->addAction(m_groupingCatAction);

    m_groupingLangCatAction = new QAction(this);
    m_groupingLangCatAction->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingActionGroup->addAction(m_groupingLangCatAction);

    m_groupingLangAction = new QAction(this);
    m_groupingLangAction->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingActionGroup->addAction(m_groupingLangAction);

    m_groupingNoneAction = new QAction(this);
    m_groupingNoneAction->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingActionGroup->addAction(m_groupingNoneAction);


    m_showHideAction = new QAction(this);
    m_showHideAction->setIcon(DU::getIcon("layer-visible-on.svg"));
    m_showHideAction->setCheckable(true);
    connect(m_showHideAction, SIGNAL(toggled(bool)),
            this, SLOT(slotShowHideModules(bool)));
}

void BtBookshelfWidget::initMenus() {
    namespace DU = util::directory;
    namespace RM = CResMgr::mainIndex;

    // Grouping menu:
    m_groupingMenu = new QMenu(this);
    m_groupingMenu->setIcon(DU::getIcon(RM::grouping::icon));
    m_groupingMenu->addAction(m_groupingCatLangAction);
    m_groupingMenu->addAction(m_groupingCatAction);
    m_groupingMenu->addAction(m_groupingLangCatAction);
    m_groupingMenu->addAction(m_groupingLangAction);
    m_groupingMenu->addAction(m_groupingNoneAction);

    // Context menu
    m_contextMenu = new QMenu(this);
    m_contextMenu->addMenu(m_groupingMenu);
    m_contextMenu->addAction(m_showHideAction);

    // Item context menu
    m_itemContextMenu = m_contextMenu;
    /// \todo
}

void BtBookshelfWidget::initWidgets() {
    QVBoxLayout *layout(new QVBoxLayout);
    layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *toolBar(new QHBoxLayout);
    // Add a small margin between the edge of the window and the label (looks better)
    toolBar->setContentsMargins(3, 0, 0, 0);
    m_nameFilterLabel = new QLabel(this);
    toolBar->addWidget(m_nameFilterLabel);

    m_nameFilterEdit = new QLineEdit(this);
    m_nameFilterEdit->installEventFilter(this);
    m_nameFilterLabel->setBuddy(m_nameFilterEdit);
    toolBar->addWidget(m_nameFilterEdit);

    m_groupingButton = new QToolButton(this);
    m_groupingButton->setPopupMode(QToolButton::InstantPopup);
    m_groupingButton->setMenu(m_groupingMenu);
    m_groupingButton->setIcon(m_groupingMenu->icon());
    m_groupingButton->setAutoRaise(true);
    toolBar->addWidget(m_groupingButton);

    m_showHideButton = new QToolButton(this);
    m_showHideButton->setDefaultAction(m_showHideAction);
    m_showHideButton->setAutoRaise(true);
    toolBar->addWidget(m_showHideButton);
    layout->addLayout(toolBar);

    m_treeView = new BtBookshelfView(this);
    layout->addWidget(m_treeView);
    setLayout(layout);
}

void BtBookshelfWidget::retranslateUi() {
    m_nameFilterLabel->setText(tr("Fi&lter:"));
    m_groupingButton->setText(tr("Grouping"));
    m_groupingButton->setToolTip(tr("Change the grouping of items in the bookshelf."));

    m_groupingMenu->setTitle(tr("Grouping"));
    m_groupingCatLangAction->setText(tr("Category/Language"));
    m_groupingCatAction->setText(tr("Category"));
    m_groupingLangCatAction->setText(tr("Language/Category"));
    m_groupingLangAction->setText(tr("Language"));
    m_groupingNoneAction->setText(tr("No grouping"));
    m_showHideAction->setText(tr("Show/hide works"));
}

BtBookshelfTreeModel::Grouping BtBookshelfWidget::loadGroupingSetting() {
    QSettings *settings(CBTConfig::getConfig());

    const char *ownGroup;
    switch (m_typeHint) {
        case HintBookshelfManagerRemovePage:
            ownGroup = "GUI/BookshelfManager/RemovePage";
            break;
        case HintInstallModuleChooserDialog:
            ownGroup = "GUI/BookshelfManager/InstallPage/ConfirmDialog";
            break;
        case HintSearchModuleChooserDialog:
            ownGroup = "GUI/SearchDialog/ModuleChooserDialog";
            break;
        case HintBookshelfDock:
            ownGroup = "GUI/MainWindow/Docks/Bookshelf";
            break;
    }
    settings->beginGroup(ownGroup);

    QVariant v;
    if (m_typeHint != HintBookshelfDock) {
        if (settings->value("hasOwnGrouping", false).toBool()) {
            m_ownGrouping = true;
            v = settings->value("grouping");
            if (ISGROUPING(v)) {
                settings->endGroup();
                return TOGROUPING(v);
            }
        }
        settings->endGroup();
        settings->beginGroup("GUI/MainWindow/Docks/Bookshelf");
    }

    v = settings->value("grouping");
    settings->endGroup();

    if (ISGROUPING(v)) {
        return TOGROUPING(v);
    } else {
        return BtBookshelfTreeModel::defaultGrouping();
    }
}

void BtBookshelfWidget::saveGroupingSetting(BtBookshelfTreeModel::Grouping grouping) {
    /// \todo
    QSettings *settings(CBTConfig::getConfig());
    settings->beginGroup("GUI/MainWindow/Docks/Bookshelf");
    settings->setValue("grouping", QVariant::fromValue(grouping));
    settings->endGroup();
}

bool BtBookshelfWidget::eventFilter(QObject *object, QEvent *event) {
    Q_ASSERT(object == m_nameFilterEdit);
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent*>(event);
        switch (e->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                QApplication::sendEvent(m_treeView, event);
                return true;
            default:
                break;
        }
    }
    return false;
}

void BtBookshelfWidget::slotGroupingActionTriggered(QAction *action) {
    BtBookshelfTreeModel::Grouping grouping;
    if (action == m_groupingCatAction) {
        grouping.append(BtBookshelfTreeModel::GROUP_CATEGORY);
    }
    else if (action == m_groupingCatLangAction) {
        grouping.append(BtBookshelfTreeModel::GROUP_CATEGORY);
        grouping.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
    }
    else if (action == m_groupingLangAction) {
        grouping.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
    }
    else if (action == m_groupingLangCatAction) {
        grouping.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
        grouping.append(BtBookshelfTreeModel::GROUP_CATEGORY);
    }
    m_treeModel->setGroupingOrder(grouping);
    m_treeView->setRootIsDecorated(!grouping.isEmpty());

    saveGroupingSetting(grouping);
}

void BtBookshelfWidget::slotPrimaryGroupingOrderChanged() {
    Q_ASSERT(m_typeHint != HintBookshelfDock);

    if (m_ownGrouping) return;

    treeModel()->setGroupingOrder(m_primaryWidget->m_treeModel->groupingOrder());
}

void BtBookshelfWidget::slotShowHideModules(bool enable) {
    if (enable) {
        m_postFilterModel->setShowHidden(true);
    }
    else {
        m_postFilterModel->setShowHidden(false);
    }
}

void BtBookshelfWidget::slotShowContextMenu(const QPoint &pos) {
    m_contextMenu->popup(pos);
}

void BtBookshelfWidget::slotShowItemContextMenu(CSwordModuleInfo *module, const QPoint &pos)
{
    if (m_itemContextMenu != 0) {
        m_itemContextMenu->setProperty("BtModule", qVariantFromValue((void*) module));
        m_itemContextMenu->popup(pos);
    } else {
        m_itemContextMenu = m_contextMenu;
        slotShowItemContextMenu(module, pos);
    }
}
