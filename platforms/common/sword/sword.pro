# Sword 1.8.1 or higher required 

DEFINES += EXCLUDEXZ EXCLUDEBZIP2
DEFINES += STDC_HEADERS

isEmpty(SWORD_PATH):SWORD_PATH = ../../../../sword
isEmpty(SWORD_VERSION_NUM):SWORD_VERSION_NUM = 107005000


INCLUDEPATH += $${SWORD_PATH}/include


SOURCES += \
    $${SWORD_PATH}/src/frontend/swdisp.cpp \
    $${SWORD_PATH}/src/frontend/swlog.cpp \
    $${SWORD_PATH}/src/keys/swkey.cpp \
    $${SWORD_PATH}/src/keys/listkey.cpp \
    $${SWORD_PATH}/src/keys/strkey.cpp \
    $${SWORD_PATH}/src/keys/treekey.cpp \
    $${SWORD_PATH}/src/keys/treekeyidx.cpp \
    $${SWORD_PATH}/src/keys/versekey.cpp \
    $${SWORD_PATH}/src/keys/versetreekey.cpp \
    $${SWORD_PATH}/src/mgr/swconfig.cpp \
    $${SWORD_PATH}/src/mgr/swmgr.cpp \
    $${SWORD_PATH}/src/mgr/swfiltermgr.cpp \
    $${SWORD_PATH}/src/mgr/encfiltmgr.cpp \
    $${SWORD_PATH}/src/mgr/markupfiltmgr.cpp \
    $${SWORD_PATH}/src/mgr/filemgr.cpp \
    $${SWORD_PATH}/src/mgr/versificationmgr.cpp \
    $${SWORD_PATH}/src/mgr/swlocale.cpp \
    $${SWORD_PATH}/src/mgr/localemgr.cpp \
    $${SWORD_PATH}/src/mgr/swcacher.cpp \
    $${SWORD_PATH}/src/mgr/swsearchable.cpp \
    $${SWORD_PATH}/src/mgr/installmgr.cpp \
    $${SWORD_PATH}/src/mgr/stringmgr.cpp \
    $${SWORD_PATH}/src/modules/swmodule.cpp \
    $${SWORD_PATH}/src/modules/comments/swcom.cpp \
    $${SWORD_PATH}/src/modules/comments/hrefcom/hrefcom.cpp \
    $${SWORD_PATH}/src/modules/comments/rawcom/rawcom.cpp \
    $${SWORD_PATH}/src/modules/comments/rawcom4/rawcom4.cpp \
    $${SWORD_PATH}/src/modules/comments/rawfiles/rawfiles.cpp \
    $${SWORD_PATH}/src/modules/comments/zcom/zcom.cpp \
    $${SWORD_PATH}/src/modules/common/rawstr.cpp \
    $${SWORD_PATH}/src/modules/common/rawstr4.cpp \
    $${SWORD_PATH}/src/modules/common/swcomprs.cpp \
    $${SWORD_PATH}/src/modules/common/lzsscomprs.cpp \
    $${SWORD_PATH}/src/modules/common/rawverse.cpp \
    $${SWORD_PATH}/src/modules/common/rawverse4.cpp \
    $${SWORD_PATH}/src/modules/common/swcipher.cpp \
    $${SWORD_PATH}/src/modules/common/zverse.cpp \
    $${SWORD_PATH}/src/modules/common/zstr.cpp \
    $${SWORD_PATH}/src/modules/common/entriesblk.cpp \
    $${SWORD_PATH}/src/modules/common/sapphire.cpp \
    $${SWORD_PATH}/src/modules/common/zipcomprs.cpp \
    $${SWORD_PATH}/src/modules/filters/swbasicfilter.cpp \
    $${SWORD_PATH}/src/modules/filters/swoptfilter.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfhtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfwebif.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfplain.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/gbffootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfredletterwords.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfmorph.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfwordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlfootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlmorph.cpp \
    $${SWORD_PATH}/src/modules/filters/thmllemma.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlscripref.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlvariants.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlgbf.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlhtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlwebif.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlwordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/teiplain.cpp \
    $${SWORD_PATH}/src/modules/filters/teirtf.cpp \
    $${SWORD_PATH}/src/modules/filters/teihtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/teixhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfthml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfosis.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlosis.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlplain.cpp \
    $${SWORD_PATH}/src/modules/filters/osisosis.cpp \
    $${SWORD_PATH}/src/modules/filters/osisheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/osisfootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/osishtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/osisxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/osiswebif.cpp \
    $${SWORD_PATH}/src/modules/filters/osismorph.cpp \
    $${SWORD_PATH}/src/modules/filters/osisstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/osisplain.cpp \
    $${SWORD_PATH}/src/modules/filters/osisrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/osislemma.cpp \
    $${SWORD_PATH}/src/modules/filters/osisredletterwords.cpp \
    $${SWORD_PATH}/src/modules/filters/osisscripref.cpp \
    $${SWORD_PATH}/src/modules/filters/osisvariants.cpp \
    $${SWORD_PATH}/src/modules/filters/osiswordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/osismorphsegmentation.cpp \
    $${SWORD_PATH}/src/modules/filters/latin1utf8.cpp \
    $${SWORD_PATH}/src/modules/filters/latin1utf16.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8utf16.cpp \
    $${SWORD_PATH}/src/modules/filters/utf16utf8.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8html.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8latin1.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8cantillation.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8hebrewpoints.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8arabicpoints.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8greekaccents.cpp \
    $${SWORD_PATH}/src/modules/filters/cipherfil.cpp \
    $${SWORD_PATH}/src/modules/filters/rtfhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/greeklexattribs.cpp \
    $${SWORD_PATH}/src/modules/filters/unicodertf.cpp \
    $${SWORD_PATH}/src/modules/filters/papyriplain.cpp \
    $${SWORD_PATH}/src/modules/genbook/swgenbook.cpp \
    $${SWORD_PATH}/src/modules/genbook/rawgenbook/rawgenbook.cpp \
    $${SWORD_PATH}/src/modules/lexdict/swld.cpp \
    $${SWORD_PATH}/src/modules/lexdict/rawld/rawld.cpp \
    $${SWORD_PATH}/src/modules/lexdict/rawld4/rawld4.cpp \
    $${SWORD_PATH}/src/modules/lexdict/zld/zld.cpp \
    $${SWORD_PATH}/src/modules/texts/swtext.cpp \
    $${SWORD_PATH}/src/modules/texts/rawtext/rawtext.cpp \
    $${SWORD_PATH}/src/modules/texts/rawtext4/rawtext4.cpp \
    $${SWORD_PATH}/src/modules/texts/ztext/ztext.cpp \
    $${SWORD_PATH}/src/utilfuns/swobject.cpp \
    $${SWORD_PATH}/src/utilfuns/utilstr.cpp \
    $${SWORD_PATH}/src/utilfuns/utilxml.cpp \
    $${SWORD_PATH}/src/utilfuns/swversion.cpp \
    $${SWORD_PATH}/src/utilfuns/swbuf.cpp \
    $${SWORD_PATH}/src/utilfuns/ftpparse.c \
    $${SWORD_PATH}/src/utilfuns/url.cpp \
    $${SWORD_PATH}/src/utilfuns/roman.cpp \
    $${SWORD_PATH}/src/utilfuns/zlib/adler32.c \
    $${SWORD_PATH}/src/utilfuns/zlib/compress.c \
    $${SWORD_PATH}/src/utilfuns/zlib/crc32.c \
    $${SWORD_PATH}/src/utilfuns/zlib/deflate.c \
    $${SWORD_PATH}/src/utilfuns/zlib/gzclose.c \
    $${SWORD_PATH}/src/utilfuns/zlib/gzread.c \
    $${SWORD_PATH}/src/utilfuns/zlib/gzwrite.c \
    $${SWORD_PATH}/src/utilfuns/zlib/infback.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inftrees.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inflate.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inffast.c \
    $${SWORD_PATH}/src/utilfuns/zlib/trees.c \
    $${SWORD_PATH}/src/utilfuns/zlib/uncompr.c \
    $${SWORD_PATH}/src/utilfuns/zlib/zutil.c \
    $${SWORD_PATH}/src/utilfuns/zlib/untgz.c \
    $${SWORD_PATH}/src/utilfuns/zlib/gzlib.c \
    $${SWORD_PATH}/src/modules/filters/osisglosses.cpp \
    $${SWORD_PATH}/src/modules/filters/osisxlit.cpp \
    $${SWORD_PATH}/src/modules/filters/osisenum.cpp \
    $${SWORD_PATH}/src/modules/filters/osisreferencelinks.cpp \
    $${SWORD_PATH}/src/modules/filters/scsuutf8.cpp \
    $${SWORD_PATH}/src/mgr/remotetrans.cpp \

