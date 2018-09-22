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

#include "btmoduletreeitem.h"

#include <QList>
#include <QString>
#include "drivers/cswordmoduleinfo.h"
#include "managers/cswordbackend.h"
#include "../util/tool.h"


//This ctor creates the root item and the tree.
BTModuleTreeItem::BTModuleTreeItem(QList<BTModuleTreeItem::Filter*>& filters, BTModuleTreeItem::Grouping grouping, QList<CSwordModuleInfo*>* modules)
        : m_moduleInfo(nullptr),
        m_firstChild(nullptr),
        m_next(nullptr),
        m_type(BTModuleTreeItem::Root),
        m_category(CSwordModuleInfo::UnknownCategory),
        m_grouping (grouping) {
    if (modules) {
        m_originalModuleList = *modules;
    }
    else {
        m_originalModuleList = CSwordBackend::instance()->moduleList();
    }
    //populate the tree with groups/modules
    create_tree(filters);
}

/**
* Private constructor which sets the members of the non-root item. This will be the first child of the
* parent, the previous firstChild will be the next sibling of this.
*/
BTModuleTreeItem::BTModuleTreeItem(BTModuleTreeItem* parentItem, const QString& text, BTModuleTreeItem::Type type, CSwordModuleInfo* info, CSwordModuleInfo::Category category)
        : m_moduleInfo(info),
        m_text(text),
        m_firstChild(nullptr),
        m_next(nullptr),
        m_type(type),
        m_category(category) {
    if (info) {
        m_text = info->name();
        m_type = BTModuleTreeItem::Module;
    }
    BTModuleTreeItem* tmp = parentItem->m_firstChild;
    parentItem->m_firstChild = this;
    if (tmp) this->m_next = tmp;
}


BTModuleTreeItem::~BTModuleTreeItem() {
    // this works recursively
    Q_FOREACH(BTModuleTreeItem * const i, children())
        delete i;
}

QList<BTModuleTreeItem*> BTModuleTreeItem::children() const {
    QList<BTModuleTreeItem*> childList;
    if (m_firstChild) {
        BTModuleTreeItem* child = m_firstChild;
        while (child) {
            childList.append(child);
            child = child->m_next;
        }
    }
    return childList;
}

bool BTModuleTreeItem::m_map_initialized = false;
QMap<CSwordModuleInfo::Category, QString> BTModuleTreeItem::m_CategoryNamesMap;

