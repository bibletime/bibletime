SET(bibletime_UIS
    src/frontend/display/bthtmlfindtext.ui
)

SET(bibletime_SRC
    # Toplevel files:
    src/bibletime.cpp
    src/bibletime_dbus.cpp
    src/bibletime_dbus_adaptor.cpp
    src/bibletime_init.cpp
    src/bibletime_slots.cpp
    src/bibletimeapp.cpp
    src/btglobal.cpp
    src/main.cpp
)

SOURCE_GROUP("src" FILES ${bibletime_SRC})

SET(bibletime_SRC_BACKEND
    # Backend top level:
    src/backend/btmoduletreeitem.cpp
    src/backend/cswordmodulesearch.cpp
    src/backend/btinstallbackend.cpp
)

SOURCE_GROUP("src\\backend" FILES ${bibletime_SRC_BACKEND})

SET(bibletime_SRC_BACKEND_BOOKSHELFMODEL
    # Bookshelf model:
    src/backend/bookshelfmodel/btbookshelffiltermodel.cpp
    src/backend/bookshelfmodel/btbookshelfmodel.cpp
    src/backend/bookshelfmodel/btbookshelftreemodel.cpp
    src/backend/bookshelfmodel/categoryitem.cpp
    src/backend/bookshelfmodel/indexingitem.cpp
    src/backend/bookshelfmodel/item.cpp
    src/backend/bookshelfmodel/languageitem.cpp
    src/backend/bookshelfmodel/moduleitem.cpp
)

SOURCE_GROUP("src\\backend\\bookshelfmodel" FILES ${bibletime_SRC_BACKEND_BOOKSHELFMODEL})

SET(bibletime_SRC_BACKEND_CONFIG
    # Backend config:
    src/backend/config/btconfig.cpp
    src/backend/config/btconfigcore.cpp
)

SOURCE_GROUP("src\\backend\\config" FILES ${bibletime_SRC_BACKEND_CONFIG})

SET(bibletime_SRC_BACKEND_DRIVERS
    # Backend drivers:
    src/backend/drivers/cswordbiblemoduleinfo.cpp
    src/backend/drivers/cswordbookmoduleinfo.cpp
    src/backend/drivers/cswordcommentarymoduleinfo.cpp
    src/backend/drivers/cswordlexiconmoduleinfo.cpp
    src/backend/drivers/cswordmoduleinfo.cpp
)

SOURCE_GROUP("src\\backend\\drivers" FILES ${bibletime_SRC_BACKEND_DRIVERS})

SET(bibletime_SRC_BACKEND_FILTERS
    # Backend filters:
    src/backend/filters/gbftohtml.cpp
    src/backend/filters/osistohtml.cpp
    src/backend/filters/plaintohtml.cpp
    src/backend/filters/teitohtml.cpp
    src/backend/filters/thmltohtml.cpp
    src/backend/filters/thmltoplain.cpp
    src/backend/filters/osismorphsegmentation.cpp
)

SOURCE_GROUP("src\\backend\\filters" FILES ${bibletime_SRC_BACKEND_FILTERS})

SET(bibletime_SRC_BACKEND_KEYS
    # Backend keys:
    src/backend/keys/cswordkey.cpp
    src/backend/keys/cswordldkey.cpp
    src/backend/keys/cswordtreekey.cpp
    src/backend/keys/cswordversekey.cpp
)

SOURCE_GROUP("src\\backend\\keys" FILES ${bibletime_SRC_BACKEND_KEYS})

SET(bibletime_SRC_BACKEND_RENDERING
    # Backend rendering:
    src/backend/rendering/cbookdisplay.cpp
    src/backend/rendering/cchapterdisplay.cpp
    src/backend/rendering/cdisplayrendering.cpp
    src/backend/rendering/centrydisplay.cpp
    src/backend/rendering/chtmlexportrendering.cpp
    src/backend/rendering/cplaintextexportrendering.cpp
    src/backend/rendering/ctextrendering.cpp
)

SOURCE_GROUP("src\\backend\\rendering" FILES ${bibletime_SRC_BACKEND_RENDERING})

SET(bibletime_SRC_BACKEND_MANAGERS
    # Backend managers:
    src/backend/managers/btstringmgr.cpp
    src/backend/managers/cdisplaytemplatemgr.cpp
    src/backend/managers/clanguagemgr.cpp
    src/backend/managers/cswordbackend.cpp
    src/backend/managers/referencemanager.cpp
)