lessThan(SWORD_VERSION_NUM, 107005000) { # 1.7 series
SOURCES += \

}
else { # 1.8 series
SOURCES += \
    $${SWORD_PATH}/src/modules/comments/zcom4/zcom4.cpp \
    $${SWORD_PATH}/src/modules/common/zverse4.cpp \
    $${SWORD_PATH}/src/modules/filters/gbflatex.cpp \
    $${SWORD_PATH}/src/modules/filters/teilatex.cpp \
    $${SWORD_PATH}/src/modules/filters/thmllatex.cpp \
    $${SWORD_PATH}/src/modules/filters/osislatex.cpp \
    $${SWORD_PATH}/src/modules/texts/ztext4/ztext4.cpp \

}

# Compressors
windows:DEFINES += EXCLUDEXZ EXCLUDEBZIP2

!contains(DEFINES, EXCLUDEXZ) {
    SOURCES += $${SWORD_PATH}/src/modules/common/xzcomprs.cpp
}
!contains(DEFINES, EXCLUDEBZIP2) {
    SOURCES += $${SWORD_PATH}/src/modules/common/bz2comprs.cpp
}


# CURL
curl {
SOURCES += $${SWORD_PATH}/src/mgr/curlftpt.cpp
SOURCES += $${SWORD_PATH}/src/mgr/curlhttpt.cpp
}
else {
SOURCES += $${SWORD_PATH}/src/mgr/ftplibftpt.cpp
}

# Non Symbian platforms
!symbian {
INCLUDEPATH += $${SWORD_PATH}/include/internal/regex

SOURCES += $${SWORD_PATH}/src/utilfuns/regex.c

!curl:SOURCES += $${SWORD_PATH}/src/utilfuns/ftplib.c
}
else {
# include ftplib from Symbian folder
!curl:SOURCES += ftplib.c
}


# Windows platform
windows {
DEFINES += _CRT_SECURE_NO_WARNINGS REGEX_MALLOC

INCLUDEPATH += $${SWORD_PATH}/src/utilfuns/win32

SOURCES += $${SWORD_PATH}/src/utilfuns/win32/dirent.cpp

LIBS += -lws2_32
}

# MeeGo platform
unix:contains(MEEGO_EDITION,harmattan) {
DEFINES += STDC_HEADERS
}
