# BibleTime project for QtCreator
#   to run this target you must copy "share" folder from original BibleTime
#   installation to parent folder of executeale location

# Configuration
VERSION = 2.10.0_dev
CONFIG += clucene svg xml webkit
QT += printsupport

INCLUDEPATH += . \

SOURCES += \
    ../../../src/bibletime.cpp \
    ../../../src/bibletime_init.cpp \
    ../../../src/bibletime_slots.cpp \
    ../../../src/bibletimeapp.cpp \
    ../../../src/main.cpp \
    ../../../src/frontend/btaboutdialog.cpp \
    ../../../src/frontend/btaboutmoduledialog.cpp \
    ../../../src/frontend/btbookshelfdockwidget.cpp \
    ../../../src/frontend/btbookshelfgroupingmenu.cpp \
    ../../../src/frontend/btbookshelfview.cpp \
    ../../../src/frontend/btbookshelfwidget.cpp \
    ../../../src/frontend/btcentralwidget.cpp \
    ../../../src/frontend/btmenuview.cpp \
    ../../../src/frontend/btmodulechooserdialog.cpp \
    ../../../src/frontend/btmoduleindexdialog.cpp \
    ../../../src/frontend/btopenworkaction.cpp \
    ../../../src/frontend/cexportmanager.cpp \
    ../../../src/frontend/cmdiarea.cpp \
    ../../../src/frontend/cprinter.cpp \
    ../../../src/frontend/bookmarks/bteditbookmarkdialog.cpp \
    ../../../src/frontend/bookmarks/cbookmarkindex.cpp \
    ../../../src/frontend/bookshelfmanager/btconfigdialog.cpp \
    ../../../src/frontend/bookshelfmanager/btmodulemanagerdialog.cpp \
    ../../../src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp \
    ../../../src/frontend/bookshelfmanager/indexpage/btindexpage.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpage.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpagemodel.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpageworkswidget.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpathdialog.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.cpp \
    ../../../src/frontend/bookshelfmanager/installpage/btrefreshprogressdialog.cpp \
    ../../../src/frontend/bookshelfmanager/removepage/btremovepage.cpp \
    ../../../src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.cpp \
    ../../../src/frontend/cinfodisplay.cpp \
    ../../../src/frontend/display/btcolorwidget.cpp \
    ../../../src/frontend/display/btfindwidget.cpp \
    ../../../src/frontend/display/btfontsizewidget.cpp \
    ../../../src/frontend/display/bthtmljsobject.cpp \
    ../../../src/frontend/display/bthtmlreaddisplay.cpp \
    ../../../src/frontend/display/cdisplay.cpp \
    ../../../src/frontend/display/chtmlwritedisplay.cpp \
    ../../../src/frontend/display/cplainwritedisplay.cpp \
    ../../../src/frontend/display/creaddisplay.cpp \
    ../../../src/frontend/displaywindow/btactioncollection.cpp \
    ../../../src/frontend/displaywindow/btdisplaysettingsbutton.cpp \
    ../../../src/frontend/displaywindow/btmodulechooserbar.cpp \
    ../../../src/frontend/displaywindow/btmodulechooserbutton.cpp \
    ../../../src/frontend/displaywindow/bttextwindowheader.cpp \
    ../../../src/frontend/displaywindow/bttextwindowheaderwidget.cpp \
    ../../../src/frontend/displaywindow/bttoolbarpopupaction.cpp \
    ../../../src/frontend/displaywindow/cbiblereadwindow.cpp \
    ../../../src/frontend/displaywindow/cbookreadwindow.cpp \
    ../../../src/frontend/displaywindow/ccommentaryreadwindow.cpp \
    ../../../src/frontend/displaywindow/cdisplaywindow.cpp \
    ../../../src/frontend/displaywindow/chtmlwritewindow.cpp \
    ../../../src/frontend/displaywindow/clexiconreadwindow.cpp \
    ../../../src/frontend/displaywindow/cplainwritewindow.cpp \
    ../../../src/frontend/displaywindow/creadwindow.cpp \
    ../../../src/frontend/keychooser/bthistory.cpp \
    ../../../src/frontend/keychooser/cbookkeychooser.cpp \
    ../../../src/frontend/keychooser/cbooktreechooser.cpp \
    ../../../src/frontend/keychooser/ckeychooser.cpp \
    ../../../src/frontend/keychooser/ckeychooserwidget.cpp \
    ../../../src/frontend/keychooser/clexiconkeychooser.cpp \
    ../../../src/frontend/keychooser/cscrollbutton.cpp \
    ../../../src/frontend/keychooser/cscrollerwidgetset.cpp \
    ../../../src/frontend/keychooser/versekeychooser/btbiblekeywidget.cpp \
    ../../../src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.cpp \
    ../../../src/frontend/keychooser/versekeychooser/btversekeymenu.cpp \
    ../../../src/frontend/keychooser/versekeychooser/cbiblekeychooser.cpp \
    ../../../src/frontend/messagedialog.cpp \
    ../../../src/frontend/searchdialog/btsearchmodulechooserdialog.cpp \
    ../../../src/frontend/searchdialog/btsearchoptionsarea.cpp \
    ../../../src/frontend/searchdialog/btsearchresultarea.cpp \
    ../../../src/frontend/searchdialog/btsearchsyntaxhelpdialog.cpp \
    ../../../src/frontend/searchdialog/chistorycombobox.cpp \
    ../../../src/frontend/searchdialog/cmoduleresultview.cpp \
    ../../../src/frontend/searchdialog/crangechooserdialog.cpp \
    ../../../src/frontend/searchdialog/csearchdialog.cpp \
    ../../../src/frontend/searchdialog/csearchresultview.cpp \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisdialog.cpp \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisitem.cpp \
    ../../../src/frontend/searchdialog/analysis/csearchanalysislegenditem.cpp \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisscene.cpp \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisview.cpp \
    ../../../src/frontend/settingsdialogs/btfontchooserwidget.cpp \
    ../../../src/frontend/settingsdialogs/btfontsettings.cpp \
    ../../../src/frontend/settingsdialogs/btshortcutsdialog.cpp \
    ../../../src/frontend/settingsdialogs/btshortcutseditor.cpp \
    ../../../src/frontend/settingsdialogs/btstandardworkstab.cpp \
    ../../../src/frontend/settingsdialogs/bttextfilterstab.cpp \
    ../../../src/frontend/settingsdialogs/cacceleratorsettings.cpp \
    ../../../src/frontend/settingsdialogs/cconfigurationdialog.cpp \
    ../../../src/frontend/settingsdialogs/cdisplaysettings.cpp \
    ../../../src/frontend/settingsdialogs/clistwidget.cpp \
    ../../../src/frontend/settingsdialogs/cswordsettings.cpp \
    ../../../src/frontend/tips/bttipdialog.cpp \
    ../../../src/util/bticons.cpp \
    ../../../src/util/btmodules.cpp \


