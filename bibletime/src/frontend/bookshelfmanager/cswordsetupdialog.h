/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDSETUPDIALOG_H
#define CSWORDSETUPDIALOG_H

#include "util/cpointers.h"

//QT includes
#include <QString>
#include <QMap>

//KDE includes
#include <kpagedialog.h>

class QWidget;
class QLabel;
class QComboBox;
class QStackedWidget;
class QButton;
class QPushButton;
class QLineEdit;
class QTreeWidget;
class QProgressDialog;
class QCloseEvent;

namespace BookshelfManager {

class BTInstallMgr;

	class CSwordSetupModuleListView;

	/**
	 * The Sword configuration dialog of BibleTime
	  * @author The BibleTime Team
	  */
class CSwordSetupDialog : public KPageDialog, public CPointers  {
		Q_OBJECT

public:
		CSwordSetupDialog(QWidget *parent=0);

		enum Parts {
			Sword,
			Install,
			Remove
		};

		/** Opens the page which contaisn the given part ID. */
		void showPart( CSwordSetupDialog::Parts ID );

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
		void initSwordConfig();
		void initInstall();
		void initRemove();
		void initManageIndices();

		void writeSwordConfig();

		void setupSwordPathListBox();
		void populateInstallCombos();

		const QString currentInstallSource();

		KPageWidgetItem* m_swordConfigPage;
		QTreeWidget* m_swordPathListBox;
		QPushButton* m_swordEditPathButton;
		QPushButton* m_swordAddPathButton;
		QPushButton* m_swordRemovePathButton;
		QStringList m_swordPathList;
		bool m_swordSetupChanged;

		KPageWidgetItem* m_removePage;
		KPageWidgetItem* m_manageIndiciesPage;
		KPageWidgetItem* m_installPage;
		QStackedWidget* m_installStackedWidget;

		QString source;
		QString target;

		QLabel* m_sourceLabel;
		QLabel* m_targetLabel;
		QComboBox* m_sourceCombo;
		QComboBox* m_targetCombo;

		QMap<QString, QString> m_targetMap;

		CSwordSetupModuleListView* m_removeModuleListView;
		QPushButton* m_removeRemoveButton;

		const bool refreshRemoteModuleCache( const QString& sourceName );
		void populateRemoveModuleListView();
		bool populateInstallModuleListView(const QString& sourceName);

		//install module stuff
		QPushButton* m_installBackButton;
		QPushButton* m_installContinueButton;

		QWidget* m_installModuleListPage;
		QWidget* m_installSourcePage;

		CSwordSetupModuleListView* m_installModuleListView;
		QProgressDialog* m_progressDialog;
		QString m_installingModule;
		bool m_refreshedRemoteSources;
		unsigned int m_installedModuleCount;

		BTInstallMgr* m_currentInstallMgr; //pointer to the current installmgr object so we can access it in the cancel install slot

private slots:
		void slot_sourceSelected(const QString &sourceName);
		void slot_targetSelected(const QString &targetName);

		void slot_doRemoveModules();

		void slotOk();

		void slot_connectToSource();
		void slot_moduleRefreshProgressCancelClicked();
		void slot_moduleRefreshCompleted(const int, const int);

		void slot_installAddSource();
		void slot_installDeleteSource();
		void slot_installModules();
		void slot_installModulesChanged();
		void slot_installProgressCancelClicked();
		void slot_showInstallSourcePage();

		void installCompleted( const int, const int );
		void slot_swordRemoveClicked();
		void slot_swordAddClicked();
		void slot_swordEditClicked();
		void slot_swordPathSelected();

signals: // Signals
		void signalSwordSetupChanged();
	};

}

#endif //CSWORDSETUPDIALOG_H
