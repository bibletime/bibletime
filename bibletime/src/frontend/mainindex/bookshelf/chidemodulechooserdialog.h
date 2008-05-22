/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CHIDEMODULECHOOSERDIALOG_H
#define CHIDEMODULECHOOSERDIALOG_H

class CSwordModuleInfo;

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
	void applyHiddenModules(QList<CSwordModuleInfo*> hiddenModules);
		

private:
	QString m_currentModule;
	QTreeWidgetItem* m_focusItem;
};



#endif
