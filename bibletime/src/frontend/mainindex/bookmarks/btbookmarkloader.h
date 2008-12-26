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

class BtBookmarkLoader
{
public:
	QTreeWidgetItem* loadTree();
	QTreeWidgetItem* loadOldTree();
	void saveTreeFromRootItem(QTreeWidgetItem* rootItem);

private:
	QTreeWidgetItem* loadFolder();
	QTreeWidgetItem* loadItem();

	QTreeWidgetItem* loadOldFolder();
	QTreeWidgetItem* loadOldItem();
};

#endif
