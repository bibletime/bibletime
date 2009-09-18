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

#ifndef BTBOOKSHELFVIEW_H
#define BTBOOKSHELFVIEW_H

#include <QTreeView>

class CSwordModuleInfo;

class BtBookshelfView: public QTreeView {
    Q_OBJECT
    public:
        BtBookshelfView(QWidget *parent = 0);
        virtual ~BtBookshelfView();

    signals:
        void rightClicked();
        void moduleRightClicked(CSwordModuleInfo *item);
        void moduleActivated(CSwordModuleInfo *item);

    protected:
        void mousePressEvent(QMouseEvent *event);
        CSwordModuleInfo *getModule(const QModelIndex &index) const;

    protected slots:
        void slotItemActivated(const QModelIndex &index);
};

#endif // BTBOOKSHELFVIEW_H
