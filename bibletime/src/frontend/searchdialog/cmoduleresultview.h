/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMODULERESULTSVIEW_H
#define CMODULERESULTSVIEW_H

//BibleTime includes
#include "backend/drivers/cswordmoduleinfo.h"
#include "csearchdialogareas.h"

//Qt includes
#include <QLabel>
#include <QTreeWidget>


//forward declarations
class QPoint;
class QMenu;
class QAction;
class QStringList;
class QContextMenuEvent;

class StrongsResultClass;


namespace Search {


class CModuleResultView : public QTreeWidget {
	Q_OBJECT
public:
	CModuleResultView(QWidget* parent);
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

   void setupStrongsResults(CSwordModuleInfo* module, QTreeWidgetItem* parent, const QString& searchedText);

protected slots: // Protected slots
	/**
	* Is executed when an item was selected in the list.
	*/
	//replace with QTreeWidget::itemActivated 
	//void executed( Q3ListViewItem* );
	/**
	* Copies the whole search result with the text into the clipboard.
	*/
	void copyResultWithText();
	/**
	* Copies the whole search result into the clipboard.
	*/
	void copyResult();
	/**
	* This event handler (reimplemented from QWidget) opens the popup menu at the given position.
	*/
	void contextMenuEvent( QContextMenuEvent * event );
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

	} m_actions;
	
	QMenu* m_popup;
	
	StrongsResultClass* strongsResults;
};


} //end of namespace Search

#endif
