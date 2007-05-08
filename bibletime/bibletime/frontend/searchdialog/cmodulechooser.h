/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMODULECHOOSER_H
#define CMODULECHOOSER_H

//BibleTime includes

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"
#include "backend/cswordmodulesearch.h"

#include "util/cpointers.h"

//Sword includes


//Qt includes
#include <qwidget.h>
#include <qstring.h>
#include <qcanvas.h>
#include <qdict.h>
#include <qtooltip.h>

//KDE includes
#include <kdialog.h>
#include <kdialogbase.h>
#include <klistview.h>

namespace Search {
	namespace Options {
	
class CModuleChooser : public KListView, public CPointers {
	Q_OBJECT
public:
	class ModuleCheckBoxItem : virtual public QCheckListItem {
	public:
		ModuleCheckBoxItem(QListViewItem* item, CSwordModuleInfo* module);
		~ModuleCheckBoxItem();
		/**
		* Returns the used module.
		*/
		CSwordModuleInfo* const module() const;
	private:
		CSwordModuleInfo* m_module;
	};

	CModuleChooser(QWidget* parent);
	~CModuleChooser();
	/**
	* Sets the list of modules and updates the state of the checkbox items.
	*/
	void setModules( ListCSwordModuleInfo modules );
	/**
	* Returns a list of selected modules.
	*/
	ListCSwordModuleInfo modules();

public slots:
	/**
	* Reimplemented to open the folders which contain checked module items
	*/
	virtual void show();


protected: // Protected methods
	/**
	* Initializes this widget and the childs of it.
	*/
	void initView();
	/**
	* Initializes the tree of this widget.
	*/
	void initTree();
};

class CModuleChooserDialog : public KDialogBase  {
	Q_OBJECT
public:
	CModuleChooserDialog(QWidget* parentDialog, ListCSwordModuleInfo modules);
	~CModuleChooserDialog();

signals:
	void modulesChanged(ListCSwordModuleInfo modules);

protected: // Protected methods
	/**
	* Initializes the view of this dialog
	*/
	void initView();
	/**
	* Initializes the connections of this dialog.
	*/
	void initConnections();

protected slots: // Protected slots
	/**
	* Reimplementation to handle the modules.
	*/
	virtual void slotOk();

private:
	CModuleChooser* m_moduleChooser;
};

	} //end of namespace Search.Options
} //end of namespace Search

#endif 
