/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDate>
#include <QDebug>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSplashScreen>
#include <QSplitter>
#include <ctime>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/btaboutmoduledialog.h"
#include "frontend/cmdiarea.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cdisplaywindowfactory.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"


using namespace Profile;

BibleTime *BibleTime::m_instance = 0;

BibleTime::BibleTime(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), m_WindowWasMaximizedBeforeFullScreen(false)
{
    namespace DU = util::directory;

    Q_ASSERT(m_instance == 0);
    m_instance = this;

    QSplashScreen splash;
    bool showSplash = CBTConfig::get(CBTConfig::logo);
    QString splashHtml;

    if (showSplash) {
        splashHtml = "<div style='background:transparent;color:white;font-weight:bold'>%1"
                     "</div>";
        const QDate date(QDate::currentDate());
        const int day = date.day();
        const int month = date.month();
        QString splashImage(DU::getPicsDir().canonicalPath().append("/"));

        if ((month >= 12 && day >= 24) || (month <= 1 && day < 6)) {
            splashImage.append("startuplogo_christmas.png");
        } else {
            splashImage.append("startuplogo.png");
        }

        QPixmap pm;
        if (!pm.load(splashImage)) {
            qWarning("Can't load startuplogo! Check your installation.");
        }
        splash.setPixmap(pm);
        splash.show();

        splash.showMessage(splashHtml.arg(tr("Initializing the SWORD engine...")),
                           Qt::AlignCenter);
    }
    initBackends();

    if (showSplash) {
        splash.showMessage(splashHtml.arg(tr("Creating BibleTime's user interface...")),
                           Qt::AlignCenter);
    }
    initView();

    if (showSplash) {
        splash.showMessage(splashHtml.arg(tr("Initializing menu- and toolbars...")),
                           Qt::AlignCenter);
    }
    initActions();
    initMenubar();
    initToolbars();
    initConnections();
    readSettings();

    setWindowTitle("BibleTime " BT_VERSION);
    setWindowIcon(DU::getIcon(CResMgr::mainWindow::icon));
    retranslateUi();
}

BibleTime::~BibleTime() {
    //  delete m_dcopInterface;
    // The backend is deleted by the BibleTimeApp instance
#ifdef BT_DEBUG
    deleteDebugWindow();
#endif
    saveSettings();
}

/** Saves the properties of BibleTime to the application wide configfile  */
void BibleTime::saveSettings() {
    /// \todo how to write settings?
    //accel()->writeSettings(CBTConfig::getConfig());

    CBTConfig::set(CBTConfig::toolbar, m_showMainToolbarAction->isChecked());

    // set the default to false
    /* CBTConfig::set(CBTConfig::autoTileVertical, false);
     CBTConfig::set(CBTConfig::autoTileHorizontal, false);
     CBTConfig::set(CBTConfig::autoCascade, false);
    */
    CBTConfig::set(CBTConfig::autoTileVertical, m_windowAutoTileVerticalAction->isChecked());
    CBTConfig::set(CBTConfig::autoTileHorizontal, m_windowAutoTileHorizontalAction->isChecked());
    CBTConfig::set(CBTConfig::autoTile, m_windowAutoTileAction->isChecked());
    CBTConfig::set(CBTConfig::autoTabbed, m_windowAutoTabbedAction->isChecked());
    CBTConfig::set(CBTConfig::autoCascade, m_windowAutoCascadeAction->isChecked());

    CProfile* p = m_profileMgr.startupProfile();
    if (p) {
        saveProfile(p);
    }
}

/** Reads the settings from the configfile and sets the right properties. */
void BibleTime::readSettings() {
    qDebug() << "******************BibleTime::readSettings******************************";
    //  accel()->readSettings(CBTConfig::getConfig());
    CBTConfig::setupAccelSettings(CBTConfig::application, m_actionCollection);

    m_showMainToolbarAction->setChecked( CBTConfig::get(CBTConfig::toolbar) );
    slotToggleMainToolbar();

    if ( CBTConfig::get(CBTConfig::autoTileVertical) ) {
        m_windowAutoTileVerticalAction->setChecked( true );
        m_windowManualModeAction->setChecked(false);
        slotAutoTileVertical();
    }
    else if ( CBTConfig::get(CBTConfig::autoTileHorizontal) ) {
        m_windowAutoTileHorizontalAction->setChecked( true );
        m_windowManualModeAction->setChecked(false);
        slotAutoTileHorizontal();
    }
    else if ( CBTConfig::get(CBTConfig::autoTile) ) {
        m_windowAutoTileAction->setChecked(true);
        m_windowManualModeAction->setChecked(false);
        slotAutoTile();
    }
    else if ( CBTConfig::get(CBTConfig::autoTabbed) ) {
        m_windowAutoTabbedAction->setChecked(true);
        m_windowManualModeAction->setChecked(false);
        slotAutoTabbed();
    }
    else if ( CBTConfig::get(CBTConfig::autoCascade) ) {
        m_windowAutoCascadeAction->setChecked(true);
        m_windowManualModeAction->setChecked(false);
        slotAutoCascade();
    }
    else {
        m_windowManualModeAction->setChecked(true);
        slotManualArrangementMode();
    }
}

/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& key) {
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );
    qDebug() << "BibleTime::createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& key)";
    CDisplayWindow* displayWindow = CDisplayWindowFactory::createReadInstance(modules, m_mdi);
    if ( displayWindow ) {
        displayWindow->init();
        m_mdi->addSubWindow(displayWindow);
        displayWindow->show();
        //   if (!key.isEmpty())
        displayWindow->lookupKey(key);
    }
    // We have to process pending events here, otherwise displayWindow is not fully painted
    qApp->processEvents();
    // Now all events, including mouse clicks for the displayWindow have been handled
    // and we can let the user click the same module again
    //m_bookshelfPage->unfreezeModules(modules);
    qApp->restoreOverrideCursor();
    return displayWindow;
}


