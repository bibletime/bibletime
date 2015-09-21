CONFIG += clucene svg xml mobile qml quick c++11

QML_ROOT_PATH = /home/gary/shared/sw/bibletime/src/mobile/qml

DEFINES += BT_MOBILE
QMAKE_CXXFLAGS += -DBT_GIT_VERSION=\\\"`git rev-parse HEAD`\\\"

include(../../common/core/core.pro)

SOURCES += \
    ../../../src/mobile/bibletimeapp.cpp \
    ../../../src/mobile/bibletime.cpp \
    ../../../src/mobile/bookshelfmanager/installmanager.cpp \
    ../../../src/mobile/bookshelfmanager/installprogress.cpp \
    ../../../src/mobile/bookshelfmanager/installsources.cpp \
    ../../../src/mobile/bookshelfmanager/installsourcesmanager.cpp \
    ../../../src/mobile/btmmain.cpp \
    ../../../src/mobile/keychooser/bookkeychooser.cpp \
    ../../../src/mobile/keychooser/keynamechooser.cpp \
    ../../../src/mobile/keychooser/versechooser.cpp \
    ../../../src/mobile/models/roleitemmodel.cpp \
    ../../../src/mobile/models/searchmodel.cpp \
    ../../../src/mobile/sessionmanager/sessionmanager.cpp \
    ../../../src/mobile/ui/btsearchinterface.cpp \
    ../../../src/mobile/ui/btstyle.cpp \
    ../../../src/mobile/ui/btwindowinterface.cpp \
    ../../../src/mobile/ui/gridchooser.cpp \
    ../../../src/mobile/ui/indexthread.cpp \
    ../../../src/mobile/ui/modulechooser.cpp \
    ../../../src/mobile/ui/moduleinterface.cpp \
    ../../../src/mobile/ui/qtquick2applicationviewer.cpp \
    ../../../src/mobile/ui/treechoosermodel.cpp \
    ../../../src/mobile/ui/viewmanager.cpp \
    ../../../src/mobile/util/findqmlobject.cpp \
    ../../../src/mobile/util/messagedialog.cpp

HEADERS += \
    ../../../src/mobile/bibletimeapp.h \
    ../../../src/mobile/bibletime.h \
    ../../../src/mobile/bookshelfmanager/installmanager.h \
    ../../../src/mobile/bookshelfmanager/installprogress.h \
    ../../../src/mobile/bookshelfmanager/installsources.h \
    ../../../src/mobile/bookshelfmanager/installsourcesmanager.h \
    ../../../src/mobile/btmmain.h \
    ../../../src/mobile/keychooser/bookkeychooser.h \
    ../../../src/mobile/ui/indexthread.h \
    ../../../src/mobile/keychooser/keynamechooser.h \
    ../../../src/mobile/keychooser/versechooser.h \
    ../../../src/mobile/models/roleitemmodel.h \
    ../../../src/mobile/models/searchmodel.h \
    ../../../src/mobile/sessionmanager/sessionmanager.h \
    ../../../src/mobile/ui/btsearchinterface.h \
    ../../../src/mobile/ui/btstyle.h \
    ../../../src/mobile/ui/btwindowinterface.h \
    ../../../src/mobile/ui/gridchooser.h \
    ../../../src/mobile/ui/modulechooser.h \
    ../../../src/mobile/ui/moduleinterface.h \
    ../../../src/mobile/ui/qtquick2applicationviewer.h \
    ../../../src/mobile/ui/treechoosermodel.h \
    ../../../src/mobile/ui/viewmanager.h \
    ../../../src/mobile/util/findqmlobject.h \
    ../../../src/mobile/util/messagedialog.h 

