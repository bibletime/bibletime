//
// C++ Implementation: btmoduletreeitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "btmoduletreeitem.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "util/cpointers.h"
#include "backend/managers/cswordbackend.h"

#include <QString>
#include <QList>

#include <QDebug>

#include <klocale.h>

//This ctor creates the root item and the tree.
BTModuleTreeItem::BTModuleTreeItem(QList<BTModuleTreeItem::Filter*>& filters, BTModuleTreeItem::Grouping grouping)
	: m_moduleInfo(0),
	m_type(BTModuleTreeItem::Root),
	m_next(0),
	m_firstChild(0)
{
	//populate the tree with groups/modules
	create_tree(filters, grouping);
}

/**
* Private constructor which sets the members of the non-root item.
*/
BTModuleTreeItem::BTModuleTreeItem(BTModuleTreeItem* previousSibling, BTModuleTreeItem* parentItem, const QString& text, BTModuleTreeItem::Type type, CSwordModuleInfo* info)
	: m_type(type),
	m_moduleInfo(info),
	m_text(text),
	m_firstChild(0),
	m_next(0)
{
	if (info) {
		m_text = info->name();
		m_type = BTModuleTreeItem::Module;
	}

	//this item has either a parent or a previous sibling
	if (previousSibling) {
		previousSibling->m_next = this;
	} else {
		parentItem->m_firstChild = this;
	}

}


BTModuleTreeItem::~BTModuleTreeItem()
{
	// this works recursively
	foreach(BTModuleTreeItem* i, children()) {
		delete i;
	}
}

QList<BTModuleTreeItem*> BTModuleTreeItem::children() const
{
	//qDebug("BTModuleTreeItem::children");
	QList<BTModuleTreeItem*> childList;
    if (m_firstChild) {
		BTModuleTreeItem* child = m_firstChild;
		while (child) {
			//qDebug() << "child:" << child->text();
			childList.append(child);
			child = child->m_next;
			//if (child) qDebug() << "next child: " << child->text();
		}
	}
	//qDebug() << "size of child list (" << this->text() <<"):" << childList.size();
	return childList;
}

//TODO
QString BTModuleTreeItem::iconPath() const
{
	return QString::null;
}


