/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "ckeychooser.h"
#include "ckeychooser.moc"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"

#include "clexiconkeychooser.h"
#include "cbiblekeychooser.h"
#include "cbookkeychooser.h"

#include <QAction>

CKeyChooser::CKeyChooser(ListCSwordModuleInfo, CSwordKey *, QWidget *parent)
: QWidget(parent),
m_inHistoryFunction(false) {}

CKeyChooser::~CKeyChooser() {}

CKeyChooser* CKeyChooser::createInstance(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent) {
	if (!modules.count()) {
		return 0;
	}

	CKeyChooser* ck = 0;
	switch ( modules.first()->type() ) {
		case CSwordModuleInfo::Commentary:  //Bibles and commentaries uise the same key chooser
		case CSwordModuleInfo::Bible:
		ck = new CBibleKeyChooser(modules,key,parent);
		break;
		case CSwordModuleInfo::Lexicon:
		ck = new CLexiconKeyChooser(modules,key,parent);
		break;
		case CSwordModuleInfo::GenericBook:
		ck = new CBookKeyChooser(modules,key,parent);
		break;
		default:
		return 0;
	}
	return ck;
}

void CKeyChooser::backInHistory() {
	backInHistory(0);
}

void CKeyChooser::backInHistory(QAction* action) {
	m_inHistoryFunction = true;
	//  qWarning("go back %d items in history", count);

	Q_ASSERT(m_prevKeyHistoryList.size());

	//find the key of the action in the list
	int index;
	if (action) {
		index = m_prevKeyHistoryList.indexOf(action->text());
	}
	else {
		index = 1;
	}
	QStringList::iterator it = m_prevKeyHistoryList.begin();
	//pop_front count items, the top item is then the new current key
	while ((index > 0) && (it != m_prevKeyHistoryList.end())) {
		//    qWarning("pop_front");

		m_nextKeyHistoryList.prepend(*it);
		it = m_prevKeyHistoryList.erase(it);
		--index;
	}

	//the first item is now the item which should be set as key
	if (it != m_nextKeyHistoryList.end() && key()) {
		CSwordKey* k = key();
		k->key(*it);
		setKey(k);
	}

	m_inHistoryFunction = false;
}

void CKeyChooser::forwardInHistory() {
	forwardInHistory(0);
}

void CKeyChooser::forwardInHistory(QAction* action) {
	m_inHistoryFunction = true;
	//  qWarning("go forward %d items in history", count);
	Q_ASSERT(m_nextKeyHistoryList.size());

	//find the key of the action in the list
	int index;
	if (action) {
		index = m_nextKeyHistoryList.indexOf(action->text());
	}
	else {
		index = 1;
	}
	QStringList::iterator it = m_nextKeyHistoryList.begin();
	//pop_front count-1 items, the top item is then the new current key
	while (index > 0 && it != m_nextKeyHistoryList.end()) {
		//    qWarning("pop_front");

		m_prevKeyHistoryList.prepend(*it);
		it = m_nextKeyHistoryList.erase(it);
		--index;
	}

	//the first item of the back list is now the new key
	it = m_prevKeyHistoryList.begin();
	if (it != m_prevKeyHistoryList.end() && key()) {
		CSwordKey* k = key();
		k->key(*it);
		setKey(k);
	}

	m_inHistoryFunction = false;
}

void CKeyChooser::addToHistory(CSwordKey* k) {
	//  qWarning("addToHistory");

	Q_ASSERT(!m_inHistoryFunction);
	if (k && !m_inHistoryFunction) {
		Q_ASSERT(k->key() == key()->key());
		m_prevKeyHistoryList.prepend(k->key());
	}

	emit historyChanged();
}

const QStringList CKeyChooser::getPreviousKeys() const {
	QStringList ret = m_prevKeyHistoryList;
	if (ret.size() >= 1) {
	ret.pop_front(); //the first item always is equal to the current key
	}

	return ret;
}

const QStringList CKeyChooser::getNextKeys() const {
	return m_nextKeyHistoryList;
}


/*!
    \fn CKeyChooser::polish()
 */
void CKeyChooser::polish() {
	//Changed to ensurePolished() in qt4, just to be cautious I save this
	ensurePolished();
}
void CKeyChooser::ensurePolished() {
	QWidget::ensurePolished();
	//connect the history calls just before we show, we want an empty history
	connect(this, SIGNAL(keyChanged(CSwordKey*)), SLOT(addToHistory(CSwordKey*)));
}
