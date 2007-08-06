//
// C++ Interface: cindexoldbookmarksfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXOLDBOOKMARKSFOLDER_H
#define CINDEXOLDBOOKMARKSFOLDER_H

#include "cindexbookmarkfolder.h"

#include <QtXml/qdom.h>
#include <QString>


class CIndexTreeFolder;



class CIndexOldBookmarksFolder : public CIndexBookmarkFolder {
public:
		CIndexOldBookmarksFolder(CIndexTreeFolder* item);
		virtual ~CIndexOldBookmarksFolder();
		virtual void initTree();
		/**
		* Returns the XML code which represents the content of this folder.
		*/
		virtual QDomElement saveToXML( QDomDocument& document );
		/**
		* Loads the content of this folder from the XML code passed as argument to this function.
		*/
		virtual void loadFromXML( QDomElement& element );
	};

class COldBookmarkImport {
public:
	/**
	* This function converts the old config based bookmarks into a valid 1.3 XML file, so importing is easy
	*/
	static const QString oldBookmarksXML( const QString& configFileName = QString::null );
private:
	// made provate because we offer one static functions which doesn't need constructor and destructor
	COldBookmarkImport();
	virtual ~COldBookmarkImport();
};

#endif
