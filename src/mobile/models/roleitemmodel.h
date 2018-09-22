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

    QHash<int, QByteArray> roleNames() const override;
    void setRoleNames(const QHash<int, QByteArray> &roleNames);

private:
    QHash<int, QByteArray> m_roleNames;

};

#endif // ROLEITEMMODEL_H
