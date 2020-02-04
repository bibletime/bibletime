/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

/**
  Total change list that should be applied after refactoring complete:
    non latin bookmark titles shown with unrecognized symbols
    feature request: hold Shift and Ctrl upon dragging item
    move loader to private class
    add ability to create bookmarks data with setData/insertRows
    unrecognized characters increaases in size file each save/load
    root folder for bookmarks
*/

#include "btbookmarksmodel.h"

#include <algorithm>
#include <memory>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QIODevice>
#include <QList>
#include <QString>
#include <QTextCodec>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"
#include "../util/directory.h"
#include "../util/tool.h"
#include "btglobal.h"
#include "config/btconfig.h"
#include "drivers/cswordmoduleinfo.h"
#include "keys/cswordversekey.h"
#include "managers/cswordbackend.h"


#define CURRENT_SYNTAX_VERSION 1


namespace {
inline QString toHeader(QString const & key, QString const & moduleName)
{ return QString::fromLatin1("%1 (%2)").arg(key).arg(moduleName); }
}

class BtBookmarksModelPrivate {

public: /* Tyepes */

    class BookmarkItemBase {

    public: /* Methods: */

        inline BookmarkItemBase(BookmarkItemBase * parent = nullptr)
            : m_parent(parent) {
            if(m_parent) {
                BT_ASSERT(!m_parent->m_children.contains(this));
                m_parent->m_children.append(this);
            }
        }
        BookmarkItemBase(const BookmarkItemBase & other)
            : m_flags(other.m_flags)
            , m_icon(other.m_icon)
            , m_parent(other.m_parent)
            , m_text(other.m_text)
            , m_tooltip(other.m_tooltip) {;}
        virtual ~BookmarkItemBase() {
            qDeleteAll(m_children);
        }

        /** Children routines. */
        inline void addChild(BookmarkItemBase * child) {
            child->setParent(this);
            BT_ASSERT(!m_children.contains(child));
            m_children.append(child);
        }

        inline int childCount() const { return m_children.size(); }

        inline BookmarkItemBase * child(int index) const {
            return m_children[index];
        }

        inline QList<BookmarkItemBase *> & children() {
            return m_children;
        }

        inline void insertChild(int index, BookmarkItemBase * child) {
            child->setParent(this);
            BT_ASSERT(!m_children.contains(child));
            m_children.insert(index, child);
        }

        inline void insertChildren(int index, QList<BookmarkItemBase *> children) {
            Q_FOREACH(BookmarkItemBase * const c, children)
                insertChild(index++, c);
        }

        inline void removeChild(int index) {
            delete m_children[index];
            m_children.removeAt(index);
        }


        inline void setText(const QString & text) { m_text = text; }

        inline const QString & text() const { return m_text; }

        inline void setToolTip(const QString & tooltip) { m_tooltip = tooltip; }

        virtual QString toolTip() const { return m_tooltip; }

        inline void setFlags(Qt::ItemFlags flags) { m_flags = flags; }

        inline Qt::ItemFlags flags() const { return m_flags; }

        inline void setIcon(const QIcon & icon) { m_icon = icon; }

        inline QIcon icon() const { return m_icon; }

        inline void setParent(BookmarkItemBase * parent) {
            m_parent = parent;
        }

        inline BookmarkItemBase * parent() const { return m_parent; }

        /**
          \returns index of this item in parent's child array.
         */
        inline int index() const {
            BT_ASSERT(parent());
            for(int i = 0; i < parent()->childCount(); ++i)
                if(parent()->child(i) == this)
                    return i;
            return -1;
        }

    private:

        QList<BookmarkItemBase *> m_children;
        Qt::ItemFlags m_flags;
        QIcon m_icon;
        BookmarkItemBase * m_parent;
        QString m_text;
        QString m_tooltip;

    };

    class BookmarkItem : public BookmarkItemBase {
    public:
        friend class BookmarkLoader;

        BookmarkItem(BookmarkItemBase * parent);

        /** Creates a bookmark with module, key and description. */
        BookmarkItem(const CSwordModuleInfo & module, const QString & key,
                       const QString & description, const QString & title);

        /** Creates a copy. */
        BookmarkItem(const BookmarkItem & other);

        /** Returns the used module, 0 if there is no such module. */
        CSwordModuleInfo * module() const;

