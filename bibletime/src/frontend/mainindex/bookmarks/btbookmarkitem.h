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


#include "btbookmarkitembase.h"

class BtBookmarkItem : public BtBookmarkItemBase
{
public:
	friend class BtBookmarkLoader;
	BtBookmarkItem(QTreeWidgetItem* parent);
	~BtBookmarkItem() {}

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

	virtual bool enableAction(const MenuAction action);
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



};

#endif
