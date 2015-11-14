/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTREMOVEPAGETREEMODEL_H
#define BTREMOVEPAGETREEMODEL_H

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

class BtRemovePageTreeModel: public BtBookshelfTreeModel {
        Q_OBJECT

    public:
        BtRemovePageTreeModel(const QString &configKey, QObject *parent = nullptr);

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
};

#endif // BTREMOVEPAGETREEMODEL_H
