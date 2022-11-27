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

#include "bttextwindowheaderwidget.h"

#include <QAction>
#include <QFontMetrics>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <Qt>
#include <QToolButton>
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "btmodulechoosermenu.h"


BtTextWindowHeaderWidget::BtTextWindowHeaderWidget(
        CSwordModuleInfo::ModuleType mtype,
        QWidget * parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_iconLabel);

    m_label = new QLabel(QString(), this);
    m_label->setStyleSheet(QStringLiteral("QLabel{font-weight:bold}"));
    layout->addWidget(m_label);

    m_button = new QToolButton( this );
    m_button->setPopupMode( QToolButton::InstantPopup );
    m_button->setArrowType(Qt::NoArrow);
    m_button->setStyleSheet(
                QStringLiteral(
                    "QToolButton{margin:0px;}"
                    "QToolButton::menu-indicator{"
                        "subcontrol-position:center center"
                    "}"));
    m_button->setToolTip(tr("Add/remove/replace"));

    auto * const popup  = new QMenu(m_button);
        m_removeAction = new QAction(tr("Remove"), popup);
        m_removeAction->setIcon(
                    CResMgr::displaywindows::general::icon_removeModule());
        BT_CONNECT(m_removeAction, &QAction::triggered,
                   [this] { Q_EMIT sigModuleRemove(m_id); });
        popup->addAction(m_removeAction);

        // Add Replace and Add menus, both have all modules in them
        m_replaceMenu =
                new BtModuleChooserMenu(
                    tr("Replace"),
                    mtype,
                    BtModuleChooserMenu::DisableNonBiblesOnFirstButton,
                    popup);
        m_replaceMenu->setIcon(
                    CResMgr::displaywindows::general::icon_replaceModule());
        BT_CONNECT(m_replaceMenu, &BtModuleChooserMenu::sigModuleChosen,
                   [this](CSwordModuleInfo * const module) {
                        BT_ASSERT(module);
                        Q_EMIT sigModuleReplace(m_id, module);
                    });
        popup->addMenu(m_replaceMenu);

        m_addMenu =
                new BtModuleChooserMenu(
                    tr("Add"),
                    mtype,
                    BtModuleChooserMenu::DisableSelectedModule,
                    popup);
        m_addMenu->setIcon(CResMgr::displaywindows::general::icon_addModule());
        BT_CONNECT(m_addMenu, &BtModuleChooserMenu::sigModuleChosen,
                   [this](CSwordModuleInfo * const module) {
                        BT_ASSERT(module);
                        Q_EMIT sigModuleAdd(m_id + 1, module);
                    });
        popup->addMenu(m_addMenu);
    m_button->setMenu(popup);

    layout->addWidget(m_button);
    layout->addStretch(1);

    auto * const separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    layout->addWidget(separator);
}

void BtTextWindowHeaderWidget::updateWidget(BtModuleList newModulesToUse,
                                            CSwordModuleInfo * module,
                                            int newIndex,
                                            int leftLikeModules)
{
    m_label->setText(module->name());
    m_iconLabel->setPixmap(
                module->moduleIcon().pixmap(m_label->fontMetrics().height()));
    m_id = newIndex;

    bool disableRemove = false;
    if (newModulesToUse.count() == 1 ||
        (newIndex == 0 && leftLikeModules == 1))
        disableRemove = true;
    m_removeAction->setDisabled(disableRemove);

    m_replaceMenu->update(newModulesToUse,
                          module,
                          newIndex,
                          leftLikeModules);
    m_addMenu->update(newModulesToUse, module, newIndex, leftLikeModules);
}
