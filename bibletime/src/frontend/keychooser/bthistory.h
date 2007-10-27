//
// C++ Interface: BTHistory
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BTHISTORY_H
#define BTHISTORY_H

#include <QLinkedList>
#include <QObject>
#include <QMutableLinkedListIterator>


class CSwordKey;
class QAction;


class BTHistory: public QObject
{
	Q_OBJECT

public:
	BTHistory();
	~BTHistory() {};

	QList<QAction*> getBackList();
	QList<QAction*> getFwList();

public slots:
	void add(CSwordKey* newKey);
	void move(QAction*);
	void back();
	void fw();
	
signals:
	void historyChanged(bool backEnabled, bool fwEnabled);
	void historyMoved(CSwordKey* newKey, CSwordKey* oldKey);

private:
	//QAction* current();
	QLinkedList<QAction*> m_historyList;
	QMutableLinkedListIterator<QAction*> m_historyListIterator;
};

#endif
