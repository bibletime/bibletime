/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bttextwindowheaderwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QToolTip>
#include <QSizePolicy>
#include <QString>
#include "../../backend/config/btconfig.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletimeapp.h"
#include "bttextwindowheader.h"


namespace {
const QString BookshelfShowHiddenKey = "GUI/bookshelfShowHidden";
} // anonymous namespace

const char* ActionType = "ActionType";
const char * ModuleName = "ModuleName";

BtTextWindowHeaderWidget::BtTextWindowHeaderWidget(BtTextWindowHeader *parent, CSwordModuleInfo::ModuleType mtype)
        : QWidget(parent),
        m_moduleType(mtype),
        m_popup(nullptr) {
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

void BtTextWindowHeaderWidget::recreateWidget(QStringList newModulesToUse, QString thisModule, int newIndex, int lefLikeModules) {
    populateMenu();
    updateWidget(newModulesToUse, thisModule, newIndex, lefLikeModules);
}

// don't remove yet, maybe we'll add icons to buttons...
// const QString BtTextWindowHeaderWidget::iconName() {
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

void BtTextWindowHeaderWidget::updateWidget(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules) {
    m_label->setText(thisModule);
    m_id = newIndex;
    // create the menu if it doesn't exist
    if (!m_popup)
        populateMenu();

    m_module = thisModule;

    //All items are iterated and the state is changed properly
    QListIterator<QMenu*> it(m_submenus);
    while (it.hasNext()) {
        QMenu* popup = it.next();
        Q_FOREACH(QAction * const a, popup->actions()) {
            a->setChecked(a->property(ModuleName).toString() == thisModule);
            a->setDisabled(
                        newModulesToUse.contains(
                            a->property(ModuleName).toString()));
        }
    }

    bool disableRemove = false;
    if (newModulesToUse.count() == 1 ||
        (newIndex == 0 && leftLikeModules == 1))
        disableRemove = true;
    m_removeAction->setDisabled(disableRemove);

    // Disable non-Bible categories on left replace menu
    if (m_moduleType == CSwordModuleInfo::Bible && m_id == 0) {
        QList<QAction*> actionsType = m_popup->actions();
        for (int t=0; t<actionsType.count(); t++) {
            QAction* actionType = actionsType.at(t);
            QString typeText = actionType->text();
            if (typeText != QObject::tr("Replace"))
                continue;
            QMenu* menuType = actionType->menu();
            if (menuType == nullptr)
                continue;
            QList<QAction*> actions = menuType->actions();
            for (int i=0; i<actions.count(); i++) {
                QAction* action = actions.at(i);
                QString text = action->text();
                if (text != QObject::tr("Bibles")) {
                    action->setDisabled(true);
                }
            }
        }
    }
}

/** Is called after a module was selected in the popup */
void BtTextWindowHeaderWidget::moduleChosen( QAction* action ) {
    if (action->property(ActionType).toInt() == RemoveAction) { // note: this is for m_popup, the toplevel!
        emit sigModuleRemove(m_id);
        return;
    }
    if (action->property(ActionType).toInt() == AddAction) {
        emit sigModuleAdd(m_id + 1, action->property(ModuleName).toString());
        return;
    }
    if (action->property(ActionType).toInt() == ReplaceAction) {
        emit sigModuleReplace(m_id, action->property(ModuleName).toString());
    }
}


void BtTextWindowHeaderWidget::populateMenu() {
    delete m_popup;
    m_popup = new QMenu(m_button);

    BT_CONNECT(m_popup, SIGNAL(triggered(QAction *)),
               this,    SLOT(moduleChosen(QAction *)));
    m_button->setMenu(m_popup);

    m_removeAction = new QAction(tr("Remove"), m_popup);
    m_removeAction->setProperty(ActionType, RemoveAction);
    m_removeAction->setIcon(CResMgr::displaywindows::general::icon_removeModule());
    m_popup->addAction(m_removeAction);

    // Add Replace and Add menus, both have all modules in them
    QMenu* replaceItem = new QMenu(tr("Replace"), m_popup);
    replaceItem->setIcon(CResMgr::displaywindows::general::icon_replaceModule());
    replaceItem->setProperty(ActionType, ReplaceAction);
    m_popup->addMenu(replaceItem);

    QMenu* addItem = new QMenu(tr("Add"), m_popup);
    addItem->setProperty(ActionType, AddAction);
    addItem->setIcon(CResMgr::displaywindows::general::icon_addModule());
    m_popup->addMenu(addItem);

    QList<QMenu*> toplevelMenus;
    toplevelMenus.append(replaceItem);
    toplevelMenus.append(addItem);

    Q_FOREACH(QMenu * const menu, toplevelMenus) {
        // ******* Add categories, languages and modules ********
        // Filters: add only non-hidden, non-locked and correct type
        BTModuleTreeItem::HiddenOff hiddenFilter;
        QList<BTModuleTreeItem::Filter*> filters;
        if (!btConfig().value<bool>(BookshelfShowHiddenKey, false)) {
            filters.append(&hiddenFilter);
        }
        TypeFilter typeFilter(m_moduleType);
        filters.append(&typeFilter);

        TypeOfAction const typeOfAction =
                static_cast<TypeOfAction>(menu->property(ActionType).toInt());
        if (m_moduleType == CSwordModuleInfo::Bible) {
            BTModuleTreeItem root(filters, BTModuleTreeItem::CatLangMod);
            QList<BTModuleTreeItem::Filter*> filters2;
            if (!btConfig().value<bool>(BookshelfShowHiddenKey, false)) {
                filters2.append(&hiddenFilter);
            }
            if (menu == addItem || menu == replaceItem) {
                TypeFilter typeFilter2(CSwordModuleInfo::Commentary);
                filters2.append(&typeFilter2);
                root.add_items(filters2);
            }
            addItemToMenu(&root, menu, typeOfAction);
        }
        else {
            BTModuleTreeItem root(filters, BTModuleTreeItem::LangMod);
            addItemToMenu(&root, menu, typeOfAction);
        }
    }
}

void BtTextWindowHeaderWidget::addItemToMenu(BTModuleTreeItem* item, QMenu* menu, TypeOfAction actionType) {
    Q_FOREACH(BTModuleTreeItem * const i, item->children()) {
        if (i->type() == BTModuleTreeItem::Language ||
            i->type() == BTModuleTreeItem::Category) {
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
            modItem->setProperty(ModuleName, name);
            menu->addAction(modItem);
        }
    }
}
