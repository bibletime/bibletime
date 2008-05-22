/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSEARCHDIALOG_H
#define CSEARCHDIALOG_H

//BibleTime includes
#include "csearchdialogareas.h"
//#include "csearchanalysis.h"

class CSwordModuleInfo;
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"

//Qt includes
#include <QString>

//KDE includes
#include <kdialog.h>

//forward declarations
class QWidget;


namespace Search {

/**
  *@author The BibleTime team
  */
class CSearchDialog : public KDialog  {
	Q_OBJECT

public:
	static void openDialog(const QList<CSwordModuleInfo*> modules, const QString& searchText = QString::null, QWidget* parentDialog = 0);

protected:
	friend class CSearchAnalysisScene;
	friend class CSearchResultArea;
	friend class BibleTime;

	/**
	* Only interesting for the class members! Useful to get the searched text etc.
	*/
	static CSearchDialog* const getSearchDialog();

	/**
	* The constructor of the dialog. It's protected because you should use the static public function openDialog.
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
	* Doesn't wait for the start button press, starts immediately
	*/
	void startSearch( const QList<CSwordModuleInfo*> modules, const QString& searchText);
	/**
	* Sets the list of modules for the search.
	*/
	void setModules( const QList<CSwordModuleInfo*> modules );
	/**
	* Returns the list of used modules.
	*/
	const QList<CSwordModuleInfo*> modules();
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
	* Returns they type of search which is set
	*/
// 	const int searchFlags() const;
	/**
	* Resets the parts to the default.
	*/
	void reset();
	/**
	* Load the settings from the resource file
	*/
	void loadDialogSettings();
	/**
	* Save the settings to the resource file
	*/
	void saveDialogSettings();

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
	CSearchResultArea* m_searchResultArea;
	CSearchOptionsArea* m_searchOptionsArea;
	
	CSwordModuleSearch m_searcher;
};


} //end of namespace Search

#endif
