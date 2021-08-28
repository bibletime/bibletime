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

#pragma once

#include <QTreeView>

#include <QObject>
#include <QString>


class CSwordModuleInfo;
class QKeyEvent;
class QModelIndex;
class QMouseEvent;
class QPoint;
class QWidget;

class BtBookshelfView: public QTreeView {
        Q_OBJECT
    public:
        BtBookshelfView(QWidget *parent = nullptr);

        CSwordModuleInfo *getModule(const QModelIndex &index) const;

    Q_SIGNALS:
        void contextMenuActivated(QPoint pos);
        void moduleContextMenuActivated(CSwordModuleInfo *item,
                                        QPoint pos);
        void moduleActivated(CSwordModuleInfo *item);
        void moduleHovered(CSwordModuleInfo *item);

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

};
