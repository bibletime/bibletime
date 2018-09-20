/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
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
#include "frontend/bibletime.h"
#include "frontend/bibletimeapp.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/cdisplay.h"
#include "frontend/displaywindow/bttoolbarpopupaction.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/cresmgr.h"


namespace {

inline QWidget * getProfileWindow(QWidget * w) {
    for (; w; w = w->parentWidget())
        if (QMdiSubWindow * const sw = qobject_cast<QMdiSubWindow *>(w))
            return sw;
    return nullptr;
}

}


CDisplayWindow::CDisplayWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent)
    : QMainWindow(parent),
      m_actionCollection(nullptr),
      m_mdi(parent),
      m_keyChooser(nullptr),
      m_swordKey(nullptr),
      m_isReady(false),
      m_moduleChooserBar(nullptr),
      m_mainToolBar(nullptr),
      m_buttonsToolBar(nullptr),
      m_formatToolBar(nullptr),
      m_headerBar(nullptr),
      m_popupMenu(nullptr),
      m_displayWidget(nullptr),
      m_history(nullptr) {

    // Cannot delete on close. QMdiSubWindow and this window work
    // as pairs. They must be deleted in a specific order.
    // QMdiSubWindow handles this procedure.
    //setAttribute(Qt::WA_DeleteOnClose);

    setObjectName("CDisplayWindow");
    m_actionCollection = new BtActionCollection(this);
    setModules(modules);

    // Connect this to the backend module list changes
    BT_CONNECT(CSwordBackend::instance(),
               SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
               SLOT(reload(CSwordBackend::SetupChangedReason)));
    BibleTime* mainwindow = btMainWindow();
    BT_CONNECT(mainwindow, SIGNAL(toggledTextWindowHeader(bool)),
               SLOT(slotShowHeader(bool)));
    BT_CONNECT(mainwindow, SIGNAL(toggledTextWindowNavigator(bool)),
               SLOT(slotShowNavigator(bool)));
    BT_CONNECT(mainwindow, SIGNAL(toggledTextWindowToolButtons(bool)),
               SLOT(slotShowToolButtons(bool)));
    BT_CONNECT(mainwindow, SIGNAL(toggledTextWindowModuleChooser(bool)),
               SLOT(slotShowModuleChooser(bool)));
    BT_CONNECT(mainwindow, SIGNAL(toggledTextWindowFormatToolbar(bool)),
               SLOT(slotShowFormatToolBar(bool)));
}

CDisplayWindow::~CDisplayWindow() {
    delete m_swordKey;
    m_swordKey = nullptr;
}

BibleTime* CDisplayWindow::btMainWindow() {
    return dynamic_cast<BibleTime*>(m_mdi->parent()->parent());
}

