/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CLEXICONKEYCHOOSER_H
#define CLEXICONKEYCHOOSER_H

#include <qwidget.h>
#include "ckeychooser.h"
#include "backend/cswordldkey.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordlexiconmoduleinfo.h"

class CKeyChooserWidget;
class QHBoxLayout;

/**
 * This class implements the KeyChooser for lexicons
 *
 * it inhertits @ref CKeyChooser
 * it uses 1 @ref CKeyChooserWidget to represent the lexicon keys
 *
  * @author The BibleTime team
  */
class CLexiconKeyChooser : public CKeyChooser  {
	Q_OBJECT
public:
	/**
	* The constructor
	*
	* you should not need to use this, use @ref CKeyChooser::createInstance instead
	*/
	CLexiconKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key=0, QWidget *parent=0, const char *name=0);

public slots:
	/**
	* see @ref CKeyChooser::getKey
	* @return Return the key object we use.
	*/
	virtual CSwordKey* const key();
	/**
	* see @ref CKeyChooser::setKey
	*/
	virtual void setKey(CSwordKey* key);
	/**
	* used to react to changes in the @ref CKeyChooserWidget
	*
	* @param index not used
	**/
	virtual void activated(int index);
	/**
	* Reimplementatuion.
	*/
	virtual void refreshContent();
	/**
	* Sets the module and refreshes the combo boxes of this keychooser.
	*/
	virtual void setModules( const ListCSwordModuleInfo& modules, const bool refresh = true );

protected:
	CKeyChooserWidget *m_widget;
	CSwordLDKey* m_key;
	QPtrList<CSwordLexiconModuleInfo> m_modules;
	QHBoxLayout *m_layout;

	virtual void adjustFont();

public slots: // Public slots
	virtual void updateKey(CSwordKey* key);
};

#endif
