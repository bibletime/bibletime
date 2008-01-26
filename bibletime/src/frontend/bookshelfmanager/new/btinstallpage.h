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


#include "frontend/bookshelfmanager/new/btconfigdialog.h"
#include "frontend/cmodulechooserdialog.h"
#include "backend/btmoduletreeitem.h"

#include <QString>
#include <QWidget>

#include <installmgr.h>

#include <boost/scoped_ptr.hpp>

class BtSourceWidget;
class Bt_InstallMgr;

class QTreeWidgetItem;
class QTreeWidget;
class QLabel;
class QComboBox;
class QPushButton;
class QTabBar;
class QStackedWidget;
class QProgressDialog;

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

private slots:
	void slotPathChanged(const QString& pathText);
	void slotEditPaths();

private:
	
	QComboBox* m_pathCombo;
	QPushButton* m_configurePathButton;
	BtSourceWidget* m_sourceWidget;
	QPushButton* m_installButton;
};

/**
* - Tree widget
* - Buttons for the current source: refresh, edit, remove
* - Add source button
*
* Each source has
* QTreeWidget, populated with the module tree if the source
* module list is in a local cache. Refreshing the source refreshes
* the cache and rebuilds the module tree. Sources are not refreshed
* automatically, only by the user action, one source at a time.
*/
class BtSourceArea : public QWidget
{
	Q_OBJECT

	friend class BtSourceWidget;
public:

	struct InstalledFilter : BTModuleTreeItem::Filter {
		InstalledFilter(QString sourceName);
		bool filter(CSwordModuleInfo*);
		sword::InstallSource m_source;
		boost::scoped_ptr<CSwordBackend> m_swordBackend;
	};

	BtSourceArea(const QString& sourceName);
	~BtSourceArea();

	void initView();
	void initTreeFirstTime();
	QTreeWidget* treeWidget();

	QMap<QString, bool>* selectedModules();

signals:
	void signalSelectionChanged(QString sourceName, int selectedCount);

private slots:
	void slotSelectionChanged(QTreeWidgetItem* item, int column);
	void slotItemDoubleClicked(QTreeWidgetItem* item, int column);
private:
	/** Create a module tree for a tree widget */
	bool createModuleTree();
	void addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem);

	QString m_sourceName;
	bool m_treeAlreadyInitialized;
	QMap<QString, bool> m_checkedModules;
	CSwordBackend* m_remoteBackend; // needed for the module list
	ListCSwordModuleInfo m_moduleList;

	QTreeWidget* m_view;
	QLabel* m_refreshTimeLabel;
	QPushButton* m_refreshButton;
	QPushButton* m_editButton;
	QPushButton* m_deleteButton;
	QPushButton* m_addButton;


};


/**
* Tabwidget which holds the source widgets.
* This widget implements the slots for the source action buttons.
*/
class BtSourceWidget : public QTabWidget
{
	Q_OBJECT
public:
	friend class BtInstallPage;

	BtSourceWidget(BtInstallPage* parent);
	virtual ~BtSourceWidget() {}

	BtSourceArea* area();
	QString currentSourceName();

public slots:
	/** Install button has been clicked. */
	void slotInstall();

private:
	void initSourceConnections();
	/** Add tabs/views for each source. */
	void initSources();
	
	/** Add one source to tabs/stack. */
	void addSource(const QString& sourceName);
	
private slots:

	void slotRefresh();
	
	void slotRefreshCanceled();

	void slotRefreshCompleted(int, int);

	/** Edit button clicked. */
	void slotEdit();
	/** Delete button clicked. */
	void slotDelete();
	/** Add button clicked. */
	void slotAdd();
	/** Modules have been checked/unchecked in the view. */
	void slotModuleSelectionChanged(QString sourceName, int selectedCount);

	void slotTabSelected(int index);
	void slotInstallAccepted(ListCSwordModuleInfo, QTreeWidget* treeWidget);

private:
	QStringList m_sourceNameList;
	BtInstallPage* m_page;
	QProgressDialog* m_progressDialog; // for refreshing
	Bt_InstallMgr* m_currentInstallMgr; // for refreshing
	QMap<QString, int> m_selectedModulesCountMap;
};



/**
* Confirmation dialog for installation. Lets the user
* uncheck modules from the list.
*/
class CInstallModuleChooserDialog : public CModuleChooserDialog
{
public:
	CInstallModuleChooserDialog(QWidget* parent, QString title, QString label, ListCSwordModuleInfo* empty);
protected:
	virtual void initModuleItem(BTModuleTreeItem*, QTreeWidgetItem*);
};

#endif