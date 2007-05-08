/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHRESULTSVIEW_H
#define CSEARCHRESULTSVIEW_H

//BibleTime includes
#include "backend/cswordmoduleinfo.h"

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

class CSearchResultView  : public KListView {
	Q_OBJECT
public:
	CSearchResultView(QWidget* parent, const char* name = 0);
	virtual ~CSearchResultView();
	/** Returns the module which is currently used. */
	CSwordModuleInfo* const module();

protected: // Protected methods
	/**
	* Initializes the view of this widget.
	*/
	void initView();
	void initConnections();
	virtual QDragObject* dragObject();

public slots: // Public slots
	void saveItems();
	/**
	* Setups the list with the given module.
	*/
	void setupTree(CSwordModuleInfo*);
   void setupStrongsTree(CSwordModuleInfo*, QStringList*);
	void copyItemsWithText();
	void copyItems();
	void saveItemsWithText();
	/**
	* Reimplementation to show the popup menu.
	*/
	virtual void showPopup(KListView*, QListViewItem* i, const QPoint& point);

protected slots: // Protected slots
	void printItems();
	/**
	* Is connected to the signal executed, which is emitted when a mew item was chosen.
	*/
	void executed(QListViewItem*);

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
	}
	m_actions;
	
	KPopupMenu* m_popup;
	CSwordModuleInfo* m_module;

signals: // Signals
	void keySelected(const QString&);
};

	} //end of namespace Result
} //end of namespace Search

#endif

