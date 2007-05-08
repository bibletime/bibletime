/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CKEYCHOOSER_H
#define CKEYCHOOSER_H

#include <qwidget.h>

#include "backend/cswordmoduleinfo.h"

class CSwordModuleInfo;
class CSwordKey;

/**
 * The base class for the KeyChooser.
 * Do not use directly, create a KeyChooser with
 * @ref #createInstance , this will create the proper one
 * of the classes that inherit from @ref CKeyChooser
 *
  * @author The BibleTime team
  */

class CKeyChooser : public QWidget {
	Q_OBJECT

public:
	/**
	* Creates a proper Instance, either
	*
	@ref CLexiconKeyChooser or
	* @ref CBibleKeyChooser
	* @param info the @ref CModuleInfo to be represented by the KeyChooser
	* @param key if not NULL, the @ref CKey the KeyChooser should be set to
	* @param parent the parent of the widget to create
	*/
	static CKeyChooser* createInstance(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent);

	const QStringList getPreviousKeys() const;
	const QStringList getNextKeys() const;

public slots:
	/**
	* sets the @ref CKey
	* @param key the key which the widget should be set to
	*/
	virtual void setKey(CSwordKey* key) = 0;
	/**
	* sets the @ref CKey
	* @param key the key which the widget should be set to
	*/
	virtual void updateKey(CSwordKey* key) = 0;
	/**
	* gets the current @ref CKey
	*
	* @return the current @ref CKey
	*/
	virtual CSwordKey* const key() = 0;
	/**
	* Sets the module of this keychooser and refreshes the comboboxes
	*/
	virtual void setModules( const ListCSwordModuleInfo& modules, const bool refresh = true ) = 0;
	/**
	* Freshes the content of the different key chooser parts.
	*/
	virtual void refreshContent() = 0;

	void addToHistory(CSwordKey*);

	void backInHistory();
	void backInHistory(int);

	void forwardInHistory();
	void forwardInHistory(int);

signals:
	/**
	* is emitted if the @ref CKey was changed by the user
	*/
	void keyChanged(CSwordKey* key);
	/**
	* Is emitted before the key is changed!
	*/
	void beforeKeyChange(const QString& key);
	void historyChanged();

protected:
	/**
	* the constructor - DO NOT USE! -- use @ref #createInstance instead!
	*/
	CKeyChooser(ListCSwordModuleInfo info, CSwordKey *key=0, QWidget *parent=0, const char *name=0);
	virtual ~CKeyChooser();
	/**
	* Set the appropriate font do display the modules
	*/
	virtual void adjustFont() = 0;

private:
	QStringList m_prevKeyHistoryList;
	QStringList m_nextKeyHistoryList;
	int m_currentKeyHistoryPos;
	bool m_inHistoryFunction;
protected:
	virtual void polish();
};

#endif
