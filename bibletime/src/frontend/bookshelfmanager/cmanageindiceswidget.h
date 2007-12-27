/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMANAGEINDICESWIDGET_H
#define CMANAGEINDICESWIDGET_H

#include "ui_manageindicesform.h"  // uic generated

#include <QWidget>


namespace BookshelfManager {
	
/**
* This class encapsulates the "Manage search indices" page of the Bookshelf
* Manager.  It allows for creation and deletion of search indicies for each
* installed module.  It also allows for deletion of orphaned indices.
*/
class CManageIndicesWidget : public QWidget, Ui_ManageIndicesForm
{
	Q_OBJECT
	
public:
	/**
	* Constructor
	*/
	CManageIndicesWidget(QWidget* parent);

	/** 
	* Destructor
	*/
	~CManageIndicesWidget();

protected:
	/**
	* Initializes the look and feel of this page
	*/
	void initView();
	/**
	* Populates the module list with installed modules and orphaned indices
	*/
	void populateModuleList();

	// member variables
	QTreeWidgetItem* m_modsWithIndices;
	QTreeWidgetItem* m_modsWithoutIndices;
	
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
};

}

#endif