SOURCE_GROUP("src\\backend\\managers" FILES ${bibletime_SRC_BACKEND_MANAGERS})

SET(bibletime_SRC_UTIL
    # Utilities:
    src/util/cresmgr.cpp
    src/util/dialogutil.cpp
    src/util/directory.cpp
    src/util/btmodules.cpp
    src/util/tool.cpp
)

SOURCE_GROUP("src\\util" FILES ${bibletime_SRC_UTIL})

SET(bibletime_SRC_FRONTEND
    # Frontend top level:
    src/frontend/btaboutdialog.cpp
    src/frontend/btaboutmoduledialog.cpp
    src/frontend/btbookshelfdockwidget.cpp
    src/frontend/btbookshelfgroupingmenu.cpp
    src/frontend/btbookshelfview.cpp
    src/frontend/btbookshelfwidget.cpp
    src/frontend/btmenuview.cpp
    src/frontend/btmodulechooserdialog.cpp
    src/frontend/btmoduleindexdialog.cpp
    src/frontend/btopenworkaction.cpp
    src/frontend/cexportmanager.cpp
    src/frontend/cinfodisplay.cpp
    src/frontend/cmdiarea.cpp
    src/frontend/cprinter.cpp
    src/frontend/crossrefrendering.cpp
)

SOURCE_GROUP("src\\frontend" FILES ${bibletime_SRC_FRONTEND})

SET(bibletime_SRC_FRONTEND_BOOKSHELFMANAGER
    # Bookshelf manager frontend:
    src/frontend/bookshelfmanager/btconfigdialog.cpp
    src/frontend/bookshelfmanager/btinstallmgr.cpp
    src/frontend/bookshelfmanager/btmodulemanagerdialog.cpp
    src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
)

SOURCE_GROUP("src\\frontend\\bookshelfmanager" FILES ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER})

SET(bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INDEXPAGE
    # Bookshelf manager frontend index page:
    src/frontend/bookshelfmanager/indexpage/btindexpage.cpp
)

SOURCE_GROUP("src\\frontend\\bookshelfmanager\\indexpage" FILES ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INDEXPAGE})

SET(bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INSTALLPAGE
    # Bookshelf manager frontend install page:
    src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.cpp
    src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.cpp
    src/frontend/bookshelfmanager/installpage/btinstallpage.cpp
    src/frontend/bookshelfmanager/installpage/btinstallpageworkswidget.cpp
    src/frontend/bookshelfmanager/installpage/btinstallpathdialog.cpp
    src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.cpp
    src/frontend/bookshelfmanager/installpage/btinstallthread.cpp
    src/frontend/bookshelfmanager/installpage/btinstallpagemodel.cpp
    src/frontend/bookshelfmanager/installpage/btrefreshprogressdialog.cpp
)

SOURCE_GROUP("src\\frontend\\bookshelfmanager\\installpage" FILES ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INSTALLPAGE})

SET(bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_REMOVEPAGE
    # Bookshelf manager frontend remove page:
    src/frontend/bookshelfmanager/removepage/btremovepage.cpp
    src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.cpp
)

SOURCE_GROUP("src\\frontend\\bookshelfmanager\\removepage" FILES ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_REMOVEPAGE})

SET(bibletime_SRC_FRONTEND_BOOKMARKS
    # Bookshelf/Bookmarks widget in main window:
    src/frontend/bookmarks/btbookmarkfolder.cpp
    src/frontend/bookmarks/btbookmarkitem.cpp
    src/frontend/bookmarks/btbookmarkloader.cpp
    src/frontend/bookmarks/bteditbookmarkdialog.cpp
    src/frontend/bookmarks/cbookmarkindex.cpp
)

SOURCE_GROUP("src\\frontend\\bookmarks" FILES ${bibletime_SRC_FRONTEND_BOOKMARKS})

SET(bibletime_SRC_FRONTEND_SEARCHDIALOG
    # Search dialog:
    src/frontend/searchdialog/btsearchmodulechooserdialog.cpp
    src/frontend/searchdialog/btsearchoptionsarea.cpp
    src/frontend/searchdialog/btsearchresultarea.cpp
    src/frontend/searchdialog/btsearchsyntaxhelpdialog.cpp
    src/frontend/searchdialog/chistorycombobox.cpp
    src/frontend/searchdialog/cmoduleresultview.cpp
    src/frontend/searchdialog/crangechooserdialog.cpp
    src/frontend/searchdialog/csearchdialog.cpp
    src/frontend/searchdialog/csearchresultview.cpp
)

