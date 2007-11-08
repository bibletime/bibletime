//
// C++ Interface: cindexbookmarkitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXBOOKMARKITEM_H
#define CINDEXBOOKMARKITEM_H


#include "cindexitembase.h"

#include <QtXml/qdom.h>
#include <QString>


class QMimeSource;

class CIndexFolderBase;
class CSwordModuleInfo;



class CIndexBookmarkItem : public CIndexItemBase
{
public:
	CIndexBookmarkItem(CIndexFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description);
	CIndexBookmarkItem(CIndexFolderBase* parentItem, QDomElement& xml);
	virtual ~CIndexBookmarkItem();
	CSwordModuleInfo* const module();
	const QString key();
	const QString& description();
	virtual const QString toolTip();

	//virtual int compare( QListViewItem * i, int col, bool ascending ) const;

	virtual void update();
	virtual void init();
	virtual const bool isMovable();
	/**
	* Reimplementation to handle  the menu entries of the main index.
	*/
	virtual const bool enableAction(const MenuAction action);
	/**
	* Prints this bookmark.
	*/
	//   void print();
	/**
	* Changes this bookmark.
	*/
	void rename();
	/**
	* Reimplementation of CItemBase::saveToXML.
	*/
	virtual QDomElement saveToXML( QDomDocument& document );
	/**
	* Loads the content of this folder from the XML code passed as argument to this function.
	*/
	virtual void loadFromXML( QDomElement& element );

private:
	QString m_key;
	QString m_description;
	QString m_moduleName;

	QDomElement m_startupXML;

protected: // Protected methods
	/**
	* Reimplementation. Returns false everytime
	* because a bookmarks 
	* has not possible drops.
	*/
	virtual bool acceptDrop(const QMimeSource * src) const;

private: // Private methods
	/**
	* Returns the english key.
	* Only used internal of this class implementation.
	*/
	const QString& englishKey() const;
};

#endif