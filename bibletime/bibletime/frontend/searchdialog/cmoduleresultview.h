/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMODULERESULTSVIEW_H
#define CMODULERESULTSVIEW_H

//BibleTime includes
#include "backend/cswordmoduleinfo.h"
#include "csearchdialogpages.h"

//Qt includes

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
		
class CModuleResultView : public KListView {
	Q_OBJECT
public:
	CModuleResultView(QWidget* parent, const char* name = 0);
	~CModuleResultView();
	
	/**
	* Setups the tree using the given list of modules.
	*/
	void setupTree( ListCSwordModuleInfo modules, const QString& searchedText );
	/**
	* Returns the currently active module.
	*/
	CSwordModuleInfo* const activeModule();

protected: // Protected methods
	/**
	* Initializes this widget.
	*/
	void initView();
	/**
	* Initializes the connections of this widget
	*/
	void initConnections();

   void setupStrongsResults(CSwordModuleInfo* module, QListViewItem* parent, const QString& searchedText);

protected slots: // Protected slots
	/**
	* Is executed when an item was selected in the list.
	*/
	void executed( QListViewItem* );
	/**
	* Copies the whole search result with the text into the clipboard.
	*/
	void copyResultWithText();
	/**
	* Copies the whole search result into the clipboard.
	*/
	void copyResult();
	/**
	* This slot opens the popup menu at the given position
	*/
	void showPopup(KListView*, QListViewItem*, const QPoint&);
	/**
	* Appends the whole search result to the printer queue.
	*/
	void printResult();
	/**
	* Saves the search result with it's text.
	*/
	void saveResultWithText();
	/**
	* Saves the search result keys.
	*/
	void saveResult();

signals:
	void moduleSelected(CSwordModuleInfo*);
	void moduleChanged();
   void strongsSelected(CSwordModuleInfo*, QStringList*);

private:
	struct {
		KActionMenu* saveMenu;
		struct {
			KAction* result;
			KAction* resultWithText;
		}
		save;

		KActionMenu* printMenu;
		struct {
			KAction* result;
		}
		print;

		KActionMenu* copyMenu;
		struct {
			KAction* result;
			KAction* resultWithText;
		}
		copy;

	} m_actions;
	
	KPopupMenu* m_popup;
	StrongsResultClass* strongsResults;
};

	} //end of namespace Search::Result
} //end of namespace Search

#endif
