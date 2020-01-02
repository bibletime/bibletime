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

#ifndef BTINSTALLPAGEMODEL_H
#define BTINSTALLPAGEMODEL_H

#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"


class BtInstallPageModel: public BtBookshelfTreeModel {

    Q_OBJECT

public: /* Methods: */

    BtInstallPageModel(Grouping const & grouping,
                       QObject * const parent = nullptr);

    QVariant data(QModelIndex const & index,
                  int role = Qt::DisplayRole) const final override;
    int columnCount(QModelIndex const & parent = QModelIndex())
            const final override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const final override;

}; /* class BtInstallPageModel */

#endif /* BTINSTALLPAGEMODEL_H */
