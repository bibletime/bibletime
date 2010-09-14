/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/
#ifndef BT_ACTION_COLLECTION_S
#define BT_ACTION_COLLECTION_S

#include <QObject>
#include <QMap>

class QString;
class QAction;

class BtActionCollection : public QObject
{
   Q_OBJECT
public:
	BtActionCollection(QObject* parent);
	~BtActionCollection();
	void addAction(const QString& name, QAction* action);
	void addAction(const QString &name, const QObject *receiver);
	QAction* action(const QString& name);

private:
   QMap<QString, QAction*> m_actions;
};

#endif
