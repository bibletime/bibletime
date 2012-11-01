/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cdisplaywindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMdiSubWindow>
#include <QMenu>
#include <QStringList>
#include <QWidget>
#include "backend/config/btconfig.h"
#include "backend/keys/cswordkey.h"
#include "bibletime.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/cdisplay.h"
#include "frontend/displaywindow/bttoolbarpopupaction.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/directory.h"
#include "util/cresmgr.h"


CDisplayWindow::CDisplayWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent)
        : QMainWindow(parent),
        m_actionCollection(0),
        m_mdi(parent),
        m_keyChooser(0),
        m_swordKey(0),
        m_isReady(false),
        m_moduleChooserBar(0),
        m_mainToolBar(0),
        m_buttonsToolBar(0),
        m_formatToolBar(0),
        m_headerBar(0),
        m_popupMenu(0),
        m_displayWidget(0),
        m_history(0) {
    setAttribute(Qt::WA_DeleteOnClose); //we want to destroy this window when it is closed
    m_actionCollection = new BtActionCollection(this);
    setModules(modules);

    // Connect this to the backend module list changes
    connect(CSwordBackend::instance(),
            SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
            SLOT(reload(CSwordBackend::SetupChangedReason)));
    BibleTime* mainwindow = btMainWindow();
    connect(mainwindow, SIGNAL(toggledTextWindowHeader(bool)), SLOT(slotShowHeader(bool)) );
    connect(mainwindow, SIGNAL(toggledTextWindowNavigator(bool)), SLOT(slotShowNavigator(bool)) );
    connect(mainwindow, SIGNAL(toggledTextWindowToolButtons(bool)), SLOT(slotShowToolButtons(bool)) );
    connect(mainwindow, SIGNAL(toggledTextWindowModuleChooser(bool)), SLOT(slotShowModuleChooser(bool)) );
    connect(mainwindow, SIGNAL(toggledTextWindowFormatToolbar(bool)), SLOT(slotShowFormatToolBar(bool)) );
}

CDisplayWindow::~CDisplayWindow() {
    delete m_swordKey;
    m_swordKey = 0;
}

QWidget * CDisplayWindow::getProfileWindow() const {
    for (QWidget * w = parentWidget(); w; w = w->parentWidget()) {
        QMdiSubWindow * sw = qobject_cast<QMdiSubWindow *>(w);
        if (sw)
            return sw;
    }
    return const_cast<CDisplayWindow *>(this);
}

BibleTime* CDisplayWindow::btMainWindow() {
    return dynamic_cast<BibleTime*>(m_mdi->parent());
}

void CDisplayWindow::setToolBarsHidden() {
    // Hide current window toolbars
    if (mainToolBar())
        mainToolBar()->setHidden(true);
    if (buttonsToolBar())
        buttonsToolBar()->setHidden(true);
    if (moduleChooserBar())
        moduleChooserBar()->setHidden(true);
    if (formatToolBar())
        formatToolBar()->setHidden(true);
}
void CDisplayWindow::clearMainWindowToolBars() {
    // Clear main window toolbars, except for works toolbar
    btMainWindow()->navToolBar()->clear();
    btMainWindow()->toolsToolBar()->clear();
    btMainWindow()->formatToolBar()->clear();
}

void CDisplayWindow::windowActivated() {
    clearMainWindowToolBars();
    setupMainWindowToolBars();
}

/** Returns the right window caption. */
const QString CDisplayWindow::windowCaption() {
    if (!m_modules.count()) {
        return QString::null;
    }

    return QString(key()->key()).append(" (").append(m_modules.join(" | ")).append(")");
}

/** Returns the used modules as a pointer list */
const QList<const CSwordModuleInfo*> CDisplayWindow::modules() const {
    return CSwordBackend::instance()->getConstPointerList(m_modules);
}

