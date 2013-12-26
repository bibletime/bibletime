
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