OTHER_FILES += \
    ../../../src/mobile/qml \
    ../../../src/mobile/qml/main.qml \
    ../../../src/mobile/qml/About.qml \
    ../../../src/mobile/qml/BtButtonStyle.qml \
    ../../../src/mobile/qml/ContextMenu.qml \
    ../../../src/mobile/qml/FontSizeSlider.qml \
    ../../../src/mobile/qml/GridChooser.qml \
    ../../../src/mobile/qml/GridChooserButton.qml \
    ../../../src/mobile/qml/InstallManagerChooser.qml \
    ../../../src/mobile/qml/KeyNameChooser.qml \
    ../../../src/mobile/qml/LeftArrow.qml \
    ../../../src/mobile/qml/ListSelectView.qml \
    ../../../src/mobile/qml/ListTextView.qml \
    ../../../src/mobile/qml/ListWorksView.qml \
    ../../../src/mobile/qml/MainToolbar.qml \
    ../../../src/mobile/qml/Menus.qml \
    ../../../src/mobile/qml/MenuButton.qml \
    ../../../src/mobile/qml/ModuleChooser.qml \
    ../../../src/mobile/qml/PrevNextArrow.qml \
    ../../../src/mobile/qml/Progress.qml \
    ../../../src/mobile/qml/Question.qml \
    ../../../src/mobile/qml/RightArrow.qml \
    ../../../src/mobile/qml/Search.qml \
    ../../../src/mobile/qml/SearchIcon.qml \
    ../../../src/mobile/qml/SearchResults.qml \
    ../../../src/mobile/qml/SetFont.qml \
    ../../../src/mobile/qml/SimpleComboBox.qml \
    ../../../src/mobile/qml/StartupBookshelfManager.qml \
    ../../../src/mobile/qml/TitleColorBar.qml \
    ../../../src/mobile/qml/TreeChooser.qml \
    ../../../src/mobile/qml/Window.qml \
    ../../../src/mobile/qml/WindowManager.qml \

lupdate_only {
    SOURCES = ../../../src/mobile/qml/*.qml
    SOURCES += ../../../src/mobile/keychooser/versechooser.cpp
    SOURCES += ../../../src/mobile/bookshelfmanager/installprogress.cpp
    SOURCES += ../../../src/mobile/bookshelfmanager/installsources.cpp
    SOURCES += ../../../src/mobile/bookshelfmanager/installsourcesmanager.cpp
    SOURCES += ../../../src/mobile/qml/StartupBookshelfManager.qml
    SOURCES += ../../../src/mobile/ui/btsearchinterface.cpp
}

TRANSLATIONS += \
    ../../../i18n/messages/mobile_ui_ar.ts \
    ../../../i18n/messages/mobile_ui_cs.ts \
    ../../../i18n/messages/mobile_ui_da.ts \
    ../../../i18n/messages/mobile_ui_de.ts \
    ../../../i18n/messages/mobile_ui_en_GB.ts \
    ../../../i18n/messages/mobile_ui_es.ts \
    ../../../i18n/messages/mobile_ui_et.ts \
    ../../../i18n/messages/mobile_ui_fi.ts \
    ../../../i18n/messages/mobile_ui_fr.ts \
    ../../../i18n/messages/mobile_ui_hu.ts \
    ../../../i18n/messages/mobile_ui_it.ts \
    ../../../i18n/messages/mobile_ui_lt.ts \
    ../../../i18n/messages/mobile_ui_pl.ts \
    ../../../i18n/messages/mobile_ui_pt_BR.ts \
    ../../../i18n/messages/mobile_ui_pt.ts \
    ../../../i18n/messages/mobile_ui_ru.ts \
    ../../../i18n/messages/mobile_ui_sk.ts \
    ../../../i18n/messages/mobile_ui.ts \
    ../../../i18n/messages/mobile_ui_zh_TW.ts \
    ../../../i18n/messages/mobile_ui_C.ts

RESOURCES += \
    ../../../src/mobile/btm.qrc \
    ../../../i18n/messages/mobile_translate.qrc \
    ../../../src/mobile/locales.qrc

