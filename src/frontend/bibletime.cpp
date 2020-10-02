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

#include "bibletime.h"

#include <cmath>
#include <cstdlib>
#include <exception>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QSplashScreen>
#include <QSplitter>
#include "../backend/config/btconfig.h"
#include "../backend/drivers/cswordbiblemoduleinfo.h"
#include "../backend/drivers/cswordbookmoduleinfo.h"
#include "../backend/drivers/cswordcommentarymoduleinfo.h"
#include "../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/keys/cswordldkey.h"
#include "../backend/keys/cswordversekey.h"
#include "../util/btassert.h"
#include "../util/cresmgr.h"
#include "../util/directory.h"
#include "btaboutmoduledialog.h"
#include "bibletimeapp.h"
#include "btbookshelfdockwidget.h"
#include "btmessageinputdialog.h"
#include "cmdiarea.h"
#include "display/btfindwidget.h"
#include "displaywindow/btactioncollection.h"
#include "displaywindow/cbiblereadwindow.h"
#include "displaywindow/cbookreadwindow.h"
#include "displaywindow/ccommentaryreadwindow.h"
#include "displaywindow/cdisplaywindow.h"
#include "displaywindow/clexiconreadwindow.h"
#include "displaywindow/creadwindow.h"
#include "keychooser/ckeychooser.h"
#include "messagedialog.h"
#include "searchdialog/csearchdialog.h"

/**
 * Auto scroll time interval in milliseconds
 * This value is divided by an integer (2, 3, 4, etc.) to get the
 * time interval that is used.
 */
static const int autoScrollTimeInterval = 200;


BibleTime *BibleTime::m_instance = nullptr;

BibleTime::BibleTime(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    namespace DU = util::directory;

    BT_ASSERT(!m_instance);
    m_instance = this;

    m_autoScroll.enabled = false;

    QSplashScreen *splash = nullptr;
    QString splashHtml;
    static auto const splashTextAlignment =
            Qt::AlignHCenter | Qt::AlignTop;

    if (btConfig().value<bool>("GUI/showSplashScreen", true)) {
        splashHtml = "<div style='background:transparent;color:white;font-weight:bold'>%1"
                     "</div>";

        static const char splash1[] = "startuplogo.png";
        static const char splash2[] = "startuplogo_christmas.png";
        static const char splash3[] = "startuplogo_easter.jpg";
        static const char * const splashes[3] = {
            splash1, splash2, splash3
        };
        QString splashImage = DU::getPicsDir().canonicalPath().append("/")
                                              .append(splashes[rand() % 3]);
        QPixmap pm;
        if (!pm.load(splashImage)) {
            qWarning("Can't load startuplogo! Check your installation.");
        }
        splash = new QSplashScreen(pm);
        splash->setAttribute(Qt::WA_DeleteOnClose);
        splash->finish(this);
        splash->showMessage(splashHtml.arg(tr("Initializing the SWORD engine...")),
                            splashTextAlignment);
        splash->show();
        qApp->processEvents();
    }
    initBackends();

    if (splash != nullptr) {
        splash->showMessage(splashHtml.arg(tr("Creating BibleTime's user interface...")),
                            splashTextAlignment);
        qApp->processEvents();
    }
    initView();

    if (splash != nullptr) {
        splash->showMessage(splashHtml.arg(tr("Initializing menu- and toolbars...")),
                            splashTextAlignment);
        qApp->processEvents();
    }
    initActions();
    initMenubar();
    initToolbars();
    initConnections();

    setWindowTitle("BibleTime " BT_VERSION);
    setWindowIcon(CResMgr::mainWindow::icon());
    retranslateUi();
}

BibleTime::~BibleTime() {
    //  delete m_dcopInterface;
    // The backend is deleted by the BibleTimeApp instance
#ifndef NDEBUG
    deleteDebugWindow();
#endif
    m_bookshelfDock->saveBookshelfState();
    saveProfile();
}

namespace {

CReadWindow * createReadInstance(QList<CSwordModuleInfo *> const modules,
                                 CMDIArea * const parent)
{
    switch (modules.first()->type()) {
        case CSwordModuleInfo::Bible:
            return new CBibleReadWindow(modules, parent);
        case CSwordModuleInfo::Commentary:
            return new CCommentaryReadWindow(modules, parent);
        case CSwordModuleInfo::Lexicon:
            return new CLexiconReadWindow(modules, parent);
        case CSwordModuleInfo::GenericBook:
            return new CBookReadWindow(modules, parent);
        default:
            qFatal("unknown module type");
            std::terminate();
    }
}

} // anonymous namespace

/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& key) {
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );
    // qDebug() << "BibleTime::createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& key)";
    CDisplayWindow * const displayWindow = createReadInstance(modules, m_mdi);
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
    return createReadDisplayWindow(QList<CSwordModuleInfo*>() << module, key);
}

void BibleTime::searchInModule(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    BtConstModuleList modules;
    modules.append(module);
    Search::CSearchDialog::openDialog(modules, QString());
}

