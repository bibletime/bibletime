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

#include "bibletime.h"

#include <cmath>
#include <cstdlib>
#include <exception>
#include <random>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QMdiSubWindow>
#include <QSplashScreen>
#include <QSplitter>
#ifdef BUILD_TEXT_TO_SPEECH
#include <QTextToSpeech>
#endif
#include <type_traits>
#include "../backend/config/btconfig.h"
#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/keys/cswordversekey.h"
#include "../backend/managers/cswordbackend.h"
#include "../util/btassert.h"
#include "../util/cresmgr.h"
#include "../util/directory.h"
#include "bibletimeapp.h"
#include "btaboutmoduledialog.h"
#include "btbookshelfdockwidget.h"
#include "btmessageinputdialog.h"
#include "cmdiarea.h"
#include "display/btfindwidget.h"
#include "display/btmodelviewreaddisplay.h"
#include "displaywindow/cbiblereadwindow.h"
#include "displaywindow/cbookreadwindow.h"
#include "displaywindow/ccommentaryreadwindow.h"
#include "displaywindow/cdisplaywindow.h"
#include "displaywindow/clexiconreadwindow.h"
#include "messagedialog.h"
#include "searchdialog/csearchdialog.h"


namespace {

template <typename T>
auto randInt(T min, T max)
        -> std::enable_if_t<std::is_integral_v<std::decay_t<T>>, T>
{
    static std::mt19937 rng((std::random_device()()));
    return std::uniform_int_distribution<std::decay_t<T>>(min, max)(rng);
}

QString const splashes[] = {
    QStringLiteral("startuplogo.png"),
    QStringLiteral("startuplogo_christmas.png"),
    QStringLiteral("startuplogo_easter.jpg"),
};

auto const splashHtml =
        QStringLiteral(
            "<div style='background:transparent;color:white;font-weight:bold'>"
            "%1</div>");

} // anonymous namespace

BibleTime *BibleTime::m_instance = nullptr;

BibleTime::BibleTime(BibleTimeApp & app, QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    namespace DU = util::directory;

    BT_ASSERT(!m_instance);
    m_instance = this;

    QSplashScreen * splash = nullptr;
    constexpr static auto const splashTextAlignment =
            Qt::AlignHCenter | Qt::AlignTop;

    if (btConfig().value<bool>(QStringLiteral("GUI/showSplashScreen"), true)) {
        auto const splashNumber =
                randInt<std::size_t>(0u,
                                     std::extent_v<decltype(splashes)> - 1u);
        QPixmap pm;
        if (pm.load(DU::getPicsDir().filePath(splashes[splashNumber]))) {
            splash = new QSplashScreen(pm);
            splash->showMessage(
                        splashHtml.arg(tr("Initializing the SWORD engine...")),
                        splashTextAlignment);
            splash->show();
            qApp->processEvents();
        } else {
            qWarning("Can't load startuplogo! Check your installation.");
        }
    }
    app.initBackends();

    if (splash) {
        splash->showMessage(
                    splashHtml.arg(
                        tr("Creating BibleTime's user interface...")),
                    splashTextAlignment);
        qApp->processEvents();
    }
    initView();

    if (splash) {
        splash->showMessage(
                    splashHtml.arg(tr("Initializing menu- and toolbars...")),
                    splashTextAlignment);
        qApp->processEvents();
        splash->setAttribute(Qt::WA_DeleteOnClose);
        splash->finish(this);
    }
    initActions();
    initMenubar();
    initToolbars();
    initConnections();

    setWindowTitle(QStringLiteral("BibleTime " BT_VERSION));
    setWindowIcon(CResMgr::mainWindow::icon());
    retranslateUi();
}

BibleTime::~BibleTime() {
    //  delete m_dcopInterface;
    // The backend is deleted by the BibleTimeApp instance

    delete m_debugWindow;
    m_bookshelfDock->saveBookshelfState();
    saveProfile();
}

/** \brief Creates a new presenter in the MDI area according to the type of the
            module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(
        QList<CSwordModuleInfo *> modules,
        QString const & key)
{
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

    CDisplayWindow * displayWindow;
    switch (modules.first()->type()) {
        case CSwordModuleInfo::Bible:
            displayWindow = new CBibleReadWindow(modules, m_mdi);
            break;
        case CSwordModuleInfo::Commentary:
            displayWindow = new CCommentaryReadWindow(modules, m_mdi);
            break;
        case CSwordModuleInfo::Lexicon:
            displayWindow = new CLexiconReadWindow(modules, m_mdi);
            break;
        case CSwordModuleInfo::GenericBook:
            displayWindow = new CBookReadWindow(modules, m_mdi);
            break;
        default:
            qFatal("unknown module type");
            std::terminate();
    }
    m_mdi->addDisplayWindow(displayWindow);
    displayWindow->show();
    displayWindow->lookupKey(key);

    /* We have to process pending events here, otherwise displayWindow is not
       fully painted. */
    qApp->processEvents();
    qApp->restoreOverrideCursor();
    return displayWindow;
}


