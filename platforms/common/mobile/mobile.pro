CONFIG += clucene svg xml mobile qml quick

DEFINES += BT_MOBILE

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
    ../../../src/mobile/keychooser/versechooser.cpp \
    ../../../src/mobile/models/roleitemmodel.cpp \
    ../../../src/mobile/sessionmanager/sessionmanager.cpp \
    ../../../src/mobile/ui/btstyle.cpp \
    ../../../src/mobile/ui/btwindowinterface.cpp \
    ../../../src/mobile/ui/gridchooser.cpp \
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
    ../../../src/mobile/keychooser/versechooser.h \
    ../../../src/mobile/models/roleitemmodel.h \
    ../../../src/mobile/sessionmanager/sessionmanager.h \
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
    ../../../src/mobile/qml/checkmark.svg \
    ../../../src/mobile/qml/main.qml \
    ../../../src/mobile/qml/ListWorksView.qml \
    ../../../src/mobile/qml/FontSizeSlider.qml \
    ../../../src/mobile/qml/InstallManagerChooser.qml \
    ../../../src/mobile/qml/MenuView.qml \
    ../../../src/mobile/qml/Settings.qml \
    ../../../src/mobile/qml/Menus.qml \
    ../../../src/mobile/qml/GridChooser.qml \
    ../../../src/mobile/qml/ModuleChooser.qml \
    ../../../src/mobile/qml/Progress.qml \
    ../../../src/mobile/qml/GridChooserButton.qml \
    ../../../src/mobile/qml/ContextMenu.qml \
    ../../../src/mobile/qml/ImageButton.qml \
    ../../../src/mobile/qml/tab.png \
    ../../../src/mobile/qml/Window.qml \
    ../../../src/mobile/qml/MenuButton.qml \
    ../../../src/mobile/qml/leftarrow.svg \
    ../../../src/mobile/qml/rightarrow.svg \
    ../../../src/mobile/qml/ListTextView.qml \
    ../../../src/mobile/qml/WindowManager.qml \
    ../../../src/mobile/qml/MainToolbar.qml \
    ../../../src/mobile/qml/TreeChooser.qml

RESOURCES += \
    ../../../src/mobile/btm.qrc \

