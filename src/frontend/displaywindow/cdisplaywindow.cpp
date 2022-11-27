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
#include "../cexportmanager.h"
#include "../cmdiarea.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../display/modelview/btqmlinterface.h"
#include "../display/modelview/btquickwidget.h"
#include "../keychooser/ckeychooser.h"
#include "../keychooser/bthistory.h"
#include "../messagedialog.h"
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


CDisplayWindow::CDisplayWindow(BtModuleList const & modules,
                               CMDIArea * const parent)
    : QMainWindow(parent)
    , m_actionCollection(new BtActionCollection(this))
    , m_mdi(parent)
    , m_modules(modules)
{
    setMinimumSize(100, 100);
    setFocusPolicy(Qt::ClickFocus);

    // Cannot delete on close. QMdiSubWindow and this window work
    // as pairs. They must be deleted in a specific order.
    // QMdiSubWindow handles this procedure.
    //setAttribute(Qt::WA_DeleteOnClose);

    // Connect this to the backend module list changes
    BT_CONNECT(CSwordBackend::instance(), &CSwordBackend::sigSwordSetupChanged,
               this,                      &CDisplayWindow::reload);

    m_swordKey = modules.first()->createKey();
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
    if (m_modules.isEmpty())
        return {};
    return QStringLiteral("%1 (%2)")
            .arg(m_swordKey->key(),
                 moduleNames().join(QStringLiteral(" | ")));
}

QStringList CDisplayWindow::moduleNames() const {
    QStringList moduleNames;
    moduleNames.reserve(m_modules.size());
    for (auto const module : m_modules)
        moduleNames.append(module->name());
    return moduleNames;
}

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
    conf.setValue<QRect>(QStringLiteral("windowRect"), rect);
    conf.setValue<bool>(QStringLiteral("staysOnTop"),
                        w->windowFlags() & Qt::WindowStaysOnTopHint);
    conf.setValue<bool>(QStringLiteral("staysOnBottom"),
                        w->windowFlags() & Qt::WindowStaysOnBottomHint);
    conf.setValue(QStringLiteral("maximized"), w->isMaximized());

    bool hasFocus = (w == dynamic_cast<CDisplayWindow *>(mdi()->activeSubWindow()));
    conf.setValue(QStringLiteral("hasFocus"), hasFocus);
    // conf.setSessionValue("type", static_cast<int>(m_modules.first()->type()));

    // Save current key:
    if (auto const * const k = m_swordKey)
        conf.setValue(QStringLiteral("key"), k->normalizedKey());

    // Save list of modules:
    conf.setValue(QStringLiteral("modules"), moduleNames());

    // Default for "not a write window":
    conf.setValue(QStringLiteral("writeWindowType"), int(0));
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
    const QRect rect = conf.value<QRect>(QStringLiteral("windowRect"));
    w->resize(rect.width(), rect.height());
    w->move(rect.x(), rect.y());
    if (conf.value<bool>(QStringLiteral("staysOnTop"), false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnTopHint);
    if (conf.value<bool>(QStringLiteral("staysOnBottom"), false))
        w->setWindowFlags(w->windowFlags() | Qt::WindowStaysOnBottomHint);
    if (conf.value<bool>(QStringLiteral("maximized")))
        w->showMaximized();

    setUpdatesEnabled(true);
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
    auto * actn = new QAction(QIcon(), tr("Copy"), a);
    actn->setShortcut(QKeySequence::Copy);
    a->addAction(QStringLiteral("copySelectedText"), actn);

    actn = new QAction(QIcon(), tr("Copy by references..."), a);
    actn->setShortcut(Qt::CTRL + Qt::Key_R);
    a->addAction(QStringLiteral("copyByReferences"), actn);

    actn = new QAction(QIcon(), tr("Find..."), a);
    actn->setShortcut(QKeySequence::Find);
    a->addAction(QStringLiteral("findText"), actn);

    actn = new QAction(QIcon(), tr("Change location"), a);
    actn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    a->addAction(QStringLiteral("openLocation"), actn);

    actn = new QAction(QIcon(), tr("Page down"), a);
    actn->setShortcut(QKeySequence(Qt::Key_PageDown));
    a->addAction(QStringLiteral("pageDown"), actn);

    actn = new QAction(QIcon(), tr("Page up"), a);
    actn->setShortcut(QKeySequence(Qt::Key_PageUp));
    a->addAction(QStringLiteral("pageUp"), actn);

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
    a->addAction(QStringLiteral("copyReferenceOnly"), actn);

    actn = new QAction(tr("Save entry as HTML"), a);
    a->addAction(QStringLiteral("saveHtml"), actn);

    actn = new QAction(tr("Print reference only"), a);
    a->addAction(QStringLiteral("printReferenceOnly"), actn);

    actn = new QAction(tr("Entry with text"), a);
    a->addAction(QStringLiteral("copyEntryWithText"), actn);

    actn = new QAction(tr("Entry as plain text"), a);
    a->addAction(QStringLiteral("saveEntryAsPlain"), actn);

    actn = new QAction(tr("Entry with text"), a);
    a->addAction(QStringLiteral("printEntryWithText"), actn);

    actn = new QAction( /* QIcon(CResMgr::displaywindows::general::findStrongs::icon), */ tr("Strong's Search"), a);
    actn->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
    a->addAction(CResMgr::displaywindows::general::findStrongs::actionName, actn);
}

