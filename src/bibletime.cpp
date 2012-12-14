/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <cstdlib>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSplashScreen>
#include <QSplitter>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/btaboutmoduledialog.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/btfindwidget.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cdisplaywindowfactory.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"


BibleTime *BibleTime::m_instance = 0;

BibleTime::BibleTime(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    namespace DU = util::directory;

    Q_ASSERT(m_instance == 0);
    m_instance = this;

    QSplashScreen *splash = 0;
    QString splashHtml;

    if (btConfig().value<bool>("GUI/showSplashScreen", true)) {
        splashHtml = "<div style='background:transparent;color:white;font-weight:bold'>%1"
                     "</div>";

        static const char * const splashes[3] = {
            "startuplogo.png",
            "startuplogo_christmas.png",
            "startuplogo_easter.jpg"
        };
        QString splashImage = DU::getPicsDir().canonicalPath().append("/")
                                              .append(splashes[rand() % 3]);
        QPixmap pm;
        if (!pm.load(splashImage)) {
            qWarning("Can't load startuplogo! Check your installation.");
        }
        splash = new QSplashScreen(this, pm);
        splash->setAttribute(Qt::WA_DeleteOnClose);
        splash->finish(this);
        splash->showMessage(splashHtml.arg(tr("Initializing the SWORD engine...")),
                            Qt::AlignCenter);
        splash->show();
        qApp->processEvents();
    }
    initBackends();

    if (splash != 0) {
        splash->showMessage(splashHtml.arg(tr("Creating BibleTime's user interface...")),
                            Qt::AlignCenter);
        qApp->processEvents();
    }
    initView();

    if (splash != 0) {
        splash->showMessage(splashHtml.arg(tr("Initializing menu- and toolbars...")),
                            Qt::AlignCenter);
        qApp->processEvents();
    }
    initActions();
    initMenubar();
    initToolbars();
    initConnections();

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
    saveProfile();
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

CDisplayWindow * BibleTime::createWriteDisplayWindow(CSwordModuleInfo * module, const QString & key, CPlainWriteWindow::WriteWindowType type) {
    qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );

    QList<CSwordModuleInfo*> modules;
    modules.append(module);

    CDisplayWindow* displayWindow = CDisplayWindowFactory::createWriteInstance(modules, m_mdi, type);
    if ( displayWindow ) {
        displayWindow->init();
        m_mdi->addSubWindow(displayWindow);
        if (m_mdi->subWindowList().isEmpty())
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
                                    CPlainWriteWindow::PlainTextWindow);
}

CDisplayWindow* BibleTime::moduleEditHtml(CSwordModuleInfo *module) {
    /// \todo Refactor this.
    return createWriteDisplayWindow(module,
                                    QString::null,
                                    CPlainWriteWindow::HTMLWindow);
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
void BibleTime::refreshDisplayWindows() const {
    Q_FOREACH (const QMdiSubWindow * const subWindow, m_mdi->subWindowList()) {
        if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
            window->reload(CSwordBackend::OtherChange);
        }
    }
}

/** Refresh main window accelerators */
void BibleTime::refreshBibleTimeAccel() {
    m_actionCollection->readShortcuts("Application shortcuts");
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
            CSwordVerseKey vk(0);
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
}

bool BibleTime::event(QEvent* event) {
    if (event->type() == QEvent::Close)
        Search::CSearchDialog::closeDialog();
    return QMainWindow::event(event);
}

const CSwordModuleInfo* BibleTime::getCurrentModule() {
    QMdiSubWindow* activeSubWindow = m_mdi->activeSubWindow();
    if (!activeSubWindow)
        return 0;
    CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(activeSubWindow->widget());
    if (!w)
        return 0;
    return w->modules().first();
}

void BibleTime::openFindWidget()
{
    m_findWidget->setVisible(true);
    m_findWidget->showAndSelect();
}
