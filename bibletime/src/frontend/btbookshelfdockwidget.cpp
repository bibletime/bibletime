/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/btbookshelfdockwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/bookshelfmodel/btcheckstatefilterproxymodel.h"
#include "backend/bookshelfmodel/btmodulenamefilterproxymodel.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btaboutmoduledialog.h"
#include "frontend/mainindex/btbookshelfview.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

BtBookshelfDockWidget::BtBookshelfDockWidget(QWidget *parent, Qt::WindowFlags f)
    : QDockWidget(parent, f)
{
    setObjectName("BookshelfDock");

    // Setup models:
    m_bookshelfModel = new BtBookshelfModel(this);
    m_bookshelfModel->addModules(CPointers::backend()->moduleList());
    m_bookshelfTreeModel = new BtBookshelfTreeModel(this);
    m_bookshelfTreeModel->setDefaultChecked(true);
    m_bookshelfTreeModel->setSourceModel(m_bookshelfModel);
    m_filterProxyModel = new BtCheckStateFilterProxyModel(this);
    m_filterProxyModel->setFilterRole(BtBookshelfTreeModel::CheckStateRole);
    m_filterProxyModel->setSourceModel(m_bookshelfTreeModel);
    m_nameFilterProxyModel = new BtModuleNameFilterProxyModel(this);
    m_nameFilterProxyModel->setSourceModel(m_filterProxyModel);

    // Setup actions and menus:
    initMenus();

    // Setup widgets:
    m_widget = new QWidget(this);
    QVBoxLayout *layout(new QVBoxLayout);
    layout->setContentsMargins(0, 0, 0, 0);
        QHBoxLayout *toolBar(new QHBoxLayout);
            m_nameFilterLabel = new QLabel(this);
            toolBar->addWidget(m_nameFilterLabel);

            m_nameFilterEdit = new QLineEdit(this);
            m_nameFilterEdit->installEventFilter(this);
            m_nameFilterLabel->setBuddy(m_nameFilterEdit);
            toolBar->addWidget(m_nameFilterEdit);

            m_groupingButton = new QToolButton(this);
            m_groupingButton->setPopupMode(QToolButton::InstantPopup);
            m_groupingButton->setMenu(m_groupingMenu);
            m_groupingButton->setIcon(m_groupingMenu->icon());
            m_groupingButton->setAutoRaise(true);
            toolBar->addWidget(m_groupingButton);

            m_showHideButton = new QToolButton(this);
            m_showHideButton->setDefaultAction(m_showHideAction);
            m_showHideButton->setAutoRaise(true);
            toolBar->addWidget(m_showHideButton);
        layout->addLayout(toolBar);

        m_view = new BtBookshelfView(this);
        m_view->setModel(m_nameFilterProxyModel);
        layout->addWidget(m_view);
    m_widget->setLayout(layout);
    setWidget(m_widget);

    connect(CPointers::backend(),
            SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
            this, SLOT(swordSetupChanged()));
    connect(m_nameFilterEdit, SIGNAL(textEdited(QString)),
            m_nameFilterProxyModel, SLOT(setFilterFixedString(QString)));
    connect(m_view, SIGNAL(contextMenuActivated(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
    connect(m_view,
            SIGNAL(moduleContextMenuActivated(CSwordModuleInfo*,QPoint)),
            this, SLOT(showItemContextMenu(CSwordModuleInfo*,QPoint)));
    connect(m_view, SIGNAL(moduleActivated(CSwordModuleInfo*)),
            this, SIGNAL(moduleOpenTriggered(CSwordModuleInfo*)));

    retranslateInterface();
}

bool BtBookshelfDockWidget::eventFilter(QObject *object, QEvent *event) {
    Q_ASSERT(object == m_nameFilterEdit);
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent*>(event);
        switch (e->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                QApplication::sendEvent(m_view, event);
                return true;
            default:
                break;
        }
    }
    return false;
}

void BtBookshelfDockWidget::initMenus() {
    typedef util::filesystem::DirectoryUtil DU;
    namespace RM = CResMgr::mainIndex;

    m_contextMenu = new QMenu(this);
        m_groupingMenu = new QMenu(this);
        m_contextMenu->addMenu(m_groupingMenu);
        m_groupingMenu->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingActionGroup = new QActionGroup(this);
            connect(m_groupingActionGroup, SIGNAL(triggered(QAction*)),
                    this, SLOT(groupingActionTriggered(QAction*)));

            m_groupingCatLangAction = new QAction(this);
            m_groupingCatLangAction->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingCatLangAction->setChecked(true);
            m_groupingActionGroup->addAction(m_groupingCatLangAction);
            m_groupingMenu->addAction(m_groupingCatLangAction);

            m_groupingCatAction = new QAction(this);
            m_groupingCatAction->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingActionGroup->addAction(m_groupingCatAction);
            m_groupingMenu->addAction(m_groupingCatAction);

            m_groupingLangCatAction = new QAction(this);
            m_groupingLangCatAction->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingActionGroup->addAction(m_groupingLangCatAction);
            m_groupingMenu->addAction(m_groupingLangCatAction);

            m_groupingLangAction = new QAction(this);
            m_groupingLangAction->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingActionGroup->addAction(m_groupingLangAction);
            m_groupingMenu->addAction(m_groupingLangAction);

            m_groupingNoneAction = new QAction(this);
            m_groupingNoneAction->setIcon(DU::getIcon(RM::grouping::icon));
            m_groupingActionGroup->addAction(m_groupingNoneAction);
            m_groupingMenu->addAction(m_groupingNoneAction);

        m_showHideAction = new QAction(this);
        m_showHideAction->setIcon(DU::getIcon(RM::search::icon));
        m_showHideAction->setCheckable(true);
        connect(m_showHideAction, SIGNAL(toggled(bool)),
                this, SLOT(showHideEnabled(bool)));
        m_contextMenu->addAction(m_showHideAction);

    m_itemContextMenu = new QMenu(this);
        m_itemActionGroup = new QActionGroup(this);
        connect(m_itemActionGroup, SIGNAL(triggered(QAction*)),
                this,              SLOT(itemActionTriggered(QAction*)));

        m_itemOpenAction = new QAction(this);
        m_itemActionGroup->addAction(m_itemOpenAction);
        m_itemContextMenu->addAction(m_itemOpenAction);

        m_itemSearchAction = new QAction(this);
        m_itemSearchAction->setIcon(DU::getIcon(RM::search::icon));
        m_itemActionGroup->addAction(m_itemSearchAction);
        m_itemContextMenu->addAction(m_itemSearchAction);

        m_itemEditMenu = new QMenu(this);
        m_itemEditMenu->setIcon(DU::getIcon(RM::editModuleMenu::icon));
        m_itemContextMenu->addMenu(m_itemEditMenu);
            m_itemEditPlainAction = new QAction(this);
            m_itemEditPlainAction->setIcon(DU::getIcon(RM::editModulePlain::icon));
            m_itemActionGroup->addAction(m_itemEditPlainAction);
            m_itemEditMenu->addAction(m_itemEditPlainAction);

            m_itemEditHtmlAction = new QAction(this);
            m_itemEditHtmlAction->setIcon(DU::getIcon(RM::editModuleHTML::icon));
            m_itemActionGroup->addAction(m_itemEditHtmlAction);
            m_itemEditMenu->addAction(m_itemEditHtmlAction);

        m_itemUnlockAction = new QAction(this);
        m_itemUnlockAction->setIcon(DU::getIcon(RM::unlockModule::icon));
        m_itemActionGroup->addAction(m_itemUnlockAction);
        m_itemContextMenu->addAction(m_itemUnlockAction);

        m_itemAboutAction = new QAction(this);
        m_itemAboutAction->setIcon(DU::getIcon(RM::aboutModule::icon));
        m_itemActionGroup->addAction(m_itemAboutAction);
        m_itemContextMenu->addAction(m_itemAboutAction);
}

void BtBookshelfDockWidget::retranslateInterface() {
    setWindowTitle(tr("Bookshelf"));

    m_nameFilterLabel->setText(tr("Fi&lter:"));
    m_groupingButton->setText(tr("Grouping"));
    m_groupingButton->setToolTip(tr("Change the grouping of items in the bookshelf."));

    m_groupingMenu->setTitle(tr("Grouping"));
        m_groupingCatLangAction->setText(tr("Category/Language"));
        m_groupingCatAction->setText(tr("Category"));
        m_groupingLangCatAction->setText(tr("Language/Category"));
        m_groupingLangAction->setText(tr("Language"));
        m_groupingNoneAction->setText(tr("No grouping"));
    m_showHideAction->setText(tr("Show/hide works"));

    m_itemOpenAction->setText(tr("&Open"));
    m_itemEditMenu->setTitle(tr("&Edit"));
        m_itemEditPlainAction->setText(tr("&Plain text"));
        m_itemEditHtmlAction->setText(tr("&HTML"));
    m_itemUnlockAction->setText(tr("&Unlock..."));
    m_itemAboutAction->setText(tr("&About..."));
}

void BtBookshelfDockWidget::swordSetupChanged() {
    QSet<CSwordModuleInfo*> added(CPointers::backend()->moduleList().toSet());
    QSet<CSwordModuleInfo*> removed(m_bookshelfModel->modules().toSet());
    QSet<CSwordModuleInfo*> t(removed);
    removed.subtract(added);
    added.subtract(t);
    m_bookshelfModel->removeModules(removed);
    m_bookshelfModel->addModules(added);
}

void BtBookshelfDockWidget::showContextMenu(QPoint pos) {
    m_contextMenu->popup(pos);
}

void BtBookshelfDockWidget::groupingActionTriggered(QAction *action) {
    BtBookshelfTreeModel::Grouping g;
    if (action == m_groupingCatAction) {
        g.append(BtBookshelfTreeModel::GROUP_CATEGORY);
    } else if (action == m_groupingCatLangAction) {
        g.append(BtBookshelfTreeModel::GROUP_CATEGORY);
        g.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
    } else if (action == m_groupingLangAction) {
        g.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
    } else if (action == m_groupingLangCatAction) {
        g.append(BtBookshelfTreeModel::GROUP_LANGUAGE);
        g.append(BtBookshelfTreeModel::GROUP_CATEGORY);
    }
    m_bookshelfTreeModel->setGroupingOrder(g);
    m_view->setRootIsDecorated(!g.isEmpty());
}

void BtBookshelfDockWidget::showHideEnabled(bool enable) {
    if (enable) {
        m_bookshelfTreeModel->setCheckable(true);
        m_filterProxyModel->setEnabled(false);
    } else {
        m_filterProxyModel->setEnabled(true);
        m_bookshelfTreeModel->setCheckable(false);
    }
}

void BtBookshelfDockWidget::showItemContextMenu(CSwordModuleInfo *module,
                                                QPoint pos)
{
    m_itemContextMenu->setProperty("BtModule", qVariantFromValue((void*) module));
    m_itemSearchAction->setText(tr("&Search in %1...").arg(module->name()));
    m_itemOpenAction->setEnabled(!module->isLocked());
    m_itemEditMenu->setEnabled(module->isWritable());
    m_itemUnlockAction->setEnabled(module->isLocked());

    m_itemContextMenu->popup(pos);
}

void BtBookshelfDockWidget::itemActionTriggered(QAction *action) {
    CSwordModuleInfo *module((CSwordModuleInfo*) m_itemContextMenu->property("BtModule").value<void*>());
    if (module == 0) return;

    if (action == m_itemOpenAction) {
        emit moduleOpenTriggered(module);
    } else if (action == m_itemSearchAction) {
        emit moduleSearchTriggered(module);
    } else if (action == m_itemEditPlainAction) {
        emit moduleEditPlainTriggered(module);
    } else if (action == m_itemEditHtmlAction) {
        emit moduleEditHtmlTriggered(module);
    } else if (action == m_itemUnlockAction) {
        emit moduleUnlockTriggered(module);
    } else if (action == m_itemAboutAction) {
        emit moduleAboutTriggered(module);
    }
}
