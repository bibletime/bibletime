//
// C++ Implementation: BTHistory
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "bthistory.h"
#include "bthistory.moc"

#include "backend/keys/cswordkey.h"


#include <QAction>
#include <QList>

#include <QDebug>


BTHistory::BTHistory()
	: m_historyList(),
	m_historyListIterator(m_historyList)
{}

void BTHistory::add(CSwordKey* newKey) {
	qDebug("BTHistory::add");
	qDebug() << "new key:" << newKey->key();

	if (!m_historyListIterator.hasPrevious() || (newKey->key() != m_historyListIterator.peekPrevious()->text()) ) {
		m_historyListIterator.insert(new QAction(newKey->key(), this) );
		// TODO: history limit
		bool backEnabled;
		bool fwEnabled;
		emit historyChanged(backEnabled, fwEnabled);
	}
}

void BTHistory::move(QAction* historyItem)
{
	qDebug("BTHistory::move");
	//Q_ASSERT(historyItem);
	Q_ASSERT(m_historyList.count());

	//m_inHistoryFunction = true;

	//find the action in the list
	m_historyListIterator.toFront();
	//move to the selected item in the list, it will be the current item
	m_historyListIterator.findNext(historyItem);
	
	//key()->key(historyItem->text());
	//setKey(key());
	bool backEnabled;
	bool fwEnabled;
	//emit historyMoved(newKey);
	emit historyChanged(backEnabled, fwEnabled);

	//m_inHistoryFunction = false;

	
}

void BTHistory::back()
{
	qDebug("BTHistory::back");
	//TODO: here is the error...
	if ( m_historyListIterator.hasPrevious() ) {
		move(m_historyListIterator.peekPrevious());
	}
}

void BTHistory::fw()
{
	qDebug("BTHistory::fw");
	if (m_historyListIterator.hasNext()) {
		move(m_historyListIterator.peekNext());
	}
}

QList<QAction*> BTHistory::getBackList()
{
// 	QList<QAction*> list();
// 	int index = indexOf(m_historyListIterator.peekPrevious());
// 	for (;index >= 0; --index) {
// 		list.append(m_historyList[index]);
// 	}
	return QList<QAction*>();
}

QList<QAction*> BTHistory::getFwList()
{
	return QList<QAction*>();
}