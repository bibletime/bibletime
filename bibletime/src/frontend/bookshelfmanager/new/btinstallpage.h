/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPAGE_H
#define BTINSTALLPAGE_H


#include "frontend/bookshelfmanager/new/btconfigpage.h"
#include "frontend/cmodulechooserdialog.h"

#include <QString>
#include <QWidget>

class BtSourceWidget;

class QTreeWidgetItem;
class QTreeWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QTabBar;
class QStackedWidget;


/**
* The Install page includes module path chooser, source/module handler and install button.
*/
class BtInstallPage : public BtConfigPage
{
	Q_OBJECT
public:
	BtInstallPage();

	// reimplemented from btinstallpage
	QString iconName();
	QString label();
	QString header();

	void setInstallEnabled(bool b);

private:
	void initView();
	void initConnections();
	void initPathCombo();
	
	void slotPathChanged(const QString& pathText);
	void slotEditPaths();
	void slotInstall();

private:
	
	QLabel* m_pathLabel;
	QComboBox* m_pathCombo;
	QPushButton* m_configurePathButton;

	BtSourceWidget* m_sourceWidget;

	QPushButton* m_installButton;
};




/**
* Widget inside the tabwidget which includes the module view
* and installation source related functions.
* 
* - Tab bar
* - Stacked tree widgets, one for each source/tab
* - Buttons for the current source: refresh, edit, remove
* - Add source button
* - Install button for all sources
*
* The current source name is taken from the tabbar. Changing
* the tab changes the stacked widget page. Each source has
* QTreeWidget, populated with the module tree if the source
* module list is in a local cache. Refreshing the source refreshes
* the cache and rebuilds the module tree. Sources are not refreshed
* automatically, only by the user action, one source at a time.
*/
class BtSourceWidget : public QWidget
{
	Q_OBJECT
public:
	friend class BtInstallPage;

	BtSourceWidget(BtInstallPage* parent);
	virtual ~BtSourceWidget() {}

private:
	void initView();
	void initConnections();
	/** Add tabs/views for each source. */
	void initSources();
	
	/** Add one source to tabs/stack. */
	void addSource(const QString& sourceName);
	/** Update one module tree widget */
	void updateList(const QString& sourceName);
	/** Create a module tree for a tree widget */
	bool createModuleTree(QTreeWidgetItem* rootItem, const QString& source);
	
private slots:
	void slotSourceSelected(int index);
	/** Refresh button clicked. */
	void slotRefresh();
	/** Edit button clicked. */
	void slotEdit();
	/** Delete button clicked. */
	void slotDelete();
	/** Add button clicked. */
	void slotAdd();
	/** Modules have been checked/unchecked in the view. */
	void slotModuleSelectionChanged();
	
private:
	BtInstallPage* m_page;
	QTabBar* m_tabBar;
	QStackedWidget* m_viewStack;
	QLabel* m_refreshTimeLabel;
	QPushButton* m_refreshButton;
	QPushButton* m_editButton;
	QPushButton* m_deleteButton;
	QPushButton* m_addButton;
};



/**
* Confirmation dialog for installation. Lets the user
* uncheck modules from the list.
*/
class CInstallModuleChooserDialog : public CModuleChooserDialog
{
public:
	CInstallModuleChooserDialog(QTabBar* tabBar, QStackedWidget* viewStack, QWidget* parent, QString title, QString label, ListCSwordModuleInfo* moduleInfo);
protected:
	virtual void initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem);
private:
	QTabBar* m_tabBar;
	QStackedWidget* m_viewStack;
};

#endif