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

#ifndef CMODULECHOOSERDIALOG_H
#define CMODULECHOOSERDIALOG_H

//#include "ui_modulechooser.h" // uic generated

#include "backend/drivers/cswordmoduleinfo.h"
#include "util/cpointers.h"


#include <QDialog>


class QTreeWidget;
class QDialogButtonBox;


namespace Search { namespace Options {

class CModuleChooserDialog : public QDialog, CPointers
{
	Q_OBJECT
public:
	CModuleChooserDialog(QWidget* parentDialog, ListCSwordModuleInfo modules);
	~CModuleChooserDialog();

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
	QTreeWidget *m_moduleChooser;
	QDialogButtonBox *m_buttonBox;
};

}} //end of namespaces Options and Search

#endif