SOURCE_GROUP("src\\frontend\\searchdialog" FILES ${bibletime_SRC_FRONTEND_SEARCHDIALOG})

SET(bibletime_SRC_FRONTEND_SEARCHDIALOG_ANALYSIS
    # Search analysis:
    src/frontend/searchdialog/analysis/csearchanalysisdialog.cpp
    src/frontend/searchdialog/analysis/csearchanalysisitem.cpp
    src/frontend/searchdialog/analysis/csearchanalysislegenditem.cpp
    src/frontend/searchdialog/analysis/csearchanalysisscene.cpp
    src/frontend/searchdialog/analysis/csearchanalysisview.cpp
)

SOURCE_GROUP("src\\frontend\\searchdialog\\analysis" FILES ${bibletime_SRC_FRONTEND_SEARCHDIALOG_ANALYSIS})

SET(bibletime_SRC_FRONTEND_SETTINGSDIALOG
    # Settings dialog (configuration):
    src/frontend/settingsdialogs/btshortcutsdialog.cpp
    src/frontend/settingsdialogs/btshortcutseditor.cpp
    src/frontend/settingsdialogs/cacceleratorsettings.cpp
    src/frontend/settingsdialogs/cconfigurationdialog.cpp
    src/frontend/settingsdialogs/cdisplaysettings.cpp
    src/frontend/settingsdialogs/btfontchooserwidget.cpp
    src/frontend/settingsdialogs/btfontsettings.cpp
    src/frontend/settingsdialogs/btlanguagesettings.cpp
    src/frontend/settingsdialogs/clistwidget.cpp
    src/frontend/settingsdialogs/cswordsettings.cpp
)

SOURCE_GROUP("src\\frontend\\settingsdialog" FILES ${bibletime_SRC_FRONTEND_SETTINGSDIALOG})

SET(bibletime_SRC_FRONTEND_TIPDIALOG
    # Tip dialog:
    src/frontend/tips/bttipdialog.cpp
)

SOURCE_GROUP("src\\frontend\\tips" FILES ${bibletime_SRC_FRONTEND_TIPDIALOG})

SET(bibletime_SRC_FRONTEND_KEYCHOOSER
    # Frontend keychooser widgets:
    src/frontend/keychooser/bthistory.cpp
    src/frontend/keychooser/cbookkeychooser.cpp
    src/frontend/keychooser/cbooktreechooser.cpp
    src/frontend/keychooser/ckeychooser.cpp
    src/frontend/keychooser/ckeychooserwidget.cpp
    src/frontend/keychooser/clexiconkeychooser.cpp
    src/frontend/keychooser/cscrollbutton.cpp
    src/frontend/keychooser/cscrollerwidgetset.cpp
)

SOURCE_GROUP("src\\frontend\\keychooser" FILES ${bibletime_SRC_FRONTEND_KEYCHOOSER})

SET(bibletime_SRC_FRONTEND_KEYCHOOSER_VERSEKEYCHOOSER
    # Frontent verse key chooser widgets:
    src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.cpp
    src/frontend/keychooser/versekeychooser/btversekeymenu.cpp
    src/frontend/keychooser/versekeychooser/cbiblekeychooser.cpp
    src/frontend/keychooser/versekeychooser/btbiblekeywidget.cpp
)

SOURCE_GROUP("src\\frontend\\keychooser\\versekeychooser" FILES ${bibletime_SRC_FRONTEND_KEYCHOOSER_VERSEKEYCHOOSER})

SET(bibletime_SRC_FRONTEND_DISPLAY
    # Behaviour for display areas:
    src/frontend/display/btcolorwidget.cpp
    src/frontend/display/btfontsizewidget.cpp
    src/frontend/display/bthtmlfindtext.cpp
    src/frontend/display/bthtmljsobject.cpp
    src/frontend/display/bthtmlreaddisplay.cpp
    src/frontend/display/cdisplay.cpp
    src/frontend/display/chtmlwritedisplay.cpp
    src/frontend/display/cplainwritedisplay.cpp
    src/frontend/display/creaddisplay.cpp
    src/frontend/display/cwritedisplay.cpp
)

SOURCE_GROUP("src\\frontend\\display" FILES ${bibletime_SRC_FRONTEND_DISPLAY})

