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

#include "frontend/mainindex/btbookshelfview.h"

#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/drivers/cswordmoduleinfo.h"

BtBookshelfView::BtBookshelfView(QWidget *parent)
    : QTreeView(parent)
{
    header()->hide();
    setRootIsDecorated(false);

    connect(this, SIGNAL(activated(QModelIndex)),
            this, SLOT(slotItemActivated(QModelIndex)));
}

BtBookshelfView::~BtBookshelfView() {
    // Intentionally empty
}

void BtBookshelfView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Menu) {
        scrollTo(currentIndex());
        CSwordModuleInfo *i(getModule(currentIndex()));
        QRect itemRect(visualRect(currentIndex()));
        QPoint p(viewport()->mapToGlobal(itemRect.bottomLeft()));
        if (i == 0) {
            emit contextMenuActivated(p);
        } else {
            emit moduleContextMenuActivated(i, p);
        }
    } else {
        QTreeView::keyPressEvent(event);
    }
}

void BtBookshelfView::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::RightButton) {
        QModelIndex clickedItemIndex(indexAt(event->pos()));
        if (clickedItemIndex.isValid()) {
            setCurrentIndex(clickedItemIndex);
        }
        CSwordModuleInfo *i(getModule(clickedItemIndex));
        if (i == 0) {
            emit contextMenuActivated(mapToGlobal(event->pos()));
        } else {
            emit moduleContextMenuActivated(i, mapToGlobal(event->pos()));
        }
    } else {
        QTreeView::mousePressEvent(event);
    }
}

CSwordModuleInfo *BtBookshelfView::getModule(const QModelIndex &index) const {
    return (CSwordModuleInfo *) model()
            ->data(index, BtBookshelfModel::ModulePointerRole).value<void*>();
}

void BtBookshelfView::slotItemActivated(const QModelIndex &index) {
    CSwordModuleInfo *i(getModule(index));
    if (i != 0) {
        emit moduleActivated(i);
    }
}
