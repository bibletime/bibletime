/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKCREATOR_H
#define BTBOOKMARKCREATOR_H

#include <QList>
#include <QDomElement>

class QTreeWidgetItem;

class BtBookmarkLoader
{
public:
	QList<QTreeWidgetItem*> loadTree();
	void saveTreeFromRootItem(QTreeWidgetItem* rootItem);

private:
	QTreeWidgetItem* handleXmlElement(QDomElement element, QTreeWidgetItem* parent);
	void saveItem(QTreeWidgetItem* item, QDomElement& parentElement);
	QString loadXmlFromFile();
};

#endif