/** Store the settings of this window in the given CProfileWindow object. */
void CDisplayWindow::storeProfileSettings(const QString & windowGroup) {
    BtConfig & conf = btConfig();

    conf.beginGroup(windowGroup);

    QWidget * w = getProfileWindow();

    /**
      \note We don't use saveGeometry/restoreGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect(w->x(), w->y(), w->width(), w->height());
    conf.setSessionValue<QRect>("windowRect", rect);

    conf.setSessionValue("maximized", w->isMaximized());

    bool hasFocus = (w == dynamic_cast<CDisplayWindow *>(mdi()->activeSubWindow()));
    conf.setSessionValue("hasFocus", hasFocus);
    // conf.setSessionValue("type", static_cast<int>(modules().first()->type()));

    // Save current key:
    if (key()) {
        CSwordKey * k = key();
        sword::VerseKey * vk = dynamic_cast<sword::VerseKey*>(k);
        QString oldLang;
        if (vk) {
            // Save keys in english only:
            const QString oldLang = QString::fromLatin1(vk->getLocale());
            vk->setLocale("en");
            conf.setSessionValue("key", k->key());
            vk->setLocale(oldLang.toLatin1());
        } else {
            conf.setSessionValue("key", k->key());
        }
    }

    // Save list of modules:
    QStringList mods;
    Q_FOREACH (const CSwordModuleInfo * module, modules())
        mods.append(module->name());
    conf.setSessionValue("modules", mods);

    conf.endGroup();
}

void CDisplayWindow::applyProfileSettings(const QString & windowGroup) {
    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    setUpdatesEnabled(false);

    QWidget * w = getProfileWindow();

    /**
      \note We don't use restoreGeometry/saveGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect = conf.sessionValue<QRect>("windowRect");
    w->resize(rect.width(), rect.height());
    w->move(rect.x(), rect.y());

    if (conf.sessionValue<bool>("maximized"))
        w->showMaximized();

    setUpdatesEnabled(true);
    conf.endGroup();
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
    namespace DU = util::directory;

    QAction* actn = new QAction(QIcon(), tr("Select all"), a);
    actn->setShortcut(QKeySequence::SelectAll);
    a->addAction("selectAll", actn);

    actn = new QAction(QIcon(), tr("Copy"), a);
    actn->setShortcut(QKeySequence::Copy);
    a->addAction("copySelectedText", actn);

    actn = new QAction(QIcon(), tr("Change location"), a);
    actn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    a->addAction("openLocation", actn);

    actn = new QAction(QIcon(DU::getIcon(CResMgr::displaywindows::general::search::icon)),
                       tr("Search with works of this window"), a);
    actn->setShortcut(CResMgr::displaywindows::general::search::accel);
    a->addAction(CResMgr::displaywindows::general::search::actionName, actn);

    BtToolBarPopupAction* action = new BtToolBarPopupAction(
        QIcon(DU::getIcon(CResMgr::displaywindows::general::backInHistory::icon)),
        tr("Back in history"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::general::backInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::backInHistory::actionName, action);

    action = new BtToolBarPopupAction(
        QIcon(DU::getIcon(CResMgr::displaywindows::general::forwardInHistory::icon)),
        tr("Forward in history"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::general::forwardInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::forwardInHistory::actionName, action);
}

void CDisplayWindow::initActions() {
    BtActionCollection* ac = actionCollection();

    CDisplayWindow::insertKeyboardActions(ac);

    QAction* actn = ac->action(CResMgr::displaywindows::general::search::actionName);
    Q_ASSERT(actn != 0);
    QObject::connect(actn, SIGNAL(triggered()),
                     this, SLOT(slotSearchInModules()));

    CDisplayConnections* conn = displayWidget()->connectionsProxy();

    actn = ac->action("openLocation");
    Q_ASSERT(actn != 0);
    QObject::connect(actn, SIGNAL(triggered()),
                     this, SLOT(setFocusKeyChooser()));
    addAction(actn);

    actn = ac->action("selectAll");
    Q_ASSERT(actn != 0);
    QObject::connect(actn, SIGNAL(triggered()),
                     conn, SLOT(selectAll()));
    addAction(actn);

    actn = ac->action("copySelectedText");
    Q_ASSERT(actn != 0);
    QObject::connect(actn, SIGNAL(triggered()),
                     conn, SLOT(copySelection()));
    addAction(actn);

    actn = ac->action(CResMgr::displaywindows::general::backInHistory::actionName);
    Q_ASSERT(actn != 0);
    bool ok = QObject::connect(actn,                    SIGNAL(triggered()),
                               keyChooser()->history(), SLOT(back()));
    Q_ASSERT(ok);
    addAction(actn);

    actn = ac->action(CResMgr::displaywindows::general::forwardInHistory::actionName);
    Q_ASSERT(actn != 0);
    ok = QObject::connect(actn,                    SIGNAL(triggered()),
                          keyChooser()->history(), SLOT(fw()));
    Q_ASSERT(ok);
    addAction(actn);

    ac->readShortcuts("Displaywindow shortcuts");
}

/** Refresh the settings of this window. */
void CDisplayWindow::reload(CSwordBackend::SetupChangedReason) {
    //first make sure all used Sword modules are still present
    QMutableStringListIterator it(m_modules);
    while (it.hasNext()) {
        if (!CSwordBackend::instance()->findModuleByName(it.next())) {
            it.remove();
        }
    }

    if (m_modules.isEmpty()) {
        close();
        return;
    }

    if (keyChooser()) keyChooser()->setModules( modules(), false );

    lookup();

    actionCollection()->readShortcuts("DisplayWindow shortcuts");
    actionCollection()->readShortcuts("Readwindow shortcuts");
    emit sigModuleListSet(m_modules);
}

void CDisplayWindow::slotAddModule(int index, QString module) {
    m_modules.insert(index, module);
    lookup();
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotReplaceModule(int index, QString newModule) {
    qDebug() << "CDisplayWindow::slotReplaceModule" << m_modules.at(index) << "with" << newModule;
    m_modules.replace(index, newModule);
    qDebug() << "window's new module list:" << m_modules;
    lookup();
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotRemoveModule(int index) {
    m_modules.removeAt(index);
    lookup();
    modulesChanged();
    emit sigModuleListChanged();
}

/** Sets the new display options for this window. */
void CDisplayWindow::setDisplayOptions(const DisplayOptions &displayOptions) {
    m_displayOptions = displayOptions;
    emit sigDisplayOptionsChanged(m_displayOptions);
}

/** Sets the new filter options of this window. */
void CDisplayWindow::setFilterOptions(const FilterOptions &filterOptions) {
    m_filterOptions = filterOptions;
    emit sigFilterOptionsChanged(m_filterOptions);
}

/** Set the ready status */
void CDisplayWindow::setReady(bool ready) {
    m_isReady = ready;
}

/** Returns true if the window may be closed. */
bool CDisplayWindow::queryClose() {
    return true;
}

/** Sets the keychooser widget for this display window. */
void CDisplayWindow::setKeyChooser( CKeyChooser* ck ) {
    m_keyChooser = ck;
}

/** Sets the new sword key. */
void CDisplayWindow::setKey( CSwordKey* key ) {
    Q_ASSERT( key );
    m_swordKey = key;
}

BTHistory* CDisplayWindow::history() {
    if (m_history == 0)
        m_history = new BTHistory(this);
    return m_history;
}

void CDisplayWindow::modulesChanged() {
    // this would only set the stringlist again
    //if (moduleChooserBar()) { //necessary for write windows
    //setModules( m_moduleChooserBar->getModuleList() );
    //}
    if (modules().isEmpty()) {
        close();
    }
    else {
        emit sigModulesChanged(modules());
        key()->setModule(modules().first());
        keyChooser()->setModules(modules());
    }
}

/** Sets the module chooser bar. */
void CDisplayWindow::setModuleChooserBar( BtModuleChooserBar* bar ) {
    if (m_moduleChooserBar) {
        m_moduleChooserBar->deleteLater();
    }

    //if a new bar should be set!
    if (bar) {
        m_moduleChooserBar = bar;
        bar->setWindowTitle(tr("Work chooser buttons"));
        bar->setLayoutDirection(Qt::LeftToRight);
        bar->setVisible(btConfig().value<bool>("GUI/showTextWindowModuleSelectorButtons", true));
    }
}

/** Setup the module header of text area. */
void CDisplayWindow::setHeaderBar( QToolBar* header ) {
    m_headerBar = header;
    header->setMovable(false);
    header->setWindowTitle(tr("Text area header"));
    header->setVisible(btConfig().value<bool>("GUI/showTextWindowHeaders", true));
}

/** Sets the modules. */
void CDisplayWindow::setModules( const QList<CSwordModuleInfo*>& newModules ) {
    m_modules.clear();

    foreach (CSwordModuleInfo* mod, newModules) {
        m_modules.append(mod->name());
    }
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
bool CDisplayWindow::init() {
    initView();
    setMinimumSize( 100, 100 );

    setWindowTitle(windowCaption());
    //setup focus stuff.
    setFocusPolicy(Qt::ClickFocus);
    parentWidget()->setFocusPolicy(Qt::ClickFocus);
    initActions();
    initToolbars();
    if (!btConfig().value<bool>("GUI/showToolbarsInEachWindow", true))
        setToolBarsHidden();
    btMainWindow()->clearMdiToolBars();
    clearMainWindowToolBars();
    initConnections();
    setupPopupMenu();

    m_filterOptions = btConfig().getFilterOptions();
    m_displayOptions = btConfig().getDisplayOptions();
    emit sigDisplayOptionsChanged(m_displayOptions);
    emit sigFilterOptionsChanged(m_filterOptions);
    emit sigModulesChanged(modules());

    setReady(true);
    return true;
}

static void prepareToolBar(QToolBar* bar, const QString& title, bool visible) {
    bar->setAllowedAreas(Qt::TopToolBarArea);
    bar->setFloatable(false);
    bar->setWindowTitle(title);
    bar->setVisible(visible);
}

/** Setup the Navigation toolbar. */
void CDisplayWindow::setMainToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Navigation"), btConfig().value<bool>("GUI/showTextWindowNavigator", true) );
    m_mainToolBar = bar;
}

/** Setup the Tools toolbar. */
void CDisplayWindow::setButtonsToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Tool"), btConfig().value<bool>("GUI/showTextWindowToolButtons", true) );
    m_buttonsToolBar = bar;
}

/** Setup the Format toolbar. */
void CDisplayWindow::setFormatToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Format"), true );
    m_formatToolBar = bar;
}

/** Sets the display settings button. */
void CDisplayWindow::setDisplaySettingsButton(BtDisplaySettingsButton *button) {
    connect(this, SIGNAL(sigDisplayOptionsChanged(const DisplayOptions&)),
        button, SLOT(setDisplayOptions(const DisplayOptions&)));
    connect(this, SIGNAL(sigFilterOptionsChanged(const FilterOptions&)),
        button, SLOT(setFilterOptions(const FilterOptions&)));
    connect(this, SIGNAL(sigModulesChanged(const QList<const CSwordModuleInfo*>&)),
        button, SLOT(setModules(const QList<const CSwordModuleInfo*>&)));

    button->setDisplayOptions(displayOptions(), false);
    button->setFilterOptions(filterOptions(), false);
    button->setModules(modules());

    connect(button, SIGNAL(sigFilterOptionsChanged(const FilterOptions&)),
            this, SLOT(setFilterOptions(const FilterOptions&)));
    connect(button, SIGNAL(sigDisplayOptionsChanged(const DisplayOptions&)),
            this, SLOT(setDisplayOptions(const DisplayOptions&)));
    connect(button, SIGNAL(sigChanged()),
            this, SLOT(lookup()));
}

void CDisplayWindow::slotShowHeader(bool show) {
    if (headerBar())
        headerBar()->setVisible(show);
}

void CDisplayWindow::slotShowNavigator(bool show) {
    if (mainToolBar())
        mainToolBar()->setVisible(show);
}

void CDisplayWindow::slotShowToolButtons(bool show) {
    if (buttonsToolBar())
        buttonsToolBar()->setVisible(show);
}

void CDisplayWindow::slotShowModuleChooser(bool show) {
    if (moduleChooserBar())
        moduleChooserBar()->setVisible(show);
}

void CDisplayWindow::slotShowFormatToolBar(bool show) {
    if (formatToolBar())
        formatToolBar()->setVisible(show);
}

/** Lookup the current key. Used to refresh the display. */
void CDisplayWindow::lookup() {
    lookupSwordKey( key() );
}

void CDisplayWindow::lookupModKey( const QString& moduleName, const QString& keyName ) {
    if (!isReady()) {
        return;
    }

    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!m) {
        return; /// \todo check if this is correct behavior
    }

    /// \todo check for containsRef compat
    if (m && modules().contains(m)) {
        key()->setKey(keyName);
        keyChooser()->setKey(key()); //the key chooser does send an update signal
        emit sigKeyChanged(key());
    }
    else {     //given module not displayed in this window
        //if the module is displayed in another display window we assume a wrong drop
        //create a new window for the given module
        QList<CSwordModuleInfo*> mList;
        mList.append(m);

        BibleTime *mainWindow = btMainWindow();
        Q_ASSERT(mainWindow != 0);
        mainWindow->createReadDisplayWindow(mList, keyName);
    }
}

