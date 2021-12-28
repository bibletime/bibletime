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

#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <Qt>


class BtInstallPageModel: public BtBookshelfTreeModel {

    Q_OBJECT

public: // methods:

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
