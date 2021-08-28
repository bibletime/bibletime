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

#include "btbookshelfview.h"

#include <QAbstractItemModel>
#include <QKeyEvent>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QVariant>
#include <QWidget>
#include <Qt>
#include "../backend/bookshelfmodel/btbookshelfmodel.h"
#include "../util/btconnect.h"


BtBookshelfView::BtBookshelfView(QWidget *parent)
        : QTreeView(parent)
{
    setHeaderHidden(true);

    /*
      Uncommenting the following statement will hide the [+]expand/[-]collapse
      controls in front of first-level items, which might not be desirable since
      hiding them also means that one has to do a total of 2 clicks (double-
      click)to expand or collapse top-level items:
    */
    // setRootIsDecorated(false);

    BT_CONNECT(this, &BtBookshelfView::activated,
               [this](QModelIndex const & index) {
                   if (auto * const module = getModule(index))
                       Q_EMIT moduleActivated(module);
               });
    BT_CONNECT(this, &BtBookshelfView::entered,
               [this](QModelIndex const & index)
               { Q_EMIT moduleHovered(getModule(index)); });
}

CSwordModuleInfo * BtBookshelfView::getModule(const QModelIndex & index) const {
    void * const module = model()->data(index,
                                        BtBookshelfModel::ModulePointerRole).value<void *>();
    return static_cast<CSwordModuleInfo *>(module);
}

void BtBookshelfView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Menu:
            scrollTo(currentIndex());
            {
                CSwordModuleInfo *i(getModule(currentIndex()));
                QRect itemRect(visualRect(currentIndex()));
                QPoint p(viewport()->mapToGlobal(itemRect.bottomLeft()));
                if (i == nullptr) {
                    Q_EMIT contextMenuActivated(p);
                }
                else {
                    Q_EMIT moduleContextMenuActivated(i, p);
                }
            }
            event->accept();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            QModelIndex i(currentIndex());
            CSwordModuleInfo *m(getModule(i));
            if (m != nullptr) {
                Q_EMIT moduleActivated(m);
            }
            else {
                setExpanded(i, !isExpanded(i));
            }
        }
        event->accept();
        break;
        default:
            QTreeView::keyPressEvent(event);
            break;
    }
}

void BtBookshelfView::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::RightButton) {
        QModelIndex clickedItemIndex(indexAt(event->pos()));
        if (clickedItemIndex.isValid()) {
            setCurrentIndex(clickedItemIndex);
        }
        CSwordModuleInfo *i(getModule(clickedItemIndex));
        if (i == nullptr) {
            Q_EMIT contextMenuActivated(mapToGlobal(event->pos()));
        }
        else {
            Q_EMIT moduleContextMenuActivated(i, mapToGlobal(event->pos()));
        }
        event->accept();
    }
    else {
        QTreeView::mousePressEvent(event);
    }
}
