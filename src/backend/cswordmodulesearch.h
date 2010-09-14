/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSWORDMODULESEARCH_H
#define CSWORDMODULESEARCH_H

//BibleTime - backend
class CSwordModuleInfo;

//BibleTime - utils
#include "util/cpointers.h"

//Qt includes
#include <QObject>
#include <QString>

//Sword includes
#include <listkey.h>

/**
 * CSwordModuleSearch manages the search on Sword modules. It manages the thread(s)
 * and manages the different modules.
  *
  * @author The BibleTime team
  * @version $Id: cswordmodulesearch.h,v 1.34 2006/08/08 19:32:48 joachim Exp $
  */

class CSwordModuleSearch: public QObject, CPointers  {
	Q_OBJECT

public:
	CSwordModuleSearch();
	/**
	* The destructor of this class. It cleans uop memory before it's deleted.
	*/
	virtual ~CSwordModuleSearch();
	/**
	* Sets the text which should be search in the modules.
	*/
	void setSearchedText( const QString& );
	/**
	* Starts the search for the search text.
	*/
	bool startSearch();
	/**
	* This function sets the modules which should be searched.
	*/
	void setModules( const QList<CSwordModuleInfo*>& );
	/**
	* Sets the search scope.
	*/
	void setSearchScope( const sword::ListKey& scope );
	/**
	* Sets the seaech scope back.
	*/
	void resetSearchScope();
	/**
	* @return "true" if in the last search the searcher found items, if no items were found return "false"
	*/
	bool foundItems() const;
	/**
	* Returns a copy of the used search scope.
	*/
	const sword::ListKey& searchScope() const;

	void connectFinished( QObject * receiver, const char * member );
	void searchFinished();

	/**
	* Returns true if all of the specified modules have indices already built.
	*/
	bool modulesHaveIndices( const QList<CSwordModuleInfo*>& );

protected:
	QString m_searchedText;
	sword::ListKey m_searchScope;
	QList<CSwordModuleInfo*> m_moduleList;

	int m_searchOptions;

	bool m_foundItems;

signals:
	void finished();

private:
	static CSwordModuleSearch* searcher;
};

#endif
