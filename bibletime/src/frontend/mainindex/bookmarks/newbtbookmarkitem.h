/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKITEM_H
#define BTBOOKMARKITEM_H

#include "backend/drivers/cswordmoduleinfo.h"

#include <QString>
#include <QTreeWidgetItem>
#include <QDomElement>
#include <QDomDocument>

class NewBtBookmarkItem : public QTreeWidgetItem
{
public:

	enum MenuAction {
		NewFolder = 0,
		ChangeFolder,

		ChangeBookmark,
		ImportBookmarks,
		ExportBookmarks,
		PrintBookmarks,

		DeleteEntries,

		ActionBegin = NewFolder,
		ActionEnd = DeleteEntries
	};

	/** Add this item to an XML document */
	virtual QDomElement saveToXml( QDomDocument& document );
	/** Take the data for this item from an XML element */
	virtual void loadFromXml( QDomElement& element );

	virtual const QString toolTip();
	//virtual void init(); init in constructor
	virtual const bool enableAction(const MenuAction action);

	virtual void rename();

};


class NewBtBookmarkProper : public NewBtBookmarkItem
{
public:

	/** Returns the used module, 0 if there is no such module. */
	CSwordModuleInfo* const module();

	/** Returns the used key. */
	const QString key();

	/** Returns the used description. */
	const QString& description();
	/** Sets the description text for this bookmark. */
	virtual void setDescription(QString text);

	//virtual QDomElement saveToXml( QDomDocument& document );
	//virtual void loadFromXml( QDomElement& element );

	virtual const QString toolTip();

	virtual const bool enableAction(const MenuAction action);
	/**
	* Changes this bookmark.
	*/
	virtual void rename();

private:
	/** Returns the english key.*/
	const QString& englishKey() const;

	QString m_key;
	QString m_description;
	QString m_moduleName;

	QDomElement m_startupXML;

};

class NewBtBookmarkFolder : public NewBtBookmarkItem
{
public:
	virtual const bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();
	
private:

};

#endif
