/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cdisplaywindow.h"

#include <QClipboard>
#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QMenu>
#include <QStringList>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/keys/cswordkey.h"
#include "../../backend/rendering/cdisplayrendering.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bibletime.h"
#include "../bibletimeapp.h"
#include "../btcopybyreferencesdialog.h"
#include "../cexportmanager.h"
#include "../cmdiarea.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../display/modelview/btquickwidget.h"
#include "../keychooser/ckeychooser.h"
#include "../keychooser/bthistory.h"
#include "../searchdialog/csearchdialog.h"
#include "bttoolbarpopupaction.h"
#include "btmodulechooserbar.h"
#include "btdisplaysettingsbutton.h"
#include "bttextwindowheader.h"


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
    BT_CONNECT(CSwordBackend::instance(), &CSwordBackend::sigSwordSetupChanged,
               this,                      &CDisplayWindow::reload);
    BibleTime* mainwindow = btMainWindow();
    BT_CONNECT(mainwindow, &BibleTime::toggledTextWindowHeader,
               this, // Needed
               [this](bool const show){
                   if (auto * const b = headerBar())
                       b->setVisible(show);
               });
    BT_CONNECT(mainwindow, &BibleTime::toggledTextWindowNavigator,
               this, // Needed
               [this](bool const show) {
                   if (auto * const b = mainToolBar())
                       b->setVisible(show);
               });
    BT_CONNECT(mainwindow, &BibleTime::toggledTextWindowToolButtons,
               this, // Needed
               [this](bool const show) {
                   if (auto * const b = buttonsToolBar())
                       b->setVisible(show);
               });
    BT_CONNECT(mainwindow, &BibleTime::toggledTextWindowModuleChooser,
               this, // Needed
               [this](bool const show) {
                   if (auto * const b = moduleChooserBar())
                       b->setVisible(show);
               });
    m_swordKey = CSwordKey::createInstance(modules.first());
}

CDisplayWindow::~CDisplayWindow() {
    delete m_swordKey;
    m_swordKey = nullptr;
}

CSwordModuleInfo::ModuleType CDisplayWindow::moduleType() const
{ return CSwordModuleInfo::Lexicon; }

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
}
void CDisplayWindow::clearMainWindowToolBars() {
    // Clear main window toolbars, except for works toolbar
    btMainWindow()->navToolBar()->clear();
    btMainWindow()->toolsToolBar()->clear();
}

void CDisplayWindow::windowActivated() {
    clearMainWindowToolBars();
    setupMainWindowToolBars();
}

/** Returns the right window caption. */
QString CDisplayWindow::windowCaption() {
    if (!m_modules.count()) {
        return QString();
    }

    return QString(key()->key()).append(" (").append(m_modules.join(" | ")).append(")");
}

/** Returns the used modules as a pointer list */
BtConstModuleList CDisplayWindow::modules() const
{ return CSwordBackend::instance()->getConstPointerList(m_modules); }

