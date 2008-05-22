/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/
#ifndef BTSOURCEWIDGET_H
#define BTSOURCEWIDGET_H

class CSwordModuleInfo;

#include <QTabWidget>
#include <QString>
#include <QMap>

class BtInstallMgr;
class BtInstallPage;
class BtSourceArea;

class QProgressDialog;
class QTreeWidget;

/**
* Tabwidget which holds the source widgets.
* This widget implements the slots for the source action buttons and
* applies the actions to the proper source(s).
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
	/** "Stop All" button clicked */
	void slotStopInstall(QTreeWidget* treeWidget);

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
	void slotInstallAccepted(QList<CSwordModuleInfo*> mi, QTreeWidget* treeWidget);



private:
	QStringList m_sourceNameList;
	BtInstallPage* m_page;
	QProgressDialog* m_progressDialog; // for refreshing
	BtInstallMgr* m_currentInstallMgr; // for refreshing
	QMap<QString, int> m_selectedModulesCountMap;
};

#endif
