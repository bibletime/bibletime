/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBOOKKEYCHOOSER_H
#define CBOOKKEYCHOOSER_H

//BibleTime includes
#include "ckeychooser.h"
#include "ckeychooserwidget.h"

//Sword includes

//Qt includes
#include <qwidget.h>
#include <qsize.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qstringlist.h>

class CSwordKey;
class CSwordBookModuleInfo;
class CSwordTreeKey;

namespace sword {
	class TreeKeyIdx;
}

class QHBoxLayout;

/** The keychooser implementation for books.
  * @author The BibleTime team
  */
class CBookKeyChooser : public CKeyChooser  {
	Q_OBJECT
public:
	CBookKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key=0, QWidget *parent=0, const char *name=0);
	~CBookKeyChooser();
	/**
	* Refreshes the content.
	*/
	virtual void refreshContent();
	/**
	* Sets another module to this keychooser
	*/
	virtual void setModules(const ListCSwordModuleInfo& modules, const bool refresh = false);
	/**
	* Returns the key of this kechooser.
	*/
	virtual CSwordKey* const key();
	/**
	* Sets a new key to this keychooser
	*/
	virtual void setKey(CSwordKey*);
	/**
	* Sets a new key to this keychooser
	*/
	void setKey(CSwordKey*, const bool emitSignal);


public slots: // Public slots
	/**
	* Updates the keychoosers for the given key but emit no signal.
	*/
	void updateKey(CSwordKey*);

protected: // Protected methods
	/**
	* Fills the combo given by depth with the items from the key having depth "depth".
	* The parent sibling is given by key.
	*/
	void setupCombo(const QString key, const int depth, const int currentItem);
	/** No descriptions */
	virtual void adjustFont();

protected slots:
	/**
	* A keychooser changed. Update and emit a signal if necessary.
	*/
	void keyChooserChanged(int);
	//is called when a keychooser widget wants to jump to the next/previous key
	//  void nextEntry();
	//  void previousEntry();

private:
	QPtrList<CKeyChooserWidget> m_chooserWidgets;
	QPtrList<CSwordBookModuleInfo> m_modules;
	CSwordTreeKey *m_key;
	QHBoxLayout* m_layout;
};

#endif
