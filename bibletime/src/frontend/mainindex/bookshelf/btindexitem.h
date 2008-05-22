/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINDEXITEM_H
#define BTINDEXITEM_H

#include "actionenum.h"

#include <QTreeWidgetItem>
#include <QString>

class CMainIndex;
class QMimeData;
class QAction;


/**
@author The BibleTime team <info@bibletime.info>
*/
class BTIndexItem : public QTreeWidgetItem
{
public:
	/** Those menu actions which are item specific. */

	BTIndexItem(QTreeWidgetItem* parent);
    virtual ~BTIndexItem() {};
	/**
	* Enables the given action if it has the "indexActionType" property which this item supports.
	* Handles only those actions which may or may not be supported, not those which
	* are always supported by all items. If the action is not supported in the current state of the
	* item this function does nothing.
	*/
    virtual const bool enableAction(QAction*) {return false;}

	//TODO: d'n'd functions so that view can delegate d'n'd to items?
	virtual bool acceptDrop(const QMimeData*) {return false;}
};

#endif
