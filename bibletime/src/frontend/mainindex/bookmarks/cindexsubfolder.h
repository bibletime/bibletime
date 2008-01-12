//
// C++ Interface: cindexsubfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CINDEXSUBFOLDER_H
#define CINDEXSUBFOLDER_H

#include "cindexbookmarkfolder.h"


#include <QtXml/qdom.h>

class CIndexFolderBase;


class CIndexSubFolder : public CIndexBookmarkFolder {
public:
		CIndexSubFolder(CIndexFolderBase* parentItem, const QString& caption);
		CIndexSubFolder(CIndexFolderBase* parentItem, QDomElement& xml);
		virtual ~CIndexSubFolder();
		virtual void init();
		/**
		* Reimplementation from  CItemBase.
		*/
		const bool enableAction(const MenuAction action);
		/**
		* Returns the XML code which represents the content of this folder.
		*/
		virtual QDomElement saveToXML( QDomDocument& document );
		/**
		* Loads the content of this folder from the XML code passed as argument to this function.
		*/
		virtual void loadFromXML( QDomElement& element );

private:
		QDomElement m_startupXML;
};

#endif
