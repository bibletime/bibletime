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
#include <qmap.h>

//KDE includes
#include <klistview.h>

class CSwordModuleInfo;
class CSwordBackend;

namespace BookshelfManager {

class CSwordSetupModuleListView : public KListView {
	Q_OBJECT

public:
	CSwordSetupModuleListView(QWidget* parent, bool is_remote, sword::InstallSource* installSource = 0);
	virtual ~CSwordSetupModuleListView();

	void addModule(CSwordModuleInfo *, QString localVersion);
	void finish();
	QStringList selectedModules();
	void clear();

	virtual QString tooltip(Q3ListViewItem* i, int column) const;
	virtual bool showTooltip(Q3ListViewItem* i, const QPoint& pos, int column) const;

protected slots:
	void slotItemClicked(Q3ListViewItem*);

private:
	void init();

	CSwordBackend* m_backend;
	bool m_is_remote;

	Q3ListViewItem* m_categoryBible;
	Q3ListViewItem* m_categoryCommentary;
	Q3ListViewItem* m_categoryLexicon;
	Q3ListViewItem* m_categoryBook;
	Q3ListViewItem* m_categoryDevotionals;
	Q3ListViewItem* m_categoryGlossaries;

signals:
	void selectedModulesChanged();
};

} //NAMESPACE

#endif //CSWORDMODULELISTVIEW_H
