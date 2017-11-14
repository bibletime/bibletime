
SET(bibletime_SRC
    # Toplevel files:
    src/bibletime.cpp
    src/bibletime_init.cpp
    src/bibletime_slots.cpp
    src/bibletimeapp.cpp
    src/main.cpp
)

SOURCE_GROUP("src" FILES ${bibletime_SRC})

SET(bibletime_SRC_BACKEND
    # Backend top level:
    src/backend/btmoduletreeitem.cpp
    src/backend/cswordmodulesearch.cpp
    src/backend/btinstallbackend.cpp
    src/backend/btinstallmgr.cpp
    src/backend/btinstallthread.cpp
    src/backend/btsourcesthread.cpp
    src/backend/btbookmarksmodel.cpp
    src/backend/btglobal.cpp
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
    src/backend/rendering/btinforendering.cpp
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

SET(bibletime_SRC_BACKEND_MODELS
    src/backend/models/btmoduletextmodel.cpp
    src/backend/models/btlistmodel.cpp
)

SOURCE_GROUP("src\\backend\\models" FILES ${bibletime_SRC_BACKEND_MODELS})

SET(bibletime_SRC_UTIL
    # Utilities:
    src/util/cresmgr.cpp
    src/util/bticons.cpp
    src/util/btmodules.cpp
    src/util/directory.cpp
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
    src/frontend/btprinter.cpp
    src/frontend/btwebenginepage.cpp
    src/frontend/btwebengineview.cpp
    src/frontend/cexportmanager.cpp
    src/frontend/cinfodisplay.cpp
    src/frontend/cmdiarea.cpp
    src/frontend/crossrefrendering.cpp
    src/frontend/messagedialog.cpp
)

SOURCE_GROUP("src\\frontend" FILES ${bibletime_SRC_FRONTEND})

SET(bibletime_SRC_FRONTEND_BOOKSHELFWIZARD
    src/frontend/bookshelfwizard/btbookshelfwizard.cpp
    src/frontend/bookshelfwizard/btbookshelfwizard.h
    src/frontend/bookshelfwizard/btbookshelftaskpage.cpp
    src/frontend/bookshelfwizard/btbookshelftaskpage.h
    src/frontend/bookshelfwizard/btbookshelfinstallfinalpage.cpp
    src/frontend/bookshelfwizard/btbookshelfinstallfinalpage.h
    src/frontend/bookshelfwizard/btbookshelfremovefinalpage.cpp
    src/frontend/bookshelfwizard/btbookshelfremovefinalpage.h
    src/frontend/bookshelfwizard/btbookshelfworkspage.cpp
    src/frontend/bookshelfwizard/btbookshelfworkspage.h
    src/frontend/bookshelfwizard/btbookshelflanguagespage.cpp
    src/frontend/bookshelfwizard/btbookshelflanguagespage.h
    src/frontend/bookshelfwizard/btbookshelfsourcespage.cpp
    src/frontend/bookshelfwizard/btbookshelfsourcespage.h
    src/frontend/bookshelfwizard/btbookshelfsourcesprogresspage.cpp
    src/frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h
    src/frontend/bookshelfwizard/cswordsetupinstallsourcesdialog.cpp
    src/frontend/bookshelfwizard/btinstallpagemodel.cpp
)

SOURCE_GROUP("src\\frontend\\bookshelfwizard" FILES ${bibletime_SRC_FRONTEND_BOOKSHELFWIZARD})

SET(bibletime_SRC_FRONTEND_BOOKMARKS
    # Bookshelf/Bookmarks widget in main window:
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
    src/frontend/settingsdialogs/btconfigdialog.cpp
    src/frontend/settingsdialogs/btshortcutsdialog.cpp
    src/frontend/settingsdialogs/btshortcutseditor.cpp
    src/frontend/settingsdialogs/bttextfilterstab.cpp
    src/frontend/settingsdialogs/btstandardworkstab.cpp
    src/frontend/settingsdialogs/cacceleratorsettings.cpp
    src/frontend/settingsdialogs/cconfigurationdialog.cpp
    src/frontend/settingsdialogs/cdisplaysettings.cpp
    src/frontend/settingsdialogs/btfontchooserwidget.cpp
    src/frontend/settingsdialogs/btfontsettings.cpp
    src/frontend/settingsdialogs/clistwidget.cpp
    src/frontend/settingsdialogs/cswordsettings.cpp
)

SOURCE_GROUP("src\\frontend\\settingsdialog" FILES ${bibletime_SRC_FRONTEND_SETTINGSDIALOG})

SET(bibletime_SRC_FRONTEND_TIPDIALOG
    # Tip dialog:
    src/frontend/tips/bttipdialog.cpp
)

SOURCE_GROUP("src\\frontend\\tips" FILES ${bibletime_SRC_FRONTEND_TIPDIALOG})

SET(bibletime_SRC_FRONTEND_WELCOMEDIALOG
    src/frontend/welcome/btwelcomedialog.cpp
    src/frontend/welcome/btwelcomedialog.h
)

SOURCE_GROUP("src\\frontend\\welcome" FILES ${bibletime_SRC_FRONTEND_WELCOMEDIALOG})

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
    src/frontend/display/btfindwidget.cpp
    src/frontend/display/btfontsizewidget.cpp
    src/frontend/display/bthtmljsobject.cpp
    src/frontend/display/bthtmlreaddisplay.cpp
    src/frontend/display/cdisplay.cpp
    src/frontend/display/chtmlwritedisplay.cpp
    src/frontend/display/cplainwritedisplay.cpp
    src/frontend/display/creaddisplay.cpp
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
    src/frontend/displaywindow/chtmlwritewindow.cpp
    src/frontend/displaywindow/clexiconreadwindow.cpp
    src/frontend/displaywindow/cplainwritewindow.cpp
    src/frontend/displaywindow/creadwindow.cpp
)

