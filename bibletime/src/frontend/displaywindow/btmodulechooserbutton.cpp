/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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
#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/cpointers.h"


BtModuleChooserButton::BtModuleChooserButton(BtModuleChooserBar *parent, CSwordModuleInfo::ModuleType mtype)
        : QToolButton(parent),
        m_moduleType(mtype),
        m_popup(0),
        m_moduleChooserBar(parent) {
    //qDebug()<<"BtModuleChooserButton::BtModuleChooserButton";
    setPopupMode(QToolButton::InstantPopup);
}

BtModuleChooserButton::~BtModuleChooserButton() {}

void BtModuleChooserButton::recreateMenu(QStringList newModulesToUse, QString thisModule, int newIndex) {
    populateMenu();
    updateMenu(newModulesToUse, thisModule, newIndex);
}

const QString BtModuleChooserButton::iconName() {
    qDebug() << "BtModuleChooserButton::iconName, has module:" << m_hasModule;
    switch (m_moduleType) {
        case CSwordModuleInfo::Bible:
            return (m_hasModule) ? CResMgr::modules::bible::icon_unlocked : CResMgr::modules::bible::icon_add;
        case CSwordModuleInfo::Commentary:
            return (m_hasModule) ? CResMgr::modules::commentary::icon_unlocked : CResMgr::modules::commentary::icon_add;
        case CSwordModuleInfo::Lexicon:
            return m_hasModule ? CResMgr::modules::lexicon::icon_unlocked : CResMgr::modules::lexicon::icon_add;
        case CSwordModuleInfo::GenericBook:
            return m_hasModule ? CResMgr::modules::book::icon_unlocked : CResMgr::modules::book::icon_add;
        default: //return as default the bible icon
            return CResMgr::modules::bible::icon_unlocked;
    }
}

void BtModuleChooserButton::updateMenu(QStringList newModulesToUse, QString thisModule, int newIndex) {
    //qDebug() << "BtModuleChooserButton::updateMenu" << newModulesToUse << thisModule << newIndex << this;
    // create the menu if it doesn't exist
    if (!m_popup) populateMenu();

    m_id = newIndex;
    m_module = thisModule;
    m_hasModule = thisModule.isEmpty() ? false : true;
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
    m_noneAction->setChecked(m_hasModule ? false : true);
    setIcon(DU::getIcon(iconName()));

    if (m_hasModule) {
        setToolTip( QString(tr("Select a work [%1]")).arg(m_module) );
    }
    else {
        setToolTip( tr("Select an additional work") );
    }
    m_noneAction->setDisabled((newModulesToUse.count() == 1) ? true : false);
    //qDebug()<<"BtModuleChooserButton::modulesChanged end";
}

/** Is called after a module was selected in the popup */
void BtModuleChooserButton::moduleChosen( QAction* action ) {
    //qDebug() << "BtModuleChooserButton::moduleChosen";

    if (action->text() == tr("NONE")) { // note: this is for m_popup, the toplevel!
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
            emit sigModuleAdd(m_id + 1, action->text());
            return;
        }
        emit sigModuleReplace(m_id, action->text());
    }
}


void BtModuleChooserButton::populateMenu() {
    //qDebug()<<"BtModuleChooserButton::populateMenu";
    qDeleteAll(m_submenus);
    m_submenus.clear();
    delete m_popup;
    m_popup = new QMenu(this);

    m_noneAction = m_popup->addAction(tr("NONE"));
    m_noneAction->setCheckable(true);
    if (m_module.isEmpty()) m_noneAction->setChecked(true);

    m_popup->addSeparator();
    connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
    setMenu(m_popup);


    // ******* Add languages and modules ********

    // Filters: add only non-hidden and right type
    BTModuleTreeItem::HiddenOff hiddenFilter;
    TypeFilter typeFilter(m_moduleType);
    QList<BTModuleTreeItem::Filter*> filters;
    if (!CBTConfig::get(CBTConfig::bookshelfShowHidden)) {
        filters.append(&hiddenFilter);
    }
    filters.append(&typeFilter);
    BTModuleTreeItem root(filters, BTModuleTreeItem::LangMod);
    // add all items recursively
    addItemToMenu(&root, m_popup);
}

void BtModuleChooserButton::addItemToMenu(BTModuleTreeItem* item, QMenu* menu) {
    qDebug() << "BtModuleChooserButton::addItemToMenu";
    foreach (BTModuleTreeItem* i, item->children()) {

        if (i->type() == BTModuleTreeItem::Language) {
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
            modItem->setCheckable(true);
            menu->addAction(modItem);
        }
    }
}
