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

#include "util/directoryutil.h"

#include <QString>
#include <QList>
#include <QDomElement>


class QTreeWidgetItem;

class BtBookmarkLoader
{
public:
	QList<QTreeWidgetItem*> loadTree(QString fileName=QString::null);
	void saveTreeFromRootItem(QTreeWidgetItem* rootItem, QString fileName=QString::null, bool forceOverwrite=true);

private:
	QTreeWidgetItem* handleXmlElement(QDomElement& element, QTreeWidgetItem* parent);
	void saveItem(QTreeWidgetItem* item, QDomElement& parentElement);
	QString loadXmlFromFile(QString fileName=QString::null);
};

#endif
