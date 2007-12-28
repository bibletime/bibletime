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
#include "util/cresmgr.h"
#include "util/ctoolclass.h"

#include <QString>
#include <QList>

#include <QDebug>



//This ctor creates the root item and the tree.
BTModuleTreeItem::BTModuleTreeItem(QList<BTModuleTreeItem::Filter*>& filters,
									BTModuleTreeItem::Grouping grouping,
									ListCSwordModuleInfo* modules)
	: m_moduleInfo(0),
	m_type(BTModuleTreeItem::Root),
	m_next(0),
	m_firstChild(0)
{
	if (modules) {
		m_originalModuleList = *modules;
	} else {
		m_originalModuleList = CPointers::backend()->moduleList();
	}
	//populate the tree with groups/modules
	create_tree(filters, grouping);
}

/**
* Private constructor which sets the members of the non-root item. This will be the first child of the
* parent, the previous firstChild will be the next sibling of this.
*/
BTModuleTreeItem::BTModuleTreeItem(BTModuleTreeItem* parentItem, const QString& text, BTModuleTreeItem::Type type, CSwordModuleInfo* info, CSwordModuleInfo::Category category)
	: m_type(type),
	m_moduleInfo(info),
	m_text(text),
	m_firstChild(0),
	m_next(0),
	m_category(category)
{
	if (info) {
		m_text = info->name();
		m_type = BTModuleTreeItem::Module;
	}
	BTModuleTreeItem* tmp = parentItem->m_firstChild;
	parentItem->m_firstChild = this;
	if (tmp) this->m_next = tmp;
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
		}
	}
	return childList;
}

//TODO
QString BTModuleTreeItem::iconName() const
{
	if (m_type == Category) {
		switch ( m_category) {
		case CSwordModuleInfo::Bibles:
			return CResMgr::categories::bibles::icon;
			break;
		case CSwordModuleInfo::Commentaries:
			return CResMgr::categories::commentaries::icon;
			break;
		case CSwordModuleInfo::Books:
			return CResMgr::categories::books::icon;
			break;
		case CSwordModuleInfo::Cult:
			return QString::null;
			break;
		case CSwordModuleInfo::Images:
			return CResMgr::categories::images::icon;
			break;
		case CSwordModuleInfo::DailyDevotional:
			return CResMgr::categories::dailydevotional::icon;
			break;
		case CSwordModuleInfo::Lexicons:
			return CResMgr::categories::lexicons::icon;
			break;
		case CSwordModuleInfo::Glossary:
			return CResMgr::categories::glossary::icon;
			break;
		}
	}
	else if (m_type == Module) {
		return CToolClass::getIconNameForModule(m_moduleInfo);
	}
	else if (m_type == Language) {
		//TODO: don't hardcode here
		return "flag.svg";
	}

	return QString::null;
}


void BTModuleTreeItem::create_tree(QList<BTModuleTreeItem::Filter*>& filters, BTModuleTreeItem::Grouping grouping)
{
	qDebug("BTModuleTreeItem::create_tree");
	static bool map_initialized = false;
	static QMap<CSwordModuleInfo::Category, QString> CategoryNamesMap;
	if (!map_initialized) {
		CategoryNamesMap.insert(CSwordModuleInfo::Commentaries, QObject::tr("Commentaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Cult, QObject::tr("Cults/Unorthodox"));
		CategoryNamesMap.insert(CSwordModuleInfo::Images, QObject::tr("Maps and Images"));
		CategoryNamesMap.insert(CSwordModuleInfo::DailyDevotional, QObject::tr("Daily Devotionals"));
		CategoryNamesMap.insert(CSwordModuleInfo::Lexicons, QObject::tr("Lexicons and Dictionaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Bibles, QObject::tr("Bibles"));
		CategoryNamesMap.insert(CSwordModuleInfo::Glossary, QObject::tr("Glossaries"));
		CategoryNamesMap.insert(CSwordModuleInfo::Books, QObject::tr("Books"));
	
		map_initialized = true;
	} 

	//ListCSwordModuleInfo originalInfoList = CPointers::backend()->moduleList();
	
	foreach (CSwordModuleInfo* info, m_originalModuleList) {
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
				BTModuleTreeItem* catItem = create_parent_item(parentGroupForCategory, CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category, info->category());

				if (grouping == BTModuleTreeItem::CatMod)
					parentGroupForModule = catItem;
				else
					parentGroupForLanguage = catItem;
			}

			if (grouping == BTModuleTreeItem::CatLangMod) {
				// category is there already, create language and make it the parent for the module
				parentGroupForModule = create_parent_item(parentGroupForLanguage, info->language()->translatedName(), BTModuleTreeItem::Language);
			}

			if (grouping == BTModuleTreeItem::LangCatMod) {
				//language is there already, create category and make it the parent for the module
				parentGroupForModule = create_parent_item(parentGroupForCategory, CategoryNamesMap.value(info->category()), BTModuleTreeItem::Category, info->category());
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
	BTModuleTreeItem* item = 0;
	foreach(BTModuleTreeItem* it, parentGroup->children()) {
		if (it->text() == itemText) {
			item = it;
			break;
		}
	}
	if (!item)
		item = new BTModuleTreeItem(parentGroup, itemText, type, 0, category);

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