void CDisplayWindow::initActions() {
    insertKeyboardActions(m_actionCollection);

    namespace DWG = CResMgr::displaywindows::general;
    initAction(DWG::search::actionName,
               [this]
               { BibleTime::instance()->openSearchDialog(constModules()); });
    initAddAction(
                QStringLiteral("openLocation"),
                [this]{
                    if (btConfig().session().value<bool>(
                            QStringLiteral("GUI/showToolbarsInEachWindow"),
                            true))
                    {
                        m_keyChooser->setFocus();
                    } else if (auto * const kc = btMainWindow()->keyChooser()) {
                        kc->setFocus();
                    }
                });
    initAddAction(QStringLiteral("pageDown"),
                  m_displayWidget,
                  &BtModelViewReadDisplay::pageDown);
    initAddAction(QStringLiteral("pageUp"),
                  m_displayWidget,
                  &BtModelViewReadDisplay::pageUp);

    initAddAction(QStringLiteral("copySelectedText"),
                  m_displayWidget,
                  &BtModelViewReadDisplay::copySelectedText);
    initAddAction(QStringLiteral("copyByReferences"),
                  m_displayWidget,
                  &BtModelViewReadDisplay::copyByReferences);
    initAddAction(QStringLiteral("findText"),
                  m_displayWidget,
                  &BtModelViewReadDisplay::openFindTextDialog);
    initAddAction(DWG::backInHistory::actionName,
                  m_keyChooser->history(),
                  &BTHistory::back);
    initAddAction(DWG::forwardInHistory::actionName,
                  m_keyChooser->history(),
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

    m_actions.findText = &ac->action(QStringLiteral("findText"));

    m_actions.findStrongs =
            &initAddAction(
                CResMgr::displaywindows::general::findStrongs::actionName,
                [this]{
                    QString searchText;
                    for (auto const & strongNumber
                         : m_displayWidget->getCurrentNodeInfo().split(
                             '|',
                             Qt::SkipEmptyParts))
                        searchText.append(
                                    QStringLiteral("strong:%1 ")
                                    .arg(strongNumber));
                    BibleTime::instance()->openSearchDialog(constModules(),
                                                            searchText);
                });

    m_actions.copy.reference =
            &initAddAction(QStringLiteral("copyReferenceOnly"),
                           m_displayWidget,
                           &BtModelViewReadDisplay::copyAnchorOnly);

    m_actions.copy.entry = &initAddAction(QStringLiteral("copyEntryWithText"),
                                          m_displayWidget,
                                          &BtModelViewReadDisplay::copyAll);

    m_actions.copy.selectedText =
            &ac->action(QStringLiteral("copySelectedText"));

    m_actions.copy.byReferences =
            &ac->action(QStringLiteral("copyByReferences"));

    m_actions.save.entryAsPlain =
            &initAddAction(
                QStringLiteral("saveEntryAsPlain"),
                [this]{
                    CExportManager mgr(true,
                                       tr("Saving"),
                                       filterOptions(),
                                       displayOptions());
                    mgr.saveKey(m_swordKey,
                                CExportManager::Text,
                                true,
                                constModules());
                });

    m_actions.save.entryAsHTML =
            &initAddAction(
                QStringLiteral("saveHtml"),
                [this]{
                    CExportManager mgr(true,
                                       tr("Saving"),
                                       filterOptions(),
                                       displayOptions());
                    mgr.saveKey(m_swordKey,
                                CExportManager::HTML,
                                true,
                                constModules());
                });

    m_actions.print.reference =
            &initAddAction(QStringLiteral("printReferenceOnly"),
                           this,
                           &CDisplayWindow::printAnchorWithText);
    addAction(m_actions.print.reference);

    m_actions.print.entry = &initAddAction(QStringLiteral("printEntryWithText"),
                                           this,
                                           &CDisplayWindow::printAll);

    // init with the user defined settings
    m_actionCollection->readShortcuts(
                QStringLiteral("Displaywindow shortcuts"));
}

void CDisplayWindow::initConnections() {
    BT_ASSERT(m_keyChooser);

    BT_CONNECT(m_keyChooser, &CKeyChooser::keyChanged,
               this,         &CDisplayWindow::lookupSwordKey);
    BT_CONNECT(history(), &BTHistory::historyChanged,
               [this](bool const backEnabled, bool const fwEnabled) {
                   BT_ASSERT(m_actions.backInHistory);
                   BT_ASSERT(m_keyChooser);

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
                        : m_keyChooser->history()->getBackList())
                       menu->addAction(actionPtr);
               });
    BT_CONNECT(m_actions.backInHistory->popupMenu(), &QMenu::triggered,
               m_keyChooser->history(), &BTHistory::move);
    BT_CONNECT(m_actions.forwardInHistory->popupMenu(), &QMenu::aboutToShow,
               this, // Needed
               [this]{
                   QMenu* menu = m_actions.forwardInHistory->popupMenu();
                   menu->clear();
                   for (auto * const actionPtr
                        : m_keyChooser->history()->getFwList())
                       menu->addAction(actionPtr);
               });
    BT_CONNECT(m_actions.forwardInHistory->popupMenu(), &QMenu::triggered,
               m_keyChooser->history(), &BTHistory::move);
}

