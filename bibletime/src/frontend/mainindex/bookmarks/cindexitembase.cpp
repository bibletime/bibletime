/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cindexitembase.h"
#include "cbookmarkindex.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QMimeData>
#include <QDropEvent>

CIndexItemBase::CIndexItemBase(CBookmarkIndex* bookmarkIndex, const Type type)
	: QTreeWidgetItem(bookmarkIndex),
	m_type(type)
{}

CIndexItemBase::CIndexItemBase(CIndexItemBase* parentItem, const Type type)
	: QTreeWidgetItem(parentItem),
	m_type(type)
{}

CIndexItemBase::~CIndexItemBase() {}

const QString CIndexItemBase::toolTip()
{
	return QString::null;
}

/** Returns the used main index. */
CBookmarkIndex* CIndexItemBase::treeWidget() const {
	return dynamic_cast<CBookmarkIndex*>( QTreeWidgetItem::treeWidget() );
}

bool CIndexItemBase::isFolder() {
	return false;
}

void CIndexItemBase::init()
{
	setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
	update();
}

void CIndexItemBase::update()
{}

const CIndexItemBase::Type& CIndexItemBase::type() const
{
	return m_type;
}

void CIndexItemBase::moveAfter( CIndexItemBase* const item )
{
	if (!item)
		return;

	if ( parent() == item->parent() ) { //same parent means level
		//TODO: take item from parent, readd it
		//moveItem(item); //both items are on the same level, so we can use moveItem
	}
}

/** Returns true if the given action should be enabled in the popup menu. */
bool CIndexItemBase::enableAction( const MenuAction /*action*/ )
{
	return false; //this base class has no valid actions
}

/** No descriptions */
bool CIndexItemBase::isMovable()
{
	return false;
}

QDomElement CIndexItemBase::saveToXML( QDomDocument& /*document*/ )
{
	return QDomElement();
}

void CIndexItemBase::loadFromXML( QDomElement& /*element*/ )
{}

void CIndexItemBase::dropped( QDropEvent* e)
{
	droppedItem(e,0);
}

bool CIndexItemBase::acceptDrop(QDropEvent* event) const
{
	if (event->provides("BibleTime/Bookmark") || event->provides("BibleTime/BookmarkItem")) {
		event->acceptProposedAction();
		// TODO: Draw the placement marker
		return true;
	}
	else
	{
		return false;
	}
}

void CIndexItemBase::droppedItem( QDropEvent* /*e*/, QTreeWidgetItem* /*after*/)
{
}

bool CIndexItemBase::allowAutoOpen( const QMimeData* ) const
{
	return false;
}



QList<QTreeWidgetItem*> CIndexItemBase::getChildList()
{
	return QList<QTreeWidgetItem*>();
}
