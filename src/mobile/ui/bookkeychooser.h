#ifndef VERSE_CHOOSER_H
#define VERSE_CHOOSER_H

#include "backend/keys/cswordtreekey.h"
#include <QObject>
#include <QList>
#include <QStringList>
#include <QStandardItemModel>
#include "roleitemmodel.h"

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;

namespace btm {

class BtBookInterface;

class BookKeyChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        BOOK,
        CHAPTER,
        VERSE
    };

public:
    BookKeyChooser(QtQuick2ApplicationViewer* viewer, BtBookInterface* bookInterface);
    void open();

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


    QtQuick2ApplicationViewer* viewer_;
    BtBookInterface* bookInterface_;
    CSwordTreeKey* key_;
    QQuickItem* treeChooserObject_;
    int state_;
    QString backPath_;
    QStringList sibblings_;
    QStandardItemModel model_;
    RoleItemModel m_roleItemModel;
};

} // end namespace

#endif
