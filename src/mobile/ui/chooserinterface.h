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

#ifndef CHOOSER_INTERFACE_H
#define CHOOSER_INTERFACE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include "mobile/models/roleitemmodel.h"

class CSwordTreeKey;

namespace btm {

class ChooserInterface : public QObject {
    Q_OBJECT

public:
    ChooserInterface();

    // Bible and Commentary
    Q_INVOKABLE QStringList getBooks(const QString& module) const;
    Q_INVOKABLE QStringList getChapters(const QString& module, const QString& book) const;
    Q_INVOKABLE QStringList getVerses(const QString& module, const QString& book, const QString& chapter) const;
    Q_INVOKABLE bool isBibleOrCommentary(const QString& module);

    // Book
    Q_INVOKABLE QString getBackPath(const QString& reference);
    Q_INVOKABLE QString getTreeReference() const;
    Q_INVOKABLE bool isBook(const QString& moduleName);
    Q_INVOKABLE QVariant getBookModel();
    Q_INVOKABLE void setupTree(const QString& moduleName, const QString& reference);
    Q_INVOKABLE void next(QString value);
    Q_INVOKABLE void back();
    Q_INVOKABLE void select(QString value);

private:
    QStringList getKeyPath() const;
    void initializeRoleNameModel();
    void parseKey(QStringList * siblings,
                  QList<int>* hasChildrenList, CSwordTreeKey* key);
    void populateRoleNameModel(const QStringList& sibblings, const QList<int>& sibblingChildCounts);

    RoleItemModel m_roleItemModel;
    CSwordTreeKey * m_treeKey;


};

}

#endif
