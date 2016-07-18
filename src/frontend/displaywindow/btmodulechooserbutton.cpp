/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/btmodulechooserbutton.h"

#include <QDebug>
#include <QHash>
#include <QMenu>
#include <QString>
#include <QToolButton>
#include <QToolTip>
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "bibletimeapp.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"


BtModuleChooserButton::BtModuleChooserButton(BtModuleChooserBar *parent, CSwordModuleInfo::ModuleType mtype)
        : QToolButton(parent),
        m_moduleType(mtype),
        m_popup(nullptr) {
    setPopupMode(QToolButton::InstantPopup);
}

void BtModuleChooserButton::recreateMenu(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules) {
    populateMenu();
    updateMenu(newModulesToUse, thisModule, newIndex, leftLikeModules);
}

QIcon const & BtModuleChooserButton::icon() {
    switch (m_moduleType) {
        case CSwordModuleInfo::Bible:
            return m_hasModule
                    ? CResMgr::modules::bible::icon_unlocked()
                    : CResMgr::modules::bible::icon_add();
        case CSwordModuleInfo::Commentary:
            return m_hasModule
                   ? CResMgr::modules::commentary::icon_unlocked()
                   : CResMgr::modules::commentary::icon_add();
        case CSwordModuleInfo::Lexicon:
            return m_hasModule
                   ? CResMgr::modules::lexicon::icon_unlocked()
                   : CResMgr::modules::lexicon::icon_add();
        case CSwordModuleInfo::GenericBook:
            return m_hasModule
                   ? CResMgr::modules::book::icon_unlocked()
                   : CResMgr::modules::book::icon_add();
        default: //return as default the bible icon
            return CResMgr::modules::bible::icon_unlocked();
    }
}

void BtModuleChooserButton::updateMenu(QStringList newModulesToUse, QString thisModule, int newIndex, int leftLikeModules) {
    m_id = newIndex;

    // create the menu if it doesn't exist
    if (!m_popup)
        populateMenu();

    m_module = thisModule;
    m_hasModule = thisModule.isEmpty() ? false : true;

    //All items are iterated and the state is changed properly
    QListIterator<QMenu*> it(m_submenus);
    while (it.hasNext()) {
        QMenu* popup = it.next();
        Q_FOREACH(QAction * const a, popup->actions()) {
            a->setChecked( (a->text() == thisModule) ? true : false );
            a->setDisabled( newModulesToUse.contains(a->text()) ? true : false );
        }
    }
    m_noneAction->setChecked(m_hasModule ? false : true);
    setIcon(icon());

    if (m_hasModule) {
        setToolTip( QString(tr("Select a work [%1]")).arg(m_module) );
    }
    else {
        setToolTip( tr("Select an additional work") );
    }
    bool disableNone = false;
    if (newModulesToUse.count() == 1 ||
        (newIndex == 0 && leftLikeModules == 1))
        disableNone = true;
    m_noneAction->setDisabled(disableNone);

    // Disable non-Bible categories on first button
    if (m_moduleType == CSwordModuleInfo::Bible && m_id == 0) {
        QList<QAction*> actions = m_popup->actions();
        for (int i=0; i<actions.count(); i++) {
            QAction* action = actions.at(i);
            QString text = action->text();
            if (text != QObject::tr("Bibles")) {
                action->setDisabled(true);
            }
        }
    }
}

/** Is called after a module was selected in the popup */
void BtModuleChooserButton::moduleChosen( QAction* action ) {
    auto modProperty(action->property("BibleTimeModule"));
    if (!modProperty.isValid()) { // note: this is for m_popup, the toplevel!
        if (m_hasModule) {
            qDebug() << "remove module" << m_id;
            emit sigModuleRemove(m_id);
            return;
        }
        else {
            // nothing else is done but the item must be set to checked
            // lest it change to unchecked
            action->setChecked(true);
        }
    }
    else {
        if (!m_hasModule) {
            emit sigModuleAdd(m_id + 1, modProperty.toString());
            return;
        }
        emit sigModuleReplace(m_id, modProperty.toString());
    }
}


void BtModuleChooserButton::populateMenu() {
    qDeleteAll(m_submenus);
    m_submenus.clear();
    delete m_popup;
    m_popup = new QMenu(this);

    m_noneAction = m_popup->addAction(tr("NONE"));
    m_noneAction->setCheckable(true);
    if (m_module.isEmpty()) m_noneAction->setChecked(true);

    m_popup->addSeparator();
    BT_CONNECT(m_popup, &QMenu::triggered,
               this,    &BtModuleChooserButton::moduleChosen);
    setMenu(m_popup);


    // ******* Add languages and modules ********

    // Filters: add only non-hidden and right type
    BTModuleTreeItem::HiddenOff hiddenFilter;
    QList<BTModuleTreeItem::Filter*> filters;
    if (!btConfig().value<bool>("GUI/bookshelfShowHidden", false)) {
        filters.append(&hiddenFilter);
    }
    TypeFilter typeFilter(m_moduleType);
    filters.append(&typeFilter);

    if (m_moduleType == CSwordModuleInfo::Bible) {
      BTModuleTreeItem root(filters, BTModuleTreeItem::CatLangMod);
        QList<BTModuleTreeItem::Filter*> filters2;
        if (!btConfig().value<bool>("GUI/bookshelfShowHidden", false)) {
            filters2.append(&hiddenFilter);
        }
        TypeFilter typeFilter2(CSwordModuleInfo::Commentary);
        filters2.append(&typeFilter2);
        root.add_items(filters2);
        // add all items recursively
        addItemToMenu(&root, m_popup);
    }
    else {
        BTModuleTreeItem root(filters, BTModuleTreeItem::LangMod);
        addItemToMenu(&root, m_popup);
    }
}

void BtModuleChooserButton::addItemToMenu(BTModuleTreeItem* item, QMenu* menu) {
    Q_FOREACH(BTModuleTreeItem * const i, item->children()) {
        if (i->type() == BTModuleTreeItem::Language ||
            i->type() == BTModuleTreeItem::Category ) {
            // argument menu was m_popup, create and add a new lang menu to it
            QMenu* langMenu = new QMenu(i->text(), this);
            menu->addMenu(langMenu);
            m_submenus.append(langMenu);
            // add the module items to the lang menu
            addItemToMenu(i, langMenu);
        }
        else {
            // item must be module, create and add it to the lang menu
            QString name(i->text());
            QAction* modItem = new QAction(name, menu);
            modItem->setProperty("BibleTimeModule", name);
            modItem->setCheckable(true);
            menu->addAction(modItem);
        }
    }
}