/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(CSwordModuleInfo* module, const QString& key) {
    QList<CSwordModuleInfo*> list;
    list.append(module);

    return createReadDisplayWindow(list, key);
}

CDisplayWindow* BibleTime::createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CWriteWindow::WriteWindowType& type) {
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );

    QList<CSwordModuleInfo*> modules;
    modules.append(module);

    CDisplayWindow* displayWindow = CDisplayWindowFactory::createWriteInstance(modules, m_mdi, type);
    if ( displayWindow ) {
        displayWindow->init();
        m_mdi->addSubWindow(displayWindow);
        if (m_mdi->subWindowList().count() == 0)
            displayWindow->showMaximized();
        else
            displayWindow->show();
        displayWindow->lookupKey(key);
    }

    qApp->restoreOverrideCursor();
    return displayWindow;
}

CDisplayWindow* BibleTime::moduleEditPlain(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    return createWriteDisplayWindow(module,
                                    QString::null,
                                    CWriteWindow::PlainTextWindow);
}

CDisplayWindow* BibleTime::moduleEditHtml(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    return createWriteDisplayWindow(module,
                                    QString::null,
                                    CWriteWindow::HTMLWindow);
}


void BibleTime::searchInModule(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    QList<const CSwordModuleInfo *> modules;
    modules.append(module);
    Search::CSearchDialog::openDialog(modules, QString::null);
}

bool BibleTime::moduleUnlock(CSwordModuleInfo *module, QWidget *parent) {
    /// \todo Write a proper unlocking dialog with integrated error messages.
    QString unlockKey;
    bool ok;
    for (;;) {
        unlockKey = QInputDialog::getText(
            parent, tr("Unlock Work"), tr("Enter the unlock key for %1.").arg(module->name()),
            QLineEdit::Normal, module->config(CSwordModuleInfo::CipherKey), &ok
        );
        if (!ok) return false;
        module->unlock(unlockKey);

        /// \todo refactor this module reload
        /* There is currently a deficiency in sword 1.6.1 in that backend->setCipherKey() does
         * not work correctly for modules from which data was already fetched. Therefore we have to
         * reload the modules.
         */
        {
            const QString moduleName(module->name());
            CSwordBackend *backend = CSwordBackend::instance();
            backend->reloadModules(CSwordBackend::OtherChange);
            module = backend->findModuleByName(moduleName);
            Q_ASSERT(module != 0);
        }

        if (!module->isLocked()) break;
        QMessageBox::warning(parent, tr("Warning: Invalid unlock key!"),
                             tr("The unlock key you provided did not properly unlock this "
                                "module. Please try again."));
    }
    return true;
}

void BibleTime::slotModuleUnlock(CSwordModuleInfo *module) {
    moduleUnlock(module, this);
}

void BibleTime::moduleAbout(CSwordModuleInfo *module) {
    BTAboutModuleDialog *dialog = new BTAboutModuleDialog(module, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Destroy dialog when closed
    dialog->show();
    dialog->raise();
}

/** Refreshes all presenters.*/
void BibleTime::refreshDisplayWindows() {
    foreach (QMdiSubWindow* subWindow, m_mdi->subWindowList()) {
        if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
            window->reload(CSwordBackend::OtherChange);
        }
    }
}

/** Refresh main window accelerators */
void BibleTime::refreshBibleTimeAccel() {
    CBTConfig::setupAccelSettings(CBTConfig::application, m_actionCollection);
}

void BibleTime::closeEvent(QCloseEvent *event) {
    /*
      Sequentially queries all open subwindows whether its fine to close them. If some sub-
      window returns false, the querying is stopped and the close event is ignored. If all
      subwindows return true, the close event is accepted.
    */
    Q_FOREACH(QMdiSubWindow *subWindow, m_mdi->subWindowList()) {
        if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
            if (!window->queryClose()) {
                event->ignore();
                return;
            }
        }
    }
    event->accept();
}

/** Restores the workspace if the flag for this is set in the config. */
void BibleTime::restoreWorkspace() {
    if (CProfile* p = m_profileMgr.startupProfile()) {
        loadProfile(p);
    }
}

void BibleTime::processCommandline(bool ignoreSession, const QString &bibleKey) {
    if (CBTConfig::get(CBTConfig::crashedTwoTimes)) {
        return;
    }

    if (!ignoreSession) {
        restoreWorkspace();
    }

    if (CBTConfig::get(CBTConfig::crashedLastTime)) {
        return;
    }

    if (!bibleKey.isNull()) {
        CSwordModuleInfo* bible = CBTConfig::get(CBTConfig::standardBible);
        if (bibleKey == "random") {
            CSwordVerseKey vk(0);
            const int maxIndex = 31100;
            time_t seconds;
            seconds = time (NULL);
            srand(seconds);
            int newIndex = rand() % maxIndex;
            vk.setPosition(sword::TOP);
            vk.Index(newIndex);
            createReadDisplayWindow(bible, vk.key());
        } else {
            createReadDisplayWindow(bible, bibleKey);
        }

        /*
          We are sure only one window is open - it should be displayed
          fullscreen in the working area:
        */
        m_mdi->myTileVertical();
    }
}

bool BibleTime::event(QEvent* event) {
    if (event->type() == QEvent::Close)
        Search::CSearchDialog::closeDialog();
    return QMainWindow::event(event);
}
