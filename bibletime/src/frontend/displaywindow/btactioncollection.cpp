/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btactioncollection.h"
#include "btactioncollection.moc"

BtActionCollection::BtActionCollection(QObject* parent)
	: QObject(parent)
{
}

BtActionCollection::~BtActionCollection()
{
}

QAction* BtActionCollection::action(const QString& name)
{
	Q_ASSERT(m_actions[name] != 0);
	return m_actions[name];
}

void BtActionCollection::addAction(const QString& name, QAction* action)
{
	Q_ASSERT(action != 0);
//	Q_ASSERT(m_actions[name] == 0);   // TODO - replacing actions is ok???
	m_actions[name] = action;
}

void BtActionCollection::addAction(const QString &name, const QObject *receiver)
{
}