/** \brief  Creates a new presenter in the MDI area according to the type of the
            module. */
CDisplayWindow * BibleTime::createReadDisplayWindow(
        CSwordModuleInfo * const module,
        QString const & key)
{ return createReadDisplayWindow(QList<CSwordModuleInfo*>() << module, key); }

bool BibleTime::moduleUnlock(CSwordModuleInfo * module, QWidget * const parent){
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
            auto & backend = CSwordBackend::instance();
            backend.reloadModules();
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
    for (auto const * const subWindow : m_mdi->subWindowList())
        if (CDisplayWindow * const window =
                dynamic_cast<CDisplayWindow*>(subWindow->widget()))
            window->reload();
}

void BibleTime::processCommandline(bool const ignoreSession,
                                   QString const & bibleKey)
{
    if (btConfig().value<bool>(QStringLiteral("state/crashedTwoTimes"), false))
        return;

    // Restore workspace if not not ignoring session data:
    if (!ignoreSession)
        reloadProfile();

    if (btConfig().value<bool>(QStringLiteral("state/crashedLastTime"), false))
        return;

    if (!bibleKey.isNull()) {
        auto * const bible =
                btConfig().getDefaultSwordModuleByType(
                    QStringLiteral("standardBible"));
        if (bibleKey == QStringLiteral("random")) {
            CSwordVerseKey vk(nullptr);
            auto const newIndex = randInt<decltype(vk.index())>(0, 31100);
            vk.positionToTop();
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

    if (btConfig().value<bool>(QStringLiteral("state/crashedLastTime"), false)){
        btConfig().setValue(QStringLiteral("state/crashedTwoTimes"), true);
    } else {
        btConfig().setValue(QStringLiteral("state/crashedLastTime"), true);
    }
    btConfig().sync();

    // temporary for testing
    Q_EMIT colorThemeChanged();
}

bool BibleTime::event(QEvent* event) {
    if (event->type() == QEvent::PaletteChange) {
        Q_EMIT colorThemeChanged();
        // allow to continue to update other parts of Qt widgets
    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent *>(event)->modifiers() > 0)
            return false;
        if (autoScrollAnyKey())
            return true;
    }
    return QMainWindow::event(event);
}

const CSwordModuleInfo* BibleTime::getCurrentModule() {
    if (auto * const activeSubWindow = m_mdi->activeSubWindow())
        if (auto * const displayWindow =
                dynamic_cast<CDisplayWindow *>(activeSubWindow->widget()))
            return displayWindow->firstModule();
    return nullptr;
}

BtModelViewReadDisplay * BibleTime::getCurrentDisplay() {
    if (auto * const activeSubWindow = m_mdi->activeSubWindow())
        if (auto * const displayWindow =
                dynamic_cast<CDisplayWindow *>(activeSubWindow->widget()))
            return displayWindow->displayWidget();
    return nullptr;
}

void BibleTime::setDisplayFocus() {
    if (auto * const display = getCurrentDisplay())
        display->setDisplayFocus();
}

void BibleTime::openSearchDialog(BtConstModuleList modules,
                                 QString const & searchText)
{
    if (!m_searchDialog)
        m_searchDialog = new Search::CSearchDialog(this);
    m_searchDialog->reset(std::move(modules), searchText);
}

void BibleTime::openFindWidget() { m_findWidget->showAndSelect(); }

#ifdef BUILD_TEXT_TO_SPEECH
void BibleTime::speakText(const QString &text)
{
    if (!m_textToSpeech)
        m_textToSpeech = createTextToSpeechInstance();

    m_textToSpeech->say(text);
}

std::unique_ptr<QTextToSpeech> BibleTime::createTextToSpeechInstance()
{
    std::unique_ptr<QTextToSpeech> tts;

    // restore settings from config
    const QString configuredEngine = btConfig().value<QString>(QStringLiteral("GUI/ttsEngine"));
    if (QTextToSpeech::availableEngines().contains(configuredEngine)) {
        tts = std::make_unique<QTextToSpeech>(configuredEngine);
    } else {
        tts = std::make_unique<QTextToSpeech>();
    }

    const QLocale configuredLocale = btConfig().value<QLocale>(QStringLiteral("GUI/ttsLocale"));
    if (tts->availableLocales().contains(configuredLocale)) {
        tts->setLocale(configuredLocale);
    }

    const QString configuredVoice = btConfig().value<QString>(QStringLiteral("GUI/ttsVoice"));
    const QVector<QVoice> voices = tts->availableVoices();
    for (const QVoice& voice : voices) {
        if (voice.name() == configuredVoice) {
            tts->setVoice(voice);
            break;
        }
    }

    return tts;
}
#endif
