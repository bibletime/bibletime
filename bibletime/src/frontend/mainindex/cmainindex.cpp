/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cmainindex.h"
#include "cmainindex.moc"



CMainIndex::CMainIndex(QWidget *parent)
	: QTabWidget(parent)
{
	m_bookmarksPage = new CBookmarksIndex(0);
	m_bookshelfPage = new CBookshelfIndex(0);
	addTab(m_bookshelfPage);
	addTab(m_bookmarksPage);


	initView();
	initConnections();
}

CMainIndex::~CMainIndex() {}



/** Initializes the view. */
void CMainIndex::initView()
{	

}

/** Initialize the SIGNAL<->SLOT connections */
void CMainIndex::initConnections()
{
	qDebug("CMainIndex::initConnections");

}
