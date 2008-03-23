/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef BTINDEXPAGE_H
#define BTINDEXPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "backend/btmoduletreeitem.h"

class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;

/**
* This class encapsulates the "Manage search indices" page of the Bookshelf
* Manager.  It allows for creation and deletion of search indicies for each
* installed module.  It also allows for deletion of orphaned indices.
*/
class BtIndexPage : public BtConfigPage
{
	Q_OBJECT
	
public:
	/**
	* Constructor
	*/
	BtIndexPage();

	/** 
	* Destructor
	*/
	~BtIndexPage();

	// BtConfigPage methods
	QString header();
	QString iconName();
	QString label();

public slots:
	void slotSwordSetupChanged();

protected:

	/**
	* Populates the module list with installed modules and orphaned indices
	*/
	void populateModuleList();


	
public slots:
	/**
	* Creates indices for selected modules if no index currently exists
	*/
	void createIndices();
	/**
	* Deletes indices for selected modules
	*/
	void deleteIndices();

public:
	/**
	* Deletes orphaned indices if the autoDeleteOrphanedIndices is true
	* Always deletes indices of existing modules where hasIndex() returns false
	*/
	static void deleteOrphanedIndices();

private:

	QCheckBox *m_autoDeleteOrphanedIndicesBox;
	QTreeWidget *m_moduleList;
	QPushButton *m_deleteButton;
	QPushButton *m_createButton;

	QTreeWidgetItem* m_modsWithIndices;
	QTreeWidgetItem* m_modsWithoutIndices;
};


#endif
