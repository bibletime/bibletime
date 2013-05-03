#include "bookkeychooser.h"

#include <cmath>
#include "mobile/ui/btbookinterface.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include <QCoreApplication>
#include <QDebug>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QStandardItem>
#include <QString>

namespace btm {

struct BookEntry {
    enum BookRoles {
        NameRole = Qt::UserRole + 1,
        ChildCountRole
    };
};

BookKeyChooser::BookKeyChooser(QtQuick2ApplicationViewer* viewer,
                               BtBookInterface* bookInterface)
    : viewer_(viewer),
      bookInterface_(bookInterface),
      key_(0),
      treeChooserObject_(0),
      state_(CLOSED ) {
    findTreeChooserObject();
    initializeRoleNameModel();
}

void BookKeyChooser::copyKey()
{
    CSwordTreeKey* bookKey = bookInterface_->getKey();
    key_ = new CSwordTreeKey(*bookKey);
}

void BookKeyChooser::findTreeChooserObject() {
    QQuickItem * rootObject = viewer_->rootObject();
    if (rootObject != 0)
        treeChooserObject_ = rootObject->findChild<QQuickItem*>("treeChooser");
}

void BookKeyChooser::initializeRoleNameModel() {
    QHash<int, QByteArray> roleNames;
    roleNames[BookEntry::NameRole] =  "name";
    roleNames[BookEntry::ChildCountRole] = "childcount";
    m_roleItemModel.setRoleNames(roleNames);
}

void BookKeyChooser::populateRoleNameModel(const QStringList& sibblings, const QList<int>& sibblingChildCounts) {
    m_roleItemModel.clear();
    Q_ASSERT(sibblings.count() == sibblingChildCounts.count());
    for (int i=0; i< sibblings.count(); ++i) {
        QString sibbling = sibblings.at(i);
        int childCount = sibblingChildCounts.at(i);
        QStandardItem* item = new QStandardItem();
        item->setData(sibbling, BookEntry::NameRole);
        item->setData(childCount, BookEntry::ChildCountRole);
        m_roleItemModel.appendRow(item);
    }
}

static QString constructPath(const QStringList& pathList) {
    QString path;
    for (int i=0; i< pathList.count(); ++i) {
        QString leaf = QString("/") + pathList.at(i);
        path.append(leaf);
    }
    return path;
}

static QString getBackPath(const CSwordTreeKey& key) {
    QString path = key.key();
    QStringList pathList = path.split('/', QString::SkipEmptyParts);
    pathList.removeLast();

    QString backPath;
    if (pathList.count() == 0)
        return backPath;

    backPath = constructPath(pathList);
    return backPath;
}

void BookKeyChooser::select(QString value) {
    openChooser(false);

    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();
    keyPathList.append(value);
    QString newPath = constructPath(keyPathList);
    key_->setKey(newPath);

    CSwordTreeKey* bookKey = bookInterface_->getKey();
    bookKey->setKey(*key_);
    bookInterface_->setDisplayed("");
}

void BookKeyChooser::open() {
    copyKey();

    CSwordTreeKey tmpKey(*key_);
    backPath_ = getBackPath(tmpKey);
    QStringList siblings;
    QList<int> hasChildrenList;
    parseKey(&siblings, &hasChildrenList, &tmpKey);
    populateRoleNameModel(siblings, hasChildrenList);
    openChooser(true);
}

void BookKeyChooser::next(QString value) {
    openChooser(false);

    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();
    keyPathList.append(value);

    QString newPath = constructPath(keyPathList);
    key_->setKey(newPath);
    key_->firstChild();

    CSwordTreeKey tmpKey(*key_);
    backPath_ = getBackPath(tmpKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);
    openChooser(true);

    key_->setKey(tmpKey);
}

void BookKeyChooser::back() {
    openChooser(false);

    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();

    QString newPath = constructPath(keyPathList);
    keyPathList.removeLast();
    key_->setKey(newPath);

    CSwordTreeKey tmpKey(*key_);
    backPath_ = getBackPath(tmpKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);

    openChooser(true);
}

void BookKeyChooser::stringCanceled() {
}

void BookKeyChooser::setProperties() {
    QQmlContext* ctx = viewer_->rootContext();
    treeChooserObject_->setProperty("path",backPath_);
    treeChooserObject_->setProperty("model", QVariant::fromValue(&m_roleItemModel));
}

void BookKeyChooser::openChooser(bool open) {
    Q_ASSERT(treeChooserObject_ != 0);
    if (treeChooserObject_ == 0)
        return;

    treeChooserObject_->disconnect();
    bool ok = connect(treeChooserObject_, SIGNAL(select(QString)),
                      this, SLOT(select(QString)));
    Q_ASSERT(ok);

    ok = connect(treeChooserObject_, SIGNAL(next(QString)),
                 this, SLOT(next(QString)));
    Q_ASSERT(ok);

    ok = connect(treeChooserObject_, SIGNAL(back()),
                 this, SLOT(back()));
    Q_ASSERT(ok);

    setProperties();

    QQmlContext* ctx = viewer_->rootContext();
    treeChooserObject_->setProperty("visible",open);
}

QStringList BookKeyChooser::getKeyPath() const {
    QString oldKey = key_->key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        key_->firstChild();
        oldKey = key_->key();
    }

    QStringList siblings; //split up key
    if (!oldKey.isEmpty()) {
        siblings = oldKey.split('/', QString::SkipEmptyParts);
    }
    return siblings;
}

static int findEntry(const QString& sibling, bool* found,
                     CSwordTreeKey* key, QString* matchingKey, QStringList* siblings, QList<int>* hasChildrenList) {
    int index = 0;
    int foundIndex = 0;
    do { //look for matching sibling
        QString keyName = key->getLocalNameUnicode();
        if (keyName == sibling) {
            *found = true;
            foundIndex = index;
            *matchingKey = key->key();
        }
        siblings->append(keyName);
        bool hasChildren = key->hasChildren();
        hasChildrenList->append(hasChildren ? 1 : 0);
        ++index;
    } while (key->nextSibling());
    return foundIndex;
}

static QStringList getSiblings(CSwordTreeKey* key) {
    QStringList siblings;
    if ( ! key->hasChildren())
            return siblings;
    key->firstChild();

    do {
        siblings << key->getLocalNameUnicode();
    }
    while (key->nextSibling());
    return siblings;
}

void BookKeyChooser::parseKey(QStringList * siblings,
            QList<int>* hasChildrenList, CSwordTreeKey* key) {
    QString oldKey = key->key();
    QStringList pathDepthList = oldKey.split("/", QString::SkipEmptyParts);
    int depth = 0;
    key->root();

    while ( key->firstChild() && (depth < pathDepthList.count()) ) {
        QString localName = key->getLocalNameUnicode();
        QString savedKey = key->key();
        int index = (depth == 0) ? -1 : 0;
        bool found = false;
        QString path = pathDepthList[depth];
        siblings->clear();
        hasChildrenList->clear();
        QString matchingKey;
        index = findEntry(path, &found, key, &matchingKey, siblings, hasChildrenList);
        if (found) {
            key->setKey(matchingKey);
        }
        depth++;
    }
}

void BookKeyChooser::createModel()
{
    model_.clear();

    QStandardItem *parentItem = model_.invisibleRootItem();
    for (int i = 0; i < 4; ++i) {
        QStandardItem *item = new QStandardItem("item1");
        parentItem->appendRow(item);
    }
}

} // end namespace
