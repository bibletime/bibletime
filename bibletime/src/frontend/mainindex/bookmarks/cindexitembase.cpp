//
// C++ Implementation: cindexitembase
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cindexitembase.h"
#include "cbookmarkindex.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QMimeData>

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

const bool CIndexItemBase::isFolder() {
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
};

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
const bool CIndexItemBase::enableAction( const MenuAction /*action*/ )
{
	return false; //this base class has no valid actions
}

/** No descriptions */
const bool CIndexItemBase::isMovable()
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
	dropped(e,0);
}

bool CIndexItemBase::acceptDrop(QDropEvent* event) const
{
	if (event->provides("BibleTime/Bookmark") || event->provides("BibleTime/BookmarkItem")) {
		event->acceptProposedAction();
		// TODO: Draw the placement marker

		return true;
	}
}

void CIndexItemBase::dropped( QDropEvent* e, QTreeWidgetItem* after)
{}

const bool CIndexItemBase::allowAutoOpen( const QMimeData* ) const
{
	return false;
};



QList<QTreeWidgetItem*> CIndexItemBase::getChildList()
{
	return QList<QTreeWidgetItem*>();
}
