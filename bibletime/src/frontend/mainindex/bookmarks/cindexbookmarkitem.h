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


/**
* The class representing the actual bookmark item which points to some module and key.
*/
class CIndexBookmarkItem : public CIndexItemBase
{
public:
	CIndexBookmarkItem(CIndexFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description);
	CIndexBookmarkItem(CIndexFolderBase* parentItem, QDomElement& xml);
	virtual ~CIndexBookmarkItem();

	/** Returns the used module, 0 if there is no such module. */
	CSwordModuleInfo* const module();

	/** Returns the used key. */
	const QString key();

	/** Returns the used description. */
	const QString& description();

	/** Reimplementation from CIndexItemBase. */
	virtual const QString toolTip();

	/** Sets up some data for this item. */
	virtual void update();

	virtual void init();

	/** Returns true because this subclass of CIndexItemBase can be moved around.*/
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
	virtual void rename();

	/** Sets the description text for this bookmark. */
	virtual void setDescription(QString text);

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
	* because bookmarks 
	* have no possible drops.
	*/
	virtual bool acceptDrop(const QMimeSource * src) const;

private: // Private methods

	/** Returns the english key.*/
	const QString& englishKey() const;
};

#endif