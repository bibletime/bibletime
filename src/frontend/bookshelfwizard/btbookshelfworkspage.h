/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "btbookshelfwizardpage.h"

#include <memory>
#include <QObject>
#include <QString>
#include <vector>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/managers/cswordbackend.h"
#include "btbookshelfwizardenums.h"


class BtBookshelfGroupingMenu;
class BtBookshelfModel;
class BtBookshelfView;
class BtInstallPageModel;
class BtModuleSet;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QToolButton;
class QWidget;

class BtBookshelfWorksPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: // methods:

    BtBookshelfWorksPage(WizardTaskType iType, QWidget * parent = nullptr);

    BtModuleSet const & checkedModules() const;
    bool isComplete() const final override;
    void initializePage() final override;
    QString installPath() const;
    int nextId() const final override;

private Q_SLOTS:

    void slotGroupingActionTriggered(
            BtBookshelfTreeModel::Grouping const & grouping);
    void slotGroupingOrderChanged(BtBookshelfTreeModel::Grouping const & g);
    void slotPathChanged(int const index);
    void slotInitPathCombo();

private: // methods:

    void retranslateUi();

private: // fields:

    WizardTaskType const m_taskType;
    BtBookshelfTreeModel::Grouping m_groupingOrder;

    QToolButton * m_groupingButton;
    BtBookshelfView * m_bookshelfView;
    QLabel * m_msgLabel;
    QLabel * m_pathLabel;
    QLabel * m_groupingLabel;
    QComboBox * m_pathCombo;

    QLabel * m_nameFilterLabel;
    QLineEdit * m_nameFilterEdit;

    BtInstallPageModel * m_installPageModel;
    std::vector<std::unique_ptr<CSwordBackend const>> m_usedBackends;
    std::shared_ptr<BtBookshelfModel> m_bookshelfModel;

    QMenu * m_contextMenu;
    BtBookshelfGroupingMenu * m_groupingMenu;
    QMenu * m_itemContextMenu;

}; /* class BtBookshelfWorksPage */
