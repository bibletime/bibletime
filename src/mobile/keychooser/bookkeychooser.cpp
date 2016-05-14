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

#include "bookkeychooser.h"

#include <cmath>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QStandardItem>
#include <QString>
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "util/btassert.h"
#include "util/btconnect.h"


namespace btm {

struct BookEntry {
    enum BookRoles {
        NameRole = Qt::UserRole + 1,
        ChildCountRole
    };
};

BookKeyChooser::BookKeyChooser(QtQuick2ApplicationViewer* viewer,
                               BtWindowInterface* windowInterface)
    : m_viewer(viewer),
      m_windowInterface(windowInterface),
      m_key(nullptr),
      m_treeChooserObject(nullptr),
      m_state(CLOSED ) {
    findTreeChooserObject();
    initializeRoleNameModel();
}

void BookKeyChooser::copyKey()
{
    CSwordKey* key = m_windowInterface->getKey();
    CSwordTreeKey* bookKey = dynamic_cast<CSwordTreeKey*>(key);
    m_key = new CSwordTreeKey(*bookKey);
}

void BookKeyChooser::findTreeChooserObject() {
    QQuickItem * rootObject = m_viewer->rootObject();
    if (rootObject != nullptr)
        m_treeChooserObject = rootObject->findChild<QQuickItem*>("treeChooser");
}

void BookKeyChooser::initializeRoleNameModel() {
    QHash<int, QByteArray> roleNames;
    roleNames[BookEntry::NameRole] =  "name";
    roleNames[BookEntry::ChildCountRole] = "childcount";
    m_roleItemModel.setRoleNames(roleNames);
}

void BookKeyChooser::populateRoleNameModel(const QStringList& sibblings, const QList<int>& sibblingChildCounts) {
    m_roleItemModel.clear();
    BT_ASSERT(sibblings.count() == sibblingChildCounts.count());
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
    CSwordTreeKey* tmpBookKey = dynamic_cast<CSwordTreeKey*>(m_key);
    tmpBookKey->setKey(newPath);

    CSwordKey* key = m_windowInterface->getKey();
    CSwordTreeKey* bookKey = dynamic_cast<CSwordTreeKey*>(key);
    bookKey->setKey(*tmpBookKey);

    emit referenceChanged();
}

void BookKeyChooser::open() {
    copyKey();

    CSwordTreeKey tmpKey(*m_key);
    m_backPath = getBackPath(tmpKey);
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
    m_key->setKey(newPath);
    m_key->firstChild();

    CSwordTreeKey tmpKey(*m_key);
    m_backPath = getBackPath(tmpKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);
    openChooser(true);

    m_key->setKey(tmpKey);
}

void BookKeyChooser::back() {
    openChooser(false);

    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();

    QString newPath = constructPath(keyPathList);
    keyPathList.removeLast();
    m_key->setKey(newPath);

    CSwordTreeKey tmpKey(*m_key);
    m_backPath = getBackPath(tmpKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);

    openChooser(true);
}

void BookKeyChooser::stringCanceled() {
}

void BookKeyChooser::setProperties() {
    m_treeChooserObject->setProperty("path",m_backPath);
    m_treeChooserObject->setProperty("model", QVariant::fromValue(&m_roleItemModel));
}

void BookKeyChooser::openChooser(bool open) {
    BT_ASSERT(m_treeChooserObject);
    if (m_treeChooserObject == nullptr)
        return;

    m_treeChooserObject->disconnect();
    BT_CONNECT(m_treeChooserObject, SIGNAL(select(QString)),
               this,                SLOT(select(QString)));
    BT_CONNECT(m_treeChooserObject, SIGNAL(next(QString)),
               this,                SLOT(next(QString)));
    BT_CONNECT(m_treeChooserObject, SIGNAL(back()),
               this,                SLOT(back()));

    setProperties();

    m_treeChooserObject->setProperty("visible",open);
}

QStringList BookKeyChooser::getKeyPath() const {
    QString oldKey = m_key->key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        m_key->firstChild();
        oldKey = m_key->key();
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

void BookKeyChooser::parseKey(QStringList * siblings,
            QList<int>* hasChildrenList, CSwordTreeKey* key) {
    QString oldKey = key->key();
    QStringList pathDepthList = oldKey.split("/", QString::SkipEmptyParts);
    int depth = 0;
    key->root();

    while ( key->firstChild() && (depth < pathDepthList.count()) ) {
        QString localName = key->getLocalNameUnicode();
        QString savedKey = key->key();
        bool found = false;
        QString path = pathDepthList[depth];
        siblings->clear();
        hasChildrenList->clear();
        QString matchingKey;
        findEntry(path, &found, key, &matchingKey, siblings, hasChildrenList);
        if (found) {
            key->setKey(matchingKey);
        }
        depth++;
    }
}

void BookKeyChooser::createModel()
{
    m_model.clear();

    QStandardItem *parentItem = m_model.invisibleRootItem();
    for (int i = 0; i < 4; ++i) {
        QStandardItem *item = new QStandardItem("item1");
        parentItem->appendRow(item);
    }
}

} // end namespace
