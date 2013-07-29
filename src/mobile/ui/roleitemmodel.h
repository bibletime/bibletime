#ifndef ROLEITEMMODEL_H
#define ROLEITEMMODEL_H

#include <QStandardItemModel>

/* class to allow easily exposing
   C++ data as a model for QML View.
*/

class RoleItemModel : public QStandardItemModel {
public:
    //  roleNames is a map describing what role id (e.g. Qt::UserRole+1)
    //  is associated with what name on QML side (e.g. 'bookTitle')
    //  RoleItemModel(const QHash<int, QByteArray> &roleNames);

    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray> &roleNames);

private:
    QHash<int, QByteArray> m_roleNames;

};

#endif // ROLEITEMMODEL_H
