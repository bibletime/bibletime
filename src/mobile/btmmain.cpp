/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include <QGuiApplication>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include <QQmlDebuggingEnabler>
#include <QMetaType>
#include <QStyleHints>
#include <QTranslator>
#include "bibletime.h"
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "util/bticons.h"
#include "mobile/bibletimeapp.h"
#include "mobile/config/btmconfig.h"
#include "mobile/models/searchmodel.h"
#include "mobile/ui/btstyle.h"
#include "mobile/ui/btbookmarkinterface.h"
#include "mobile/ui/btsearchinterface.h"
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/chooserinterface.h"
#include "mobile/ui/configinterface.h"
#include "mobile/ui/installinterface.h"
#include "mobile/ui/moduleinterface.h"
#include "mobile/ui/sessioninterface.h"
#include "util/btassert.h"
#include "util/directory.h"
#include <swlog.h>

static QObject* s_rootObject = nullptr;
static QFont* defaultFont;


void register_gml_classes() {
    QQmlDebuggingEnabler enabler;

    qmlRegisterType<btm::BtBookmarkInterface>("BibleTime", 1, 0, "BtBookmarkInterface");
    qmlRegisterType<btm::BtWindowInterface>("BibleTime", 1, 0, "BtWindowInterface");
    qmlRegisterType<btm::BtStyle>("BibleTime", 1, 0, "BtStyle");
    qmlRegisterType<btm::BtmConfig>("BibleTime", 1, 0, "BtmConfig");
    qmlRegisterType<btm::InstallInterface>("BibleTime", 1, 0, "InstallInterface");
    qmlRegisterType<btm::ModuleInterface>("BibleTime", 1, 0, "ModuleInterface");
    qmlRegisterType<btm::ChooserInterface>("BibleTime", 1, 0, "ChooserInterface");
    qmlRegisterType<btm::SessionInterface>("BibleTime", 1, 0, "SessionInterface");
    qmlRegisterType<btm::SearchModel>("BibleTime", 1, 0, "SearchModel");
    qmlRegisterType<btm::BtSearchInterface>("BibleTime", 1, 0, "BtSearchInterface");
    qmlRegisterType<btm::ConfigInterface>("BibleTime", 1, 0, "ConfigInterface");
}

void saveSession() {
    QMetaObject::invokeMethod(s_rootObject, "saveSession");
}

QFont getDefaultFont() {
    return *defaultFont;
}

#if defined(Q_OS_WIN) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
// Copies locale.qrc files into home sword directory under locales.d directory
static void installSwordLocales(QDir& homeSword)
{
    QDir sourceSwordLocales(":/sword/locales.d");
    if (!sourceSwordLocales.exists())
        return;

    QStringList filters;
    filters << "*.conf";
    QFileInfoList fileInfoList = sourceSwordLocales.entryInfoList(filters);

    if (!homeSword.exists("locales.d"))
        homeSword.mkdir(("locales.d"));
    homeSword.cd("locales.d");

    for (auto sourceFileInfo : fileInfoList) {

        QString fileName = sourceFileInfo.fileName();
        QString sourceFilePath = sourceFileInfo.absoluteFilePath();
        QFile sourceFile(sourceFilePath);

        QFileInfo destinationFileInfo(homeSword, fileName);
        QString destinationFilePath = destinationFileInfo.absoluteFilePath();
        QFile destinationFile(destinationFileInfo.absoluteFilePath());

        destinationFile.remove();
        sourceFile.copy(destinationFilePath);
    }
}
#endif

/*******************************************************************************
  Handle Qt's meta type system.
*******************************************************************************/

void registerMetaTypes() {
    qRegisterMetaType<FilterOptions>("FilterOptions");
    qRegisterMetaType<DisplayOptions>("DisplayOptions");
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");

    qRegisterMetaType<BTModuleTreeItem::Grouping>("Grouping");
    qRegisterMetaTypeStreamOperators<BTModuleTreeItem::Grouping>("Grouping");

    qRegisterMetaType<BtConfig::StringMap>("StringMap");
    qRegisterMetaTypeStreamOperators<BtConfig::StringMap>("StringMap");

    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
}


int main(int argc, char *argv[]) {
    namespace DU = util::directory;

    BibleTimeApp app(argc, argv);

    // Adjust start scrolling drag distance
    QStyleHints * sh = app.styleHints();
    sh->setStartDragDistance(30);

    btm::BtStyle::setCurrentStyle(btm::BtStyle::darkTheme);

    registerMetaTypes();

    defaultFont = new QFont();
    *defaultFont = app.font();

    if (!DU::initDirectoryCache()) {
        qFatal("Error initializing directory cache!");
        return EXIT_FAILURE;
    }

#if defined(Q_OS_WIN) || defined(Q_OS_ANDROID)  || defined(Q_OS_IOS)
    // change directory to the Sword or .sword directory in the $HOME dir so that
    // the sword.conf is found. It points to the sword/locales.d directory
    // This is also needed for the AugmentPath or DataPath to work
    QString homeSwordDir = util::directory::getUserHomeSwordDir().absolutePath();
    QDir dir;
    dir.setCurrent(homeSwordDir);
#endif

    app.startInit();
    if (!app.initBtConfig()) {
        return EXIT_FAILURE;
    }

#if defined(Q_OS_WIN) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (btm::BtStyle::getAppVersion() > btConfig().value<QString>("btm/version")) {
        installSwordLocales(dir);
        btConfig().setValue<QString>("btm/version", btm::BtStyle::getAppVersion());
    }
#endif

    //first install QT's own translations
    QTranslator qtTranslator;
    QString locale = QLocale::system().name();
    qtTranslator.load("qt_" + locale);
    app.installTranslator(&qtTranslator);
    //then our own
    QTranslator bibleTimeTranslator;
    bibleTimeTranslator.load( QString("bibletime_ui_").append(locale), DU::getLocaleDir().canonicalPath());
    app.installTranslator(&bibleTimeTranslator);
    QTranslator mobileTranslator;
    mobileTranslator.load( QString("mobile_ui_").append(locale), DU::getLocaleDir().canonicalPath());
    app.installTranslator(&mobileTranslator);

    // Initialize display template manager:
    if (!app.initDisplayTemplateManager()) {
        qFatal("Error initializing display template manager!");
        return EXIT_FAILURE;
    }

    BtIcons btIcons;

    register_gml_classes();

    btm::BibleTime btm;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/share/bibletime/qml/main.qml")));
    s_rootObject = engine.rootObjects().at(0);

    int rtn = app.exec();
    saveSession();
    return rtn;
}
