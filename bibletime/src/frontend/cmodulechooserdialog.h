/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMODULECHOOSERDIALOG_H
#define CMODULECHOOSERDIALOG_H

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/btmoduletreeitem.h"

#include <QDialog>
#include <QList>

class QTreeWidget;
class QTreeWidgetItem;
class QDialogButtonBox;


/**
* Abstract dialog which lets the user select modules with checkboxes.
* The dialog will be destroyed after closing. Connect the modulesChanged() signal
* to handle the selection before the dialog is destroyed.
*/
class CModuleChooserDialog : public QDialog
{
	Q_OBJECT
public:
	
	/**
	* Use your own constructor to set extra members.
	* Filters list is given to the module tree creator, see BTModuleTreeItem.
	* For module list see BTModuleTreeItem constructor documentation.
	* Call init() after the constructor, either in the end of your own constructor or from outside.
	*/
	CModuleChooserDialog(QWidget* parent, QString title, QString label,
		QList<BTModuleTreeItem::Filter*> filters, ListCSwordModuleInfo*  modules = 0);
	
	virtual ~CModuleChooserDialog() {}
	
	/** Call this after/from the constructor.*/
	void init();
	
	/** Set the module tree grouping.
	* Initially it's taken from the CBTConfig so it needs to be set only if that default is not adequate.
	* This must be called before the tree is initialized, i.e. before init().
	*/
	void setGrouping(BTModuleTreeItem::Grouping grouping) {m_grouping = grouping;}

signals:
	
	/** The signal is sent when the OK button is clicked. The list includes the selected (checked) modules. */
	void modulesChanged(ListCSwordModuleInfo*);

protected:

	/**
	* Initialize one tree widget item.
	* To be overridden. This is called for each QTreeWidgetItem when it is created.
	* Here you can set for example the checked status of the item.
	*/
	virtual void initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem) = 0;
	

	
private slots:
	
	/** Emits the signal modulesChanged() with the list of the selected modules. */
	void slotOk();

private:
	/** Initialize the module tree.	*/
	void initTree();

	/** Initializes the view of this dialog.*/
	void initView();

	/** Call this from initTree(). */
	void createModuleTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem);

	QTreeWidget *m_moduleChooser;
	QDialogButtonBox *m_buttonBox;
	QString m_title;
	QString m_labelText;
	QList<BTModuleTreeItem::Filter*> m_filters;
	BTModuleTreeItem::Grouping m_grouping;
	ListCSwordModuleInfo* m_moduleList;
};


#endif
