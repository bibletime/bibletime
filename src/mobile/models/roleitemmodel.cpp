#include "roleitemmodel.h"

/* Example usage:

Enumerate the role ID's somewhere
---------------------------------

struct RedditEntry {

    enum RedditRoles {
        UrlRole = Qt::UserRole + 1,
        DescRole,
        ...
    };
    ...
}

Instantiate the class
---------------------

    QHash<int, QByteArray> roleNames;
    roleNames[RedditEntry::UrlRole] =  "url";
    roleNames[RedditEntry::ScoreRole] = "score";
    m_linksmodel = new RoleItemModel(roleNames);

Populate with data:
-------------------

    QStandardItem* it = new QStandardItem();
    it->setData(e.desc, RedditEntry::DescRole);
    it->setData(e.score, RedditEntry::ScoreRole);

    m_linksmodel->appendRow(it);

Expose to QML:
-------------

QDeclarativeContext *ctx = ...

ctx->setContextProperty("mdlLinks", m_linksmodel);

*/

//RoleItemModel::RoleItemModel(const QHash<int, QByteArray> &roleNames)
//    : m_roleNames(roleNames) {
//}

QHash<int, QByteArray> RoleItemModel::roleNames() const {
    return m_roleNames;
}

void RoleItemModel::setRoleNames(const QHash<int, QByteArray> &roleNames) {
    m_roleNames = roleNames;
}
