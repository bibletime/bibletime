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

#include <QList>
#include <QObject>


class CSwordKey;
class QAction;


class BTHistory: public QObject
{
	Q_OBJECT

public:
	BTHistory();
	~BTHistory() {};

	/**
	* Return a list of Actions behind the current point, the first of the history list will be the
	* last in the returned list and vice versa.
	*/
	QList<QAction*> getBackList();
	/**
	* Return a list of Actions after the current point.
	*/
	QList<QAction*> getFwList();

public slots:
	/**
	* Add a new key to the history.
	*/
	void add(CSwordKey* newKey);
	/**
	* Move the current point in history list.
	*/
	void move(QAction*);
	/**
	* Go back one step in history.
	*/
	void back();
	/**
	* Go forward one step in history.
	*/
	void fw();
	
signals:
	/**
	* Signal will be sent when the history has been changed (added, moved)
	*/
	void historyChanged(bool backEnabled, bool fwEnabled);
	/**
	* Signal will be sent when the current point in history has moved
	*/
	void historyMoved(QString& newKey);

private:

	void sendChangedSignal();
	bool class_invariant();

	QList<QAction*> m_historyList;
	int m_index; //pointer to the current item; -1==empty, 0==first etc.
	bool m_inHistoryFunction; //to prevent recursive behaviour
};

#endif
