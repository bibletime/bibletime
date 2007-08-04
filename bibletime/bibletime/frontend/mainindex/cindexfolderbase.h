//
// C++ Interface: cindexfolderbase
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXFOLDERBASE_H
#define CINDEXFOLDERBASE_H


#include "cindexitembase.h"


#include <QList>
#include <QTreeWidgetItem>

class CMainIndex;

class QMimeSource;


class CIndexFolderBase : public CIndexItemBase
{
public:
	CIndexFolderBase(CMainIndex* mainIndex, const Type type);
	CIndexFolderBase(CIndexFolderBase* parentFolder, const Type type);
	CIndexFolderBase(CIndexFolderBase* parentFolder, const QString& caption);
	virtual ~CIndexFolderBase();

	virtual const bool isFolder();

	virtual void update();
	virtual void init();
	virtual void setExpanded( bool open );
	/**
	* The function which renames this folder.
	*/
	void rename();
	virtual void newSubFolder();
	
	/**
	* Reimplemented from cindexitembase.
	*/
	virtual int getDirectChildCount();

	virtual QList<QTreeWidgetItem*> getChildList();

protected:
	/**
	* Reimplementation. Returns true if the auto opening of this folder is allowd
	*/
	virtual const bool allowAutoOpen( const QMimeSource* src ) const;
	/**
	* Reimplementation. Returns false because folders have no use for drops
	* (except for the bookmark folders) 
	*/
	bool acceptDrop(const QMimeSource * src) const;
};

#endif
