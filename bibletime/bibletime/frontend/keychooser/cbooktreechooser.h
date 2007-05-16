/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBOOKTREECHOOSER_H
#define CBOOKTREECHOOSER_H


/** The treechooser implementation for books.
  * @author The BibleTime team
  */
//BibleTime includes
#include "ckeychooser.h"
#include "ckeychooserwidget.h"

//Sword includes

//Qt includes
#include <qwidget.h>
#include <qsize.h>
#include <qmap.h>
#include <q3ptrlist.h>
#include <qstringlist.h>

//KDE includes
#include <klistview.h>

class CSwordKey;
class CSwordBookModuleInfo;
class CSwordTreeKey;

namespace sword {
	class TreeKeyIdx;
}

/** The keychooser implementeation for books.
  * @author The BibleTime team
  */
class CBookTreeChooser : public CKeyChooser  {
	Q_OBJECT
public:
	CBookTreeChooser(ListCSwordModuleInfo modules, CSwordKey *key=0, QWidget *parent=0, const char *name=0);
	~CBookTreeChooser();
	/**
	* Refreshes the content.
	*/
	virtual void refreshContent();
	/**
	* Sets another module to this keychooser
	*/
	virtual void setModules(const ListCSwordModuleInfo& modules, const bool refresh = true);
	/**
	* Returns the key of this kechooser.
	*/
	virtual CSwordKey* const key();
	/**
	* Sets a new key to this keychooser
	*/
	virtual void setKey(CSwordKey*);
	void setKey(CSwordKey*, const bool emitSinal);

public slots: // Public slots
	virtual void updateKey( CSwordKey* );
	/**
	* Reimplementationm to handle tree creation on show.
	*/
	virtual void show();


protected: // Protected methods
	/**
	* Creates the first level of the tree structure.
	*/
	void setupTree();
	virtual void adjustFont();

protected slots: // Protected slots
	void itemActivated( Q3ListViewItem* item );

private:
class TreeItem : public KListViewItem {
public:
		TreeItem(Q3ListViewItem* parent, Q3ListViewItem* after, CSwordTreeKey* key, const QString keyName);
		TreeItem(Q3ListViewItem* parent, CSwordTreeKey* key, const QString keyName);
		TreeItem(Q3ListView* view,Q3ListViewItem* after, CSwordTreeKey* key, const QString keyName);
		const QString& key() const;
		void createChilds();
		virtual void setOpen(bool);

protected:
		/**
		* Initializes this item with the correct caption.
		*/
		virtual void setup();
private:
		CSwordTreeKey* m_key;
		QString m_keyName;
	};

	Q3PtrList<CSwordBookModuleInfo> m_modules;
	CSwordTreeKey* m_key;
	KListView* m_treeView;
};

#endif
