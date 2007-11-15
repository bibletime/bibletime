//
// C++ Interface: btindexitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BTINDEXITEM_H
#define BTINDEXITEM_H

#include "actionenum.h"

#include <QTreeWidgetItem>
#include <QString>

class CMainIndex;
class QMimeData;
class KAction;


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
    virtual const bool enableAction(KAction*) {}

	//TODO: d'n'd functions so that view can delegate d'n'd to items?
	virtual bool acceptDrop(const QMimeData*) {return false;}
};

#endif
