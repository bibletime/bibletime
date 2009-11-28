/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/bttextwindowheaderwidget.h"

#include <QDebug>
//#include <QHash>
#include <QMenu>
#include <QString>
#include <QToolButton>
#include <QToolTip>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/displaywindow/bttextwindowheader.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/cpointers.h"

const char* ActionType = "ActionType";

BtTextWindowHeaderWidget::BtTextWindowHeaderWidget(BtTextWindowHeader *parent, CSwordModuleInfo::ModuleType mtype)
        : QWidget(parent),
        m_moduleType(mtype),
        m_popup(0),
        m_header(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_label = new QLabel("", this);
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    m_label->setSizePolicy(sizePolicy);
    m_label->setStyleSheet("QLabel{font-weight:bold}");
    layout->addWidget(m_label, 0, Qt::AlignRight);

    m_button = new QToolButton( this );
    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_button->setPopupMode( QToolButton::InstantPopup );
    m_button->setArrowType(Qt::NoArrow);
    m_button->setStyleSheet("QToolButton{margin:0px;}QToolButton::menu-indicator{subcontrol-position: center center;}");
    m_button->setToolTip( tr("Add/remove/replace") );

    layout->addWidget(m_button, 0, Qt::AlignLeft);

    m_separator = new QFrame(this);
    m_separator->setFrameShape(QFrame::VLine);
    layout->addWidget(m_separator);
}

BtTextWindowHeaderWidget::~BtTextWindowHeaderWidget() {}

void BtTextWindowHeaderWidget::recreateWidget(QStringList newModulesToUse, QString thisModule, int newIndex) {
    populateMenu();
    updateWidget(newModulesToUse, thisModule, newIndex);
}

// don't remove yet, maybe we'll add icons to buttons...
// const QString BtTextWindowHeaderWidget::iconName() {
//     qDebug() << "BtTextWindowHeaderWidget::iconName, has module:" << m_hasModule;
//     switch (m_moduleType) {
//         case CSwordModuleInfo::Bible:
//             return (m_hasModule) ? CResMgr::modules::bible::icon_unlocked : CResMgr::modules::bible::icon_add;
//         case CSwordModuleInfo::Commentary:
//             return (m_hasModule) ? CResMgr::modules::commentary::icon_unlocked : CResMgr::modules::commentary::icon_add;
//         case CSwordModuleInfo::Lexicon:
//             return m_hasModule ? CResMgr::modules::lexicon::icon_unlocked : CResMgr::modules::lexicon::icon_add;
//         case CSwordModuleInfo::GenericBook:
//             return m_hasModule ? CResMgr::modules::book::icon_unlocked : CResMgr::modules::book::icon_add;
//         default: //return as default the bible icon
//             return CResMgr::modules::bible::icon_unlocked;
//     }
// }

void BtTextWindowHeaderWidget::updateWidget(QStringList newModulesToUse, QString thisModule, int newIndex) {
    //qDebug() << "BtTextWindowHeaderWidget::updateMenu" << newModulesToUse << thisModule << newIndex << this;
    m_label->setText(thisModule);
    // create the menu if it doesn't exist
    if (!m_popup) populateMenu();

    m_id = newIndex;
    m_module = thisModule;
    namespace DU = util::directory;

    //All items are iterated and the state is changed properly
    QListIterator<QMenu*> it(m_submenus);
    while (it.hasNext()) {
        QMenu* popup = it.next();
        foreach (QAction* a, popup->actions()) {
            a->setChecked( (a->text() == thisModule) ? true : false );
            a->setDisabled( newModulesToUse.contains(a->text()) ? true : false );
        }
    }

    if (m_id == newModulesToUse.count() - 1) {
        // this is the rightmost module, hide the separator
        m_separator->hide();
    }
    else {
        m_separator->show();
    }
    m_removeAction->setDisabled((newModulesToUse.count() == 1) ? true : false);
}

/** Is called after a module was selected in the popup */
void BtTextWindowHeaderWidget::moduleChosen( QAction* action ) {
    //qDebug() << "BtTextWindowHeaderWidget::moduleChosen";

    if (action->property(ActionType).toInt() == RemoveAction) { // note: this is for m_popup, the toplevel!
        emit sigModuleRemove(m_id);
        return;
    }
    if (action->property(ActionType).toInt() == AddAction) {
        emit sigModuleAdd(m_id + 1, action->text());
        return;
    }
    if (action->property(ActionType).toInt() == ReplaceAction) {
        emit sigModuleReplace(m_id, action->text());
    }
}


void BtTextWindowHeaderWidget::populateMenu() {
    //qDebug()<<"BtTextWindowHeaderWidget::populateMenu";
    delete m_popup;
    m_popup = new QMenu(m_button);

    connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
    m_button->setMenu(m_popup);

    m_removeAction = new QAction(tr("Remove"), m_popup);
    m_removeAction->setProperty(ActionType, RemoveAction);
    m_removeAction->setIcon(util::directory::getIcon(CResMgr::displaywindows::general::removemoduleicon));
    m_popup->addAction(m_removeAction);

    // Add Replace and Add menus, both have all modules in them
    QMenu* replaceItem = new QMenu(tr("Replace"), m_popup);
    replaceItem->setIcon(util::directory::getIcon(CResMgr::displaywindows::general::replacemoduleicon));
    replaceItem->setProperty(ActionType, ReplaceAction);
    m_popup->addMenu(replaceItem);

    QMenu* addItem = new QMenu(tr("Add"), m_popup);
    addItem->setProperty(ActionType, AddAction);
    addItem->setIcon(util::directory::getIcon(CResMgr::displaywindows::general::addmoduleicon));
    m_popup->addMenu(addItem);

    QList<QMenu*> toplevelMenus;
    toplevelMenus.append(replaceItem);
    toplevelMenus.append(addItem);

    foreach(QMenu* menu, toplevelMenus) {
        // ******* Add languages and modules ********
        //m_popup->addSeparator();

        // Filters: add only non-hidden, non-locked and correct type
        BTModuleTreeItem::HiddenOff hiddenFilter;
        TypeFilter typeFilter(m_moduleType);
        QList<BTModuleTreeItem::Filter*> filters;
        if (!CBTConfig::get(CBTConfig::bookshelfShowHidden)) {
            filters.append(&hiddenFilter);
        }
        filters.append(&typeFilter);
        BTModuleTreeItem root(filters, BTModuleTreeItem::LangMod);
        // add all items recursively
        addItemToMenu(&root, menu, (TypeOfAction)menu->property(ActionType).toInt());
    }
}

void BtTextWindowHeaderWidget::addItemToMenu(BTModuleTreeItem* item, QMenu* menu, TypeOfAction actionType) {
    qDebug() << "BtTextWindowHeaderWidget::addItemToMenu";
    foreach (BTModuleTreeItem* i, item->children()) {

        if (i->type() == BTModuleTreeItem::Language) {
            // argument menu was m_popup, create and add a new lang menu to it
            QMenu* langMenu = new QMenu(i->text(), this);
            menu->addMenu(langMenu);
            m_submenus.append(langMenu);
            // add the module items to the lang menu
            addItemToMenu(i, langMenu, actionType);
        }
        else {
            // item must be module, create and add it to the lang menu
            QString name(i->text());
            QAction* modItem = new QAction(name, menu);
            modItem->setCheckable(true);
            modItem->setProperty(ActionType, actionType);
            menu->addAction(modItem);
        }
    }
}