void BTModuleTreeItem::create_tree(QList<BTModuleTreeItem::Filter*>& filters) {
    if (!m_map_initialized) {
        m_CategoryNamesMap.insert(CSwordModuleInfo::Commentaries, QObject::tr("Commentaries"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Cult, QObject::tr("Cults/Unorthodox"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Images, QObject::tr("Maps and Images"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::DailyDevotional, QObject::tr("Daily Devotionals"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Lexicons, QObject::tr("Lexicons and Dictionaries"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Bibles, QObject::tr("Bibles"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Glossary, QObject::tr("Glossaries"));
        m_CategoryNamesMap.insert(CSwordModuleInfo::Books, QObject::tr("Books"));

        m_map_initialized = true;
    }
    add_items(filters);
}

void BTModuleTreeItem::add_items(QList<BTModuleTreeItem::Filter*>& filters) {
    Q_FOREACH(CSwordModuleInfo * const info, m_originalModuleList) {
        bool included;
        included = true;
        Q_FOREACH(BTModuleTreeItem::Filter const * const f, filters) {
            if (!f->filter(*info)) {
                included = false;
                break;
            }
        }
        if (included) {
            BTModuleTreeItem* parentGroupForModule = this;
            BTModuleTreeItem* parentGroupForLanguage = this;
            BTModuleTreeItem* parentGroupForCategory = this;

            //the order of if(grouping...) clauses is important
            if (m_grouping == BTModuleTreeItem::LangMod || m_grouping == BTModuleTreeItem::LangCatMod) {
                BTModuleTreeItem* langItem = create_parent_item(parentGroupForLanguage, info->language()->translatedName(), BTModuleTreeItem::Language);

                if (m_grouping == BTModuleTreeItem::LangMod)
                    parentGroupForModule = langItem;
                else
                    parentGroupForCategory = langItem;
            }

            if (m_grouping == BTModuleTreeItem::CatMod || m_grouping == BTModuleTreeItem::CatLangMod) {
                BTModuleTreeItem* catItem = create_parent_item(parentGroupForCategory, m_CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category, info->category());

                if (m_grouping == BTModuleTreeItem::CatMod)
                    parentGroupForModule = catItem;
                else
                    parentGroupForLanguage = catItem;
            }

            if (m_grouping == BTModuleTreeItem::CatLangMod) {
                // category is there already, create language and make it the parent for the module
                parentGroupForModule = create_parent_item(parentGroupForLanguage, info->language()->translatedName(), BTModuleTreeItem::Language);
            }

            if (m_grouping == BTModuleTreeItem::LangCatMod) {
                //language is there already, create category and make it the parent for the module
                parentGroupForModule = create_parent_item(parentGroupForCategory, m_CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category, info->category());
            }

            // the parent group for module has been set above, now just add the module to it
            new BTModuleTreeItem(parentGroupForModule, QString::null, BTModuleTreeItem::Module, info);

        } // end: if (included)
    }

    // Finally sort the items
    sort_children(this);
}

BTModuleTreeItem* BTModuleTreeItem::create_parent_item(
    BTModuleTreeItem* parentGroup,
    const QString& itemText,
    BTModuleTreeItem::Type type,
    CSwordModuleInfo::Category category)
{
    Q_FOREACH(BTModuleTreeItem * const item, parentGroup->children())
        if (item->text() == itemText)
            return item;
    return new BTModuleTreeItem(parentGroup, itemText, type, nullptr, category);
}

void BTModuleTreeItem::sort_children(BTModuleTreeItem* parent) {
    // sort each child recursively depth-first
    Q_FOREACH(BTModuleTreeItem * const item, parent->children())
        sort_children(item);

    QList<BTModuleTreeItem*> items = parent->children();
    if (items.size() > 0) {
        // Sort the list of the children according to each item's text
        qSort(items.begin(), items.end(), BTModuleTreeItem::localeAwareLessThan);
        //put the children back to tree in sorted order
        BTModuleTreeItem* first = items.at(0);
        BTModuleTreeItem* prev = first;
        Q_FOREACH(BTModuleTreeItem * const item2, items) {
            prev->m_next = item2;
            prev = item2;
        }
        prev->m_next = nullptr;
        parent->m_firstChild = first; // attach the partial tree to the parent
    }
}

bool BTModuleTreeItem::localeAwareLessThan(BTModuleTreeItem* first, BTModuleTreeItem* second) {
    static bool map_initialized = false;
    static QMap<QString, int> CategoryNameValueMap;
    if (!map_initialized) {
        //this is the sorting order for categories
        CategoryNameValueMap.insert(QObject::tr("Bibles"), 1);
        CategoryNameValueMap.insert(QObject::tr("Commentaries"), 2);
        CategoryNameValueMap.insert(QObject::tr("Books"), 3);
        CategoryNameValueMap.insert(QObject::tr("Lexicons and Dictionaries"), 4);
        CategoryNameValueMap.insert(QObject::tr("Glossaries"), 5);
        CategoryNameValueMap.insert(QObject::tr("Daily Devotionals"), 6);
        CategoryNameValueMap.insert(QObject::tr("Maps and Images"), 7);
        CategoryNameValueMap.insert(QObject::tr("Cults/Unorthodox"), 8);
        map_initialized = true;
    }

    //Categories are always in the same order, not alphabetically
    if (first->type() == BTModuleTreeItem::Category) {
        return (CategoryNameValueMap.value(first->text()) < CategoryNameValueMap.value(second->text()));
    }
    return (QString::localeAwareCompare(first->text(), second->text()) < 0 );
}

QDataStream &operator<<(QDataStream &out, const BTModuleTreeItem::Grouping &grouping) {
    out << static_cast<qint8>(grouping);
    return out;
}

QDataStream &operator>>(QDataStream &in, BTModuleTreeItem::Grouping &grouping) {
    qint8 i;
    in >> i;
    grouping = static_cast<BTModuleTreeItem::Grouping>(i);
    return in;
}
