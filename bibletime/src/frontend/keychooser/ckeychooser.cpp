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

#include "backend/keys/cswordkey.h"

#include "clexiconkeychooser.h"
#include "cbiblekeychooser.h"
#include "cbookkeychooser.h"

#include <QAction>
#include <QDebug>

CKeyChooser::CKeyChooser(ListCSwordModuleInfo, CSwordKey *, QWidget *parent)
	: QWidget(parent),
	m_inHistoryFunction(false),
	//m_historyList(),
	//m_historyListIterator(m_historyList)
	m_history()
{}

CKeyChooser::~CKeyChooser() {}

CKeyChooser* CKeyChooser::createInstance(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent) {
	if (!modules.count()) {
		return 0;
	}

	CKeyChooser* ck = 0;
	switch ( modules.first()->type() ) {
		case CSwordModuleInfo::Commentary:  //Bibles and commentaries use the same key chooser
		case CSwordModuleInfo::Bible:
			return new CBibleKeyChooser(modules,key,parent);
			break;
		case CSwordModuleInfo::Lexicon:
			return new CLexiconKeyChooser(modules,key,parent);
		case CSwordModuleInfo::GenericBook:
			return new CBookKeyChooser(modules,key,parent);
		default:
			return 0;
	}
}


// KeyChooserHistoryIterator CKeyChooser::getHistoryIterator() const
// {
// 	qDebug("CKeyChooser::getHistoryIterator");
// 	//Q_ASSERT(m_historyListIterator.hasPrevious());
// 	KeyChooserHistoryIterator constIterator(m_historyList);
// 	KeyChooserHistoryIterator testIt(m_historyList);
// 	if (m_historyListIterator.hasPrevious()) {
// 		qDebug() << "the original iterator has previous: " << m_historyListIterator.peekPrevious()->text();
// 		constIterator.findNext(m_historyListIterator.peekPrevious());
// 		testIt.findNext(m_historyListIterator.peekPrevious());
// 	}
// 	qDebug() << "test iterator forward:";
// 	while (testIt.hasNext()){
// 		qDebug() << testIt.next()->text();
// 	}
// 	qDebug() << "and backwards, from the end: ";
// 	while (testIt.hasPrevious()){
// 		qDebug() << testIt.previous()->text();
// 	}
// 	return constIterator;
// }


BTHistory* CKeyChooser::history()
{
	return &m_history;
}

// void CKeyChooser::moveInHistory(QAction* historyItem)
// {
// 	m_history->move(historyItem);
// }
// 
// void CKeyChooser::backInHistory() {
// 	m_history->back();
// }
// 
// 
// 
// void CKeyChooser::forwardInHistory() {
// 	m_history->fw();
// }
// 
// 
// void CKeyChooser::addToHistory(CSwordKey* newKey, CSwordKey* oldKey) {
// 	m_history->add(newKey);
// }

