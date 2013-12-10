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
    QHash<int, QByteArray>    TreeChooserModel::roleNames() const;

};

#endif
