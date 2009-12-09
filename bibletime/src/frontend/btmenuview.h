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

#ifndef BTMENUVIEW_H
#define BTMENUVIEW_H

#include <QMenu>

#include <QMap>
#include <QPersistentModelIndex>

class QAbstractItemModel;
class QActionGroup;

/**
  \warning This menu does not properly handle changes in the source model while
           being shown, so beware to check whether the index emitted by
           triggered() is valid.
*/
class BtMenuView: public QMenu {
    Q_OBJECT
    public:
        BtMenuView(QWidget *parent = 0);
        ~BtMenuView();

        void setModel(QAbstractItemModel *model);
        inline QAbstractItemModel *model() const { return m_model; }

    signals:
        void triggered(QModelIndex index);

    protected:
        void buildMenu(QMenu *parentMenu, const QModelIndex &parent);

    protected slots:
        void slotActionTriggered(QAction *action);
        void slotAboutToShow();
        void slotAboutToHide();

    protected:
        QAbstractItemModel *m_model;
        QActionGroup *m_actions;
        QMap<QAction *, QPersistentModelIndex> m_indexMap;
};

#endif // BTMENUVIEW_H
