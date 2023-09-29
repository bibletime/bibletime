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

#include "btmenuview.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QActionGroup>
#include <QCoreApplication>
#include <QEvent>
#include <QIcon>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QVariant>
#include <Qt>
#include "../util/btassert.h"
#include "../util/btconnect.h"


namespace {

char const persistentIndexPropertyName[] = "BtMenuView persistent index";
constexpr auto const rebuildEventType = QEvent::User;

} // anonymous namespace

BtMenuView::BtMenuView(QWidget * parent)
    : BtMenuView(QString(), parent)
{}

BtMenuView::BtMenuView(QString const & title, QWidget * parent)
    : QMenu(title, parent)
    , m_model(nullptr)
    , m_actions(nullptr)
{}

BtMenuView::~BtMenuView() {
    delete m_actions;
}

void BtMenuView::setModel(QAbstractItemModel *model) {
    if (m_model)
        m_model->disconnect(this);
    m_model = model;
    rebuildMenu();
    if (model) {
        /* Instead of connecting each signal to rebuildMenu() directly, we
           instead queue events to this BtMenuView instance. The multiple
           "rebuild" events can be merged by the event handler to avoid
           consecutive rebuild events from causing needless rebuilds. */
        auto const triggerRebuild =
                [this] {
                    QCoreApplication::postEvent(this,
                                                new QEvent(rebuildEventType),
                                                Qt::HighEventPriority);
                };
        BT_CONNECT(model, &QAbstractItemModel::dataChanged, triggerRebuild);
        BT_CONNECT(model, &QAbstractItemModel::layoutChanged, triggerRebuild);
        BT_CONNECT(model, &QAbstractItemModel::modelReset, triggerRebuild);
        BT_CONNECT(model, &QAbstractItemModel::rowsInserted, triggerRebuild);
        BT_CONNECT(model, &QAbstractItemModel::rowsMoved, triggerRebuild);
        BT_CONNECT(model, &QAbstractItemModel::rowsRemoved, triggerRebuild);
    }
}

bool BtMenuView::event(QEvent * const e) {
    if (e->type() == rebuildEventType) {
        e->accept();
        QCoreApplication::removePostedEvents(this, rebuildEventType);
        rebuildMenu();
        return true;
    }
    return QMenu::event(e);
}

void BtMenuView::preBuildMenu(QActionGroup *) {
    // Intentionally empty. Reimplement in subclass if needed.
}

void BtMenuView::postBuildMenu(QActionGroup *) {
    // Intentionally empty. Reimplement in subclass if needed.
}

QAction *BtMenuView::newAction(QMenu *parentMenu, const QModelIndex &itemIndex) {
    QVariant displayData(m_model->data(itemIndex, Qt::DisplayRole));
    QVariant iconData(m_model->data(itemIndex, Qt::DecorationRole));
    QVariant toolTipData(m_model->data(itemIndex, Qt::ToolTipRole));
    QVariant statusTipData(m_model->data(itemIndex, Qt::StatusTipRole));
    QVariant whatsThisData(m_model->data(itemIndex, Qt::WhatsThisRole));

    QAction *childAction = new QAction(parentMenu);

    // Set text:
    if (displayData.canConvert<QString>()) {
        childAction->setText(displayData.toString());
    }

    // Set icon:
    if (iconData.canConvert<QIcon>()) {
        childAction->setIcon(iconData.value<QIcon>());
    }

    // Set tooltip:
    if (toolTipData.canConvert<QString>()) {
        childAction->setToolTip(toolTipData.toString());
    }

    // Set status tip:
    if (statusTipData.canConvert<QString>()) {
        childAction->setStatusTip(statusTipData.toString());
    }

    // Set whatsthis:
    if (whatsThisData.canConvert<QString>()) {
        childAction->setWhatsThis(whatsThisData.toString());
    }

    // Set checkable:
    if (m_model->flags(itemIndex).testFlag(Qt::ItemIsUserCheckable)) {
        childAction->setCheckable(true);
    }

    // Set checked:
    QVariant checkData(m_model->data(itemIndex, Qt::CheckStateRole));
    bool ok;
    Qt::CheckState const state =
            static_cast<Qt::CheckState>(checkData.toInt(&ok));
    if (ok)
        childAction->setChecked(state == Qt::Checked);

    return childAction;
}

QMenu *BtMenuView::newMenu(QMenu *parentMenu, const QModelIndex &itemIndex) {
    QVariant displayData(m_model->data(itemIndex, Qt::DisplayRole));
    QVariant iconData(m_model->data(itemIndex, Qt::DecorationRole));
    QVariant toolTipData(m_model->data(itemIndex, Qt::ToolTipRole));
    QVariant statusTipData(m_model->data(itemIndex, Qt::StatusTipRole));
    QVariant whatsThisData(m_model->data(itemIndex, Qt::WhatsThisRole));

    QMenu *childMenu = new QMenu(parentMenu);

    // Set text:
    if (displayData.canConvert<QString>()) {
        childMenu->setTitle(displayData.toString());
    }

    // Set icon:
    if (iconData.canConvert<QIcon>()) {
        childMenu->setIcon(iconData.value<QIcon>());
    }

    // Set tooltip:
    if (toolTipData.canConvert<QString>()) {
        childMenu->setToolTip(toolTipData.toString());
    }

    // Set status tip:
    if (statusTipData.canConvert<QString>()) {
        childMenu->setStatusTip(statusTipData.toString());
    }

    // Set whatsthis:
    if (whatsThisData.canConvert<QString>()) {
        childMenu->setWhatsThis(whatsThisData.toString());
    }

    return childMenu;
}

void BtMenuView::rebuildMenu() {
    removeMenus();

    delete m_actions;
    m_actions = m_model ? new QActionGroup(this) : nullptr;

    preBuildMenu(m_actions);
    if (m_model)
        buildMenu(this, QModelIndex());
    postBuildMenu(m_actions);

    if (m_actions)
        BT_CONNECT(m_actions, &QActionGroup::triggered,
                   [this](QAction * const action){
                        auto const indexProperty =
                                action->property(persistentIndexPropertyName);
                        if (!indexProperty.isValid())
                            return;
                        Q_EMIT triggered(
                                    indexProperty.toPersistentModelIndex());
                    });
}

void BtMenuView::buildMenu(QMenu *parentMenu, const QModelIndex &parentIndex) {
    BT_ASSERT(m_model);
    BT_ASSERT(m_actions);

    int children = m_model->rowCount(parentIndex);
    for (int i = 0; i < children; i++) {
        QModelIndex childIndex(m_model->index(i, 0, parentIndex));

        if (m_model->rowCount(childIndex) > 0) {
            QMenu *childMenu = newMenu(parentMenu, childIndex);

            if (childMenu != nullptr) {
                // Add the child menu and populate it:
                parentMenu->addMenu(childMenu);
                buildMenu(childMenu, childIndex);
            }
        } else {
            QAction *childAction = newAction(parentMenu, childIndex);

            if (childAction != nullptr) {

                // Add action to action group:
                childAction->setActionGroup(m_actions);

                // Add action to menu:
                parentMenu->addAction(childAction);

                // Pass persistent model index for handling of triggered():
                childAction->setProperty(persistentIndexPropertyName,
                                         QPersistentModelIndex(childIndex));
            }
        }
    }
}
void BtMenuView::removeMenus() {
    // QMenu::clear() is documented only to delete direct child actions:
    clear();

    // Delete submenus also:
    for (auto * const childMenu
         : findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly))
        childMenu->deleteLater();
}
