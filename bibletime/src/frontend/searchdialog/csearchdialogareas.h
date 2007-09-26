/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHDIALOGAREAS_H
#define CSEARCHDIALOGAREAS_H

//BibleTime includes
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "backend/cswordmodulesearch.h"

//#include "ui_searchoptionsform.h" // uic generated
#include "ui_searchresultsform.h" // uic generated

//Qt includes
#include <QList>
#include <QStringList>
#include <QWidget>
#include <QSize>

//forward declarations
class CReadDisplay;

class QHBoxLayout;
class QGroupBox;
class QGridLayout;
class QPushButton;
class KComboBox;
class KHistoryComboBox;
class QLabel;

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
   StrongsResult();
   StrongsResult(const QString& text, const QString &keyName);

	QString keyText() const;
	int keyCount() const;
	void addKeyName(const QString& keyName);
	QStringList* getKeyList();

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

typedef QList<StrongsResult> StrongsResultList;

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
class CSearchResultArea : public QWidget, Ui::SearchResultsForm {
	Q_OBJECT
public:
	CSearchResultArea(QWidget *parent=0);
	~CSearchResultArea();
	/**
	* Sets the modules which contain the result of each.
	*/
	void setSearchResult(ListCSwordModuleInfo modules);
	
	QSize sizeHint() const { return baseSize(); }
	QSize minimumSizeHint() const { return minimumSize(); }

public slots: // Public slots
	/**
	* Resets the current list of modules and the displayed list of found entries.
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
		
class CSearchOptionsArea : public QWidget {
	Q_OBJECT
public:
	CSearchOptionsArea(QWidget *parent=0);
	~CSearchOptionsArea();
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

private:
	ListCSwordModuleInfo m_modules;

	QHBoxLayout *hboxLayout;
    QGroupBox *searchGroupBox;
    QGridLayout *gridLayout;
    QLabel *m_searchTextLabel;
    QPushButton *m_syntaxButton;
    QPushButton *m_chooseModulesButton;
    QPushButton *m_chooseRangeButton;
    QLabel *m_searchScopeLabel;
    KComboBox *m_rangeChooserCombo;
    KHistoryComboBox *m_searchTextCombo;
    QLabel *m_modulesLabel;

};

	} //end of namespace Search::Options
} //end of namespace Search

#endif
