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

#include "chooserinterface.h"

#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/cswordbackend.h"

namespace btm {

struct BookEntry {
    enum BookRoles {
        NameRole = Qt::UserRole + 1,
        ChildCountRole
    };
};

ChooserInterface::ChooserInterface() :
    QObject(),
    m_treeKey(nullptr) {
    initializeRoleNameModel();
}

void ChooserInterface::initializeRoleNameModel() {
    QHash<int, QByteArray> roleNames;
    roleNames[BookEntry::NameRole] =  "name";
    roleNames[BookEntry::ChildCountRole] = "childcount";
    m_roleItemModel.setRoleNames(roleNames);
}

QStringList ChooserInterface::getBooks(const QString& moduleName) const {
    QStringList books;
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* bibleModule = qobject_cast<const CSwordBibleModuleInfo*>(module);
    if (bibleModule)
        books = *bibleModule->books();
    return books;
}

QStringList ChooserInterface::getChapters(const QString& moduleName, const QString& book) const {
    QStringList chapters;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* module = qobject_cast<const CSwordBibleModuleInfo*>(m);
    if (module) {
        CSwordKey * key = CSwordKey::createInstance(m);
        CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(key);
        if (verseKey) {
            int count = module->chapterCount(book);
            for (int i = 1; i <= count; i++) {
                chapters << QString::number(i);
            }
        }
    }
    return chapters;
}

QStringList ChooserInterface::getVerses(const QString& moduleName, const QString& book, const QString& strChapter) const {
    QStringList verses;
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    const CSwordBibleModuleInfo* module = qobject_cast<const CSwordBibleModuleInfo*>(m);
    if (module) {
        int chapter = strChapter.toInt();
        int count = module->verseCount(book,chapter);
        for (int i = 1; i <= count; i++) {
            verses << QString::number(i);
        }
    }
    return verses;
}

bool ChooserInterface::isBibleOrCommentary(const QString& moduleName) {
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!module)
        return false;
    CSwordModuleInfo::Category category = module->category();
    if (category == CSwordModuleInfo::Bibles ||
            category == CSwordModuleInfo::Cult ||
            category == CSwordModuleInfo::Commentaries)
        return true;
    return false;
}

bool ChooserInterface::isBook(const QString& moduleName) {
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!module)
        return false;
    return module->category() == CSwordModuleInfo::Books;
}

static QString constructPath(const QStringList& pathList) {
    QString path;
    for (int i=0; i< pathList.count(); ++i) {
        QString leaf = QString("/") + pathList.at(i);
        path.append(leaf);
    }
    return path;
}

QString ChooserInterface::getBackPath(const QString& /*reference*/) {
    QString reference = getTreeReference();
    QStringList pathList = reference.split('/', QString::SkipEmptyParts);
    pathList.removeLast();
    QString backPath;
    if (pathList.count() == 0)
        return backPath;
    backPath = constructPath(pathList);
    return backPath;
}

QString ChooserInterface::getTreeReference() const {
    return m_treeKey->key();
}

void ChooserInterface::setupTree(const QString& moduleName, const QString& reference) {
    CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    CSwordKey* key = CSwordKey::createInstance(module);
    m_treeKey = dynamic_cast<CSwordTreeKey*>(key);
    m_treeKey->setKey(reference);
    QStringList siblings;
    QList<int> hasChildrenList;
    parseKey(&siblings, &hasChildrenList, m_treeKey);
    populateRoleNameModel(siblings, hasChildrenList);
}

void ChooserInterface::next(QString value) {
    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();
    keyPathList.append(value);

    QString newPath = constructPath(keyPathList);
    m_treeKey->setKey(newPath);
    m_treeKey->firstChild();

    CSwordTreeKey tmpKey(*m_treeKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);

    m_treeKey->setKey(tmpKey);
}

void ChooserInterface::back() {
    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();

    QString newPath = constructPath(keyPathList);
    keyPathList.removeLast();
    m_treeKey->setKey(newPath);

    CSwordTreeKey tmpKey(*m_treeKey);
    QStringList siblings;
    QList<int> counts;
    parseKey(&siblings, &counts, &tmpKey);
    populateRoleNameModel(siblings, counts);

}

void ChooserInterface::select(QString value) {
    QStringList keyPathList = getKeyPath();
    keyPathList.removeLast();
    keyPathList.append(value);
    QString newPath = constructPath(keyPathList);
    m_treeKey->setKey(newPath);
}

QStringList ChooserInterface::getKeyPath() const {
    QString oldKey = m_treeKey->key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        m_treeKey->firstChild();
        oldKey = m_treeKey->key();
    }

    QStringList siblings; //split up key
    if (!oldKey.isEmpty()) {
        siblings = oldKey.split('/', QString::SkipEmptyParts);
    }
    return siblings;
}

void ChooserInterface::populateRoleNameModel(const QStringList& sibblings, const QList<int>& sibblingChildCounts) {
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

void ChooserInterface::parseKey(QStringList * siblings,
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

QVariant ChooserInterface::getBookModel() {
    return QVariant::fromValue(&m_roleItemModel);
}

}

