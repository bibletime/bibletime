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

#ifndef TREE_CHOOSER_MODEL_H
#define TREE_CHOOSER_MODEL_H

#include <QAbstractItemModel>

class TreeChooserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TreeEntryRoles {
        NameRole = Qt::UserRole + 1,
        ChildCountRole
    };

    TreeChooserModel(QObject *parent = 0);
    QHash<int, QByteArray>    roleNames() const;

};

#endif
