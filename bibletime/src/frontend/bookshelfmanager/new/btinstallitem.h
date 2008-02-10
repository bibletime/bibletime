/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLITEM_H
#define BTINSTALLITEM_H

#include <QTreeWidgetItem>

class BtInstallThread;

/**
* Tree widget item which represents one module which is being installed.
*/
class BtInstallItem : public QTreeWidgetItem
{

public:
	BtInstallItem(QTreeWidget*, QString sourceName, QString moduleName, QString destinationName);

	~BtInstallItem();

private:
	BtInstallThread* m_thread;
};

#endif
