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

#include "frontend/btmenuview.h"

#include <QActionGroup>


BtMenuView::BtMenuView(QWidget *parent)
    : QMenu(parent), m_model(0), m_parentIndex(QModelIndex()), m_actions(0)
{
    connect(this, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShow()));
    connect(this, SIGNAL(aboutToHide()),
            this, SLOT(slotAboutToHide()));
}

BtMenuView::~BtMenuView() {
    if (m_actions != 0) {
        delete m_actions;
    }
}

void BtMenuView::setModel(QAbstractItemModel *model) {
    m_model = model;
    if (m_actions != 0) {
        delete m_actions;
    }
    m_indexMap.clear();
    m_parentIndex = QModelIndex();
}

void BtMenuView::setParentIndex(const QModelIndex &parentIndex) {
    if (parentIndex.isValid() && parentIndex.model() != m_model) return;
    m_parentIndex = parentIndex;
}

void BtMenuView::preBuildMenu() {
    // Intentionally empty. Reimplement in subclass if needed.
}

void BtMenuView::postBuildMenu() {
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
    if (qVariantCanConvert<QString>(displayData)) {
        childAction->setText(displayData.toString());
    }

    // Set icon:
    if (qVariantCanConvert<QIcon>(iconData)) {
        childAction->setIcon(iconData.value<QIcon>());
    }

    // Set tooltip:
    if (qVariantCanConvert<QString>(toolTipData)) {
        childAction->setToolTip(toolTipData.toString());
    }

    // Set status tip:
    if (qVariantCanConvert<QString>(statusTipData)) {
        childAction->setStatusTip(statusTipData.toString());
    }

    // Set whatsthis:
    if (qVariantCanConvert<QString>(whatsThisData)) {
        childAction->setWhatsThis(whatsThisData.toString());
    }

    // Set checkable:
    if (m_model->flags(itemIndex).testFlag(Qt::ItemIsUserCheckable)) {
        childAction->setCheckable(true);
    }

    // Set checked:
    QVariant checkData(m_model->data(itemIndex, Qt::CheckStateRole));
    bool ok;
    Qt::CheckState state = (Qt::CheckState) checkData.toInt(&ok);
    if (ok) {
        childAction->setChecked(state == Qt::Checked);
    }

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
    if (qVariantCanConvert<QString>(displayData)) {
        childMenu->setTitle(displayData.toString());
    }

    // Set icon:
    if (qVariantCanConvert<QIcon>(iconData)) {
        childMenu->setIcon(iconData.value<QIcon>());
    }

    // Set tooltip:
    if (qVariantCanConvert<QString>(toolTipData)) {
        childMenu->setToolTip(toolTipData.toString());
    }

    // Set status tip:
    if (qVariantCanConvert<QString>(statusTipData)) {
        childMenu->setStatusTip(statusTipData.toString());
    }

    // Set whatsthis:
    if (qVariantCanConvert<QString>(whatsThisData)) {
        childMenu->setWhatsThis(whatsThisData.toString());
    }

    return childMenu;
}

void BtMenuView::buildMenu(QMenu *parentMenu, const QModelIndex &parentIndex) {
    Q_ASSERT(m_model != 0);

    int children = m_model->rowCount(parentIndex);
    for (int i = 0; i < children; i++) {
        QModelIndex childIndex(m_model->index(i, 0, parentIndex));

        if (m_model->rowCount(childIndex) > 0) {
            QMenu *childMenu = newMenu(parentMenu, childIndex);

            if (childMenu != 0) {
                parentMenu->addMenu(childMenu);

                QVariant populate(childMenu->property("BtMenuView_NoPopulate"));
                if (!populate.isValid() || populate.toBool()) {
                    buildMenu(childMenu, childIndex);
                }
            }
        } else {
            QAction *childAction = newAction(parentMenu, childIndex);

            if (childAction != 0) {
                // Map index
                m_indexMap[childAction] = QPersistentModelIndex(childIndex);

                // Add action to action group:
                m_actions->addAction(childAction);

                // Add action to menu:
                parentMenu->addAction(childAction);
            }
        }
    }
}

void BtMenuView::slotAboutToShow() {
    if (m_model == 0) return;

    if (m_actions != 0) {
        delete m_actions;
    }
    m_indexMap.clear();

    m_actions = new QActionGroup(this);
    connect(m_actions, SIGNAL(triggered(QAction*)),
            this, SLOT(slotActionTriggered(QAction*)));

    preBuildMenu();
    buildMenu(this, m_parentIndex);
    postBuildMenu();
}

void BtMenuView::slotAboutToHide() {
    clear();
}

void BtMenuView::slotActionTriggered(QAction *action) {
    Q_ASSERT(m_indexMap.contains(action));
    QPersistentModelIndex itemIndex(m_indexMap.value(action));
    if (itemIndex.isValid()) {
        emit triggered(itemIndex);
    }
}
