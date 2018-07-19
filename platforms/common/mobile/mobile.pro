CONFIG += clucene  mobile c++11
QT += svg xml qml quick androidextras

QML_ROOT_PATH = ../../../src/mobile/qml

DEFINES += BT_MOBILE

GIT_VERSION = $$system(git rev-parse HEAD)
DEFINES += BT_GIT_VERSION=\\\"$$GIT_VERSION\\\"

QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_DEBUG += -O0

include(../../common/core/core.pro)

SOURCES += \
    ../../../src/mobile/bibletimeapp.cpp \
    ../../../src/mobile/bibletime.cpp \
    ../../../src/mobile/bookshelfmanager/installsources.cpp \
    ../../../src/mobile/btmmain.cpp \
    ../../../src/mobile/config/btmconfig.cpp \
    ../../../src/mobile/models/roleitemmodel.cpp \
    ../../../src/mobile/models/searchmodel.cpp \
    ../../../src/mobile/ui/btbookmarkinterface.cpp \
    ../../../src/mobile/ui/btsearchinterface.cpp \
    ../../../src/mobile/ui/btstyle.cpp \
    ../../../src/mobile/ui/btmmoduletextfilter.cpp \
    ../../../src/mobile/ui/btwindowinterface.cpp \
    ../../../src/mobile/ui/indexthread.cpp \
    ../../../src/mobile/ui/installinterface.cpp \
    ../../../src/mobile/ui/moduleinterface.cpp \
    ../../../src/mobile/ui/sessioninterface.cpp \
    ../../../src/mobile/ui/configinterface.cpp \
    ../../../src/mobile/ui/chooserinterface.cpp \
    ../../../src/mobile/ui/treechoosermodel.cpp \
    ../../../src/mobile/util/messagedialog.cpp

HEADERS += \
    ../../../src/mobile/bibletimeapp.h \
    ../../../src/mobile/bibletime.h \
    ../../../src/mobile/bookshelfmanager/installsources.h \
    ../../../src/mobile/btmmain.h \
    ../../../src/mobile/config/btmconfig.h \
    ../../../src/mobile/ui/indexthread.h \
    ../../../src/mobile/models/roleitemmodel.h \
    ../../../src/mobile/models/searchmodel.h \
    ../../../src/mobile/ui/btbookmarkinterface.h \
    ../../../src/mobile/ui/btsearchinterface.h \
    ../../../src/mobile/ui/btstyle.h \
    ../../../src/mobile/ui/btmmoduletextfilter.h \
    ../../../src/mobile/ui/btwindowinterface.h \
    ../../../src/mobile/ui/installinterface.h \
    ../../../src/mobile/ui/moduleinterface.h \
    ../../../src/mobile/ui/sessioninterface.h \
    ../../../src/mobile/ui/configinterface.h \
    ../../../src/mobile/ui/chooserinterface.h \
    ../../../src/mobile/ui/treechoosermodel.h \
    ../../../src/mobile/util/messagedialog.h 

OTHER_FILES += \
    ../../../src/mobile/qml \
    ../../../src/mobile/qml/main.qml \
    ../../../src/mobile/qml/About.qml \
    ../../../src/mobile/qml/AddBookmark.qml \
    ../../../src/mobile/qml/AddFolder.qml \
    ../../../src/mobile/qml/Back.qml \
    ../../../src/mobile/qml/BookmarkFolders.qml \
    ../../../src/mobile/qml/BookmarkManager.qml \
    ../../../src/mobile/qml/BtButtonStyle.qml \
    ../../../src/mobile/qml/ContextMenu.qml \
    ../../../src/mobile/qml/BookIcon.qml \
    ../../../src/mobile/qml/ChooseReference.qml \
    ../../../src/mobile/qml/CopyVerses.qml \
    ../../../src/mobile/qml/DefaultDoc.qml \
    ../../../src/mobile/qml/Folder.qml \
    ../../../src/mobile/qml/FontSizeSlider.qml \
    ../../../src/mobile/qml/GridChooser.qml \
    ../../../src/mobile/qml/GridChooserButton.qml \
    ../../../src/mobile/qml/InformationDialog.qml \
    ../../../src/mobile/qml/InstallManagerChooser.qml \
    ../../../src/mobile/qml/KeyNameChooser.qml \
    ../../../src/mobile/qml/Left.qml \
    ../../../src/mobile/qml/LeftArrow.qml \
    ../../../src/mobile/qml/ListSelectView.qml \
    ../../../src/mobile/qml/ListTextView.qml \
    ../../../src/mobile/qml/ListWorksView.qml \
    ../../../src/mobile/qml/MagView.qml \
    ../../../src/mobile/qml/MainToolbar.qml \
    ../../../src/mobile/qml/Menus.qml \
    ../../../src/mobile/qml/MenuButton.qml \
    ../../../src/mobile/qml/ModuleChooser.qml \
    ../../../src/mobile/qml/ModuleDisplay.qml \
    ../../../src/mobile/qml/PrevNextArrow.qml \
    ../../../src/mobile/qml/Progress.qml \
    ../../../src/mobile/qml/Question.qml \
    ../../../src/mobile/qml/ReferenceDisplay.qml \
    ../../../src/mobile/qml/RightArrow.qml \
    ../../../src/mobile/qml/Search.qml \
    ../../../src/mobile/qml/SearchIcon.qml \
    ../../../src/mobile/qml/SearchResults.qml \
    ../../../src/mobile/qml/SetFont.qml \
    ../../../src/mobile/qml/SimpleComboBox.qml \
    ../../../src/mobile/qml/StartupBookshelfManager.qml \
    ../../../src/mobile/qml/TextEditor.qml \
    ../../../src/mobile/qml/TitleColorBar.qml \
    ../../../src/mobile/qml/TreeChooser.qml \
    ../../../src/mobile/qml/VerseChooser.qml \
    ../../../src/mobile/qml/Window.qml \
    ../../../src/mobile/qml/WindowManager.qml \

lupdate_only {
    SOURCES  = ../../../src/mobile/qml/*.qml
    SOURCES += ../../../src/mobile/ui/btbookmarkinterface.cpp
    SOURCES += ../../../src/mobile/ui/btsearchinterface.cpp
    SOURCES += ../../../src/mobile/ui/btwindowinterface.cpp
    SOURCES += ../../../src/mobile/ui/configinterface.cpp
    SOURCES += ../../../src/mobile/ui/chooserinterface.cpp
    SOURCES += ../../../src/mobile/ui/installinterface.cpp
    SOURCES += ../../../src/mobile/ui/sessionlinterface.cpp
    SOURCES += ../../../src/mobile/ui/moduleinterface.cpp
    SOURCES += ../../../src/mobile/bookshelfmanager/installsources.cpp
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
    ../../../i18n/messages/bibletime_translate.qrc \
    ../../../i18n/messages/mobile_translate.qrc \
    ../../../src/mobile/locales.qrc