HEADERS += \
    ../../../src/bibletimeapp.h \
    ../../../src/bibletime.h \
    ../../../src/frontend/btaboutdialog.h \
    ../../../src/frontend/btaboutmoduledialog.h \
    ../../../src/frontend/btbookshelfdockwidget.h \
    ../../../src/frontend/btbookshelfgroupingmenu.h \
    ../../../src/frontend/btbookshelfview.h \
    ../../../src/frontend/btbookshelfwidget.h \
    ../../../src/frontend/btcentralwidget.h \
    ../../../src/frontend/btmenuview.h \
    ../../../src/frontend/btmodulechooserdialog.h \
    ../../../src/frontend/btmoduleindexdialog.h \
    ../../../src/frontend/btopenworkaction.h \
    ../../../src/frontend/cdragdrop.h \
    ../../../src/frontend/cexportmanager.h \
    ../../../src/frontend/cmdiarea.h \
    ../../../src/frontend/cprinter.h \
    ../../../src/frontend/bookmarks/bteditbookmarkdialog.h \
    ../../../src/frontend/bookmarks/cbookmarkindex.h \
    ../../../src/frontend/bookshelfmanager/btconfigdialog.h \
    ../../../src/frontend/bookshelfmanager/btmodulemanagerdialog.h \
    ../../../src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h \
    ../../../src/frontend/bookshelfmanager/indexpage/btindexpage.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpage.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpagemodel.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpageworkswidget.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallpathdialog.h \
    ../../../src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.h \
    ../../../src/frontend/bookshelfmanager/installpage/btrefreshprogressdialog.h \
    ../../../src/frontend/bookshelfmanager/removepage/btremovepage.h \
    ../../../src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.h \
    ../../../src/frontend/cinfodisplay.h \
    ../../../src/frontend/display/btcolorwidget.h \
    ../../../src/frontend/display/btfindwidget.h \
    ../../../src/frontend/display/btfontsizewidget.h \
    ../../../src/frontend/display/bthtmljsobject.h \
    ../../../src/frontend/display/bthtmlreaddisplay.h \
    ../../../src/frontend/display/cdisplay.h \
    ../../../src/frontend/display/chtmlwritedisplay.h \
    ../../../src/frontend/display/cplainwritedisplay.h \
    ../../../src/frontend/display/creaddisplay.h \
    ../../../src/frontend/displaywindow/btactioncollection.h \
    ../../../src/frontend/displaywindow/btdisplaysettingsbutton.h \
    ../../../src/frontend/displaywindow/btmodulechooserbar.h \
    ../../../src/frontend/displaywindow/btmodulechooserbutton.h \
    ../../../src/frontend/displaywindow/bttextwindowheader.h \
    ../../../src/frontend/displaywindow/bttextwindowheaderwidget.h \
    ../../../src/frontend/displaywindow/bttoolbarpopupaction.h \
    ../../../src/frontend/displaywindow/btwindowmodulechooser.h \
    ../../../src/frontend/displaywindow/cbiblereadwindow.h \
    ../../../src/frontend/displaywindow/cbookreadwindow.h \
    ../../../src/frontend/displaywindow/ccommentaryreadwindow.h \
    ../../../src/frontend/displaywindow/cdisplaywindow.h \
    ../../../src/frontend/displaywindow/chtmlwritewindow.h \
    ../../../src/frontend/displaywindow/clexiconreadwindow.h \
    ../../../src/frontend/displaywindow/cplainwritewindow.h \
    ../../../src/frontend/displaywindow/creadwindow.h \
    ../../../src/frontend/keychooser/bthistory.h \
    ../../../src/frontend/keychooser/cbookkeychooser.h \
    ../../../src/frontend/keychooser/cbooktreechooser.h \
    ../../../src/frontend/keychooser/ckeychooser.h \
    ../../../src/frontend/keychooser/ckeychooserwidget.h \
    ../../../src/frontend/keychooser/clexiconkeychooser.h \
    ../../../src/frontend/keychooser/cscrollbutton.h \
    ../../../src/frontend/keychooser/cscrollerwidgetset.h \
    ../../../src/frontend/keychooser/versekeychooser/btbiblekeywidget.h \
    ../../../src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.h \
    ../../../src/frontend/keychooser/versekeychooser/btversekeymenu.h \
    ../../../src/frontend/keychooser/versekeychooser/cbiblekeychooser.h \
    ../../../src/frontend/searchdialog/btsearchmodulechooserdialog.h \
    ../../../src/frontend/searchdialog/btsearchoptionsarea.h \
    ../../../src/frontend/searchdialog/btsearchresultarea.h \
    ../../../src/frontend/searchdialog/btsearchsyntaxhelpdialog.h \
    ../../../src/frontend/searchdialog/chistorycombobox.h \
    ../../../src/frontend/searchdialog/cmoduleresultview.h \
    ../../../src/frontend/searchdialog/crangechooserdialog.h \
    ../../../src/frontend/searchdialog/csearchdialog.h \
    ../../../src/frontend/searchdialog/csearchresultview.h \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisdialog.h \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisitem.h \
    ../../../src/frontend/searchdialog/analysis/csearchanalysislegenditem.h \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisscene.h \
    ../../../src/frontend/searchdialog/analysis/csearchanalysisview.h \
    ../../../src/frontend/settingsdialogs/btfontchooserwidget.h \
    ../../../src/frontend/settingsdialogs/btfontsettings.h \
    ../../../src/frontend/settingsdialogs/btshortcutsdialog.h \
    ../../../src/frontend/settingsdialogs/btshortcutseditor.h \
    ../../../src/frontend/settingsdialogs/btstandardworkstab.h \
    ../../../src/frontend/settingsdialogs/bttextfilterstab.h \
    ../../../src/frontend/settingsdialogs/cacceleratorsettings.h \
    ../../../src/frontend/settingsdialogs/cconfigurationdialog.h \
    ../../../src/frontend/settingsdialogs/cdisplaysettings.h \
    ../../../src/frontend/settingsdialogs/clistwidget.h \
    ../../../src/frontend/settingsdialogs/cswordsettings.h \
    ../../../src/frontend/tips/bttipdialog.h \
    ../../../src/util/btmodules.h \
    ../../../src/util/macros.h


RESOURCES += \


OTHER_FILES += \


# BibleTime Core
include(../../common/core/core.pro)

# show translations in project explorer
OTHER_FILES += $${TRANSLATIONS}
