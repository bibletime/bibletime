/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKSHELFWORKSPAGE
#define BTBOOKSHELFWORKSPAGE

#include "backend/drivers/btmoduleset.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QWizardPage>

class BtBookshelfFilterModel;
class BtBookshelfGroupingMenu;
class BtBookshelfModel;
class BtBookshelfView;
class BtBookshelfWizard;
class BtInstallPageModel;
class CSwordModuleInfo;
class QComboBox;
class QHBoxLayout;
class QLabel;
class QMenu;
class QToolButton;
class QVBoxLayout;

class BtBookshelfWorksPage: public QWizardPage {

    Q_OBJECT

public:

    enum InstallType {
        install,
        update,
        remove
    };

    BtBookshelfWorksPage(
            BtBookshelfWorksPage::InstallType iType,
            QWidget *parent = 0);

    bool isComplete() const;
    void initializePage();
    int nextId() const;
    BtModuleSet selectedWorks() const;
    bool manageWorks();

private slots:
    void slotDataChanged();
    void slotEditPaths();
    void slotGroupingActionTriggered(const BtBookshelfTreeModel::Grouping &grouping);
    void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);
    void slotPathChanged(const QString & pathText);
    void slotSwordSetupChanged();

private:
    BtBookshelfWizard *btWizard();
    bool destinationPathIsWritable();
    bool filter(const CSwordModuleInfo *mInfo);
    bool filterInstalls(const CSwordModuleInfo *mInfo);
    bool filterUpdates(const CSwordModuleInfo *mInfo);
    bool filterRemoves(const CSwordModuleInfo *mInfo);
    void initConnections();
    void initMenus();
    void initPathCombo();
    bool moduleIsInstalled(const CSwordModuleInfo *mInfo);
    bool moduleUsesSelectedLanguage(const CSwordModuleInfo *mInfo);
    bool removeWorks();
    void retranslateUi();
    QString selectedInstallPath();
    void setupModels();
    void setupUi();
    void updateModels();

    InstallType m_installType;
    BtBookshelfTreeModel::Grouping m_groupingOrder;

    QToolButton *m_groupingButton;
    BtBookshelfView *m_bookshelfView;
    QLabel *m_pathLabel;
    QLabel *m_groupingLabel;
    QComboBox *m_pathCombo;
    QVBoxLayout *m_verticalLayout;

    BtInstallPageModel *m_installPageModel;
    BtBookshelfModel *m_bookshelfModel;
    BtBookshelfFilterModel *m_bookshelfFilterModel;

    QMenu *m_contextMenu;
    BtBookshelfGroupingMenu *m_groupingMenu;
    QMenu *m_itemContextMenu;

    QStringList m_sources;
    QStringList m_languages;
    QMap<CSwordModuleInfo *, QString> m_moduleSourceMap;
};

#endif