void CDisplayWindow::initView() {
    // Create display widget for this window
    auto readDisplay = new BtModelViewReadDisplay(this, this);
    setDisplayWidget(readDisplay);
    setCentralWidget(m_displayWidget->view());
    readDisplay->setModules(moduleNames());

    // Add the Navigation toolbar
    addToolBar(mainToolBar());

    // Create keychooser
    auto const constMods = constModules();
    setKeyChooser(CKeyChooser::createInstance(constMods,
                                              history(),
                                              m_swordKey,
                                              mainToolBar()));

    addModuleChooserBar();

    // Add the Tools toolbar
    addToolBar(buttonsToolBar());

    // Create the Text Header toolbar
    addToolBarBreak();
    m_headerBar = new QToolBar(this);
    m_headerBar->setMovable(false);
    m_headerBar->setWindowTitle(tr("Text area header"));
    m_headerBar->setVisible(
                btConfig().session().value<bool>(
                    QStringLiteral("GUI/showTextWindowHeaders"),
                    true));
    BT_CONNECT(btMainWindow(), &BibleTime::toggledTextWindowHeader,
               m_headerBar, &QToolBar::setVisible);

    addToolBar(m_headerBar);
}

void CDisplayWindow::initToolbars() {
    //Navigation toolbar
    BT_ASSERT(m_actions.backInHistory);
    mainToolBar()->addWidget(m_keyChooser);
    mainToolBar()->addAction(m_actions.backInHistory); //1st button
    mainToolBar()->addAction(m_actions.forwardInHistory); //2nd button

    //Tools toolbar
    buttonsToolBar()->addAction(
                &m_actionCollection->action(
                    CResMgr::displaywindows::general::search::actionName));

    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    buttonsToolBar()->addWidget(button);

    // Text Header toolbar
    auto * const h =
            new BtTextWindowHeader(m_modules.first()->type(),
                                   m_modules,
                                   this);
    BT_CONNECT(this, &CDisplayWindow::sigModuleListSet,
               h, &BtTextWindowHeader::slotBackendModulesChanged);
    BT_CONNECT(this, &CDisplayWindow::sigModuleListChanged,
               h, &BtTextWindowHeader::slotWindowModulesChanged);
    BT_CONNECT(h, &BtTextWindowHeader::moduleAdded,
               this, &CDisplayWindow::slotAddModule);
    BT_CONNECT(h, &BtTextWindowHeader::moduleReplaced,
               this, &CDisplayWindow::slotReplaceModule);
    BT_CONNECT(h, &BtTextWindowHeader::moduleRemoved,
               this, &CDisplayWindow::slotRemoveModule);
    m_headerBar->addWidget(h);
}

