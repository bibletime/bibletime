/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef BTREMOVEPAGE_H
#define BTREMOVEPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include "backend/btmoduletreeitem.h"

#include <QString>
#include <QMultiMap>

//class BTModuleTreeItem;

class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;


class BtRemovePage : public BtConfigPage
{
	Q_OBJECT

public:
	BtRemovePage();

	~BtRemovePage(){}

	// BtConfigPage methods
	QString header();
	QString iconName();
	QString label();

	void populateModuleList();

signals:
	void swordSetupChanged();

public slots:
	void slotSwordSetupChanged();

private slots:

	void slotRemoveModules();
	/** Handles activating the Remove button. */
	void slotSelectionChanged(QTreeWidgetItem* item, int column);
	void slotItemDoubleClicked(QTreeWidgetItem* item, int column);

private: // methods
	void addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem);

private: // data
	QTreeWidget* m_view;
	QPushButton* m_removeButton;
	/** Map of module name and install path (absolute path from the moduleinfo config entry).*/
	//QMultiMap<QString, QString> m_selectedModules;
	ListCSwordModuleInfo m_selectedModules;
	ListCSwordModuleInfo m_installedModules;
};

#endif