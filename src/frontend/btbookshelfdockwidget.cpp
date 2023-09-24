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

#include "btbookshelfdockwidget.h"

#include <memory>
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QStackedWidget>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>
#include <utility>
#include "../backend/bookshelfmodel/btbookshelfmodel.h"
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../backend/config/btconfig.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/managers/cswordbackend.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"
#include "bibletime.h"
#include "btbookshelfview.h"
#include "btbookshelfwidget.h"
#include "messagedialog.h"


namespace {
auto const groupingOrderKey =
        QStringLiteral("GUI/MainWindow/Docks/Bookshelf/grouping");
}

BtBookshelfDockWidget *BtBookshelfDockWidget::m_instance = nullptr;

BtBookshelfDockWidget::BtBookshelfDockWidget(QWidget *parent, Qt::WindowFlags f)
        : QDockWidget(parent, f)
{
    BT_ASSERT(!m_instance);
    m_instance = this;

    // Setup actions and menus:
    initMenus();

    // Setup tree model:
    m_treeModel = new BtBookshelfTreeModel(btConfig(), groupingOrderKey, this);

    // Get backend model:
    auto bookshelfModelPtr(CSwordBackend::instance().model());

    // Setup bookshelf widgets:
    m_bookshelfWidget = new BtBookshelfWidget(this);
    m_bookshelfWidget->setTreeModel(m_treeModel);
    m_bookshelfWidget->setSourceModel(bookshelfModelPtr);
    m_bookshelfWidget->setItemContextMenu(m_itemContextMenu);
    m_bookshelfWidget->treeView()->setMouseTracking(true); // required for moduleHovered
    /// \bug The correct grouping action is not selected on startup.

    // Setup welcome widgets:
    m_welcomeWidget = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout;
    m_installLabel = new QLabel(this);
    m_installLabel->setWordWrap(true);
    m_installLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    welcomeLayout->addWidget(m_installLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    m_installButton = new QPushButton(this);
    welcomeLayout->addWidget(m_installButton, 0, Qt::AlignHCenter | Qt::AlignTop);
    m_welcomeWidget->setLayout(welcomeLayout);

    // Setup stacked widget:
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->addWidget(m_bookshelfWidget);
    m_stackedWidget->addWidget(m_welcomeWidget);
    m_stackedWidget->setCurrentWidget(bookshelfModelPtr->moduleList().empty()
                                      ? m_welcomeWidget
                                      : m_bookshelfWidget);
    setWidget(m_stackedWidget);

    // Connect signals:
    BT_CONNECT(m_bookshelfWidget->treeView(), &BtBookshelfView::moduleActivated,
               [this](CSwordModuleInfo * module) {
                   if (!module->isLocked()) {
                       Q_EMIT moduleOpenTriggered(module);
                   } else {
                       /**
                         \todo Implement a better unlock dialog, which could
                               incorporate the following warning message.
                               Actually the whole case when the user tries to
                               open a locked module needs to be rethought and
                               refactored.
                       */
                       message::showWarning(
                                   this,
                                   tr("Warning: Module locked!"),
                                   tr("You are trying to access an encrypted "
                                      "module. Please provide an unlock key in "
                                      "the following dialog to open the "
                                      "module."));

                       /** \todo We need to keep the module name because
                                 unlocking currently reloads sword. */
                       auto const moduleName(module->name());
                       if (BibleTime::moduleUnlock(module)) {
                           // Re-initialize module pointer:
                           auto const & backend = CSwordBackend::instance();
                           module = backend.findModuleByName(moduleName);
                           BT_ASSERT(module);
                           Q_EMIT moduleOpenTriggered(module);
                       }
                   }
               });
    BT_CONNECT(m_bookshelfWidget->treeView(), &BtBookshelfView::moduleHovered,
               this, &BtBookshelfDockWidget::moduleHovered);
    BT_CONNECT(m_treeModel, &BtBookshelfTreeModel::moduleChecked,
               [](CSwordModuleInfo * const module, bool const checked)
               { module->setHidden(!checked); });
    BT_CONNECT(m_treeModel, &BtBookshelfTreeModel::groupingOrderChanged,
               [this](BtBookshelfTreeModel::Grouping const & g) {
                   g.saveTo(btConfig(), groupingOrderKey);
                   Q_EMIT groupingOrderChanged(g);
               });
    BT_CONNECT(m_bookshelfWidget->showHideAction(), &QAction::toggled,
               m_treeModel, &BtBookshelfTreeModel::setCheckable);
    auto modulesChangedSlot =
            [this]{
                auto const & moduleList =
                        CSwordBackend::instance().moduleList();
                m_stackedWidget->setCurrentWidget(moduleList.empty()
                                                  ? m_welcomeWidget
                                                  : m_bookshelfWidget);
            };
    BT_CONNECT(bookshelfModelPtr.get(), &BtBookshelfModel::rowsInserted,
               this/*needed*/, modulesChangedSlot);
    BT_CONNECT(bookshelfModelPtr.get(), &BtBookshelfModel::rowsRemoved,
               this/*needed*/, std::move(modulesChangedSlot));
    BT_CONNECT(m_installButton, &QPushButton::clicked,
               this, &BtBookshelfDockWidget::installWorksClicked);

    retranslateUi();
}

void BtBookshelfDockWidget::initMenus() {
    namespace RM = CResMgr::mainIndex;

    m_itemContextMenu = new QMenu(this);
    auto const addMenuAction =
        [this](void (BtBookshelfDockWidget::* signal)(CSwordModuleInfo *)) {
            auto * const action = new QAction(this);
            BT_CONNECT(action, &QAction::triggered,
                       [this, signal] {
                           if (CSwordModuleInfo * const module =
                                   static_cast<CSwordModuleInfo *>(
                                       m_itemContextMenu->property("BtModule")
                                           .value<void *>()))
                               Q_EMIT (this->*signal)(module);
                       });
            m_itemContextMenu->addAction(action);
            return action;
        };

    m_itemOpenAction =
            addMenuAction(&BtBookshelfDockWidget::moduleOpenTriggered);

    m_itemSearchAction =
            addMenuAction(&BtBookshelfDockWidget::moduleSearchTriggered);
    m_itemSearchAction->setIcon(RM::search::icon());

    m_itemUnlockAction =
            addMenuAction(&BtBookshelfDockWidget::moduleUnlockTriggered);
    m_itemUnlockAction->setIcon(RM::unlockModule::icon());

    m_itemAboutAction =
            addMenuAction(&BtBookshelfDockWidget::moduleAboutTriggered);
    m_itemAboutAction->setIcon(RM::aboutModule::icon());

    BT_CONNECT(m_itemContextMenu, &QMenu::aboutToShow,
               [this]{
                   void * v =
                           m_itemContextMenu->property("BtModule")
                                                .value<void*>();
                   CSwordModuleInfo *module = static_cast<CSwordModuleInfo*>(v);
                   m_itemOpenAction->setEnabled(!module->isLocked());
                   m_itemSearchAction->setText(
                               tr("&Search in %1...").arg(module->name()));
                   m_itemSearchAction->setEnabled(!module->isLocked());
                   m_itemUnlockAction->setEnabled(module->isLocked());
               });
}

void BtBookshelfDockWidget::retranslateUi() {
    setWindowTitle(tr("Bookshelf"));

    m_itemOpenAction->setText(tr("&Open"));
    m_itemUnlockAction->setText(tr("&Unlock..."));
    m_itemAboutAction->setText(tr("&About..."));

    m_installLabel->setText(tr("There are currently no works installed. Please "
                               "click the button below to install new works."));
    m_installButton->setText(tr("&Install works..."));
}

void BtBookshelfDockWidget::loadBookshelfState() {
    m_bookshelfWidget->loadBookshelfState();
}

void BtBookshelfDockWidget::saveBookshelfState() {
    m_bookshelfWidget->saveBookshelfState();
}