void BTModuleTreeItem::create_tree(QList<BTModuleTreeItem::Filter*>& filters, BTModuleTreeItem::Grouping grouping)
{
	qDebug("BTModuleTreeItem::create_tree");
	static bool map_initialized = false;
	static QMap<CSwordModuleInfo::Category, QString> CategoryNamesMap;
	if (!map_initialized) {
		CategoryNamesMap.insert(CSwordModuleInfo::Commentaries, i18n("Commentaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Cult, i18n("Cults/Unorthodox"));
		CategoryNamesMap.insert(CSwordModuleInfo::Images, i18n("Maps and Images"));
		CategoryNamesMap.insert(CSwordModuleInfo::DailyDevotional, i18n("Daily Devotionals"));
		CategoryNamesMap.insert(CSwordModuleInfo::Lexicons, i18n("Lexicons and Dictionaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Bibles, i18n("Bibles"));
		CategoryNamesMap.insert(CSwordModuleInfo::Glossary, i18n("Glossaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Books, i18n("Books"));
	
		map_initialized = true;
	} 

	ListCSwordModuleInfo originalInfoList = CPointers::backend()->moduleList();
	
	foreach (CSwordModuleInfo* info, originalInfoList) {
		bool included;
		included = true;
		foreach (BTModuleTreeItem::Filter* f, filters) {
			if (!f->filter(info)) {
				included = false;
				break;
			}
		}
		if (included) {
			//qDebug() << "a module will be included: " << info->name();
			
			BTModuleTreeItem* parentGroupForModule = this;
			BTModuleTreeItem* parentGroupForLanguage = this;
			BTModuleTreeItem* parentGroupForCategory = this;


			//the order of if(grouping...) clauses is important
			if (grouping == BTModuleTreeItem::LangMod || grouping == BTModuleTreeItem::LangCatMod) {				
				BTModuleTreeItem* langItem = create_parent_item(parentGroupForLanguage, info->language()->translatedName(), BTModuleTreeItem::Language);

				if (grouping == BTModuleTreeItem::LangMod)
					parentGroupForModule = langItem;
				else
					parentGroupForCategory = langItem;
			}

			if (grouping == BTModuleTreeItem::CatMod || grouping == BTModuleTreeItem::CatLangMod) {
				BTModuleTreeItem* catItem = create_parent_item(parentGroupForCategory, CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category);

				if (grouping == BTModuleTreeItem::CatMod)
					parentGroupForModule = catItem;
				else
					parentGroupForLanguage = catItem;
			}

			if (grouping == BTModuleTreeItem::CatLangMod) {
				// category is there already, create language and make it the parent for the module
				BTModuleTreeItem* langItem = create_parent_item(parentGroupForLanguage, info->language()->translatedName(), BTModuleTreeItem::Language);

				parentGroupForModule = langItem;
			}

			if (grouping == BTModuleTreeItem::LangCatMod) {
				//language is there already, create category and make it the parent for the module
				BTModuleTreeItem* catItem = create_parent_item(parentGroupForCategory, CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category);

				parentGroupForModule = catItem;
			}

			// the parent group for module has been set above, now just add the module to it
			//qDebug() << "parent groups have been created/set, add add module to:" << parentGroupForModule->text();
			BTModuleTreeItem* tempItem = parentGroupForModule->m_firstChild;
			new BTModuleTreeItem(0, parentGroupForModule, QString::null, BTModuleTreeItem::Module, info);
			parentGroupForModule->m_firstChild->m_next = tempItem;
			//qDebug() << "included new module into tree:" << parentGroupForModule->m_firstChild;

		} // end: if (included)
	}

	// Finally sort the items
	sort_children(this);
}

BTModuleTreeItem* BTModuleTreeItem::create_parent_item(
	BTModuleTreeItem* parentGroup,
	const QString& itemText,
	BTModuleTreeItem::Type type)
{
	BTModuleTreeItem* item = 0;
	foreach(BTModuleTreeItem* it, parentGroup->children()) {
		if (it->text() == itemText) {
			item = it;
			break;
		}
	}
	if (!item) {
		BTModuleTreeItem* tempItem = parentGroup->m_firstChild;
		item = new BTModuleTreeItem(0, parentGroup, itemText, type, 0); // give type as argument
		if (tempItem) parentGroup->m_firstChild->m_next = tempItem;
	}
	return item;
}

void BTModuleTreeItem::sort_children(BTModuleTreeItem* parent)
{
	//qDebug("BTModuleTreeItem::sort_children");
	
	// sort each child recursively depth-first
	foreach(BTModuleTreeItem* item, parent->children()) {
		sort_children(item);
	}

	QList<BTModuleTreeItem*> items = parent->children();
	if (items.size() > 0) {
		// Sort the list of the children according to each item's text
		qSort(items.begin(), items.end(), BTModuleTreeItem::localeAwareLessThan);
		//put the children back to tree in sorted order
		BTModuleTreeItem* first = items.at(0);
		BTModuleTreeItem* prev = first;
		foreach (BTModuleTreeItem* item2, items) {
			prev->m_next = item2;
			prev = item2;
		}
		prev->m_next = 0;
		parent->m_firstChild = first; // attach the partial tree to the parent
	}
}

bool BTModuleTreeItem::localeAwareLessThan(BTModuleTreeItem* first, BTModuleTreeItem* second)
{
	static bool map_initialized = false;
	static QMap<QString, int> CategoryNameValueMap;
	if (!map_initialized) {
		//this is the sorting order for categories
		CategoryNameValueMap.insert(i18n("Bibles"), 1);
		CategoryNameValueMap.insert(i18n("Commentaries"), 2);
		CategoryNameValueMap.insert(i18n("Books"), 3);
		CategoryNameValueMap.insert(i18n("Lexicons and Dictionaries"), 4);
		CategoryNameValueMap.insert(i18n("Glossaries"), 5);
		CategoryNameValueMap.insert(i18n("Daily Devotionals"), 6);
		CategoryNameValueMap.insert(i18n("Maps and Images"), 7);
		CategoryNameValueMap.insert(i18n("Cults/Unorthodox"), 8);
		map_initialized = true;
	}

	//Categories are always in the same order, not alphabetically
	if (first->type() == BTModuleTreeItem::Category) {
		return (CategoryNameValueMap.value(first->text()) < CategoryNameValueMap.value(second->text()));
	}
	return (QString::localeAwareCompare(first->text(), second->text()) < 0 );
}