/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSWORDMODULELISTVIEW_H
#define CSWORDMODULELISTVIEW_H

//Sword includes
#include <installmgr.h>

//QT includes
#include <QString>
#include <QTreeWidget>

//KDE includes
//#include <klistview.h>

class CSwordModuleInfo;
class CSwordBackend;
class QTreeWidgetItem;

namespace BookshelfManager {

class CSwordSetupModuleListView : public QTreeWidget {
	Q_OBJECT

public:
	CSwordSetupModuleListView(QWidget* parent, bool is_remote, sword::InstallSource* installSource = 0);
	virtual ~CSwordSetupModuleListView();

	void addModule(CSwordModuleInfo *, QString localVersion);
	void finish();
	QStringList selectedModules();
	void clear();

	virtual QString tooltip(QTreeWidgetItem* i, int column) const;
	virtual bool showTooltip(QTreeWidgetItem* i, const QPoint& pos, int column) const;

protected slots:
	void slotItemClicked(QTreeWidgetItem*);

private:
	void init();

	CSwordBackend* m_backend;
	bool m_is_remote;

	QTreeWidgetItem* m_categoryBible;
	QTreeWidgetItem* m_categoryCommentary;
	QTreeWidgetItem* m_categoryLexicon;
	QTreeWidgetItem* m_categoryBook;
	QTreeWidgetItem* m_categoryDevotionals;
	QTreeWidgetItem* m_categoryGlossaries;

signals:
	void selectedModulesChanged();
};

} //NAMESPACE

#endif //CSWORDMODULELISTVIEW_H
