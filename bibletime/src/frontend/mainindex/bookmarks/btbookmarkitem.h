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

#include <QString>

class BtBookmarkFolder;
class CSwordModuleInfo;

class BtBookmarkItem : public BtBookmarkItemBase
{
public:
	friend class BtBookmarkLoader;
	BtBookmarkItem(QTreeWidgetItem* parent);
	BtBookmarkItem(BtBookmarkItemBase* sibling, BtBookmarkItemBase::Location location, CSwordModuleInfo* module, QString key, QString& description);
	BtBookmarkItem(BtBookmarkFolder* parent, CSwordModuleInfo* module, QString key, QString& description);
	BtBookmarkItem(const BtBookmarkItem& other);
	~BtBookmarkItem() {}

	/** Returns the used module, 0 if there is no such module. */
	CSwordModuleInfo* module();

	/** Returns the used key. */
	QString key();

	/** Returns the used description. */
	const QString& description();
	/** Sets the description text for this bookmark. */
	virtual void setDescription(QString text);

	virtual QString toolTip();

	virtual bool enableAction(MenuAction action);
	/**
	* Changes this bookmark.
	*/
	virtual void rename();

	void update();

private:
	/** Returns the english key.*/
	QString englishKey() const;

	QString m_key;
	QString m_description;
	QString m_moduleName;



};

#endif