void CDisplayWindow::setToolBarsHidden() {
    // Hide current window toolbars
    if (m_mainToolBar)
        m_mainToolBar->setHidden(true);
    if (m_buttonsToolBar)
        m_buttonsToolBar->setHidden(true);
    if (m_moduleChooserBar)
        m_moduleChooserBar->setHidden(true);
    if (m_formatToolBar)
        m_formatToolBar->setHidden(true);
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
const BtConstModuleList CDisplayWindow::modules() const {
    return CSwordBackend::instance()->getConstPointerList(m_modules);
}

/** Store the settings of this window in the given CProfileWindow object. */
void CDisplayWindow::storeProfileSettings(QString const & windowGroup) const {
    BtConfig & conf = btConfig();

    conf.beginGroup(windowGroup);

    QWidget const * const w = getProfileWindow(parentWidget());
    BT_ASSERT(w);

    /**
      \note We don't use saveGeometry/restoreGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect(w->x(), w->y(), w->width(), w->height());
    conf.setSessionValue<QRect>("windowRect", rect);
    conf.setSessionValue<bool>("staysOnTop",
                               w->windowFlags() & Qt::WindowStaysOnTopHint);
    conf.setSessionValue<bool>("staysOnBottom",
                               w->windowFlags() & Qt::WindowStaysOnBottomHint);
    conf.setSessionValue("maximized", w->isMaximized());

    bool hasFocus = (w == dynamic_cast<CDisplayWindow *>(mdi()->activeSubWindow()));
    conf.setSessionValue("hasFocus", hasFocus);
    // conf.setSessionValue("type", static_cast<int>(modules().first()->type()));

    // Save current key:
    if (CSwordKey * const k = key()) {
        if (sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(k)) {
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
    conf.setSessionValue("modules", m_modules);

    // Default for "not a write window":
    conf.setSessionValue("writeWindowType", int(0));

    conf.endGroup();
}

void CDisplayWindow::applyProfileSettings(const QString & windowGroup) {
    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    setUpdatesEnabled(false);

    QWidget * const w = getProfileWindow(parentWidget());
    BT_ASSERT(w);

    /**
      \note We don't use restoreGeometry/saveGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect = conf.sessionValue<QRect>("windowRect");
    w->resize(rect.width(), rect.height());
    w->move(rect.x(), rect.y());
    if (conf.sessionValue<bool>("staysOnTop", false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnTopHint);
    if (conf.sessionValue<bool>("staysOnBottom", false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnBottomHint);
    if (conf.sessionValue<bool>("maximized"))
        w->showMaximized();

    setUpdatesEnabled(true);
    conf.endGroup();
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
    QAction* actn = new QAction(QIcon(), tr("Select all"), a);
    actn->setShortcut(QKeySequence::SelectAll);
    a->addAction("selectAll", actn);

    actn = new QAction(QIcon(), tr("Copy"), a);
    actn->setShortcut(QKeySequence::Copy);
    a->addAction("copySelectedText", actn);

    actn = new QAction(QIcon(), tr("Find..."), a);
    actn->setShortcut(QKeySequence::Find);
    a->addAction("findText", actn);

    actn = new QAction(QIcon(), tr("Change location"), a);
    actn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    a->addAction("openLocation", actn);

    actn = new QAction(QIcon(), tr("Page down"), a);
    actn->setShortcut(QKeySequence(Qt::Key_PageDown));
    a->addAction("pageDown", actn);

    actn = new QAction(QIcon(), tr("Page up"), a);
    actn->setShortcut(QKeySequence(Qt::Key_PageUp));
    a->addAction("pageUp", actn);

    actn = new QAction(CResMgr::displaywindows::general::search::icon(),
                       tr("Search with works of this window"), a);
    actn->setShortcut(CResMgr::displaywindows::general::search::accel);
    a->addAction(CResMgr::displaywindows::general::search::actionName, actn);

    BtToolBarPopupAction* action = new BtToolBarPopupAction(
                CResMgr::displaywindows::general::backInHistory::icon(),
                tr("Back in history"),
                a
                );
    action->setShortcut(CResMgr::displaywindows::general::backInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::backInHistory::actionName, action);

    action = new BtToolBarPopupAction(
                CResMgr::displaywindows::general::forwardInHistory::icon(),
                tr("Forward in history"),
                a
                );
    action->setShortcut(CResMgr::displaywindows::general::forwardInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::forwardInHistory::actionName, action);
}

void CDisplayWindow::initActions() {
    BtActionCollection* ac = actionCollection();

    insertKeyboardActions(ac);

    namespace DWG = CResMgr::displaywindows::general;
    initAction(DWG::search::actionName,
               this,
               &CDisplayWindow::slotSearchInModules);
    initAddAction("openLocation", this, &CDisplayWindow::setFocusKeyChooser);
    initAddAction("pageDown", this, &CDisplayWindow::pageDown);
    initAddAction("pageUp", this, &CDisplayWindow::pageUp);

    CDisplayConnections * const conn = displayWidget()->connectionsProxy();
    initAddAction("selectAll",
                  conn,
                  &CDisplayConnections::selectAll);
    initAddAction("copySelectedText",
                  conn,
                  &CDisplayConnections::copySelection);
    initAddAction("findText",
                  conn,
                  &CDisplayConnections::openFindTextDialog);
    initAddAction(DWG::backInHistory::actionName,
                  keyChooser()->history(),
                  &BTHistory::back);
    initAddAction(DWG::forwardInHistory::actionName,
                  keyChooser()->history(),
                  &BTHistory::fw);

    ac->readShortcuts("Displaywindow shortcuts");
}

/** Refresh the settings of this window. */
void CDisplayWindow::reload(CSwordBackend::SetupChangedReason) {
    { // First make sure all used Sword modules are still present:
        CSwordBackend & backend = *(CSwordBackend::instance());
        QMutableStringListIterator it(m_modules);
        while (it.hasNext())
            if (!backend.findModuleByName(it.next()))
                it.remove();
    }

    if (m_modules.isEmpty()) {
        close();
        return;
    }

    if (CKeyChooser * const kc = keyChooser())
        kc->setModules(modules(), false);

    lookup();

    m_actionCollection->readShortcuts("Displaywindow shortcuts");
    m_actionCollection->readShortcuts("Readwindow shortcuts");
    emit sigModuleListSet(m_modules);
}

void CDisplayWindow::slotAddModule(int index, QString module) {
    m_modules.insert(index, module);
    displayWidget()->setModules(m_modules);
    lookup();
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotReplaceModule(int index, QString newModule) {
    m_modules.replace(index, newModule);
    displayWidget()->setModules(m_modules);
    lookup();
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotRemoveModule(int index) {
    m_modules.removeAt(index);
    displayWidget()->setModules(m_modules);
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
    BT_ASSERT(key);
    m_swordKey = key;
}

BTHistory* CDisplayWindow::history() {
    if (m_history == nullptr)
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
        bar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true));
    }
}

/** Setup the module header of text area. */
void CDisplayWindow::setHeaderBar( QToolBar* header ) {
    m_headerBar = header;
    header->setMovable(false);
    header->setWindowTitle(tr("Text area header"));
    header->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowHeaders", true));
}

/** Sets the modules. */
void CDisplayWindow::setModules( const QList<CSwordModuleInfo*>& newModules ) {
    m_modules.clear();

    Q_FOREACH(CSwordModuleInfo const * const mod, newModules)
        m_modules.append(mod->name());
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
    if (!btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true))
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

    m_isReady = true;
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
    prepareToolBar(bar, tr("Navigation"), btConfig().sessionValue<bool>("GUI/showTextWindowNavigator", true));
    m_mainToolBar = bar;
}

