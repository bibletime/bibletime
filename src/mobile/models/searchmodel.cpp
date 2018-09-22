/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "searchmodel.h"

#include "backend/models/btmoduletextmodel.h"
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QVariant>

namespace btm {

int SearchModel::indexMatchingKeyName(const QString& keyName, const QVariant& vModel) {
    BtModuleTextModel* model = vModel.value<BtModuleTextModel*>();
    if (model == nullptr)
        return 0;

    int first = 0;
    int last = model->rowCount();

    int trials = 24;
    while (trials > 0 && (last - first) > 1) {
        int middle = (first + last) /2;
        QString name = model->indexToKeyName(middle);
        int c = name.compare(keyName,Qt::CaseInsensitive);
        if (c < 0)
            first = middle;
        else if (c > 0)
            last = middle;
        else
            return middle;
        --trials;
    }
    return first;
}

}