QMenu * CDisplayWindow::newDisplayWidgetPopupMenu() {
    auto * const popupMenu = new QMenu(this);
    BT_CONNECT(popupMenu, &QMenu::aboutToShow,
                [this] {
                    // enable the action depending on the supported module features
                    m_actions.findStrongs->setEnabled(
                                !m_displayWidget->getCurrentNodeInfo().isNull());

                    bool const hasActiveAnchor = m_displayWidget->hasActiveAnchor();
                    m_actions.copy.reference->setEnabled(hasActiveAnchor);

                    m_actions.print.reference->setEnabled(hasActiveAnchor);

                    m_actions.copy.selectedText->setEnabled(hasSelectedText());
                });
    popupMenu->setTitle(tr("Lexicon window"));
    popupMenu->setIcon(util::tool::getIconForModule(m_modules.first()));
    popupMenu->addAction(m_actions.findText);
    popupMenu->addAction(m_actions.findStrongs);
    popupMenu->addSeparator();

    m_actions.copyMenu = new QMenu(tr("Copy..."), popupMenu);
    m_actions.copyMenu->addAction(m_actions.copy.selectedText);
    m_actions.copyMenu->addAction(m_actions.copy.byReferences);
    m_actions.copyMenu->addSeparator();
    m_actions.copyMenu->addAction(m_actions.copy.reference);
    m_actions.copyMenu->addAction(m_actions.copy.entry);
    popupMenu->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(
                tr("Save..."),
                popupMenu
                );
    m_actions.saveMenu->addAction(m_actions.save.entryAsPlain);
    m_actions.saveMenu->addAction(m_actions.save.entryAsHTML);

    popupMenu->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(
                tr("Print..."),
                popupMenu
                );
    m_actions.printMenu->addAction(m_actions.print.reference);
    m_actions.printMenu->addAction(m_actions.print.entry);
    popupMenu->addMenu(m_actions.printMenu);
    return popupMenu;
}

void CDisplayWindow::setupMainWindowToolBars() {
    // Navigation toolbar
    QString keyReference = m_swordKey->key();
    auto const constMods = constModules();
    auto * const keyChooser =
            CKeyChooser::createInstance(constMods,
                                        history(),
                                        m_swordKey,
                                        btMainWindow()->navToolBar());
    keyChooser->key()->setKey(keyReference);
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    BT_CONNECT(keyChooser, &CKeyChooser::keyChanged,
               this,       &CDisplayWindow::lookupSwordKey);
    BT_CONNECT(this,       &CDisplayWindow::sigKeyChanged,
               keyChooser, &CKeyChooser::updateKey);
    btMainWindow()->navToolBar()->addAction(m_actions.backInHistory); //1st button
    btMainWindow()->navToolBar()->addAction(m_actions.forwardInHistory); //2nd button

    // Works toolbar
    btMainWindow()->worksToolBar()->setModules(m_modules, constMods.first()->type(), this);

    // Tools toolbar
    btMainWindow()->toolsToolBar()->addAction(
                &m_actionCollection->action(
                    CResMgr::displaywindows::general::search::actionName));
    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    btMainWindow()->toolsToolBar()->addWidget(button);
}

bool CDisplayWindow::hasSelectedText()
{ return m_displayWidget->qmlInterface()->hasSelectedText(); }

void CDisplayWindow::copyDisplayedText()
{ CExportManager().copyKey(m_swordKey, CExportManager::Text, true); }

int CDisplayWindow::getSelectedColumn() const
{ return m_displayWidget->quickWidget()->getSelectedColumn(); }

int CDisplayWindow::getFirstSelectedIndex() const
{ return m_displayWidget->quickWidget()->getFirstSelectedIndex(); }

int CDisplayWindow::getLastSelectedIndex() const
{ return m_displayWidget->quickWidget()->getLastSelectedIndex(); }

CSwordKey* CDisplayWindow::getMouseClickedKey() const
{ return m_displayWidget->quickWidget()->getMouseClickedKey(); }

