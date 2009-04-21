/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "cmainindex.h"

#include "backend/drivers/cswordmoduleinfo.h"

#include "bookshelf/cbookshelfindex.h"
#include "bookmarks/cbookmarkindex.h"

#include <QTabWidget>


CMainIndex::CMainIndex(QWidget *parent)	: QTabWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	m_bookmarksPage = new CBookmarkIndex(0);
	m_bookshelfPage = new CBookshelfIndex(0);
	addTab(m_bookshelfPage, tr("Bookshelf"));
	addTab(m_bookmarksPage, tr("Bookmarks"));

	//shortcut some signals from pages to signals of this widget so that outsiders
	// do not have to use the pages directly
	QObject::connect(m_bookshelfPage, SIGNAL(createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& )), this, SIGNAL(createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& )));
	QObject::connect(m_bookmarksPage, SIGNAL(createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& )), this, SIGNAL(createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& )));

	QObject::connect(m_bookshelfPage, SIGNAL(createWriteDisplayWindow( CSwordModuleInfo*, const QString&, const  CDisplayWindow::WriteWindowType& )), this, SIGNAL(createWriteDisplayWindow( CSwordModuleInfo*, const QString&, const  CDisplayWindow::WriteWindowType&)));

}