/** Setup the Tools toolbar. */
void CDisplayWindow::setButtonsToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Tool"), btConfig().sessionValue<bool>("GUI/showTextWindowToolButtons", true));
    m_buttonsToolBar = bar;
}

/** Setup the Format toolbar. */
void CDisplayWindow::setFormatToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Format"), true );
    m_formatToolBar = bar;
}

/** Sets the display settings button. */
void CDisplayWindow::setDisplaySettingsButton(BtDisplaySettingsButton *button) {
    BT_CONNECT(this,   SIGNAL(sigDisplayOptionsChanged(DisplayOptions const &)),
               button, SLOT(setDisplayOptions(DisplayOptions const &)));
    BT_CONNECT(this,   SIGNAL(sigFilterOptionsChanged(FilterOptions const &)),
               button, SLOT(setFilterOptions(FilterOptions const &)));
    BT_CONNECT(this,   SIGNAL(sigModulesChanged(BtConstModuleList const &)),
               button, SLOT(setModules(BtConstModuleList const &)));

    button->setDisplayOptions(displayOptions(), false);
    button->setFilterOptions(filterOptions(), false);
    button->setModules(modules());

    BT_CONNECT(button, SIGNAL(sigFilterOptionsChanged(FilterOptions const &)),
               this,   SLOT(setFilterOptions(FilterOptions const &)));
    BT_CONNECT(button, SIGNAL(sigDisplayOptionsChanged(DisplayOptions const &)),
               this,   SLOT(setDisplayOptions(DisplayOptions const &)));
    BT_CONNECT(button, SIGNAL(sigChanged()),
               this,   SLOT(lookup()));
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
        BibleTime *mainWindow = btMainWindow();
        BT_ASSERT(mainWindow);
        mainWindow->createReadDisplayWindow(m, keyName);
    }
}

void CDisplayWindow::lookupKey( const QString& keyName ) {
    /* This function is called for example after a bookmark was dropped on this window
    */
    BT_ASSERT(modules().first());

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
        BT_CONNECT(m_popupMenu, SIGNAL(aboutToShow()),
                   this,        SLOT(updatePopupMenu()));
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
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true)) {
        keyChooser()->setFocus();
    } else {
        CKeyChooser* mainWinKeyChooser = btMainWindow()->keyChooser();
        if (mainWinKeyChooser)
            mainWinKeyChooser->setFocus();
    }
}

void CDisplayWindow::pageDown() {

}

void CDisplayWindow::pageUp() {

}
