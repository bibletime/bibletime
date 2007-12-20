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

#include "backend/drivers/cswordmoduleinfo.h"
#include "util/cpointers.h"


#include <QDialog>


class QTreeWidget;
class QTreeWidgetItem;
class QDialogButtonBox;

class BTModuleTreeItem;

namespace Search {

class CSearchModuleChooserDialog : public QDialog, CPointers
{
	Q_OBJECT
public:
	CSearchModuleChooserDialog(QWidget* parentDialog, ListCSwordModuleInfo modules);
	~CSearchModuleChooserDialog();

signals:
	void modulesChanged(ListCSwordModuleInfo modules);

protected: // Protected methods
	/**
	* Initializes the view of this dialog
	*/
	void initView();

	/**
	* Initializes the connections of this dialog.
	*/
	void initConnections();

	/**
	* Initialized the module tree
	*/
	void setModules(ListCSwordModuleInfo& modules);

protected slots: // Protected slots
	/**
	* Handle the modules after the dialog is accepted.
	*/
	virtual void slotOk();

private:

	void createModuleTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem, ListCSwordModuleInfo& selected);

	QTreeWidget *m_moduleChooser;
	QDialogButtonBox *m_buttonBox;
};

} //end of namespace Search

#endif
