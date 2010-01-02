/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDate>
#include <QDebug>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QSplashScreen>
#include <QSplitter>
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
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directory.h"


using namespace Profile;

BibleTime::BibleTime(QWidget *parent, Qt::WindowFlags flags)
        : QMainWindow(parent, flags), m_WindowWasMaximizedBeforeFullScreen(false) {
    namespace DU = util::directory;

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
    initConnections();
    readSettings();

    setWindowTitle("BibleTime " BT_VERSION);
    setWindowIcon(DU::getIcon(CResMgr::mainWindow::icon));
}

BibleTime::~BibleTime() {
    //  delete m_dcopInterface;
    // The backend is deleted by the BibleTimeApp instance
}

/** Saves the properties of BibleTime to the application wide configfile  */
void BibleTime::saveSettings() {
    /// \todo how to write settings?
    //accel()->writeSettings(CBTConfig::getConfig());

    CBTConfig::set(CBTConfig::toolbar, m_viewToolbar_action->isChecked());

    // set the default to false
    /* CBTConfig::set(CBTConfig::autoTileVertical, false);
     CBTConfig::set(CBTConfig::autoTileHorizontal, false);
     CBTConfig::set(CBTConfig::autoCascade, false);
    */
    CBTConfig::set(CBTConfig::autoTileVertical, m_windowAutoTileVertical_action->isChecked());
    CBTConfig::set(CBTConfig::autoTileHorizontal, m_windowAutoTileHorizontal_action->isChecked());
    CBTConfig::set(CBTConfig::autoTile, m_windowAutoTile_action->isChecked());
    CBTConfig::set(CBTConfig::autoCascade, m_windowAutoCascade_action->isChecked());

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

    m_viewToolbar_action->setChecked( CBTConfig::get(CBTConfig::toolbar) );
    slotToggleToolbar();

    if ( CBTConfig::get(CBTConfig::autoTileVertical) ) {
        m_windowAutoTileVertical_action->setChecked( true );
        m_windowManualMode_action->setChecked(false);
        slotAutoTileVertical();
    }
    else if ( CBTConfig::get(CBTConfig::autoTileHorizontal) ) {
        m_windowAutoTileHorizontal_action->setChecked( true );
        m_windowManualMode_action->setChecked(false);
        slotAutoTileHorizontal();
    }
    else if ( CBTConfig::get(CBTConfig::autoTile) ) {
        m_windowAutoTile_action->setChecked(true);
        m_windowManualMode_action->setChecked(false);
        slotAutoTile();
    }
    else if ( CBTConfig::get(CBTConfig::autoCascade) ) {
        m_windowAutoCascade_action->setChecked(true);
        m_windowManualMode_action->setChecked(false);
        slotAutoCascade();
    }
    else {
        m_windowManualMode_action->setChecked(true);
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
        if (m_mdi->subWindowList().count() == 0)
            displayWindow->showMaximized();
        else
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

CDisplayWindow* BibleTime::createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CDisplayWindow::WriteWindowType& type) {
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );

    QList<CSwordModuleInfo*> modules;
    modules.append(module);

    CDisplayWindow* displayWindow = CDisplayWindowFactory::createWriteInstance(modules, m_mdi, type);
    if ( displayWindow ) {
        displayWindow->init();
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
                                    CDisplayWindow::PlainTextWindow);
}

CDisplayWindow* BibleTime::moduleEditHtml(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    return createWriteDisplayWindow(module,
                                    QString::null,
                                    CDisplayWindow::HTMLWindow);
}


void BibleTime::searchInModule(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    QList<CSwordModuleInfo *> modules;
    modules.append(module);
    Search::CSearchDialog::openDialog(modules, QString::null);
}

void BibleTime::moduleUnlock(CSwordModuleInfo *module) {
    bool ok;
    const QString unlockKey =
        QInputDialog::getText(
            this,
            tr("Unlock Work"),
            tr("Enter the unlock key for this work."),
            QLineEdit::Normal,
            module->config(CSwordModuleInfo::CipherKey),
            &ok
        );
    if (ok) {
        /// \todo Refactor. Unlock the module via a global modules model.
        if (module->unlock(unlockKey)) {
            CPointers::backend()->reloadModules(CSwordBackend::OtherChange);
        }
    }
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
void::BibleTime::refreshBibleTimeAccel() {
    CBTConfig::setupAccelSettings(CBTConfig::application, m_actionCollection);
}

/** Called before quit. */
void BibleTime::slot_aboutToQuit() {
    saveSettings();
}

/** Called before a window is closed */
bool BibleTime::queryClose() {
    qDebug() << "BibleTime::queryClose";
    bool ret = true;

    foreach(QMdiSubWindow* subWindow, m_mdi->subWindowList()) {
        if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
            ret = ret && window->queryClose();
        }
        qDebug() << "return value:" << ret;
    }
    qDebug() << "final return value:" << ret;
    return ret;
}

/** Restores the workspace if the flag for this is set in the config. */
void BibleTime::restoreWorkspace() {
    if (CProfile* p = m_profileMgr.startupProfile()) {
        loadProfile(p);
    }
}

/** Processes the commandline options given to BibleTime. */
void BibleTime::processCommandline() {
    QStringList args = qApp->QCoreApplication::arguments();

    if (args.contains("--help") || args.contains("-h") || args.contains("/h") || args.contains("/?")) {
        std::cout << "BibleTime" << std::endl << "--help (-h, /h, /?): Show this help message and exit"
                  << std::endl << "--ignore-session: open a clean session" << std:: endl << "--open-default-bible <ref>: "
                  << "Open the default Bible with the reference <ref>" << std::endl;
        std::cout << "Some Qt arguments:" << std::endl << "-reverse: reverse the UI layout direction"
                  << std::endl;
        exit(0);
        //printHelpAndExit();
    }
    if ( !CBTConfig::get(CBTConfig::crashedTwoTimes) &&
            !args.contains("--ignore-session") ) {
        restoreWorkspace();
    }

    if ( args.contains("--open-default-bible") &&
            !CBTConfig::get(CBTConfig::crashedLastTime) &&
            !CBTConfig::get(CBTConfig::crashedTwoTimes)) {
        int index = args.indexOf("--open-default-bible");
        QString bibleKey;
        if (index >= 0 && (index + 1) < args.size()) {
            bibleKey = args.at(index + 1);
        }
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
            bibleKey = vk.key();
        }
        createReadDisplayWindow(bible, bibleKey);
        m_mdi->myTileVertical();//we are sure only one window is open, which should be displayed fullscreen in the working area
    }
}

bool BibleTime::event(QEvent* event) {
    if (event->type() == QEvent::Close)
        Search::CSearchDialog::closeDialog();
    return QMainWindow::event(event);
}

QAction* BibleTime::getAction(const QString& actionName)
{
    return m_actionCollection->action(actionName);
}