SET(bibletime_SRC_FRONTEND_DISPLAYWINDOW
    # Display windows and their widgets:
    src/frontend/displaywindow/btactioncollection.cpp
    src/frontend/displaywindow/btdisplaysettingsbutton.cpp
    src/frontend/displaywindow/btmodulechooserbar.cpp
    src/frontend/displaywindow/btmodulechooserbutton.cpp
    src/frontend/displaywindow/bttextwindowheader.cpp
    src/frontend/displaywindow/bttextwindowheaderwidget.cpp
    src/frontend/displaywindow/bttoolbarpopupaction.cpp
    src/frontend/displaywindow/cbiblereadwindow.cpp
    src/frontend/displaywindow/cbookreadwindow.cpp
    src/frontend/displaywindow/ccommentaryreadwindow.cpp
    src/frontend/displaywindow/cdisplaywindow.cpp
    src/frontend/displaywindow/cdisplaywindowfactory.cpp
    src/frontend/displaywindow/chtmlwritewindow.cpp
    src/frontend/displaywindow/clexiconreadwindow.cpp
    src/frontend/displaywindow/cplainwritewindow.cpp
    src/frontend/displaywindow/creadwindow.cpp
    src/frontend/displaywindow/cwritewindow.cpp
)

SOURCE_GROUP("src\\frontend\\displaywindow" FILES ${bibletime_SRC_FRONTEND_DISPLAYWINDOW})

# Mocable headers:
SET(bibletime_MOCABLE_HEADERS
    src/bibletime.h
    src/bibletime_dbus_adaptor.h
    src/backend/bookshelfmodel/btbookshelffiltermodel.h
    src/backend/bookshelfmodel/btbookshelfmodel.h
    src/backend/bookshelfmodel/btbookshelftreemodel.h
    src/backend/cswordmodulesearch.h
    src/backend/drivers/cswordbiblemoduleinfo.h
    src/backend/drivers/cswordbookmoduleinfo.h
    src/backend/drivers/cswordcommentarymoduleinfo.h
    src/backend/drivers/cswordlexiconmoduleinfo.h
    src/backend/drivers/cswordmoduleinfo.h
    src/backend/managers/cswordbackend.h
    src/frontend/bookmarks/bteditbookmarkdialog.h
    src/frontend/bookmarks/cbookmarkindex.h
    src/frontend/bookshelfmanager/btconfigdialog.h
    src/frontend/bookshelfmanager/btinstallmgr.h
    src/frontend/bookshelfmanager/btmodulemanagerdialog.h
    src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h
    src/frontend/bookshelfmanager/indexpage/btindexpage.h
    src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h
    src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h
    src/frontend/bookshelfmanager/installpage/btinstallpage.h
    src/frontend/bookshelfmanager/installpage/btinstallpageworkswidget.h
    src/frontend/bookshelfmanager/installpage/btinstallpathdialog.h
    src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.h
    src/frontend/bookshelfmanager/installpage/btinstallthread.h
    src/frontend/bookshelfmanager/installpage/btinstallpagemodel.h
    src/frontend/bookshelfmanager/installpage/btrefreshprogressdialog.h
    src/frontend/bookshelfmanager/removepage/btremovepage.h
    src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.h
    src/frontend/btaboutdialog.h
    src/frontend/btaboutmoduledialog.h
    src/frontend/btbookshelfdockwidget.h
    src/frontend/btbookshelfgroupingmenu.h
    src/frontend/btbookshelfview.h
    src/frontend/btbookshelfwidget.h
    src/frontend/btmenuview.h
    src/frontend/btmodulechooserdialog.h
    src/frontend/btmoduleindexdialog.h
    src/frontend/btopenworkaction.h
    src/frontend/cdragdrop.h
    src/frontend/cinfodisplay.h
    src/frontend/cmdiarea.h
    src/frontend/cprinter.h
    src/frontend/display/btcolorwidget.h
    src/frontend/display/btfontsizewidget.h
    src/frontend/display/bthtmlfindtext.h
    src/frontend/display/bthtmljsobject.h
    src/frontend/display/bthtmlreaddisplay.h
    src/frontend/display/cdisplay.h
    src/frontend/display/chtmlwritedisplay.h
    src/frontend/displaywindow/btactioncollection.h
    src/frontend/displaywindow/btdisplaysettingsbutton.h
    src/frontend/displaywindow/btmodulechooserbar.h
    src/frontend/displaywindow/btmodulechooserbutton.h
    src/frontend/displaywindow/bttextwindowheader.h
    src/frontend/displaywindow/bttextwindowheaderwidget.h
    src/frontend/displaywindow/bttoolbarpopupaction.h
    src/frontend/displaywindow/cbiblereadwindow.h
    src/frontend/displaywindow/cbookreadwindow.h
    src/frontend/displaywindow/ccommentaryreadwindow.h
    src/frontend/displaywindow/cdisplaywindow.h
    src/frontend/displaywindow/chtmlwritewindow.h
    src/frontend/displaywindow/clexiconreadwindow.h
    src/frontend/displaywindow/cplainwritewindow.h
    src/frontend/displaywindow/creadwindow.h
    src/frontend/displaywindow/cwritewindow.h
    src/frontend/keychooser/bthistory.h
    src/frontend/keychooser/cbookkeychooser.h
    src/frontend/keychooser/cbooktreechooser.h
    src/frontend/keychooser/ckeychooser.h
    src/frontend/keychooser/ckeychooserwidget.h
    src/frontend/keychooser/clexiconkeychooser.h
    src/frontend/keychooser/cscrollbutton.h
    src/frontend/keychooser/cscrollerwidgetset.h
    src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.h
    src/frontend/keychooser/versekeychooser/btversekeymenu.h
    src/frontend/keychooser/versekeychooser/cbiblekeychooser.h
    src/frontend/keychooser/versekeychooser/btbiblekeywidget.h
    src/frontend/searchdialog/analysis/csearchanalysisdialog.h
    src/frontend/searchdialog/analysis/csearchanalysisscene.h
    src/frontend/searchdialog/btsearchmodulechooserdialog.h
    src/frontend/searchdialog/btsearchoptionsarea.h
    src/frontend/searchdialog/btsearchresultarea.h
    src/frontend/searchdialog/btsearchsyntaxhelpdialog.h
    src/frontend/searchdialog/chistorycombobox.h
    src/frontend/searchdialog/cmoduleresultview.h
    src/frontend/searchdialog/crangechooserdialog.h
    src/frontend/searchdialog/csearchdialog.h
    src/frontend/searchdialog/csearchresultview.h
    src/frontend/settingsdialogs/btshortcutsdialog.h
    src/frontend/settingsdialogs/btshortcutseditor.h
    src/frontend/settingsdialogs/cacceleratorsettings.h
    src/frontend/settingsdialogs/cconfigurationdialog.h
    src/frontend/settingsdialogs/cdisplaysettings.h
    src/frontend/settingsdialogs/btfontchooserwidget.h
    src/frontend/settingsdialogs/btfontsettings.h
    src/frontend/settingsdialogs/btlanguagesettings.h
    src/frontend/settingsdialogs/clistwidget.h
    src/frontend/settingsdialogs/cswordsettings.h
    src/frontend/tips/bttipdialog.h
    src/util/btsignal.h
)