        /** Returns the used key. */
        QString key() const;

        inline void setKey(const QString & key) { m_key = key; }

        /** Returns the used description. */
        inline const QString &description() const { return m_description; }

        inline void setDescription(const QString & description) { m_description = description; }

        /** Returns a tooltip for this bookmark. */
        QString toolTip() const override;

        /** Returns the english key.*/
        inline const QString & englishKey() const { return m_key; }

        inline void setModule(const QString & moduleName) { m_moduleName = moduleName; }

        inline const QString & moduleName() const { return m_moduleName; }

    private:
        QString m_key;
        QString m_description;
        QString m_moduleName;

    };

    class BookmarkFolder : public BookmarkItemBase {
    public:

        BookmarkFolder(const QString & name, BookmarkItemBase * parent = nullptr);

        /** Returns a list of direct childs of this item. */
        QList<BookmarkItemBase *> getChildList() const;

        /** Returns true if the given item is this or a direct or indirect subitem of this. */
        bool hasDescendant(BookmarkItemBase const * item) const;

        /** Creates a deep copy of this item. */
        BookmarkFolder * deepCopy() const;

    };


public: /* Methods */

    BtBookmarksModelPrivate(BtBookmarksModel * parent)
        : m_rootItem(new BookmarkFolder("Root"))
        , q_ptr(parent)
    {
        m_saveTimer.setInterval(0.5 * 60 * 1000);
        m_saveTimer.setSingleShot(true);
    }
    ~BtBookmarksModelPrivate() { delete m_rootItem; }

    inline static QString defaultBookmarksFile() {
        return util::directory::getUserBaseDir().absolutePath() + "/bookmarks.xml";
    }

    BookmarkItemBase * item(const QModelIndex & index) const {
        if(index.isValid()) {
#ifdef QT_DEBUG
            {
                // check for item in tree
                QList<BookmarkItemBase *> items;
                items << m_rootItem;
                for(int c = 0; ; ++c) {
                    if(items[c] == index.internalPointer())
                        break;
                    if(items[c]->childCount())
                        items.append(items[c]->children());
                    BT_ASSERT(c < items.size());
                }
            }
#endif
            return reinterpret_cast<BookmarkItemBase *>(index.internalPointer());
        }
        else
            return m_rootItem;
    }

    template <typename T>
    inline T * itemAs(QModelIndex const & index) const
    { return dynamic_cast<T *>(item(index)); }

    /// \test
    void printItems() const {
        QList<BookmarkItemBase *> items;
        QList<int> spaces;
        items << m_rootItem;
        spaces << 0;
        for(int c = 0; c < items.size(); ++c) {
            // qDebug() << QString().fill('\t', spaces[c]) << items[c]->text().left(24) << items[c]
            //       << items[c]->parent() << items[c]->childCount();
            if(items[c]->childCount())
                for(int i = 0; i < items[c]->childCount(); ++i) {
                    items.insert(c + i + 1, items[c]->children()[i]);
                    spaces.insert(c + i + 1, spaces[c] + 1);
                }
        }
    }

    void needSave(){
        if(m_defaultModel == q_ptr){
            if(!m_saveTimer.isActive())
                m_saveTimer.start();
        }
    }
public: /* Loader */

    /** Loads a list of items (with subitem trees) from a named file
    * or from the default bookmarks file. */
    QList<BookmarkItemBase *> loadTree(QString fileName = QString()) {
        QList<BookmarkItemBase*> itemList;

        QDomDocument doc;
        QString bookmarksFile = loadXmlFromFile(fileName);
        if (bookmarksFile.isNull())
            return QList<BookmarkItemBase*>();

        doc.setContent(bookmarksFile);
        QDomElement document = doc.documentElement();
        if ( document.tagName() != "SwordBookmarks" ) {
            qWarning("Not a BibleTime Bookmark XML file");
            return QList<BookmarkItemBase*>();
        }

        QDomElement child = document.firstChild().toElement();

        while ( !child.isNull() && child.parentNode() == document) {
            BookmarkItemBase* i = handleXmlElement(child, nullptr);
            itemList.append(i);
            if (!child.nextSibling().isNull()) {
                child = child.nextSibling().toElement();
            }
            else {
                child = QDomElement(); //null
            }

        }

        return itemList;
    }