SOURCE_GROUP("src\\frontend\\displaywindow" FILES ${bibletime_SRC_FRONTEND_DISPLAYWINDOW})

# Mocable headers:
SET(bibletime_COMMON_MOCABLE_HEADERS
    src/backend/btsignal.h
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
    src/backend/models/btmoduletextmodel.h
    src/backend/models/btlistmodel.h
    src/backend/btinstallmgr.h
    src/backend/btinstallthread.h
    src/backend/btsourcesthread.h
    src/backend/btbookmarksmodel.h
)

SET(bibletime_FRONTEND_DESKTOP_MOCABLE_HEADERS
    src/bibletime.h
    src/bibletimeapp.h
    src/frontend/bookmarks/bteditbookmarkdialog.h
    src/frontend/bookmarks/cbookmarkindex.h
    src/frontend/bookshelfwizard/cswordsetupinstallsourcesdialog.h
    src/frontend/bookshelfwizard/btinstallpagemodel.h
    src/frontend/bookshelfwizard/btbookshelfwizard.h
    src/frontend/bookshelfwizard/btbookshelfwizardpage.h
    src/frontend/bookshelfwizard/btbookshelftaskpage.h
    src/frontend/bookshelfwizard/btbookshelfinstallfinalpage.h
    src/frontend/bookshelfwizard/btbookshelfremovefinalpage.h
    src/frontend/bookshelfwizard/btbookshelfworkspage.h
    src/frontend/bookshelfwizard/btbookshelflanguagespage.h
    src/frontend/bookshelfwizard/btbookshelfsourcespage.h
    src/frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h
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
    src/frontend/btprinter.h
    src/frontend/btwebenginepage.h
    src/frontend/btwebengineview.h
    src/frontend/cdragdrop.h
    src/frontend/cinfodisplay.h
    src/frontend/cmdiarea.h
    src/frontend/display/btcolorwidget.h
    src/frontend/display/btfindwidget.h
    src/frontend/display/btfontsizewidget.h
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
    src/frontend/settingsdialogs/bttextfilterstab.h
    src/frontend/settingsdialogs/btstandardworkstab.h
    src/frontend/settingsdialogs/cacceleratorsettings.h
    src/frontend/settingsdialogs/cconfigurationdialog.h
    src/frontend/settingsdialogs/cdisplaysettings.h
    src/frontend/settingsdialogs/btfontchooserwidget.h
    src/frontend/settingsdialogs/btfontsettings.h
    src/frontend/settingsdialogs/clistwidget.h
    src/frontend/settingsdialogs/cswordsettings.h
    src/frontend/tips/bttipdialog.h
    src/frontend/welcome/btwelcomedialog.h
)

