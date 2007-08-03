//
// C++ Interface: cindexglossaryfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CINDEXGLOSSARYFOLDER_H
#define CINDEXGLOSSARYFOLDER_H



#include "cindextreefolder.h"

#include <QString>

class CMainIndex;
class CIndexFolderBase;

class CIndexGlossaryFolder : public CIndexTreeFolder {
public:
	CIndexGlossaryFolder(CMainIndex* mainIndex, const Type type, const QString& fromLanguage, const QString& toLanguage );
	CIndexGlossaryFolder(CIndexFolderBase* parentFolder, const Type type, const QString& fromLanguage, const QString& toLanguage );
	virtual ~CIndexGlossaryFolder();

	virtual void initTree();
	virtual void init();
	virtual void addGroup(const Type /*type*/, const QString& /*fromLanguage*/);
	; //standard reimpl to overload the old one right
	virtual void addGroup(const Type type, const QString& fromLanguage, const QString& toLanguage);
	/**
	* Returns the language this glossary folder maps from.
	*/
	const QString& fromLanguage() const;
	/**
	* Returns the language this glossary folder maps from.
	*/
	const QString& toLanguage() const;

private:
	QString m_fromLanguage;
	QString m_toLanguage;
};


#endif
