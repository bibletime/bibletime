/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

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
		bool enableAction(const MenuAction action);
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
