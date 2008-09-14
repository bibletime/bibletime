/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMAININDEX_H
#define CMAININDEX_H

#include "frontend/displaywindow/cdisplaywindow.h"

#include <QTabWidget>
#include <QFocusEvent>

class CBookmarkIndex;
class CBookshelfIndex;
class CSwordModuleInfo;

/** The class which manages all bookmarks and modules. The modules are put into own, fixed subfolders sorted by language.
  * @author The BibleTime team
  */
class CMainIndex : public QTabWidget {
	Q_OBJECT

public:
	CMainIndex(QWidget *parent);
	virtual ~CMainIndex() {};

	//void reloadSword();

	CBookshelfIndex* bookshelfIndex() {return m_bookshelfPage;}

signals:
	/**
	* Is emitted when a module should be opened,
	*/
	void createReadDisplayWindow( QList<CSwordModuleInfo*>, const QString& );
	/**
	 * Is emitted when a write window should be created.
	 */
	void createWriteDisplayWindow( CSwordModuleInfo*, const QString&, const  CDisplayWindow::WriteWindowType& );

protected:
	/** QWidget method - move focus to the active page widget */
	virtual void focusInEvent(QFocusEvent*) {currentWidget()->setFocus();}

private:

	CBookmarkIndex* m_bookmarksPage;
	CBookshelfIndex* m_bookshelfPage;

};

#endif
