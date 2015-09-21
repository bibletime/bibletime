# BibleTime Core, could be used for Desktop, Mini and Mobile
BT_VERSION = 2.10.0
DEFINES += BT_VERSION=\\\"$${BT_VERSION}\\\"

INCLUDEPATH += ../../../src

SOURCES += \
    ../../../src/backend/cswordmodulesearch.cpp \
    ../../../src/backend/btmoduletreeitem.cpp \
    ../../../src/backend/btinstallbackend.cpp \
    ../../../src/backend/managers/referencemanager.cpp \
    ../../../src/backend/managers/cswordbackend.cpp \
    ../../../src/backend/managers/clanguagemgr.cpp \
    ../../../src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../src/backend/managers/btstringmgr.cpp \
    ../../../src/util/directory.cpp \
    ../../../src/util/cresmgr.cpp \
    ../../../src/backend/config/btconfig.cpp \
    ../../../src/backend/config/btconfigcore.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../src/backend/btbookmarksmodel.cpp \
    ../../../src/backend/btinstallmgr.cpp \
    ../../../src/backend/keys/cswordversekey.cpp \
    ../../../src/backend/keys/cswordldkey.cpp \
    ../../../src/backend/keys/cswordkey.cpp \
    ../../../src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../src/backend/rendering/ctextrendering.cpp \
    ../../../src/backend/rendering/chtmlexportrendering.cpp \
    ../../../src/backend/rendering/centrydisplay.cpp \
    ../../../src/backend/rendering/cdisplayrendering.cpp \
    ../../../src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../src/backend/bookshelfmodel/languageitem.cpp \
    ../../../src/backend/bookshelfmodel/item.cpp \
    ../../../src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../src/backend/btinstallthread.cpp \
    ../../../src/util/tool.cpp \
    ../../../src/util/geticon.cpp \
    ../../../src/backend/rendering/cchapterdisplay.cpp \
    ../../../src/backend/rendering/cbookdisplay.cpp \
    ../../../src/backend/filters/thmltohtml.cpp \
    ../../../src/backend/filters/teitohtml.cpp \
    ../../../src/backend/filters/plaintohtml.cpp \
    ../../../src/backend/filters/osistohtml.cpp \
    ../../../src/backend/filters/gbftohtml.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../src/backend/filters/thmltoplain.cpp \
    ../../../src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../src/backend/keys/cswordtreekey.cpp \
    ../../../src/backend/filters/btosismorphsegmentation.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelffiltermodel.cpp \
    ../../../src/backend/rendering/cplaintextexportrendering.cpp \
    ../../../src/backend/models/btmoduletextmodel.cpp \
    ../../../src/btglobal.cpp

	
HEADERS += \
    ../../../src/backend/cswordmodulesearch.h \
    ../../../src/backend/btmoduletreeitem.h \
    ../../../src/backend/btinstallbackend.h \
    ../../../src/backend/managers/referencemanager.h \
    ../../../src/backend/managers/cswordbackend.h \
    ../../../src/backend/managers/clanguagemgr.h \
    ../../../src/backend/managers/cdisplaytemplatemgr.h \
    ../../../src/backend/managers/btstringmgr.h \
    ../../../src/util/directory.h \
    ../../../src/util/cresmgr.h \
    ../../../src/backend/config/btconfig.h \
    ../../../src/backend/config/btconfigcore.h \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../src/backend/btbookmarksmodel.h \
    ../../../src/backend/btinstallmgr.h \
    ../../../src/backend/btinstallthread.h \
    ../../../src/backend/keys/cswordversekey.h \
    ../../../src/backend/keys/cswordldkey.h \
    ../../../src/backend/keys/cswordkey.h \
    ../../../src/backend/keys/cswordtreekey.h \
    ../../../src/backend/drivers/cswordmoduleinfo.h \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../src/backend/rendering/ctextrendering.h \
    ../../../src/backend/rendering/chtmlexportrendering.h \
    ../../../src/backend/rendering/centrydisplay.h \
    ../../../src/backend/rendering/cdisplayrendering.h \
    ../../../src/backend/rendering/cplaintextexportrendering.h \
    ../../../src/backend/bookshelfmodel/moduleitem.h \
    ../../../src/backend/bookshelfmodel/languageitem.h \
    ../../../src/backend/bookshelfmodel/item.h \
    ../../../src/backend/bookshelfmodel/categoryitem.h \
    ../../../src/util/tool.h \
    ../../../src/util/btsignal.h \
    ../../../src/backend/rendering/cchapterdisplay.h \
    ../../../src/backend/rendering/cbookdisplay.h \
    ../../../src/backend/filters/thmltohtml.h \
    ../../../src/backend/filters/teitohtml.h \
    ../../../src/backend/filters/plaintohtml.h \
    ../../../src/backend/filters/osistohtml.h \
    ../../../src/backend/filters/gbftohtml.h \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../src/backend/filters/thmltoplain.h \
    ../../../src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../src/backend/bookshelfmodel/indexingitem.h \
    ../../../src/backend/keys/cswordtreekey.h \
    ../../../src/backend/filters/btosismorphsegmentation.h \
    ../../../src/backend/bookshelfmodel/btbookshelffiltermodel.h
	

# Translation
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


# Core Platform Section

# iOS Platform
mac:CONFIG -= webkit

# Android platform
android {
greaterThan(QT_MAJOR_VERSION, 4):CONFIG -= webkit
DEFINES += STDC_HEADERS
}

# Symbian platform
# on S60 webkit not works, maybe wrong packaging?
symbian:!greaterThan(S60_VERSION, 5.0):CONFIG -= webkit

# BlackBerry10 Platform
blackberry {
CONFIG -= webkit
DEFINES += unix
LIBS += -lsocket
}

# Qt
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
svg:QT += svg xml
qml {
DEFINES += BT_MINI_QML
QT += qml quick
}

# Core Configuration Section

# WebKit
# should be after platforms section, optional
webkit {
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}
else {
    QT += webkit
}
DEFINES += BT_MINI_WEBKIT
}
else:!mini:!mobile {
warning("Non Mini build: WebKit required")
}

# Clucene
clucene:include(../../common/clucene/clucene.pro)
!clucene:DEFINES += BT_NO_CLUCENE

# CURL
# optional
curl:include(../../common/curl/curl.pro)

# ICU
# optional
curl:include(../../common/icu/icu.pro)

# Sword
include(../../common/sword/sword.pro)