/** Refresh the settings of this window. */
void CDisplayWindow::reload(CSwordBackend::SetupChangedReason) {
    { // First make sure all used Sword modules are still present:
        CSwordBackend & backend = *(CSwordBackend::instance());
        QMutableListIterator<CSwordModuleInfo *> it(m_modules);
        while (it.hasNext())
            if (!backend.moduleList().contains(it.next()))
                it.remove();
    }

    if (m_modules.isEmpty()) {
        close();
    } else {
        m_displayWidget->reloadModules();

        if (auto * const kc = m_keyChooser)
            kc->setModules(constModules(), false);

        lookup();

        m_actionCollection->readShortcuts(
                    QStringLiteral("Displaywindow shortcuts"));
        m_actionCollection->readShortcuts(
                    QStringLiteral("Readwindow shortcuts"));
        Q_EMIT sigModuleListSet(m_modules);
    }

    m_displayWidget->settingsChanged();

    m_actionCollection->readShortcuts(QStringLiteral("Lexicon shortcuts"));
}

void CDisplayWindow::slotAddModule(int index, CSwordModuleInfo * module) {
    BT_ASSERT(index <= m_modules.size());
    m_modules.insert(index, module);
    m_displayWidget->setModules(moduleNames());
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged(m_modules);
}

void CDisplayWindow::slotReplaceModule(int index, CSwordModuleInfo * newModule){
    BT_ASSERT(index < m_modules.size());
    m_modules.replace(index, newModule);
    m_displayWidget->setModules(moduleNames());
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged(m_modules);
}

void CDisplayWindow::slotRemoveModule(int index) {
    BT_ASSERT(index < m_modules.size());
    m_modules.removeAt(index);
    m_displayWidget->setModules(moduleNames());
    lookup();
    modulesChanged();
    Q_EMIT sigModuleListChanged(m_modules);
}

void CDisplayWindow::setBibleReference(const QString& reference) {
    if (moduleType() == CSwordModuleInfo::Bible) {
        lookupKey(reference);
        return;
    }
    auto * const bibleModule =
            btConfig().getDefaultSwordModuleByType(
                QStringLiteral("standardBible"));
    if (bibleModule) {
        BibleTime *mainWindow = btMainWindow();
        BT_ASSERT(mainWindow);
        mainWindow->createReadDisplayWindow(bibleModule, reference);
        return;
    }
    message::showInformation(this, tr("Choose Standard Bible"),
                             tr("Please choose a Bible in the Settings > Configure dialog."));
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
    if (m_modules.isEmpty()) {
        close();
    }
    else {
        auto const constMods = constModules();
        Q_EMIT sigModulesChanged(constMods);
        m_swordKey->setModule(constMods.first());
        m_keyChooser->setModules(constMods);
    }
}

void CDisplayWindow::lookupSwordKey(CSwordKey * newKey) {
    BT_ASSERT(newKey);

    if (!m_isInitialized || !newKey || m_modules.empty() || !m_modules.first())
        return;

    if (m_swordKey != newKey)
        m_swordKey->setKey(newKey->key());

    m_displayWidget->setDisplayOptions(displayOptions());
    m_displayWidget->setFilterOptions(filterOptions());
    m_displayWidget->scrollToKey(newKey);
    BibleTime::instance()->autoScrollStop();

    setWindowTitle(windowCaption());
}