void CDisplayWindow::lookupKey( const QString& keyName ) {
    /* This function is called for example after a bookmark was dropped on this window
    */
    Q_ASSERT(modules().first());

    lookupModKey(modules().first()->name(), keyName);
}

/** Update the status of the popup menu entries. */
void CDisplayWindow::updatePopupMenu() {
    /// \todo Verify this should be empty and comment.
}


///** Returns the installed popup menu. */
QMenu* CDisplayWindow::popup() {
    // qWarning("CReadWindow::popup()");
    if (!m_popupMenu) {
        m_popupMenu = new QMenu(this);
        connect(m_popupMenu, SIGNAL(aboutToShow()), this, SLOT(updatePopupMenu()));
        if (displayWidget()) {
            displayWidget()->installPopup(m_popupMenu);
        }
        /*   else {
            qWarning("CDisplayWindow:: can't instal popup menu");
            }*/
    }
    return m_popupMenu;
}

/** Sets the display widget used by this display window. */
void CDisplayWindow::setDisplayWidget( CDisplay* newDisplay ) {
    m_displayWidget = newDisplay;
}

void CDisplayWindow::closeEvent(QCloseEvent* e) {
    if (!queryClose()) {
        e->ignore();
    }
    else {
        e->accept();
    }
}

void CDisplayWindow::slotSearchInModules() {
    Search::CSearchDialog::openDialog(modules());
}

void CDisplayWindow::printAll() {
    m_displayWidget->connectionsProxy()->printAll( m_displayOptions, m_filterOptions);
}

void CDisplayWindow::printAnchorWithText() {
    m_displayWidget->connectionsProxy()->printAnchorWithText( m_displayOptions, m_filterOptions);
}

void CDisplayWindow::setFocusKeyChooser() {
    keyChooser()->setFocus();
}

