/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BOOK_KEY_CHOOSER_H
#define BOOK_KEY_CHOOSER_H

#include "backend/keys/cswordtreekey.h"
#include <QObject>
#include <QList>
#include <QStringList>
#include <QStandardItemModel>
#include "mobile/models/roleitemmodel.h"

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;

namespace btm {

class BtWindowInterface;

class BookKeyChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        BOOK,
        CHAPTER,
        VERSE
    };

public:
    BookKeyChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* windowInterface);
    void open();

signals:
    void referenceChanged();

private slots:
    void select(QString value);
    void next(QString value);
    void back();
    void stringCanceled();

private:
    void copyKey();
    void findTreeChooserObject();
    QStringList getKeyPath() const;
    void showGridChooser(const QStringList& list);
    void setProperties(const QStringList& list);
    void parseKey(QStringList * sibblings, QList<int>* hasChildrenList, CSwordTreeKey* key);
    void createModel();
    void setProperties();
    void initializeRoleNameModel();
    void populateRoleNameModel(const QStringList& sibblings, const QList<int>& sibblingChildCounts);
    void openChooser(bool open);


    QtQuick2ApplicationViewer* m_viewer;
    BtWindowInterface* m_windowInterface;
    CSwordTreeKey* m_key;
    QQuickItem* m_treeChooserObject;
    int m_state;
    QString m_backPath;
    QStringList m_sibblings;
    QStandardItemModel m_model;
    RoleItemModel m_roleItemModel;
};

} // end namespace

#endif