SET(bibletime_COMMON_SOURCES
    ${bibletime_SRC_BACKEND}
    ${bibletime_SRC_BACKEND_BOOKSHELFMODEL}
    ${bibletime_SRC_BACKEND_CONFIG}
    ${bibletime_SRC_BACKEND_DRIVERS}
    ${bibletime_SRC_BACKEND_FILTERS}
    ${bibletime_SRC_BACKEND_KEYS}
    ${bibletime_SRC_BACKEND_RENDERING}
    ${bibletime_SRC_BACKEND_MANAGERS}
    ${bibletime_SRC_BACKEND_MODELS}
    ${bibletime_SRC_UTIL})

SET(bibletime_FRONTEND_DESKTOP_SOURCES
    ${bibletime_SRC}
    ${bibletime_SRC_FRONTEND}
    ${bibletime_SRC_FRONTEND_BOOKMARKS}
    ${bibletime_SRC_FRONTEND_BOOKSHELFWIZARD}
    ${bibletime_SRC_FRONTEND_SEARCHDIALOG}
    ${bibletime_SRC_FRONTEND_SEARCHDIALOG_ANALYSIS}
    ${bibletime_SRC_FRONTEND_SETTINGSDIALOG}
    ${bibletime_SRC_FRONTEND_TIPDIALOG}
    ${bibletime_SRC_FRONTEND_WELCOMEDIALOG}
    ${bibletime_SRC_FRONTEND_KEYCHOOSER}
    ${bibletime_SRC_FRONTEND_KEYCHOOSER_VERSEKEYCHOOSER}
    ${bibletime_SRC_FRONTEND_DISPLAY}
    ${bibletime_SRC_FRONTEND_DISPLAYWINDOW}
)



SET(bibletime_SRC_MOBILE_SOURCES
    src/mobile/bibletime.cpp
    src/mobile/bibletime.h
    src/mobile/btmmain.cpp
    src/mobile/bibletimeapp.cpp
)

SET(bibletime_SRC_MOBILE_MODEL_SOURCES
    src/mobile/models/roleitemmodel.cpp
    src/mobile/models/roleitemmodel.h
    src/mobile/models/searchmodel.cpp
    src/mobile/models/searchmodel.h
)

SET(bibletime_SRC_MOBILE_SESSIONMANAGER_SOURCES
    src/mobile/sessionmanager/sessionmanager.cpp
    src/mobile/sessionmanager/sessionmanager.h
)

SET(bibletime_SRC_MOBILE_UTIL_SOURCES
    src/mobile/util/findqmlobject.cpp
    src/mobile/util/findqmlobject.h
    src/mobile/util/messagedialog.cpp
    src/mobile/util/messagedialog.h
)

SET(bibletime_SRC_MOBILE_UI_SOURCES
    src/mobile/bookshelfmanager/installmanager.cpp
    src/mobile/bookshelfmanager/installmanager.h
    src/mobile/bookshelfmanager/installprogress.cpp
    src/mobile/bookshelfmanager/installprogress.h
    src/mobile/bookshelfmanager/installsourcesmanager.cpp
    src/mobile/bookshelfmanager/installsourcesmanager.h
    src/mobile/bookshelfmanager/installsources.cpp
    src/mobile/bookshelfmanager/installsources.h
    src/mobile/keychooser/bookkeychooser.cpp
    src/mobile/keychooser/bookkeychooser.h
    src/mobile/keychooser/keynamechooser.cpp
    src/mobile/keychooser/keynamechooser.h
    src/mobile/keychooser/versechooser.cpp
    src/mobile/keychooser/versechooser.h
    src/mobile/ui/btstyle.cpp
    src/mobile/ui/btstyle.h
    src/mobile/ui/btsearchinterface.cpp
    src/mobile/ui/btsearchinterface.h
    src/mobile/ui/btwindowinterface.cpp
    src/mobile/ui/btwindowinterface.h
    src/mobile/ui/indexthread.cpp
    src/mobile/ui/indexthread.h
    src/mobile/ui/modulechooser.cpp
    src/mobile/ui/modulechooser.h
    src/mobile/ui/moduleinterface.cpp
    src/mobile/ui/moduleinterface.h
    src/mobile/ui/qtquick2applicationviewer.cpp
    src/mobile/ui/qtquick2applicationviewer.h
    src/mobile/ui/gridchooser.cpp
    src/mobile/ui/gridchooser.h
    src/mobile/ui/viewmanager.cpp
    src/mobile/ui/viewmanager.h
)

