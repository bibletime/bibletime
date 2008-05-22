/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLMODULECHOOSERDIALOG_H
#define BTINSTALLMODULECHOOSERDIALOG_H

#include "frontend/cmodulechooserdialog.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>

class BTModuleTreeItem;

class QWidget;
class QTreeWidgetItem;


/**
* Confirmation dialog for installation. Lets the user
* uncheck modules from the list.
*/
class BtInstallModuleChooserDialog : public CModuleChooserDialog
{
	Q_OBJECT

public:
	BtInstallModuleChooserDialog(QWidget* parent, QString title, QString label, QList<CSwordModuleInfo*>* empty);

	void initModuleItem(QString name, QTreeWidgetItem* sourceItem);
	void enableOk(bool enabled);

public slots:
	void slotItemChecked(QTreeWidgetItem* item, int column);

protected:
	virtual void initModuleItem(BTModuleTreeItem*, QTreeWidgetItem*);

	QList<QTreeWidgetItem*> findModuleItemsByName(QString name);
private:
	QStringList m_nameList;
	QMap<QString, bool> m_doubleCheckedModules;

};

#endif
