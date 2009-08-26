//
// C++ Interface: cmodulechooserdialog
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSEARCHMODULECHOOSERDIALOG_H
#define CSEARCHMODULECHOOSERDIALOG_H

class CSwordModuleInfo;

#include "frontend/cmodulechooserdialog.h"

class QTreeWidgetItem;

class BTModuleTreeItem;


namespace Search {

class CSearchModuleChooserDialog : public CModuleChooserDialog
{
	Q_OBJECT
public:
	CSearchModuleChooserDialog(QWidget* parent, QString title, QString label, QList<CSwordModuleInfo*> selectedModules);
	~CSearchModuleChooserDialog();

protected: // Protected methods
	virtual void initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem);

private:
	QList<CSwordModuleInfo*> m_selectedModules;
	BTModuleTreeItem::HiddenOff* m_hiddenFilter;
};

} //end of namespace Search

#endif
