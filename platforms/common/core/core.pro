# BibleTime Core, could be used for Desktop, Mini and Mobile
BT_VERSION = 2.10.0_rc1
DEFINES += BT_VERSION=\\\"$${BT_VERSION}\\\"

# Useless warnings
gcc:QMAKE_CXXFLAGS_DEBUG += -Wno-switch -Wno-unused-parameter -Wno-unused-variable -Wno-reorder -Wno-missing-field-initializers

INCLUDEPATH += ../../../src

SOURCES += \
    ../../../src/backend/bookshelfmodel/btbookshelffiltermodel.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../src/backend/bookshelfmodel/languageitem.cpp \
    ../../../src/backend/bookshelfmodel/item.cpp \
    ../../../src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../src/backend/btbookmarksmodel.cpp \
    ../../../src/backend/btglobal.cpp \
    ../../../src/backend/btinstallbackend.cpp \
    ../../../src/backend/btinstallmgr.cpp \
    ../../../src/backend/btinstallthread.cpp \
    ../../../src/backend/config/btconfig.cpp \
    ../../../src/backend/config/btconfigcore.cpp \
    ../../../src/backend/cswordmodulesearch.cpp \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../src/backend/filters/gbftohtml.cpp \
    ../../../src/backend/filters/osistohtml.cpp \
    ../../../src/backend/filters/plaintohtml.cpp \
    ../../../src/backend/filters/teitohtml.cpp \
    ../../../src/backend/filters/thmltohtml.cpp \
    ../../../src/backend/keys/cswordkey.cpp \
    ../../../src/backend/keys/cswordldkey.cpp \
    ../../../src/backend/keys/cswordtreekey.cpp \
    ../../../src/backend/keys/cswordversekey.cpp \
    ../../../src/backend/managers/btstringmgr.cpp \
    ../../../src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../src/backend/managers/clanguagemgr.cpp \
    ../../../src/backend/managers/cswordbackend.cpp \
    ../../../src/backend/managers/referencemanager.cpp \
    ../../../src/backend/models/btmoduletextmodel.cpp \
    ../../../src/backend/rendering/btinforendering.cpp \
    ../../../src/backend/rendering/cbookdisplay.cpp \
    ../../../src/backend/rendering/cchapterdisplay.cpp \
    ../../../src/backend/rendering/cdisplayrendering.cpp \
    ../../../src/backend/rendering/centrydisplay.cpp \
    ../../../src/backend/rendering/chtmlexportrendering.cpp \
    ../../../src/backend/rendering/cplaintextexportrendering.cpp \
    ../../../src/backend/rendering/ctextrendering.cpp \
    ../../../src/backend/btsignal.cpp \
    ../../../src/util/cresmgr.cpp \
    ../../../src/util/directory.cpp \
    ../../../src/util/bticons.cpp \
    ../../../src/util/btmodules.cpp \
    ../../../src/util/tool.cpp \


