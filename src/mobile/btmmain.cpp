/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bibletime.h"
#include "backend/config/btconfig.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "mobile/bibletimeapp.h"
#include "mobile/bookshelfmanager/installmanager.h"
#include "mobile/ui/btstyle.h"
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/moduleinterface.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QBrush>
#include <QColor>
#include <QGuiApplication>
#include <QPalette>
#include <QQuickItem>
#include <QQmlDebuggingEnabler>
#include <QMetaType>
#include <QTranslator>
#include "util/directory.h"

btm::ViewManager* mgr = 0;

void register_gml_classes() {
    QQmlDebuggingEnabler enabler;

    qmlRegisterType<btm::BtWindowInterface>("BibleTime", 1, 0, "BtWindowInterface");
    qmlRegisterType<btm::BtStyle>("BibleTime", 1, 0, "BtStyle");
    qmlRegisterType<btm::InstallManager>("BibleTime", 1, 0, "InstallManager");
    qmlRegisterType<btm::ModuleInterface>("BibleTime", 1, 0, "ModuleInterface");
}

btm::ViewManager* getViewManager() {
    return mgr;
}

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

    BibleTimeApp app(argc, argv); //for QApplication

    registerMetaTypes();

    if (!DU::initDirectoryCache()) {
        qFatal("Error initializing directory cache!");
        return EXIT_FAILURE;
    }

    app.startInit();
    if (!app.initBtConfig()) {
        return EXIT_FAILURE;
    }

    //first install QT's own translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);
    //then our own
    QTranslator BibleTimeTranslator;
    BibleTimeTranslator.load( QString("bibletime_ui_").append(QLocale::system().name()), DU::getLocaleDir().canonicalPath());
    app.installTranslator(&BibleTimeTranslator);

//    // a new BibleTime version was installed (maybe a completely new installation)
//    if (btConfig().value<QString>("bibletimeVersion", BT_VERSION) != BT_VERSION) {
//        btConfig().setValue("bibletimeVersion", QString(BT_VERSION));
//        mainWindow->saveConfigSettings();
//    }

    register_gml_classes();
    mgr = new btm::ViewManager;
    mgr->show();
    btm::BibleTime btm;

    return app.exec();
}
