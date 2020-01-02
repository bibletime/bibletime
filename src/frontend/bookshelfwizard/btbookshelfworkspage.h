/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTBOOKSHELFWORKSPAGE
#define BTBOOKSHELFWORKSPAGE

#include "btbookshelfwizardpage.h"

#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/drivers/btmoduleset.h"
#include "btbookshelfwizardenums.h"


class BtBookshelfGroupingMenu;
class BtBookshelfModel;
class BtBookshelfView;
class BtInstallPageModel;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QToolButton;

class BtBookshelfWorksPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfWorksPage(WizardTaskType iType, QWidget * parent = nullptr);

    BtModuleSet const & checkedModules() const;
    bool isComplete() const final override;
    void initializePage() final override;
    QString installPath() const;
    int nextId() const final override;

private slots:

    void slotGroupingActionTriggered(
            BtBookshelfTreeModel::Grouping const & grouping);
    void slotGroupingOrderChanged(BtBookshelfTreeModel::Grouping const & g);
    void slotPathChanged(int const index);
    void slotInitPathCombo();

private: /* Methods: */

    void retranslateUi();

private: /* Fields: */

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
    BtBookshelfModel * m_bookshelfModel;

    QMenu * m_contextMenu;
    BtBookshelfGroupingMenu * m_groupingMenu;
    QMenu * m_itemContextMenu;

}; /* class BtBookshelfWorksPage */

#endif