SET(bibletime_SRC_MOBILE_MOCABLE_HEADERS
    src/mobile/bibletime.h
    src/mobile/bibletimeapp.h
    src/mobile/bookshelfmanager/installmanager.h
    src/mobile/bookshelfmanager/installprogress.h
    src/mobile/bookshelfmanager/installsourcesmanager.h
    src/mobile/bookshelfmanager/installsources.h
    src/mobile/keychooser/bookkeychooser.h
    src/mobile/keychooser/keynamechooser.h
    src/mobile/keychooser/versechooser.h
    src/mobile/models/searchmodel.h
    src/mobile/sessionmanager/sessionmanager.h
    src/mobile/ui/btstyle.h
    src/mobile/ui/btsearchinterface.h
    src/mobile/ui/btwindowinterface.h
    src/mobile/ui/indexthread.h
    src/mobile/ui/modulechooser.h
    src/mobile/ui/moduleinterface.h
    src/mobile/ui/qtquick2applicationviewer.h
    src/mobile/ui/gridchooser.h
    src/mobile/ui/viewmanager.h
)

SET(bibletime_MOBILE_RESOURCE_FILES
    src/mobile/btm.qrc
)

SET(bibletime_MOBILE_QML_FILES
    src/mobile/qml/About.qml
    src/mobile/qml/BtButtonStyle.qml
    src/mobile/qml/ContextMenu.qml
    src/mobile/qml/GridChooserButton.qml
    src/mobile/qml/GridChooser.qml
    src/mobile/qml/InstallManagerChooser.qml
    src/mobile/qml/KeyNameChooser.qml
    src/mobile/qml/LeftArrow.qml
    src/mobile/qml/ListSelectView.qml
    src/mobile/qml/ListTextView.qml
    src/mobile/qml/ListWorksView.qml
    src/mobile/qml/main.qml
    src/mobile/qml/MainToolbar.qml
    src/mobile/qml/Menus.qml
    src/mobile/qml/MenuButton.qml
    src/mobile/qml/ModuleChooser.qml
    src/mobile/qml/PrevNextArrow.qml
    src/mobile/qml/Progress.qml
    src/mobile/qml/Question.qml
    src/mobile/qml/RightArrow.qml
    src/mobile/qml/Search.qml
    src/mobile/qml/SearchIcon.qml
    src/mobile/qml/SearchResults.qml
    src/mobile/qml/SimpleComboBox.qml
    src/mobile/qml/StartupBookshelfManager.qml
    src/mobile/qml/TitleColorBar.qml
    src/mobile/qml/FontSizeSlider.qml
    src/mobile/qml/TreeChooser.qml
    src/mobile/qml/Window.qml
    src/mobile/qml/WindowManager.qml
)

SET(bibletime_FRONTEND_MOBILE_SOURCES
    ${bibletime_SRC_MOBILE_SOURCES}
    ${bibletime_SRC_MOBILE_MOCABLE_HEADERS}
    ${bibletime_SRC_MOBILE_MODEL_SOURCES}
    ${bibletime_SRC_MOBILE_SESSIONMANAGER_SOURCES}
    ${bibletime_SRC_MOBILE_UI_SOURCES}
    ${bibletime_SRC_MOBILE_UTIL_SOURCES}
)

SET(bibletime_FRONTEND_MOBILE_MOCABLE_HEADERS
    ${bibletime_SRC_MOBILE_MOCABLE_HEADERS}
)