SET(bibletime_SOURCES
    ${bibletime_SRC}
    ${bibletime_SRC_BACKEND}
    ${bibletime_SRC_BACKEND_BOOKSHELFMODEL}
    ${bibletime_SRC_BACKEND_CONFIG}
    ${bibletime_SRC_BACKEND_DRIVERS}
    ${bibletime_SRC_BACKEND_FILTERS}
    ${bibletime_SRC_BACKEND_KEYS}
    ${bibletime_SRC_BACKEND_RENDERING}
    ${bibletime_SRC_BACKEND_MANAGERS}
    ${bibletime_SRC_UTIL}
    ${bibletime_SRC_FRONTEND}
    ${bibletime_SRC_FRONTEND_BOOKMARKS}
    ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER}
    ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INDEXPAGE}
    ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_INSTALLPAGE}
    ${bibletime_SRC_FRONTEND_BOOKSHELFMANAGER_REMOVEPAGE}
    ${bibletime_SRC_FRONTEND_SEARCHDIALOG}
    ${bibletime_SRC_FRONTEND_SEARCHDIALOG_ANALYSIS}
    ${bibletime_SRC_FRONTEND_SETTINGSDIALOG}
    ${bibletime_SRC_FRONTEND_TIPDIALOG}
    ${bibletime_SRC_FRONTEND_KEYCHOOSER}
    ${bibletime_SRC_FRONTEND_KEYCHOOSER_VERSEKEYCHOOSER}
    ${bibletime_SRC_FRONTEND_DISPLAY}
    ${bibletime_SRC_FRONTEND_DISPLAYWINDOW}
)
