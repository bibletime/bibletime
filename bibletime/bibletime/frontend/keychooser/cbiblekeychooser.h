/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBIBLEKEYCHOOSER_H
#define CBIBLEKEYCHOOSER_H

#include <qwidget.h>
#include "ckeychooser.h"

#include "backend/cswordbiblemoduleinfo.h"

class CKeyReferenceWidget;
class CSwordVerseKey;
class CSwordBibleModuleInfo;

/** This class implements the KeyChooser for bibles and commentaries
 *
 * it inhertits @ref CKeyChooser
 *
 * it uses a CKeyReferenceWidget 's to represent the bible keys
 *
  * @author The BibleTime team
  */

class CBibleKeyChooser : public CKeyChooser  {
	Q_OBJECT

public:
	/**
	* the constructor
	* you should not need to use this, use @ref CKeyChooser::createInstance instead
	*/
	CBibleKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key=0, QWidget *parent=0, const char *name=0);

public slots:
	/**
	* see @ref CKeyChooser::getKey
	*/
	CSwordKey* const key();
	/**
	* see @ref CKeyChooser::setKey
	*/
	virtual void setKey(CSwordKey *key);
	/**
	* Sets the module
	*/
	virtual void setModules(const ListCSwordModuleInfo& modules, const bool refresh = true);
	/**
	* used to react to changes
	* @param index not used
	*/
	void refChanged(CSwordVerseKey *key);

	void updateKey(CSwordKey* key);
	void adjustFont();
	void refreshContent();


private:
	CKeyReferenceWidget* w_ref;
	QPtrList<CSwordBibleModuleInfo> m_modules;
	CSwordVerseKey *m_key;
};

#endif