void CDisplayWindow::addModuleChooserBar() {
    BT_ASSERT(!m_moduleChooserBar);
    m_moduleChooserBar = new BtModuleChooserBar(this);
    m_moduleChooserBar->setWindowTitle(tr("Work chooser buttons"));
    m_moduleChooserBar->setLayoutDirection(Qt::LeftToRight);
    m_moduleChooserBar->setVisible(
                btConfig().session().value<bool>(
                    QStringLiteral(
                        "GUI/showTextWindowModuleSelectorButtons"),
                    true));
    m_moduleChooserBar->setModules(m_modules, m_modules.first()->type(), this);

    BT_CONNECT(btMainWindow(), &BibleTime::toggledTextWindowModuleChooser,
               m_moduleChooserBar, &BtModuleChooserBar::setVisible);

    addToolBar(m_moduleChooserBar);
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
bool CDisplayWindow::init() {
    initView();
    BT_ASSERT(m_displayWidget);

    setWindowIcon(util::tool::getIconForModule(m_modules.first()));
    setWindowTitle(windowCaption());
    initActions();
    initToolbars();

    auto const & conf = btConfig();
    if (!conf.session().value<bool>(
            QStringLiteral("GUI/showToolbarsInEachWindow"),
            true))
        setToolBarsHidden();
    btMainWindow()->clearMdiToolBars();
    clearMainWindowToolBars();
    initConnections();

    m_displayWidget->installPopup(newDisplayWidgetPopupMenu());

    m_filterOptions = conf.getFilterOptions();
    m_displayOptions = conf.getDisplayOptions();
    Q_EMIT sigDisplayOptionsChanged(m_displayOptions);
    Q_EMIT sigFilterOptionsChanged(m_filterOptions);
    Q_EMIT sigModulesChanged(constModules());

    m_isInitialized = true;
    return true;
}

static void prepareToolBar(QToolBar* bar, const QString& title, bool visible) {
    bar->setAllowedAreas(Qt::TopToolBarArea);
    bar->setFloatable(false);
    bar->setWindowTitle(title);
    bar->setVisible(visible);
}

QToolBar * CDisplayWindow::mainToolBar() {
    if (!m_mainToolBar) {
        m_mainToolBar = new QToolBar(this);
        prepareToolBar(m_mainToolBar,
                       tr("Navigation"),
                       btConfig().session().value<bool>(
                           QStringLiteral("GUI/showTextWindowNavigator"),
                           true));
        BT_CONNECT(btMainWindow(), &BibleTime::toggledTextWindowNavigator,
                   m_mainToolBar, &QToolBar::setVisible);
    }
    return m_mainToolBar;
}

QToolBar * CDisplayWindow::buttonsToolBar() {
    if (!m_buttonsToolBar) {
        m_buttonsToolBar = new QToolBar(this);
        prepareToolBar(m_buttonsToolBar,
                       tr("Tool"),
                       btConfig().session().value<bool>(
                           QStringLiteral("GUI/showTextWindowToolButtons"),
                           true));
        BT_CONNECT(btMainWindow(), &BibleTime::toggledTextWindowToolButtons,
                   m_buttonsToolBar, &QToolBar::setVisible);
    }
    return m_buttonsToolBar;
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
    button->setModules(constModules());

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
                   m_displayWidget->setDisplayOptions(displayOptions);
                   Q_EMIT sigDisplayOptionsChanged(m_displayOptions);
               });
    BT_CONNECT(button, &BtDisplaySettingsButton::sigChanged,
               this,   &CDisplayWindow::lookup);
}

/** Lookup the current key. Used to refresh the display. */
void CDisplayWindow::lookup() { lookupSwordKey(m_swordKey); }

void CDisplayWindow::lookupKey( const QString& keyName ) {
    /* This function is called for example after a bookmark was dropped on this window
    */
    BT_ASSERT(m_modules.first());

    if (!m_isInitialized) {
        return;
    }

    CSwordModuleInfo *m =
            CSwordBackend::instance()->findModuleByName(
                m_modules.first()->name());
    if (!m) {
        return; /// \todo check if this is correct behavior
    }

    /// \todo check for containsRef compat
    if (m && m_modules.contains(m)) {
        m_swordKey->setKey(keyName);
        m_keyChooser->setKey(m_swordKey); //the key chooser does send an update signal
        Q_EMIT sigKeyChanged(m_swordKey);
    }
    else {     //given module not displayed in this window
        //if the module is displayed in another display window we assume a wrong drop
        //create a new window for the given module
        BibleTime *mainWindow = btMainWindow();
        BT_ASSERT(mainWindow);
        mainWindow->createReadDisplayWindow(m, keyName);
    }
}

/** Sets the display widget used by this display window. */
void CDisplayWindow::setDisplayWidget(BtModelViewReadDisplay * newDisplay) {
    // Lets be orwellianly paranoid here:
    BT_ASSERT(newDisplay);

    m_displayWidget = newDisplay;

    BT_CONNECT(btMainWindow(), &BibleTime::colorThemeChanged,
               [newDisplay]{ newDisplay->qmlInterface()->changeColorTheme(); });
}

void CDisplayWindow::printAll()
{ m_displayWidget->printAll(m_displayOptions, m_filterOptions); }

void CDisplayWindow::printAnchorWithText()
{ m_displayWidget->printAnchorWithText(m_displayOptions, m_filterOptions); }
