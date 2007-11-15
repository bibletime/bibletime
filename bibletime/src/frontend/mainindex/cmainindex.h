/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMAININDEX_H
#define CMAININDEX_H


#include <QTabWidget>

class CBookmarkIndex;
class CBookshelfIndex;

/** The class which manages all bookmarks and modules. The modules are put into own, fixed subfolders sorted by language.
  * @author The BibleTime team
  */
class CMainIndex : public QTabWidget {
	Q_OBJECT

public:
	CMainIndex(QWidget *parent);
	virtual ~CMainIndex() {};

	void reloadSword();

private:

	CBookmarkIndex* m_bookmarksPage;
	CBookshelfIndex* m_bookshelfPage;

};

#endif