SET(bibletime_DESKTOP_TRANSLATION_FILE_PREFIX "bibletime_ui_")
SET(bibletime_MOBILE_TRANSLATION_FILE_PREFIX "mobile_ui_")

SET(bibletime_LINK_DESKTOP_TRANSLATION_FILES FALSE)
SET(bibletime_LINK_MOBILE_TRANSLATION_FILES TRUE)

SET(bibletime_DESKTOP_TRANSLATION_RESOURCE_FILE "")
SET(bibletime_MOBILE_TRANSLATION_RESOURCE_FILE "mobile_translate.qrc")

# Default to building a desktop frontend:
IF(NOT (DEFINED BIBLETIME_FRONTEND))
  SET(BIBLETIME_FRONTEND "DESKTOP")
ENDIF(NOT (DEFINED BIBLETIME_FRONTEND))

# Check for valid frontend:
SET(bibletime_AVAILABLE_FRONTENDS DESKTOP MOBILE)
LIST(FIND bibletime_AVAILABLE_FRONTENDS ${BIBLETIME_FRONTEND} bibletime_FRONTEND_INDEX)
IF(${bibletime_FRONTEND_INDEX} EQUAL -1)
  MESSAGE(STATUS "Invalid frontend specified: ${BIBLETIME_FRONTEND}.")
  MESSAGE(STATUS "Available frontends are:")
  FOREACH(frontend ${bibletime_AVAILABLE_FRONTENDS})
    MESSAGE(STATUS "    ${frontend}")
  ENDFOREACH(frontend ${bibletime_AVAILABLE_FRONTENDS})
  MESSAGE(FATAL_ERROR "Aborting processing because of invalid BIBLETIME_FRONTEND")
ENDIF(${bibletime_FRONTEND_INDEX} EQUAL -1)
MESSAGE(STATUS "Selected frontend: ${BIBLETIME_FRONTEND}")

SET(bibletime_SOURCES
    ${bibletime_FRONTEND_${BIBLETIME_FRONTEND}_SOURCES})
SET(bibletime_MOCABLE_HEADERS
    ${bibletime_FRONTEND_${BIBLETIME_FRONTEND}_MOCABLE_HEADERS})
SET(bibletime_RESOURCE_FILES
    ${bibletime_${BIBLETIME_FRONTEND}_RESOURCE_FILES})
SET(bibletime_QML_FILES
    ${bibletime_${BIBLETIME_FRONTEND}_QML_FILES})

SET(bibletime_TRANSLATION_FILE_PREFIX "${bibletime_${BIBLETIME_FRONTEND}_TRANSLATION_FILE_PREFIX}")
SET(bibletime_LINK_TRANSLATION_FILES "${bibletime_LINK_${BIBLETIME_FRONTEND}_TRANSLATION_FILES}")
SET(bibletime_TRANSLATION_RESOURCE_FILE "${bibletime_${BIBLETIME_FRONTEND}_TRANSLATION_RESOURCE_FILE}")

QT5_WRAP_CPP(bibletime_COMMON_MOCABLE_SOURCES ${bibletime_COMMON_MOCABLE_HEADERS} OPTIONS ${BIBLETIME_MOC_OPTIONS})
QT5_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS} OPTIONS ${BIBLETIME_MOC_OPTIONS})
QT5_ADD_RESOURCES(bibletime_RESOURCE_SOURCES ${bibletime_RESOURCE_FILES})

SET(common_bibletime_SOURCES
    ${bibletime_SOURCES}
    ${bibletime_RESOURCE_SOURCES}
    ${bibletime_MOC_SOURCES}
    ${bibletime_QML_FILES}
    ${bibletime_RC}
    ${bibletime_UI_translations}
)

# For the Windows Application Icon
IF(MSVC)
  LIST(APPEND common_bibletime_SOURCES "cmake/BTWinIcon.rc")
  SOURCE_GROUP("Icon Files" FILES "cmake/BTWinIcon.rc")
ENDIF()

SOURCE_GROUP("QM Files" REGULAR_EXPRESSION ".*\\.qm")
