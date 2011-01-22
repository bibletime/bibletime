/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTINSTALLMODULECHOOSERDIALOGMODEL_H
#define BTINSTALLMODULECHOOSERDIALOGMODEL_H

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QMap>


class BtInstallModuleChooserDialogModel: public BtBookshelfTreeModel {
    Q_OBJECT
    public:
        BtInstallModuleChooserDialogModel(const Grouping &grouping, QObject *parent = 0);
        ~BtInstallModuleChooserDialogModel();

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;

    private slots:
        void parentDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    private:
        bool isMulti(CSwordModuleInfo *module) const;
        bool isMulti(const QModelIndex &index) const;

    private:
        QMap<QString, int> m_nameCounts;
        bool m_dataChangedFired;
};

#endif // BTINSTALLMODULECHOOSERDIALOGMODEL_H
