/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTINSTALLPAGEMODEL_H
#define BTINSTALLPAGEMODEL_H

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QMap>


class BtInstallPageModel: public BtBookshelfTreeModel {
    Q_OBJECT
    public:
        BtInstallPageModel(const Grouping &grouping, QObject *parent = 0);

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
};

#endif // BTINSTALLPAGEMODEL_H
