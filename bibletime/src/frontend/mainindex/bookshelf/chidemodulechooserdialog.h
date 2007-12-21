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

#ifndef CHIDEMODULECHOOSERDIALOG_H
#define CHIDEMODULECHOOSERDIALOG_H

#include "backend/drivers/cswordmoduleinfo.h"

#include "frontend/cmodulechooserdialog.h"

class QTreeWidgetItem;

class BTModuleTreeItem;



class CHideModuleChooserDialog : public CModuleChooserDialog
{
	Q_OBJECT
public:
	CHideModuleChooserDialog(QWidget* parent, QString title, QString label, QString currentModule);
	~CHideModuleChooserDialog() {}

protected:
	virtual void initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem);
protected slots:
	void applyHiddenModules(ListCSwordModuleInfo hiddenModules);
		

private:
	QString m_currentModule;
	QTreeWidgetItem* m_focusItem;
};



#endif