HEADERS += \
    ../../../src/backend/bookshelfmodel/btbookshelffiltermodel.h \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../src/backend/bookshelfmodel/categoryitem.h \
    ../../../src/backend/bookshelfmodel/indexingitem.h \
    ../../../src/backend/bookshelfmodel/item.h \
    ../../../src/backend/bookshelfmodel/languageitem.h \
    ../../../src/backend/bookshelfmodel/moduleitem.h \
    ../../../src/backend/btbookmarksmodel.h \
    ../../../src/backend/btglobal.h \
    ../../../src/backend/btinstallbackend.h \
    ../../../src/backend/btinstallmgr.h \
    ../../../src/backend/btinstallthread.h \
    ../../../src/backend/btsignal.h \
    ../../../src/backend/config/btconfig.h \
    ../../../src/backend/config/btconfigcore.h \
    ../../../src/backend/cswordmodulesearch.h \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../src/backend/drivers/cswordmoduleinfo.h \
    ../../../src/backend/filters/gbftohtml.h \
    ../../../src/backend/filters/osistohtml.h \
    ../../../src/backend/filters/plaintohtml.h \
    ../../../src/backend/filters/teitohtml.h \
    ../../../src/backend/filters/thmltohtml.h \
    ../../../src/backend/managers/btstringmgr.h \
    ../../../src/backend/managers/cdisplaytemplatemgr.h \
    ../../../src/backend/managers/clanguagemgr.h \
    ../../../src/backend/managers/cswordbackend.h \
    ../../../src/backend/managers/referencemanager.h \
    ../../../src/backend/keys/cswordkey.h \
    ../../../src/backend/keys/cswordldkey.h \
    ../../../src/backend/keys/cswordtreekey.h \
    ../../../src/backend/keys/cswordversekey.h \
    ../../../src/backend/models/btmoduletextmodel.h \
    ../../../src/backend/rendering/cbookdisplay.h \
    ../../../src/backend/rendering/cchapterdisplay.h \
    ../../../src/backend/rendering/cdisplayrendering.h \
    ../../../src/backend/rendering/centrydisplay.h \
    ../../../src/backend/rendering/chtmlexportrendering.h \
    ../../../src/backend/rendering/cplaintextexportrendering.h \
    ../../../src/backend/rendering/ctextrendering.h \
    ../../../src/util/cresmgr.h \
    ../../../src/util/directory.h \
    ../../../src/util/bticons.h \
    ../../../src/util/btmodules.h \
    ../../../src/util/tool.h \

TRANSLATIONS += \
    ../../../i18n/messages/bibletime_ui_ar.ts \
    ../../../i18n/messages/bibletime_ui_cs.ts \
    ../../../i18n/messages/bibletime_ui_da.ts \
    ../../../i18n/messages/bibletime_ui_de.ts \
    ../../../i18n/messages/bibletime_ui_en_GB.ts \
    ../../../i18n/messages/bibletime_ui_es.ts \
    ../../../i18n/messages/bibletime_ui_et.ts \
    ../../../i18n/messages/bibletime_ui_fi.ts \
    ../../../i18n/messages/bibletime_ui_hu.ts \
    ../../../i18n/messages/bibletime_ui_it.ts \
    ../../../i18n/messages/bibletime_ui_ko.ts \
    ../../../i18n/messages/bibletime_ui_lt.ts \
    ../../../i18n/messages/bibletime_ui_pl.ts \
    ../../../i18n/messages/bibletime_ui_pt_BR.ts \
    ../../../i18n/messages/bibletime_ui_pt.ts \
    ../../../i18n/messages/bibletime_ui_ru.ts \
    ../../../i18n/messages/bibletime_ui_sk.ts \
    ../../../i18n/messages/bibletime_ui.ts \
    ../../../i18n/messages/bibletime_ui_zh_TW.ts 

lupdate_only {
    SOURCES = ../../../src/backend/drivers/cswordmoduleinfo.cpp \
}

# Core Platform Section

# iOS Platform
mac:CONFIG -= webkit
mac:DEFINES += unix
mac:DEFINES += __unix__

# Android platform
android {
DEFINES += STDC_HEADERS
}

# Symbian platform
# on S60 webkit not works, maybe wrong packaging?
symbian {
DEFINES -= BT_VERSION=\\\"$${BT_VERSION}\\\"
greaterThan(S60_VERSION, 5.0) {
DEFINES += BT_VERSION=\"$${BT_VERSION}\"
}
else {
DEFINES += BT_VERSION=\"\\\"$${BT_VERSION}\\\"\"
CONFIG -= webkit
}
}

# BlackBerry10 Platform
blackberry {
CONFIG -= webkit
DEFINES += unix
LIBS += -lsocket
}

# Core Configuration Section

# Clucene
include(../../common/clucene/clucene.pro)

# CURL
# optional
curl:include(../../common/curl/curl.pro)

# ICU
# optional
icu:include(../../common/icu/icu.pro)

# Sword
include(../../common/sword/sword.pro)
