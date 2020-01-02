/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
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
        BtBookshelfView(QWidget *parent = nullptr);

        CSwordModuleInfo *getModule(const QModelIndex &index) const;

    signals:
        void contextMenuActivated(QPoint pos);
        void moduleContextMenuActivated(CSwordModuleInfo *item,
                                        QPoint pos);
        void moduleActivated(CSwordModuleInfo *item);
        void moduleHovered(CSwordModuleInfo *item);

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

    protected slots:
        void slotItemActivated(const QModelIndex &index);
        void slotItemHovered(const QModelIndex &index);
};

#endif // BTBOOKSHELFVIEW_H