    /** Create a new item from a document element. */
    BookmarkItemBase * handleXmlElement(QDomElement & element, BookmarkItemBase * parent) {
        BookmarkItemBase* newItem = nullptr;
        if (element.tagName() == "Folder") {
            BookmarkFolder* newFolder = new BookmarkFolder(QString(), parent);
            if (element.hasAttribute("caption")) {
                newFolder->setText(element.attribute("caption"));
            }
            QDomNodeList childList = element.childNodes();
            for (int i = 0; i < childList.length(); i++) {
                QDomElement newElement = childList.at(i).toElement();
                handleXmlElement(newElement, newFolder); // passing parent in constructor will add items to tree
            }
            newItem = newFolder;
        }
        else if (element.tagName() == "Bookmark") {
            BookmarkItem* newBookmarkItem = new BookmarkItem(parent);
            if (element.hasAttribute("modulename")) {
                //we use the name in all cases, even if the module isn't installed anymore
                newBookmarkItem->setModule(element.attribute("modulename"));
            }
            if (element.hasAttribute("key")) {
                newBookmarkItem->setKey(element.attribute("key"));
            }
            if (element.hasAttribute("description")) {
                newBookmarkItem->setDescription(element.attribute("description"));
            }
            if (element.hasAttribute("title")) {
                newBookmarkItem->setText(element.attribute("title"));
            }
            newItem = newBookmarkItem;
        }
        return newItem;
    }

    /** Loads a bookmark XML document from a named file or from the default bookmarks file. */
    QString loadXmlFromFile(QString fileName = QString()) {

        if (fileName.isEmpty())
            fileName = defaultBookmarksFile();

        QFile file(fileName);
        if (!file.exists())
            return QString();

        QString xml;
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream t;
            t.setAutoDetectUnicode(false);
            t.setCodec(QTextCodec::codecForName("UTF-8"));
            t.setDevice(&file);
            xml = t.readAll();
            file.close();
        }
        return xml;
    }

    /** Takes one item and saves the tree which is under it to a named file
    * or to the default bookmarks file, asking the user about overwriting if necessary. */
    QString serializeTreeFromRootItem(BookmarkItemBase * rootItem) {
        BT_ASSERT(rootItem);

        QDomDocument doc("DOC");
        doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

        QDomElement content = doc.createElement("SwordBookmarks");
        content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
        doc.appendChild(content);

        //append the XML nodes of all child items

        for (int i = 0; i < rootItem->childCount(); i++) {
            saveItem(rootItem->child(i), content);
        }
        return doc.toString();
    }

    /** Writes one item to a document element. */
    void saveItem(BookmarkItemBase * item, QDomElement & parentElement) {
        BookmarkFolder* folderItem = nullptr;
        BookmarkItem* bookmarkItem = nullptr;

        if ((folderItem = dynamic_cast<BookmarkFolder*>(item))) {
            QDomElement elem = parentElement.ownerDocument().createElement("Folder");
            elem.setAttribute("caption", folderItem->text());

            parentElement.appendChild(elem);

            for (int i = 0; i < folderItem->childCount(); i++) {
                saveItem(folderItem->child(i), elem);
            }
        }
        else if ((bookmarkItem = dynamic_cast<BookmarkItem*>(item))) {
            QDomElement elem = parentElement.ownerDocument().createElement("Bookmark");

            elem.setAttribute("key", bookmarkItem->englishKey());
            elem.setAttribute("description", bookmarkItem->description());
            elem.setAttribute("modulename", bookmarkItem->moduleName());
            elem.setAttribute("moduledescription", bookmarkItem->module() ? bookmarkItem->module()->config(CSwordModuleInfo::Description) : QString());
        if (!bookmarkItem->text().isEmpty()) {
            elem.setAttribute("title", bookmarkItem->text());
        }
            parentElement.appendChild(elem);
        }
    }


public: /* Fields */

    BookmarkFolder * m_rootItem;
    QTimer m_saveTimer;
    static BtBookmarksModel * m_defaultModel;

    Q_DECLARE_PUBLIC(BtBookmarksModel);
    BtBookmarksModel * const q_ptr;

};

BtBookmarksModel * BtBookmarksModelPrivate::m_defaultModel = nullptr;

