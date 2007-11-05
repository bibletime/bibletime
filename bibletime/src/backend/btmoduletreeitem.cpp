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
	//Q_ASSERT(class_invariant());
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
	//Q_ASSERT(previousSibling || parentItem);
	//Q_ASSERT(info != 0 || ( (type!=BTModuleTreeItem::Module) && !text.isEmpty() ) );

	if (info) {
		m_text = info->name();
		m_type = BTModuleTreeItem::Module;
	}

	//this item must have either a parent or a previous sibling
	if (previousSibling) {
		previousSibling->m_next = this;
	} else {
		parentItem->m_firstChild = this;
	}

}


BTModuleTreeItem::~BTModuleTreeItem()
{
	//qDebug("BTModuleTreeItem::~BTModuleTreeItem");
	//qDebug() << this->text();
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


QString BTModuleTreeItem::iconPath() const
{
	return QString::null;
}

// class invariant was not a good idea here because items are changed in many ways while creating the tree.
// bool BTModuleTreeItem::class_invariant() const
// {
// 	//TODO: add #ifdef DEBUG or something similar around this function body
// 	bool only_module_has_info = (m_moduleInfo != 0 || ( (m_type!=BTModuleTreeItem::Module) ) );
// 	bool nonrecursive = (m_next != this);
// 	bool has_text = (!m_text.isEmpty());
// 	bool module_has_no_children = !(m_type==BTModuleTreeItem::Module && m_next);
// 	bool category_and_language_have_children = !((m_type==BTModuleTreeItem::Category || m_type==BTModuleTreeItem::Language) && !m_firstChild);
// 	bool next_is_same_type = !(m_next && m_type != m_next->m_type);
// 	bool child_is_different_type = !(m_firstChild && m_type == m_firstChild->m_type);
// 
// 	return (only_module_has_info
// 			&& nonrecursive
// 			&& has_text
// 			&& module_has_no_children
// 			&& category_and_language_have_children
// 			&& next_is_same_type
// 			&& child_is_different_type);
// }

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
			BTModuleTreeItem* tempItem = 0;
			BTModuleTreeItem* parentGroupForModule = this;
			BTModuleTreeItem* parentGroupForLanguage = this;
			BTModuleTreeItem* parentGroupForCategory = this;


			//the order of if(grouping...) clauses is important
			// TODO: remove repetition with a new function, refactor if you find a better (simpler) way
			if (grouping == BTModuleTreeItem::LangMod || grouping == BTModuleTreeItem::LangCatMod) {
				BTModuleTreeItem* langItem = 0;
				QString langName = info->language()->translatedName();
				foreach(BTModuleTreeItem* it, parentGroupForLanguage->children()) {
					if (it->text() == langName) {
						langItem = it;
						break;
					} 
				}
				if (!langItem) {
					//qDebug() << "create a new language group:" << langName;
					tempItem = parentGroupForLanguage->m_firstChild;
					langItem = new BTModuleTreeItem(0, parentGroupForLanguage, langName, BTModuleTreeItem::Language, 0);
					if (tempItem)  parentGroupForLanguage->m_firstChild->m_next = tempItem;
				}
				if (grouping == BTModuleTreeItem::LangMod) {
					parentGroupForModule = langItem;
				} else if (grouping == BTModuleTreeItem::LangCatMod) {
					parentGroupForCategory = langItem;
				}
			}

			if (grouping == BTModuleTreeItem::CatMod || grouping == BTModuleTreeItem::CatLangMod) {
				BTModuleTreeItem* catItem = 0;
				QString cat = CategoryNamesMap.value(info->category());
				foreach(BTModuleTreeItem* it, parentGroupForCategory->children()) {
					if (it->text() == cat) {
						//qDebug() << "category" << cat << "existed already";
						catItem = it;
						break;
					} 
				}
				if (!catItem) {
					//qDebug() << "create a new Category group:" << cat;
					tempItem = parentGroupForCategory->m_firstChild;
					catItem = new BTModuleTreeItem(0, parentGroupForCategory, cat, BTModuleTreeItem::Category, 0);
					if (tempItem) parentGroupForCategory->m_firstChild->m_next = tempItem;
				}
				if (grouping == BTModuleTreeItem::CatMod) {
					parentGroupForModule = catItem;
				} else if (grouping == BTModuleTreeItem::CatLangMod) {
					parentGroupForLanguage = catItem;
				}
			}

			if (grouping == BTModuleTreeItem::CatLangMod) {
				// category is there already, create language and make it the parent for the module
				BTModuleTreeItem* langItem = 0;
				QString langName = info->language()->translatedName();
				foreach(BTModuleTreeItem* it, parentGroupForLanguage->children()) {
					if (it->text() == langName) {
						langItem = it;
						break;
					} 
				}
				if (!langItem) {
					//qDebug() << "create a new language group under category:" << langName;
					tempItem = parentGroupForLanguage->m_firstChild;
					langItem = new BTModuleTreeItem(0, parentGroupForLanguage, langName, BTModuleTreeItem::Language, 0);
					if (tempItem) parentGroupForLanguage->m_firstChild->m_next = tempItem;
				}
				parentGroupForModule = langItem;
				
			}

			if (grouping == BTModuleTreeItem::LangCatMod) {
				//language is there already, create category and make it the parent for the module
				BTModuleTreeItem* catItem = 0;
				QString cat = CategoryNamesMap.value(info->category());
				foreach(BTModuleTreeItem* it, parentGroupForCategory->children()) {
					if (it->text() == cat) {
						catItem = it;
						break;
					} 
				}
				if (!catItem) {
					//qDebug() << "create a new category under language:" << cat;
					tempItem = parentGroupForCategory->m_firstChild;
					catItem = new BTModuleTreeItem(0, parentGroupForCategory, cat, BTModuleTreeItem::Category, 0);
					if (tempItem) parentGroupForCategory->m_firstChild->m_next = tempItem;
				}
				parentGroupForModule = catItem;
				
			}

			// the parent group for module has been set above, now just add the module to it
			//qDebug() << "parent groups have been created/set, add add module to:" << parentGroupForModule->text();
			tempItem = parentGroupForModule->m_firstChild;
			new BTModuleTreeItem(0, parentGroupForModule, QString::null, BTModuleTreeItem::Module, info);
			parentGroupForModule->m_firstChild->m_next = tempItem;
			//qDebug() << "included new module into tree:" << parentGroupForModule->m_firstChild;

		} // end: if (included)
	}

	// Finally sort the items
	sort_children(this);
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