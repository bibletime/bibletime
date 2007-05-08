/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHDIALOGPAGES_H
#define CSEARCHDIALOGPAGES_H

//BibleTime includes
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"
#include "backend/cswordmodulesearch.h"

#include "frontend/searchdialog/searchoptionsform.h" // uic generated
#include "frontend/searchdialog/searchresultsform.h" // uic generated

//Qt includes
#include <qwidget.h>
#include <qvgroupbox.h>

//KDE includes
#include <klistview.h>

//forward declarations
class QLabel;
class QCheckBox;
class QPushButton;
class QRadioButton;

class KComboBox;
class KActionMenu;
class KAction;
class KHistoryCombo;
class KProgress;
class KPopupMenu;

class CReadDisplay;

namespace Search {
	namespace Result {
		
/**
* This class is used to keep track of the text strongs results.
* It only keeps track of one instance of a strongs text result.
*
*    The functions of the class are:
*       - Store an instance of a strongs text result.
*       - Each strongs text result will contain a list of verses (keyNames).
*       - The number of verses (keyNames) is returned by keyCount().
*       - The text for the strongs text result is returned by keyText().
*       - The list of verses (keyNames) is returned by getKeyList() [as QStringList].
*
*   To add a new verse to a strongs text result use addKeyName.
*/
class StrongsResult {
public:
   StrongsResult() /*: text(QString::null)*/ {
	   	//keyNameList.clear();
   }
   
   StrongsResult(const QString& text, const QString &keyName)
	   : text(text)
    {
		//keyNameList.clear();
		keyNameList.append(keyName);
	}

	QString keyText() const {
		return text;
	}
	int keyCount() const {
		return keyNameList.count();
	}
	void addKeyName(const QString& keyName) {
		if (keyNameList.findIndex(keyName) == -1)
			keyNameList.append(keyName);
	}
	QStringList* getKeyList() {
		return & keyNameList;
	}

	/* ????
	bool operator==(const StrongsResult &l, const StrongsResult &r)
		{ return (l.keyText() == r.keyText()); }

	bool operator<(const StrongsResult &l, const StrongsResult &r)
		{ return (l->keyText() < r->keyText()); }

	bool operator>(const StrongsResult &l, const StrongsResult &r)
		{ return (l->keyText() > r->keyText()); }
	*/
private:
	QString text;
	QStringList keyNameList;
};

typedef QValueList<StrongsResult> StrongsResultList;

/**
* This class is used to keep track of the text strongs results.
* It keeps track of all instances of all strongs text results.
* This class makes use of the above class StrongsResult.
*
*    The functions of the class are:
*       - Store an instance of a strongs text result.
*       - Each strongs text result will contain a list of verses (keyNames).
*       - The number of verses (keyNames) is returned by keyCount().
*       - The text for the strongs text result is returned by keyText().
*       - The list of verses (keyNames) is returned by getKeyList() [as QStringList].
*
*   To add a new verse to a strongs text result use addKeyName.
*/
class StrongsResultClass {
public:
	StrongsResultClass(CSwordModuleInfo* module, const QString& strongsNumber)
		: srModule(module), lemmaText(strongsNumber)
	{
		initStrongsResults();
	}

	QString keyText(int index) const {
		return srList[index].keyText();
	}
	int keyCount(int index) const {
		return srList[index].keyCount();
	}
	QStringList* getKeyList(int index) {
		return srList[index].getKeyList();
	}
	int Count() const {
		return srList.count();
	}

private:
	void initStrongsResults(void);
	QString getStrongsNumberText(const QString& verseContent, int *startIndex);

	StrongsResultList srList;
	CSwordModuleInfo* srModule;
	QString lemmaText;
};

/** The page of the search dialog which contains the search result part.
  * @author The BibleTime team
  */
class CSearchResultPage : public SearchResultsForm {
	Q_OBJECT
public:
	CSearchResultPage(QWidget *parent=0, const char *name=0);
	~CSearchResultPage();
	/**
	* Sets the modules which contain the result of each.
	*/
	void setSearchResult(ListCSwordModuleInfo modules);
	
	QSize sizeHint() const { return baseSize(); }
	QSize minimumSizeHint() const { return minimumSize(); }

public slots: // Public slots
	/**
	* Resets the current l�st of modules and the displayed list of found entries.
	*/
	void reset();

protected: // Protected methods
	/**
	* Initializes the view of this widget.
	*/
	void initView();
	/**
	* Initializes the signal slot conections of the child widgets
	*/
	void initConnections();
	/**
	* This function breakes the queryString into clucene tokens
	*/	
	QStringList QueryParser(const QString& queryString);
	/**
	* This function highlights the searched text in the content using the search type given by search flags
	*/
	const QString highlightSearchedText(const QString& content, const QString& searchedText/*, const int searchFlags*/);

private:
	CReadDisplay* m_previewDisplay;
	ListCSwordModuleInfo m_modules;

protected slots: // Protected slots
	/**
	* Update the preview of the selected key.
	*/
	void updatePreview(const QString& key);
	/**
	* Shows a dialog with the search analysis of the current search.
	*/
	void showAnalysis();

};
	} //end of namespace Search::Result
	
	namespace Options {
		
class CSearchOptionsPage : public SearchOptionsForm  {
	Q_OBJECT
public:
	CSearchOptionsPage(QWidget *parent=0, const char *name=0);
	~CSearchOptionsPage();
	/**
	* Sets the search text used in the page.
	*/
	void setSearchText(const QString& text);
	/**
	* Returns the search text set in this page.
	*/
	const QString searchText();
	/**
	* Returns the list of used modules.
	*/
	const ListCSwordModuleInfo modules();
	/**
	* Return the selected search type,.
	*/
//	const int searchFlags();
	/**
	* Sets all options back to the default.
	*/
	void reset();
	/**
	* Returns the selected search scope if a search scope was selected.
	*/
	sword::ListKey searchScope();
	/**
	* Returns the selected scope type.
	*/
	//const CSwordModuleSearch::scopeType scopeType();

	QSize sizeHint() const { return baseSize(); }
	QSize minimumSizeHint() const { return minimumSize(); }
    bool hasSearchScope();

private:
	ListCSwordModuleInfo m_modules;
	
protected: // Protected methods
	/**
	* Initializes this page.
	*/
	void initView();
	/**
	* Reads the settings of the last searchdialog session.
	*/
	void readSettings();
	/**
	* Reads the settings for the searchdialog from disk.
	*/
	void saveSettings();

public slots: // Public slots
	/**
	* Sets the modules used by the search.
	*/
	void setModules( ListCSwordModuleInfo modules );
	/**
	* Reimplementation.
	*/
	void aboutToShow();
	/**
	* Refreshes the list of ranges and the range combobox. 
	*/
	void refreshRanges();
	/**
	 * Opens the modules chooser dialog.
	 */
	void chooseModules();

protected slots: // Protected slots
	void setupRanges();
	void syntaxHelp();

signals:
	void sigSetSearchButtonStatus(bool);
};

	} //end of namespace Search::Options
} //end of namespace Search

#endif