using BookmarkItemBase = BtBookmarksModelPrivate::BookmarkItemBase;
using BookmarkItem = BtBookmarksModelPrivate::BookmarkItem;
using BookmarkFolder = BtBookmarksModelPrivate::BookmarkFolder;


BookmarkFolder::BookmarkFolder(const QString & name, BookmarkItemBase * parent)
        : BookmarkItemBase(parent) {
    setText(name);
    setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    setIcon(CResMgr::mainIndex::closedFolder::icon());
}

QList<BookmarkItemBase*> BookmarkFolder::getChildList() const {
    QList<BookmarkItemBase*> list;
    for (int i = 0; i < childCount(); i++) {
        list.append(child(i));
    }
    return list;
}

bool BookmarkFolder::hasDescendant(BookmarkItemBase const * const item) const {
    if (this == item)
        return true;
    if (getChildList().indexOf(const_cast<BookmarkItemBase *>(item)) > -1)
        return true;
    Q_FOREACH(BookmarkItemBase const * const childItem, getChildList())
        if (BookmarkFolder const * const folder =
                dynamic_cast<BookmarkFolder const *>(childItem))
            if (folder->hasDescendant(childItem))
                return true;
    return false;
}

BookmarkFolder * BookmarkFolder::deepCopy() const {
    BookmarkFolder* newFolder = new BookmarkFolder(this->text());
    Q_FOREACH(BookmarkItemBase const * const subitem, getChildList()) {
        if (BookmarkItem const * const bmItem =
                dynamic_cast<BookmarkItem const *>(subitem))
        {
            newFolder->addChild(new BookmarkItem(*bmItem));
        } else if (BookmarkFolder const * const bmFolder =
                           dynamic_cast<BookmarkFolder const *>(subitem))
        {
            newFolder->addChild(bmFolder->deepCopy());
        }
    }
    return newFolder;
}

