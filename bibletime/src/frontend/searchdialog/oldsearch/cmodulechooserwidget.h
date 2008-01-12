//
// C++ Interface: cmodulechooserwidget
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CMODULECHOOSERWIDGET_H
#define CMODULECHOOSERWIDGET_H

class CModuleChooserWidget : public QTreeWidget, public CPointers {
	Q_OBJECT
public:
	
	CModuleChooserWidget(QWidget* parent);
	~CModuleChooserWidget();
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

#endif
