/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHDIALOG_H
#define CSEARCHDIALOG_H

//BibleTime includes
#include "csearchdialogpages.h"
#include "csearchanalysis.h"

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"

#include "util/cpointers.h"

//Sword includes


//Qt includes
#include <qwidget.h>
#include <qstring.h>
#include <qcanvas.h>
#include <qdict.h>
#include <qtooltip.h>

//KDE includes
#include <kdialog.h>
#include <kdialogbase.h>

//forward declarations

class QLineEdit;
class QTextEdit;

namespace Search {
/**
  *@author The BibleTime team
  */
class CSearchDialog : public KDialogBase  {
	Q_OBJECT

public:
	static void openDialog(const ListCSwordModuleInfo modules, const QString& searchText = QString::null, QWidget* parentDialog = 0);

protected:
	friend class Analysis::CSearchAnalysis;
	friend class Result::CSearchResultPage;
	friend class BibleTime;

	/**
	* Only interesting for the class members! Useful to get the searched text etc.
	*/
	static CSearchDialog* const getSearchDialog();

	/**
	* The cinstructor of the dialog. It's protected because you should use the static public function openDialog.
	* The dialog destroys itself if it was closed.
	*/
	CSearchDialog(QWidget *parent);
	~CSearchDialog();

	/**
	 * Initializes this object.
	 */
	void initView();
	/**
	* Starts the search with the given module list and given search text.
	* Doesn't wait for the start button press, starts immediatly
	*/
	void startSearch( const ListCSwordModuleInfo modules, const QString& searchText);
	/**
	* Sets the list of modules for the search.
	*/
	void setModules( const ListCSwordModuleInfo modules );
	/**
	* Returns the list of used modules.
	*/
	const ListCSwordModuleInfo modules();
	/**
	* Returns the search text which is used for the search.
	*/
	void setSearchText( const QString searchText );
	/**
	* Returns the search text which is set currently.
	*/
	const QString searchText();
	/**
	* Returns the used search scope as a list key
	*/
	sword::ListKey searchScope();
	/**
	* Returns true if the search used a scope, otherwise false.
	*/
	//const CSwordModuleSearch::scopeType searchScopeType() const;
	/**
	* Returns they type of search which is set
	*/
// 	const int searchFlags() const;
	/**
	* Resets the parts to the default.
	*/
	void reset();

protected slots:
	/**
	* Starts the search with the set modules and the set search text.
	*/
	void startSearch();
	void searchFinished();
	void showModulesSelector();
	/**
	* Initializes the signal slot connections
	*/
	void initConnections();
	/**
	* Reimplementation.
	*/
	virtual void slotClose();

private:
	Result::CSearchResultPage* m_searchResultPage;
	Options::CSearchOptionsPage* m_searchOptionsPage;
	
	CSwordModuleSearch m_searcher;
};


} //end of namespace Search

#endif