BookmarkItem::BookmarkItem(CSwordModuleInfo const & module,
                               const QString & key,
                               const QString & description,
                               const QString & title)
        : m_description(description)
        , m_moduleName(module.name())
{
    Q_UNUSED(title);

    if (((module.type() == CSwordModuleInfo::Bible) || (module.type() == CSwordModuleInfo::Commentary))) {
        /// here we only translate \param key into english
        sword::VerseKey vk(key.toUtf8().constData(), key.toUtf8().constData(),
            static_cast<sword::VerseKey *>(module.module().getKey())->getVersificationSystem());
        CSwordVerseKey k(&vk, &module);
        k.setLocale("en");
        m_key = k.key();
    }
    else {
        m_key = key;
    };

    setIcon(CResMgr::mainIndex::bookmark::icon());
    setText(toHeader(key, module.name()));
    setFlags(Qt::ItemIsSelectable /*| Qt::ItemIsEditable*/ | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
}

BookmarkItem::BookmarkItem(BookmarkItemBase * parent)
        : BookmarkItemBase(parent) {
    setFlags(Qt::ItemIsSelectable /*| Qt::ItemIsEditable*/ | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    setIcon(CResMgr::mainIndex::bookmark::icon());
    setText(toHeader(key(), module() ? module()->name() : QObject::tr("unknown")));
}

BookmarkItem::BookmarkItem(const BookmarkItem & other)
        : BookmarkItemBase(other)
        , m_key(other.m_key)
        , m_description(other.m_description)
        , m_moduleName(other.m_moduleName)
{
    setIcon(CResMgr::mainIndex::bookmark::icon());
    setText(toHeader(key(), module() ? module()->name() : QObject::tr("unknown")));
}

CSwordModuleInfo *BookmarkItem::module() const {
    return CSwordBackend::instance()->findModuleByName(m_moduleName);
}

QString BookmarkItem::key() const {
    const QString englishKeyName = englishKey();
    if (!module()) {
        return englishKeyName;
    }

    QString returnKeyName = englishKeyName;
    if ((module()->type() == CSwordModuleInfo::Bible) || (module()->type() == CSwordModuleInfo::Commentary)) {
        /// here we only translate \param key into current book name language
        sword::VerseKey vk(englishKeyName.toUtf8().constData(), englishKeyName.toUtf8().constData(),
            static_cast<sword::VerseKey *>(module()->module().getKey())->getVersificationSystem());
        CSwordVerseKey k(&vk, module());
        k.setLocale(CSwordBackend::instance()->booknameLanguage().toLatin1() );
        returnKeyName = k.key();
    }

    return returnKeyName;
}

QString BookmarkItem::toolTip() const {
    if (!module())
        return QString();

    FilterOptions filterOptions = btConfig().getFilterOptions();
    filterOptions.footnotes = false;
    filterOptions.scriptureReferences = false;
    CSwordBackend::instance()->setFilterOptions(filterOptions);

    std::unique_ptr<CSwordKey> k(CSwordKey::createInstance(module()));
    BT_ASSERT(k);
    k->setKey(key());

    // const CLanguageMgr::Language* lang = module()->language();
    // BtConfig::FontSettingsPair fontPair = getBtConfig().getFontForLanguage(lang);

    QString const header(toHeader(key(), module()->name()));
    QString ret("<b>");
    ret.append(header).append(")</b>");
    QString const txt(text());
    if (txt != header)
        ret.append("<br>").append(txt);
    return ret.append("<hr>").append(description());
}


BtBookmarksModel::BtBookmarksModel(QObject * parent)
    : QAbstractItemModel(parent)
    , d_ptr(new BtBookmarksModelPrivate(this))
{
    load();
}

BtBookmarksModel::BtBookmarksModel(QString const & fileName, QObject * parent)
    : QAbstractItemModel(parent)
    , d_ptr(new BtBookmarksModelPrivate(this))
{ load(fileName); }

BtBookmarksModel::~BtBookmarksModel() {
    Q_D(BtBookmarksModel);

    if(d->m_saveTimer.isActive())
        save();

    delete d_ptr;
}

int BtBookmarksModel::rowCount(const QModelIndex & parent) const {
    Q_D(const BtBookmarksModel);

    return d->item(parent)->childCount();
}

int BtBookmarksModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return 1;
}

bool BtBookmarksModel::hasChildren(const QModelIndex & parent) const {
    return rowCount(parent) > 0;
}
QModelIndex BtBookmarksModel::index(int row, int column, const QModelIndex & parent) const {
    Q_D(const BtBookmarksModel);

    const BookmarkItemBase * i = d->item(parent);
    if(i->childCount() > row && row >= 0)
        return createIndex(row, column, i->child(row));
    return QModelIndex();
}

QModelIndex BtBookmarksModel::parent(const QModelIndex & index) const {
    Q_D(const BtBookmarksModel);

    const BookmarkItemBase * i = d->item(index);
    return (i->parent() == nullptr || i->parent()->parent() == nullptr) ?
                QModelIndex() : createIndex(i->parent()->index(), 0, i->parent());
}

QVariant BtBookmarksModel::data(const QModelIndex & index, int role) const {
    Q_D(const BtBookmarksModel);

    const BookmarkItemBase * i = d->item(index);

    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return i->text();
        case Qt::ToolTipRole:
            return i->toolTip();
        case Qt::DecorationRole:
            return i->icon();
        case TypeRole:
            {
                if(isBookmark(index))
                    return "bookmark";
                else
                    return "folder";
            }
    }
    return QVariant();
}

Qt::ItemFlags BtBookmarksModel::flags(const QModelIndex & index) const {
    Q_D(const BtBookmarksModel);

    return d->item(index)->flags();
}

QVariant BtBookmarksModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

bool BtBookmarksModel::setData(const QModelIndex & index, const QVariant & val, int role) {
    Q_D(BtBookmarksModel);

    BookmarkItemBase * i = d->item(index);
    switch(role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            i->setText(val.toString());
            if(dynamic_cast<BookmarkFolder *>(i) || dynamic_cast<BookmarkItem *>(i))
                d->needSave();
            return true;
        }
        case Qt::ToolTipRole:
        {
            i->setToolTip(val.toString());
            if(dynamic_cast<BookmarkFolder *>(i) || dynamic_cast<BookmarkItem *>(i))
                d->needSave();
            return true;
        }
    }
    return false;
}

bool BtBookmarksModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_D(BtBookmarksModel);

    BT_ASSERT(rowCount(parent) >= row + count);

    beginRemoveRows(parent, row, row + count - 1);

    for(int i = 0; i < count; ++i) {
        d->item(parent)->removeChild(row);
    }
    endRemoveRows();

    d->needSave();

    return true;
}

