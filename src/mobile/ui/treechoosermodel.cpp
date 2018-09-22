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

#include "treechoosermodel.h"


TreeChooserModel::TreeChooserModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray>    TreeChooserModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ChildCountRole] = "childCount";
    return roles;
}

