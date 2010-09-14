/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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


class CSwordKey;
class CSwordBookModuleInfo;
class CSwordTreeKey;

namespace sword {
	class TreeKeyIdx;
}

class QTreeWidget;
class QTreeWidgetItem;

/** The keychooser implementation for books.
  * @author The BibleTime team
  */
class CBookTreeChooser : public CKeyChooser  {
	Q_OBJECT
public:
	CBookTreeChooser(QList<CSwordModuleInfo*> modules, CSwordKey *key=0, QWidget *parent=0);
	~CBookTreeChooser();
	/**
	* Refreshes the content.
	*/
	virtual void refreshContent();
	/**
	* Sets another module to this keychooser
	*/
	virtual void setModules(const QList<CSwordModuleInfo*>& modules, const bool refresh = true);
	/**
	* Returns the key of this keychooser.
	*/
	virtual CSwordKey* key();
	/**
	* Sets a new key to this keychooser
	*/
	virtual void setKey(CSwordKey*);
	void setKey(CSwordKey*, const bool emitSinal);

public slots: // Public slots
	virtual void updateKey( CSwordKey* );
	/**
	* Reimplementation to handle tree creation on show.
	*/
	virtual void show();


protected: // Protected methods
	/**
	* Creates the first level of the tree structure.
	*/
	void setupTree();
	virtual void adjustFont();
	void addKeyChildren(CSwordTreeKey* key, QTreeWidgetItem* item);

protected slots: // Protected slots
	void itemActivated( QTreeWidgetItem* item );
	void setKey(QString& newKey);

private:
	QList<CSwordBookModuleInfo*> m_modules;
	CSwordTreeKey* m_key;
	QTreeWidget* m_treeView;
};

#endif
