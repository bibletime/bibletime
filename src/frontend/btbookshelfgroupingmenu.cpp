/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelfgroupingmenu.h"

#include <QAction>
#include <QActionGroup>
#include <QVariant>
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"


namespace {

inline void setActionRef(QAction *a, const BtBookshelfTreeModel::Grouping &g) {
    a->setProperty("groupingPointer",
                   QVariant::fromValue(
                       const_cast<void *>(static_cast<void const *>(&g))));
}

inline const BtBookshelfTreeModel::Grouping &getActionRef(const QAction *a) {
    return *static_cast<BtBookshelfTreeModel::Grouping const *>(
                a->property("groupingPointer").value<void *>());
}

} // anonymous namespace


void BtBookshelfGroupingMenu::setGrouping(
        BtBookshelfTreeModel::Grouping const & grouping)
{
    using G = BtBookshelfTreeModel::Grouping;
    if (grouping == G::CAT_LANG) {
        m_groupingCatLangAction->setChecked(true);
    } else if (grouping == G::CAT) {
        m_groupingCatAction->setChecked(true);
    } else if (grouping == G::LANG_CAT) {
        m_groupingLangCatAction->setChecked(true);
    } else if (grouping == G::LANG) {
        m_groupingLangAction->setChecked(true);
    } else {
        BT_ASSERT(grouping == G::NONE);
        if (m_groupingNoneAction)
            m_groupingNoneAction->setChecked(true);
    }
}

void BtBookshelfGroupingMenu::initMenu(bool showNoGrouping) {
    setIcon(CResMgr::mainIndex::grouping::icon());

    m_groupingActionGroup = new QActionGroup(this);
    m_groupingActionGroup->setExclusive(true);
    BT_CONNECT(m_groupingActionGroup, &QActionGroup::triggered,
               [this](QAction * const action)
               { Q_EMIT signalGroupingOrderChanged(getActionRef(action)); });

    using G = BtBookshelfTreeModel::Grouping;

    m_groupingCatLangAction = new QAction(this);
    m_groupingCatLangAction->setCheckable(true);
    setActionRef(m_groupingCatLangAction, G::CAT_LANG);
    m_groupingActionGroup->addAction(m_groupingCatLangAction);
    addAction(m_groupingCatLangAction);

    m_groupingCatAction = new QAction(this);
    m_groupingCatAction->setCheckable(true);
    setActionRef(m_groupingCatAction, G::CAT);
    m_groupingActionGroup->addAction(m_groupingCatAction);
    addAction(m_groupingCatAction);

    m_groupingLangCatAction = new QAction(this);
    m_groupingLangCatAction->setCheckable(true);
    setActionRef(m_groupingLangCatAction, G::LANG_CAT);
    m_groupingActionGroup->addAction(m_groupingLangCatAction);
    addAction(m_groupingLangCatAction);

    m_groupingLangAction = new QAction(this);
    m_groupingLangAction->setCheckable(true);
    setActionRef(m_groupingLangAction, G::LANG);
    m_groupingActionGroup->addAction(m_groupingLangAction);
    addAction(m_groupingLangAction);

    if (showNoGrouping) {
        m_groupingNoneAction = new QAction(this);
        m_groupingNoneAction->setCheckable(true);
        setActionRef(m_groupingNoneAction, G::NONE);
        m_groupingActionGroup->addAction(m_groupingNoneAction);
        addAction(m_groupingNoneAction);
    } else {
        m_groupingNoneAction = nullptr;
    }

    retranslateUi();
}

void BtBookshelfGroupingMenu::retranslateUi() {
    m_groupingCatLangAction->setText(tr("Category/Language"));
    m_groupingCatAction->setText(tr("Category"));
    m_groupingLangCatAction->setText(tr("Language/Category"));
    m_groupingLangAction->setText(tr("Language"));

    if (m_groupingNoneAction != nullptr) {
        m_groupingNoneAction->setText(tr("No grouping"));
    }
}
