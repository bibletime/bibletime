
#include "treechoosermodel.h"


TreeChooserModel::TreeChooserModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray>	TreeChooserModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ChildCountRole] = "childCount";
    return roles;
}

void TreeChooserModel::addEntry(const QString& name, int childCount) {
    int count = rowCount();
    beginInsertRows(count, count);
    insertRow(count);
    endInsertRows();
}