bool BtBookmarksModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_D(BtBookmarksModel);

    BT_ASSERT(rowCount(parent) >= row + count - 1);

    beginInsertRows(parent, row, row + count - 1);

    for(int i = 0; i < count; ++i) {
        d->item(parent)->insertChild(row, new BookmarkItemBase);
    }
    endInsertRows();

    return true;

}

bool BtBookmarksModel::save(QString fileName, const QModelIndex & rootItem) {
    Q_D(BtBookmarksModel);

    QString const serializedTree(
            d->serializeTreeFromRootItem(d->item(rootItem)));
    if (fileName.isEmpty())
        fileName = BtBookmarksModelPrivate::defaultBookmarksFile();

    util::tool::savePlainFile(fileName,
                              serializedTree,
                              QTextCodec::codecForName("UTF-8"));

    if(d->m_saveTimer.isActive())
        d->m_saveTimer.stop();

    return true;
}

bool BtBookmarksModel::load(QString fileName, const QModelIndex & rootItem) {
    Q_D(BtBookmarksModel);

    BookmarkItemBase * i = d->item(rootItem);
    QList<BookmarkItemBase *> items = d->loadTree(fileName);

    if(!rootItem.isValid() && fileName.isEmpty()) {
        BT_ASSERT(!d->m_defaultModel && "Only one default model allowed!");
        BT_CONNECT(&d->m_saveTimer, &QTimer::timeout,
                   this,            &BtBookmarksModel::slotSave);
        d->m_defaultModel = this;
    }

    if(items.size() == 0)
        return false;

    beginInsertRows(rootItem, i->childCount(), i->childCount() + items.size() - 1);

    i->insertChildren(i->childCount(), items);

    endInsertRows();

    return true;
}

bool BtBookmarksModel::isFolder(const QModelIndex &index) const
{
    Q_D(const BtBookmarksModel);
    return d->itemAs<BookmarkFolder const>(index);
}

bool BtBookmarksModel::isBookmark(const QModelIndex &index) const
{
    Q_D(const BtBookmarksModel);
    return d->itemAs<BookmarkItem const>(index);
}

QModelIndexList BtBookmarksModel::copyItems(int row, const QModelIndex & parent, const QModelIndexList & toCopy)
{
    Q_D(BtBookmarksModel);

    bool bookmarksOnly = true;
    bool targetIncluded = false;
    bool moreThanOneFolder = false;

    QList<BookmarkItemBase *> newList;

    Q_FOREACH(QModelIndex const index, toCopy) {
        if (BookmarkFolder const * const folder =
                d->itemAs<BookmarkFolder const>(index))
        {
            bookmarksOnly = false;
            if (toCopy.count() > 1) { // only one item allowed if a folder is selected
                moreThanOneFolder = true;
                break;
            }
            if (folder->hasDescendant(d->item(parent))) { // dropping to self or descendand not allowed
                targetIncluded = true;
                break;
            }
        }
        else {
            newList.append(new BookmarkItem(*(d->itemAs<BookmarkItem>(index))));
        }
    }

    if (!bookmarksOnly && toCopy.count() == 1)
        newList.append(d->itemAs<BookmarkFolder const>(toCopy[0])->deepCopy());
    if (!bookmarksOnly && toCopy.count() > 1) {
        // wrong amount of items
        moreThanOneFolder = true;
    }

    if (moreThanOneFolder || targetIncluded) {
        return QModelIndexList();
    }


    beginInsertRows(parent, row, row + newList.size() - 1);

    d->item(parent)->insertChildren(row, newList);

    endInsertRows();

    d->needSave();

    QModelIndexList result;
    for(int i = 0; i < newList.size(); ++i) {
        result.append(index(row + i, 0, parent));
    }
    return result;
}

CSwordModuleInfo * BtBookmarksModel::module(const QModelIndex & index) const
{
    Q_D(const BtBookmarksModel);

    if (BookmarkItem const * const i = d->itemAs<BookmarkItem const>(index))
        return i->module();
    return nullptr;
}

QString BtBookmarksModel::key(const QModelIndex & index) const
{
    Q_D(const BtBookmarksModel);

    if (BookmarkItem const * const i = d->itemAs<BookmarkItem const>(index))
        return i->key();
    return QString();
}

