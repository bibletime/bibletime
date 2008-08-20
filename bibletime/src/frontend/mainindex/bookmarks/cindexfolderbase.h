/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

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
	CIndexFolderBase(CBookmarkIndex* mainIndex, const Type type);
	CIndexFolderBase(CIndexFolderBase* parentFolder, const Type type);
	CIndexFolderBase(CIndexFolderBase* parentFolder, const QString& caption);
	virtual ~CIndexFolderBase();

	virtual const bool isFolder();

	virtual void update();
	virtual void init();
	virtual void setExpanded( bool open );

	virtual void newSubFolder();
	virtual void rename();
	/**
	* Reimplemented from cindexitembase.
	*/
	virtual int getDirectChildCount();

	virtual QList<QTreeWidgetItem*> getChildList();

protected:
	/**
	* Reimplementation. Returns true if the auto opening of this folder is allowd
	*/
	virtual const bool allowAutoOpen(const QMimeData* data) const;
	/**
	* Reimplementation. Returns false because folders have no use for drops
	* (except for the bookmark folders) 
	*/
	bool acceptDrop(QDropEvent* e) const;
};

#endif
