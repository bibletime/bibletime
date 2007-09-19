/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHRESULTSVIEW_H
#define CSEARCHRESULTSVIEW_H

//BibleTime includes
#include "backend/drivers/cswordmoduleinfo.h"

#include <QLabel>
#include <QTreeWidget>



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
//class KPopupMenu;
class QMenu;

class CReadDisplay;

//to be removed
//class Q3ListViewItem;
//class Q3DragObject;

namespace Search {
	namespace Result {

class CSearchResultView  : public QTreeWidget {
	Q_OBJECT
public:
	CSearchResultView(QWidget* parent);
	virtual ~CSearchResultView();
	/** Returns the module which is currently used. */
	CSwordModuleInfo* const module();

protected: // Protected methods
	/**
	* Initializes the view of this widget.
	*/
	void initView();
	void initConnections();
	//not in qt4:
	//virtual Q3DragObject* dragObject();

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
	virtual void showPopup(QTreeWidget*, int* i, const QPoint& point);

protected slots: // Protected slots
	void printItems();
	/**
	* Is connected to the signal executed, which is emitted when a new item was chosen.
	*/
	void executed(QTreeWidgetItem*);

private:
	//TODO: convert KActionMenu to QMenu?
	struct {
		QMenu* saveMenu;
		struct {
			QAction* result;
			QAction* resultWithText;
		}
		save;

		QMenu* printMenu;
		struct {
			QAction* result;
		}
		print;

		QMenu* copyMenu;
		struct {
			QAction* result;
			QAction* resultWithText;
		}
		copy;
	}
	m_actions;
	
	QMenu* m_popup;
	CSwordModuleInfo* m_module;

signals: // Signals
	void keySelected(const QString&);
};

	} //end of namespace Result
} //end of namespace Search

#endif