QString BtBookmarksModel::description(const QModelIndex &index) const
{
    Q_D(const BtBookmarksModel);

    if (BookmarkItem const * const i = d->itemAs<BookmarkItem const>(index))
        return i->description();
    return QString();
}

void BtBookmarksModel::setDescription(const QModelIndex &index, const QString &description)
{
    Q_D(BtBookmarksModel);

    if (BookmarkItem * const i = d->itemAs<BookmarkItem>(index)) {
        i->setDescription(description);
        d->needSave();
    }
}

QModelIndex BtBookmarksModel::addBookmark(int const row,
                                          QModelIndex const & parent,
                                          CSwordModuleInfo const & module,
                                          QString const & key,
                                          QString const & description,
                                          QString const & title)
{
    Q_D(BtBookmarksModel);

    if (BookmarkFolder * const i = d->itemAs<BookmarkFolder>(parent)) {
        int r = row < 0 ? row + rowCount(parent) + 1 : row;

        beginInsertRows(parent, r, r);

        BookmarkItem * c = new BookmarkItem(module, key, description, title);
        i->insertChild(r, c);

        endInsertRows();

        d->needSave();

        return createIndex(c->index(), 0, c);
    }
    return QModelIndex();
}

QModelIndex BtBookmarksModel::addFolder(int row, const QModelIndex &parent, const QString &name)
{
    Q_D(BtBookmarksModel);

    if (BookmarkFolder * const i = d->itemAs<BookmarkFolder>(parent)) {
        beginInsertRows(parent, row, row);

        BookmarkFolder * c = new BookmarkFolder(name.isEmpty() ? QObject::tr("New folder") : name);
        i->insertChild(row, c);

        endInsertRows();

        d->needSave();

        return createIndex(c->index(), 0, c);
    }
    return QModelIndex();
}

bool BtBookmarksModel::hasDescendant(const QModelIndex &baseIndex, const QModelIndex &testIndex) const
{
    Q_D(const BtBookmarksModel);

    if (BookmarkFolder const * const f =
            d->itemAs<BookmarkFolder const>(baseIndex))
        return f->hasDescendant(d->item(testIndex));
    return false;
}

bool BtBookmarksModelSortAscending(BookmarkItemBase * i1, BookmarkItemBase * i2)
{
    return i1->text().localeAwareCompare(i2->text()) < 0;
}

bool BtBookmarksModelSortDescending(BookmarkItemBase * i1, BookmarkItemBase * i2)
{
    return i1->text().localeAwareCompare(i2->text()) > 0;
}

void BtBookmarksModel::sortItems(QModelIndex const & parent,
                                 Qt::SortOrder const order)
{
    Q_D(BtBookmarksModel);

    if(BookmarkFolder * const f = d->itemAs<BookmarkFolder>(parent)) {
        QList<BookmarkFolder *> parents;
        if(f == d->m_rootItem) {
            QList<BookmarkItemBase *> items;
            items.append(f);
            for(int i = 0; i < items.size(); ++i) {
                items.append(items[i]->children());
                if(BookmarkFolder * ff = dynamic_cast<BookmarkFolder *>(items[i]))
                    parents.append(ff);
            }
        }
        else
            parents.append(f);

        Q_FOREACH(BookmarkFolder * const f, parents) {
            emit layoutAboutToBeChanged();

            QModelIndexList indexes;
            for(int i = 0; i < f->children().size(); ++i)
                indexes.append(createIndex(i, 0, f->children()[i]));

            std::sort(f->children().begin(),
                      f->children().end(),
                      order == Qt::AscendingOrder
                      ? BtBookmarksModelSortAscending
                      : BtBookmarksModelSortDescending);

            for(int i = 0; i < f->children().size(); ++i) {
                BookmarkItemBase * iii = f->children()[i];
                for(int ii = 0; ii < indexes.size(); ++ii)
                    if(iii == indexes[ii].internalPointer())
                        changePersistentIndex(createIndex(ii, 0, iii), createIndex(i, 0, iii));
            }
            emit layoutChanged();

            d->needSave();
        }
    }
}

QHash<int, QByteArray> BtBookmarksModel::roleNames() const {
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "display";
    roleNames[Qt::DecorationRole] = "icon";
    roleNames[Qt::EditRole] = "edit";
    roleNames[TypeRole] = "itemtype";
    return roleNames;
}