bool BibleTime::moduleUnlock(CSwordModuleInfo * module, QWidget * parent) {
    BT_ASSERT(module);

    /// \todo Write a proper unlocking dialog with integrated error messages.
    BtMessageInputDialog unlockKeyInputDialog(
                tr("Unlock Work"),
                tr("Enter the unlock key for %1.").arg(module->name()),
                BtMessageInputDialog::Password,
                module->config(CSwordModuleInfo::CipherKey),
                module->getUnlockInfo(),
                parent);

    while (unlockKeyInputDialog.exec() == QDialog::Accepted) {
        module->unlock(unlockKeyInputDialog.getUserInput());

        /// \todo refactor this module reload
        /* There is currently a deficiency in SWORD 1.8.1 in that
           backend->setCipherKey() does not work correctly for modules from
           which data was already fetched. Therefore we have to reload the
           modules. */
        {
            auto const moduleName(module->name());
            auto & backend = *CSwordBackend::instance();
            backend.reloadModules(CSwordBackend::OtherChange);
            module = backend.findModuleByName(moduleName);
            BT_ASSERT(module);
        }

        // Return true if the module was succesfully unlocked:
        if (!module->isLocked())
            return true;

        message::showWarning(
                    parent,
                    tr("Warning: Invalid unlock key!"),
                    tr("The unlock key you provided did not properly unlock "
                       "this module. Please try again."));
    }
    return false;
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
void BibleTime::refreshDisplayWindows() const {
    Q_FOREACH(QMdiSubWindow const * const subWindow, m_mdi->subWindowList())
        if (CDisplayWindow * const window =
                dynamic_cast<CDisplayWindow*>(subWindow->widget()))
            window->reload(CSwordBackend::OtherChange);
}

void BibleTime::closeEvent(QCloseEvent *event) {
    /*
      Sequentially queries all open subwindows whether its fine to close them. If some sub-
      window returns false, the querying is stopped and the close event is ignored. If all
      subwindows return true, the close event is accepted.
    */
    Q_FOREACH (QMdiSubWindow * const subWindow, m_mdi->subWindowList()) {
        if (CDisplayWindow * const window = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
            if (!window->queryClose()) {
                event->ignore();
                return;
            }
        }
    }
    event->accept();
}

void BibleTime::processCommandline(bool ignoreSession, const QString &bibleKey) {
    if (btConfig().value<bool>("state/crashedTwoTimes", false)) {
        return;
    }

    // Restore workspace if not not ignoring session data:
    if (!ignoreSession)
        reloadProfile();

    if (btConfig().value<bool>("state/crashedLastTime", false)) {
        return;
    }

    if (!bibleKey.isNull()) {
        CSwordModuleInfo* bible = btConfig().getDefaultSwordModuleByType("standardBible");
        if (bibleKey == "random") {
            CSwordVerseKey vk(nullptr);
            const int maxIndex = 31100;
            int newIndex = rand() % maxIndex;
            vk.setPosition(sword::TOP);
            vk.setIndex(newIndex);
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

    if (btConfig().value<bool>("state/crashedLastTime", false)) {
        btConfig().setValue("state/crashedTwoTimes", true);
    }
    else {
        btConfig().setValue("state/crashedLastTime", true);
    }
    btConfig().sync();

    // temporary for testing
    emit colorThemeChanged();
}

bool BibleTime::event(QEvent* event) {
    if (event->type() == QEvent::PaletteChange) {
        emit colorThemeChanged();
        // allow to continue to update other parts of Qt widgets
    }
    if (event->type() == QEvent::Close)
        Search::CSearchDialog::closeDialog();
    else if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->modifiers() > 0)
            return false;
        int value = keyEvent->key();
        bool accepted = autoScrollAnyKey(value);
        if (accepted) {
            return true;
        }
    }
    return QMainWindow::event(event);
}

const CSwordModuleInfo* BibleTime::getCurrentModule() {
    QMdiSubWindow* activeSubWindow = m_mdi->activeSubWindow();
    if (!activeSubWindow)
        return nullptr;
    CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(activeSubWindow->widget());
    if (!w)
        return nullptr;
    return w->modules().first();
}

CDisplay* BibleTime::getCurrentDisplay() {
    QMdiSubWindow* activeSubWindow = m_mdi->activeSubWindow();
    if (!activeSubWindow)
        return nullptr;
    CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(activeSubWindow->widget());
    if (!w)
        return nullptr;
    return w->displayWidget();
}

void BibleTime::setDisplayFocus() {
    CDisplay* display = getCurrentDisplay();
    if (display)
        display->setDisplayFocus();
}

void BibleTime::openFindWidget()
{
    m_findWidget->setVisible(true);
    m_findWidget->showAndSelect();
}

void BibleTime::setAutoScrollTimerInterval() {
    if (m_autoScroll.speed == 0) {
        m_autoScrollTimer.stop();
        m_autoScrollTimer.setInterval(autoScrollTimeInterval/2);
        m_autoScroll.enabled = false;
    } else {
        double timeDivisor = std::pow(0.6,std::abs(m_autoScroll.speed));
        int interval = static_cast<int>(autoScrollTimeInterval*timeDivisor);
        m_autoScrollTimer.setInterval(interval);
    }
}