/** Store the settings of this window in the given CProfileWindow object. */
void CDisplayWindow::storeProfileSettings(BtConfigCore & conf) const {
    QWidget const * const w = getProfileWindow(parentWidget());
    BT_ASSERT(w);

    /**
      \note We don't use saveGeometry/restoreGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect(w->x(), w->y(), w->width(), w->height());
    conf.setValue<QRect>("windowRect", rect);
    conf.setValue<bool>("staysOnTop",
                        w->windowFlags() & Qt::WindowStaysOnTopHint);
    conf.setValue<bool>("staysOnBottom",
                        w->windowFlags() & Qt::WindowStaysOnBottomHint);
    conf.setValue("maximized", w->isMaximized());

    bool hasFocus = (w == dynamic_cast<CDisplayWindow *>(mdi()->activeSubWindow()));
    conf.setValue("hasFocus", hasFocus);
    // conf.setSessionValue("type", static_cast<int>(modules().first()->type()));

    // Save current key:
    if (CSwordKey * const k = key()) {
        if (sword::VerseKey * const vk = dynamic_cast<sword::VerseKey *>(k)) {
            // Save keys in english only:
            const QString oldLang = QString::fromLatin1(vk->getLocale());
            vk->setLocale("en");
            conf.setValue("key", k->key());
            vk->setLocale(oldLang.toLatin1());
        } else {
            conf.setValue("key", k->key());
        }
    }

    // Save list of modules:
    conf.setValue("modules", m_modules);

    // Default for "not a write window":
    conf.setValue("writeWindowType", int(0));
}

void CDisplayWindow::applyProfileSettings(BtConfigCore const & conf) {
    setUpdatesEnabled(false);

    QWidget * const w = getProfileWindow(parentWidget());
    BT_ASSERT(w);

    /**
      \note We don't use restoreGeometry/saveGeometry for MDI subwindows,
            because they give slightly incorrect results with some window
            managers. Might be related to Qt bug QTBUG-7634.
    */
    const QRect rect = conf.value<QRect>("windowRect");
    w->resize(rect.width(), rect.height());
    w->move(rect.x(), rect.y());
    if (conf.value<bool>("staysOnTop", false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnTopHint);
    if (conf.value<bool>("staysOnBottom", false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnBottomHint);
    if (conf.value<bool>("maximized"))
        w->showMaximized();

    setUpdatesEnabled(true);
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
    auto * actn = new QAction(QIcon(), tr("Copy"), a);
    actn->setShortcut(QKeySequence::Copy);
    a->addAction("copySelectedText", actn);

    actn = new QAction(QIcon(), tr("Copy by references..."), a);
    actn->setShortcut(Qt::CTRL + Qt::Key_R);
    a->addAction("copyByReferences", actn);

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

    actn = new QAction(tr("Copy reference only"), a);
    a->addAction("copyReferenceOnly", actn);

    actn = new QAction(tr("Save entry as HTML"), a);
    a->addAction("saveHtml", actn);

    actn = new QAction(tr("Print reference only"), a);
    a->addAction("printReferenceOnly", actn);

    actn = new QAction(tr("Entry with text"), a);
    a->addAction("copyEntryWithText", actn);

    actn = new QAction(tr("Entry as plain text"), a);
    a->addAction("saveEntryAsPlain", actn);

    actn = new QAction(tr("Entry with text"), a);
    a->addAction("printEntryWithText", actn);

    actn = new QAction( /* QIcon(CResMgr::displaywindows::general::findStrongs::icon), */ tr("Strong's Search"), a);
    actn->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
    a->addAction(CResMgr::displaywindows::general::findStrongs::actionName, actn);
}

void CDisplayWindow::initActions() {
    insertKeyboardActions(m_actionCollection);

    namespace DWG = CResMgr::displaywindows::general;
    initAction(DWG::search::actionName,
               this,
               &CDisplayWindow::slotSearchInModules);
    initAddAction("openLocation", this, &CDisplayWindow::setFocusKeyChooser);
    initAddAction("pageDown", this, &CDisplayWindow::pageDown);
    initAddAction("pageUp", this, &CDisplayWindow::pageUp);

    auto * const dw = displayWidget();
    initAddAction("copySelectedText",
                  dw,
                  &BtModelViewReadDisplay::copySelectedText);
    initAddAction("copyByReferences",
                  dw,
                  &BtModelViewReadDisplay::copyByReferences);
    initAddAction("findText",
                  dw,
                  &BtModelViewReadDisplay::openFindTextDialog);
    initAddAction(DWG::backInHistory::actionName,
                  keyChooser()->history(),
                  &BTHistory::back);
    initAddAction(DWG::forwardInHistory::actionName,
                  keyChooser()->history(),
                  &BTHistory::fw);

    auto * const ac = m_actionCollection;
    m_actions.backInHistory =
            &ac->actionAs<BtToolBarPopupAction>(
                CResMgr::displaywindows::general::backInHistory::actionName);
    addAction(m_actions.backInHistory);

    m_actions.forwardInHistory =
            &ac->actionAs<BtToolBarPopupAction>(
                CResMgr::displaywindows::general::forwardInHistory::actionName);
    addAction(m_actions.forwardInHistory);

    m_actions.findText = &ac->action("findText");

    m_actions.findStrongs =
            &initAddAction(
                CResMgr::displaywindows::general::findStrongs::actionName,
                this,
                &CDisplayWindow::openSearchStrongsDialog);

    m_actions.copy.reference =
            &initAddAction("copyReferenceOnly",
                           dw,
                           &BtModelViewReadDisplay::copyAnchorOnly);

    m_actions.copy.entry = &initAddAction("copyEntryWithText",
                                          dw,
                                          &BtModelViewReadDisplay::copyAll);

    m_actions.copy.selectedText = &ac->action("copySelectedText");

    m_actions.copy.byReferences = &ac->action("copyByReferences");

    m_actions.save.entryAsPlain = &initAddAction("saveEntryAsPlain",
                                                 this,
                                                 &CDisplayWindow::saveAsPlain);

    m_actions.save.entryAsHTML = &initAddAction("saveHtml",
                                                this,
                                                &CDisplayWindow::saveAsHTML);

    m_actions.print.reference =
            &initAddAction("printReferenceOnly",
                           this,
                           &CDisplayWindow::printAnchorWithText);
    addAction(m_actions.print.reference);

    m_actions.print.entry = &initAddAction("printEntryWithText",
                                           this,
                                           &CDisplayWindow::printAll);

    // init with the user defined settings
    m_actionCollection->readShortcuts("Displaywindow shortcuts");
}

void CDisplayWindow::initConnections() {
    BT_ASSERT(keyChooser());

    BT_CONNECT(keyChooser(), &CKeyChooser::keyChanged,
               this,         &CDisplayWindow::lookupSwordKey);
    BT_CONNECT(history(), &BTHistory::historyChanged,
               [this](bool const backEnabled, bool const fwEnabled) {
                   BT_ASSERT(m_actions.backInHistory);
                   BT_ASSERT(keyChooser());

                   m_actions.backInHistory->setEnabled(backEnabled);
                   m_actions.forwardInHistory->setEnabled(fwEnabled);
               });

    //connect the history actions to the right slots
    BT_CONNECT(m_actions.backInHistory->popupMenu(), &QMenu::aboutToShow,
               this, // Needed
               [this]{
                   QMenu * menu = m_actions.backInHistory->popupMenu();
                   menu->clear();
                   for (auto * const actionPtr
                        : keyChooser()->history()->getBackList())
                       menu->addAction(actionPtr);
               });
    BT_CONNECT(m_actions.backInHistory->popupMenu(), &QMenu::triggered,
               keyChooser()->history(), &BTHistory::move);
    BT_CONNECT(m_actions.forwardInHistory->popupMenu(), &QMenu::aboutToShow,
               this, // Needed
               [this]{
                   QMenu* menu = m_actions.forwardInHistory->popupMenu();
                   menu->clear();
                   for (auto * const actionPtr
                        : keyChooser()->history()->getFwList())
                       menu->addAction(actionPtr);
               });
    BT_CONNECT(m_actions.forwardInHistory->popupMenu(), &QMenu::triggered,
               keyChooser()->history(), &BTHistory::move);
}

void CDisplayWindow::initView() {
    // Create display widget for this window
    auto readDisplay = new BtModelViewReadDisplay(this, this);
    setDisplayWidget(readDisplay);
    setCentralWidget( displayWidget()->view() );
    readDisplay->setModules(getModuleList());
    setWindowIcon(util::tool::getIconForModule(modules().first()));

    // Create the Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    addToolBar(mainToolBar());

    // Create keychooser
    setKeyChooser( CKeyChooser::createInstance(modules(), history(), key(), mainToolBar()) );

    // Create the Works toolbar
    setModuleChooserBar( new BtModuleChooserBar(this));
    moduleChooserBar()->setModules(getModuleList(), modules().first()->type(), this);
    addToolBar(moduleChooserBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    addToolBar(buttonsToolBar());

    // Create the Text Header toolbar
    addToolBarBreak();
    setHeaderBar(new QToolBar(this));
    addToolBar(headerBar());
}

void CDisplayWindow::initToolbars() {
    //Navigation toolbar
    BT_ASSERT(m_actions.backInHistory);
    mainToolBar()->addWidget(keyChooser());
    mainToolBar()->addAction(m_actions.backInHistory); //1st button
    mainToolBar()->addAction(m_actions.forwardInHistory); //2nd button

    //Tools toolbar
    buttonsToolBar()->addAction(
                &actionCollection()->action(
                    CResMgr::displaywindows::general::search::actionName));

    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    buttonsToolBar()->addWidget(button);

    // Text Header toolbar
    BtTextWindowHeader *h = new BtTextWindowHeader(modules().first()->type(), getModuleList(), this);
    headerBar()->addWidget(h);
}

void CDisplayWindow::setupPopupMenu() {
    popup()->setTitle(tr("Lexicon window"));
    popup()->setIcon(util::tool::getIconForModule(modules().first()));
    popup()->addAction(m_actions.findText);
    popup()->addAction(m_actions.findStrongs);
    popup()->addSeparator();

    m_actions.copyMenu = new QMenu(tr("Copy..."), popup());
    m_actions.copyMenu->addAction(m_actions.copy.selectedText);
    m_actions.copyMenu->addAction(m_actions.copy.byReferences);
    m_actions.copyMenu->addSeparator();
    m_actions.copyMenu->addAction(m_actions.copy.reference);
    m_actions.copyMenu->addAction(m_actions.copy.entry);
    popup()->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(
                tr("Save..."),
                popup()
                );
    m_actions.saveMenu->addAction(m_actions.save.entryAsPlain);
    m_actions.saveMenu->addAction(m_actions.save.entryAsHTML);

    // Save raw HTML action for debugging purposes
    if (btApp->debugMode()) {
        QAction* debugAction = new QAction("Raw HTML", this);
        BT_CONNECT(debugAction, &QAction::triggered,
                   this,        &CDisplayWindow::saveRawHTML);
        m_actions.saveMenu->addAction(debugAction);
    } // end of Save Raw HTML

    popup()->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(
                tr("Print..."),
                popup()
                );
    m_actions.printMenu->addAction(m_actions.print.reference);
    m_actions.printMenu->addAction(m_actions.print.entry);
    popup()->addMenu(m_actions.printMenu);
}

void CDisplayWindow::updatePopupMenu() {
    //enable the action depending on the supported module features

    auto const & display = *displayWidget();

    m_actions.findStrongs->setEnabled(!display.getCurrentNodeInfo().isNull());

    bool const hasActiveAnchor = display.hasActiveAnchor();
    m_actions.copy.reference->setEnabled(hasActiveAnchor);

    m_actions.print.reference->setEnabled(hasActiveAnchor);

    m_actions.copy.selectedText->setEnabled(hasSelectedText());
}

void CDisplayWindow::setupMainWindowToolBars() {
    // Navigation toolbar
    QString keyReference = key()->key();
    CKeyChooser* keyChooser = CKeyChooser::createInstance(modules(), history(), key(), btMainWindow()->navToolBar() );
    keyChooser->key()->setKey(keyReference);
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    BT_CONNECT(keyChooser, &CKeyChooser::keyChanged,
               this,       &CDisplayWindow::lookupSwordKey);
    BT_CONNECT(this,       &CDisplayWindow::sigKeyChanged,
               keyChooser, &CKeyChooser::updateKey);
    btMainWindow()->navToolBar()->addAction(m_actions.backInHistory); //1st button
    btMainWindow()->navToolBar()->addAction(m_actions.forwardInHistory); //2nd button

    // Works toolbar
    btMainWindow()->worksToolBar()->setModules(getModuleList(), modules().first()->type(), this);

    // Tools toolbar
    btMainWindow()->toolsToolBar()->addAction(
                &actionCollection()->action(
                    CResMgr::displaywindows::general::search::actionName));
    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    btMainWindow()->toolsToolBar()->addWidget(button);
}

void CDisplayWindow::copySelectedText() {
    if (auto * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        QGuiApplication::clipboard()->setText(
                    v->qmlInterface()->getSelectedText());
}

void CDisplayWindow::copyByReferences() {
    if (auto const * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
    {
        auto const & qml = *v->qmlInterface();
        BtCopyByReferencesDialog dlg(modules(),
                                     history(),
                                     key(),
                                     qml.textModel(),
                                     this);
        if (dlg.exec() != QDialog::Accepted)
            return;

        auto const & m = *modules().at(dlg.getColumn());
        if (m.type() == CSwordModuleInfo::Bible
            || m.type() == CSwordModuleInfo::Commentary)
        {
            qml.copyVerseRange(dlg.getReference1(), dlg.getReference2(), &m);
        } else {
            qml.copyRange(dlg.getIndex1(), dlg.getIndex2());
        }
    }
}

bool CDisplayWindow::hasSelectedText() {
    if (auto const * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        return v->qmlInterface()->hasSelectedText();
    return false;
}

void CDisplayWindow::copyDisplayedText()
{ CExportManager().copyKey(key(), CExportManager::Text, true); }

int CDisplayWindow::getSelectedColumn() const {
    if (auto const * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        return v->quickWidget()->getSelectedColumn();
    return 0;
}

int CDisplayWindow::getFirstSelectedIndex() const {
    if (auto const * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        return v->quickWidget()->getFirstSelectedIndex();
    return 0;
}

int CDisplayWindow::getLastSelectedIndex() const {
    if (auto const * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        return v->quickWidget()->getLastSelectedIndex();
    return 0;
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
    } else {
        displayWidget()->reloadModules();

        if (CKeyChooser * const kc = keyChooser())
            kc->setModules(modules(), false);

        lookup();

        m_actionCollection->readShortcuts("Displaywindow shortcuts");
        m_actionCollection->readShortcuts("Readwindow shortcuts");
        Q_EMIT sigModuleListSet(m_modules);
    }

    if (auto * const dw =
            dynamic_cast<BtModelViewReadDisplay *>(displayWidget()))
        dw->settingsChanged();

    actionCollection()->readShortcuts("Lexicon shortcuts");
}

void CDisplayWindow::slotAddModule(int index, QString module) {
    BT_ASSERT(index <= m_modules.size());
    m_modules.insert(index, module);
    displayWidget()->setModules(m_modules);
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged();
}

void CDisplayWindow::slotReplaceModule(int index, QString newModule) {
    BT_ASSERT(index < m_modules.size());
    m_modules.replace(index, newModule);
    displayWidget()->setModules(m_modules);
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged();
}

void CDisplayWindow::slotRemoveModule(int index) {
    BT_ASSERT(index < m_modules.size());
    m_modules.removeAt(index);
    displayWidget()->setModules(m_modules);
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged();
}

/** Sets the keychooser widget for this display window. */
void CDisplayWindow::setKeyChooser( CKeyChooser* ck ) {
    m_keyChooser = ck;
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
        Q_EMIT sigModulesChanged(modules());
        key()->setModule(modules().first());
        keyChooser()->setModules(modules());
    }
}

void CDisplayWindow::lookupSwordKey(CSwordKey * newKey) {
    BT_ASSERT(newKey);

    if (!isReady() || !newKey || modules().empty() || !modules().first())
        return;

    if (key() != newKey)
        key()->setKey(newKey->key());

    /// \todo next-TODO how about options?
    auto * const display = modules().first()->getDisplay();
    BT_ASSERT(display);

    displayWidget()->setDisplayOptions(displayOptions());
    displayWidget()->setFilterOptions(filterOptions());
    displayWidget()->scrollToKey(newKey);
    BibleTime::instance()->autoScrollStop();

    setWindowTitle(windowCaption());
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
        bar->setVisible(btConfig().session().value<bool>("GUI/showTextWindowModuleSelectorButtons", true));
    }
}

/** Setup the module header of text area. */
void CDisplayWindow::setHeaderBar( QToolBar* header ) {
    m_headerBar = header;
    header->setMovable(false);
    header->setWindowTitle(tr("Text area header"));
    header->setVisible(btConfig().session().value<bool>("GUI/showTextWindowHeaders", true));
}

/** Sets the modules. */
void CDisplayWindow::setModules( const QList<CSwordModuleInfo*>& newModules ) {
    m_modules.clear();

    for (auto const * const mod : newModules)
        m_modules.append(mod->name());
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
bool CDisplayWindow::init() {
    initView();
    setMinimumSize( 100, 100 );

    auto const & conf = btConfig();

    setWindowTitle(windowCaption());
    //setup focus stuff.
    setFocusPolicy(Qt::ClickFocus);
    parentWidget()->setFocusPolicy(Qt::ClickFocus);
    initActions();
    initToolbars();
    if (!conf.session().value<bool>("GUI/showToolbarsInEachWindow", true))
        setToolBarsHidden();
    btMainWindow()->clearMdiToolBars();
    clearMainWindowToolBars();
    initConnections();
    setupPopupMenu();

    m_filterOptions = conf.getFilterOptions();
    m_displayOptions = conf.getDisplayOptions();
    Q_EMIT sigDisplayOptionsChanged(m_displayOptions);
    Q_EMIT sigFilterOptionsChanged(m_filterOptions);
    Q_EMIT sigModulesChanged(modules());

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
    prepareToolBar(bar, tr("Navigation"), btConfig().session().value<bool>("GUI/showTextWindowNavigator", true));
    m_mainToolBar = bar;
}

/** Setup the Tools toolbar. */
void CDisplayWindow::setButtonsToolBar( QToolBar* bar ) {
    prepareToolBar(bar, tr("Tool"), btConfig().session().value<bool>("GUI/showTextWindowToolButtons", true));
    m_buttonsToolBar = bar;
}

/** Sets the display settings button. */
void CDisplayWindow::setDisplaySettingsButton(BtDisplaySettingsButton *button) {
    BT_CONNECT(this,   &CDisplayWindow::sigDisplayOptionsChanged,
               button, &BtDisplaySettingsButton::setDisplayOptions);
    BT_CONNECT(this,   &CDisplayWindow::sigFilterOptionsChanged,
               button, &BtDisplaySettingsButton::setFilterOptions);
    BT_CONNECT(this,   &CDisplayWindow::sigModulesChanged,
               button, &BtDisplaySettingsButton::setModules);

    button->setDisplayOptionsNoRepopulate(displayOptions());
    button->setFilterOptionsNoRepopulate(filterOptions());
    button->setModules(modules());

    BT_CONNECT(button, &BtDisplaySettingsButton::sigFilterOptionsChanged,
               this, // Needed
               [this](FilterOptions const & filterOptions) {
                   m_filterOptions = filterOptions;
                   Q_EMIT sigFilterOptionsChanged(m_filterOptions);
               });
    BT_CONNECT(button, &BtDisplaySettingsButton::sigDisplayOptionsChanged,
               this, // Needed
               [this](DisplayOptions const & displayOptions) {
                   m_displayOptions = displayOptions;
                   displayWidget()->setDisplayOptions(displayOptions);
                   Q_EMIT sigDisplayOptionsChanged(m_displayOptions);
               });
    BT_CONNECT(button, &BtDisplaySettingsButton::sigChanged,
               this,   &CDisplayWindow::lookup);
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
        Q_EMIT sigKeyChanged(key());
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

///** Returns the installed popup menu. */
QMenu* CDisplayWindow::popup() {
    // qWarning("CDisplayWindow::popup()");
    if (!m_popupMenu) {
        m_popupMenu = new QMenu(this);
        BT_CONNECT(m_popupMenu, &QMenu::aboutToShow,
                   [this]{ updatePopupMenu(); });
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
void CDisplayWindow::setDisplayWidget(BtModelViewReadDisplay * newDisplay) {
    // Lets be orwellianly paranoid here:
    BT_ASSERT(newDisplay);

    m_displayWidget = newDisplay;
    m_readDisplayWidget = newDisplay;

    BT_CONNECT(btMainWindow(), &BibleTime::colorThemeChanged,
               this,           &CDisplayWindow::colorThemeChangedSlot);
}

void CDisplayWindow::slotSearchInModules() {
    Search::CSearchDialog::openDialog(modules());
}

void CDisplayWindow::printAll()
{ m_displayWidget->printAll(m_displayOptions, m_filterOptions); }

void CDisplayWindow::printAnchorWithText()
{ m_displayWidget->printAnchorWithText(m_displayOptions, m_filterOptions); }

void CDisplayWindow::setFocusKeyChooser() {
    if (btConfig().session().value<bool>("GUI/showToolbarsInEachWindow", true)){
        keyChooser()->setFocus();
    } else {
        CKeyChooser* mainWinKeyChooser = btMainWindow()->keyChooser();
        if (mainWinKeyChooser)
            mainWinKeyChooser->setFocus();
    }
}

void CDisplayWindow::pageDown() {
    if (auto * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        v->pageDown();
}

void CDisplayWindow::pageUp() {
    if (auto * const v =
                dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget))
        v->pageUp();
}

/** This function saves the entry as html using the CExportMgr class. */
void CDisplayWindow::saveAsHTML() {
    CExportManager mgr(true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(key(), CExportManager::HTML, true, modules());
}

/** This function saves the entry as html using the CExportMgr class. */
void CDisplayWindow::saveAsPlain() {
    CExportManager mgr(true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(key(), CExportManager::Text, true, modules());
}

/** Saving the raw HTML for debugging purposes */
void CDisplayWindow::saveRawHTML() {
    auto const savefilename =
            QFileDialog::getSaveFileName(
                nullptr,
                QObject::tr("Save file"),
                "",
                QObject::tr("HTML files") + " (*.html *.htm);;"
                + QObject::tr("All files") + " (*)");
    if (savefilename.isEmpty()) return;
    BtModelViewReadDisplay* disp = dynamic_cast<BtModelViewReadDisplay*>(displayWidget());
    if (disp) {
        QFile file(savefilename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "saveRawHTML: could not open file" << savefilename;
            return;
        }
        QString source = disp->getCurrentSource();
        file.write(source.toUtf8());
        file.close();
        file.flush();
    }
}

void CDisplayWindow::openSearchStrongsDialog() {
    QString searchText;
    Q_FOREACH(QString const & strongNumber,
              displayWidget()->getCurrentNodeInfo().split(
                  '|',
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                  QString::SkipEmptyParts))
#else
                  Qt::SkipEmptyParts))
#endif
        searchText.append("strong:").append(strongNumber).append(' ');
    Search::CSearchDialog::openDialog(modules(), searchText, nullptr);
}

void CDisplayWindow::colorThemeChangedSlot() {
    if (BtModelViewReadDisplay * const v =
            dynamic_cast<BtModelViewReadDisplay *>(m_readDisplayWidget)) {
        v->qmlInterface()->changeColorTheme();
    }
}
