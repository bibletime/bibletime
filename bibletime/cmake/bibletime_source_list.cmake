SET(bibletime_SOURCES
	#backend filters
	src/backend/filters/bt_gbfhtml.cpp
	src/backend/filters/bt_osishtml.cpp
	src/backend/filters/bt_plainhtml.cpp
	src/backend/filters/bt_thmlhtml.cpp
	src/backend/filters/bt_thmlplain.cpp
	src/backend/filters/osismorphsegmentation.cpp
	#backend rendering
	src/backend/rendering/cbookdisplay.cpp
	src/backend/rendering/cchapterdisplay.cpp
	src/backend/rendering/cdisplayrendering.cpp
	src/backend/rendering/centrydisplay.cpp
	src/backend/rendering/chtmlexportrendering.cpp
	src/backend/rendering/cplaintextexportrendering.cpp
	src/backend/rendering/ctextrendering.cpp
	#backend managers
	src/backend/managers/btstringmgr.cpp
	src/backend/managers/cdisplaytemplatemgr.cpp
	src/backend/managers/clanguagemgr.cpp
	src/backend/managers/creferencemanager.cpp
	src/backend/managers/cswordbackend.cpp
	#backend module drivers
	src/backend/drivers/cswordmoduleinfo.cpp
	src/backend/drivers/cswordbiblemoduleinfo.cpp
	src/backend/drivers/cswordbookmoduleinfo.cpp
	src/backend/drivers/cswordcommentarymoduleinfo.cpp
	src/backend/drivers/cswordlexiconmoduleinfo.cpp
	#backend keys
	src/backend/keys/cswordkey.cpp
	src/backend/keys/cswordldkey.cpp
	src/backend/keys/cswordtreekey.cpp
	src/backend/keys/cswordversekey.cpp
	#backend
	src/backend/cswordmodulesearch.cpp
	src/backend/btmoduletreeitem.cpp

	#utilities
	src/util/cresmgr.cpp
	src/util/cpointers.cpp
	src/util/ctoolclass.cpp
	src/util/directoryutil.cpp
	src/util/migrationutil.cpp
	
	# frontend top level
	src/frontend/cdragdrop.cpp
	src/frontend/kstartuplogo.cpp
	src/frontend/crossrefrendering.cpp
	src/frontend/cprinter.cpp
	src/frontend/cbtconfig.cpp
	src/frontend/cmoduleindexdialog.cpp
	src/frontend/cmdiarea.cpp
	src/frontend/cinfodisplay.cpp
	src/frontend/cinputdialog.cpp
	src/frontend/cexportmanager.cpp
	src/frontend/btaboutmoduledialog.cpp
	src/frontend/cmodulechooserdialog.cpp

	# Bookshelf/Bookmarks widget in main window
	src/frontend/mainindex/cmainindex.cpp
	
	src/frontend/mainindex/bookmarks/cbookmarkindex.cpp
	src/frontend/mainindex/bookmarks/cindexbookmarkitem.cpp
	src/frontend/mainindex/bookmarks/cindexfolderbase.cpp
	src/frontend/mainindex/bookmarks/cindextreefolder.cpp
	src/frontend/mainindex/bookmarks/cindexbookmarkfolder.cpp
	src/frontend/mainindex/bookmarks/cindexoldbookmarksfolder.cpp
	src/frontend/mainindex/bookmarks/cindexsubfolder.cpp
	src/frontend/mainindex/bookmarks/cindexitembase.cpp

	src/frontend/mainindex/bookshelf/cbookshelfindex.cpp
	src/frontend/mainindex/bookshelf/btindexitem.cpp
	src/frontend/mainindex/bookshelf/btindexmodule.cpp
	src/frontend/mainindex/bookshelf/btindexfolder.cpp
	src/frontend/mainindex/bookshelf/chidemodulechooserdialog.cpp

	#Settings dialog (configuration)
	src/frontend/settingsdialogs/cacceleratorsettings.cpp
	src/frontend/settingsdialogs/cdisplaysettings.cpp
	src/frontend/settingsdialogs/cswordsettings.cpp
	src/frontend/settingsdialogs/clanguagesettings.cpp
	src/frontend/settingsdialogs/cconfigurationdialog.cpp

	#Bookshelf manager frontend
	src/frontend/bookshelfmanager/cmanageindiceswidget.cpp
	src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
	src/frontend/bookshelfmanager/cswordsetupmodulelistview.cpp
	src/frontend/bookshelfmanager/cswordsetupdialog.cpp
	src/frontend/bookshelfmanager/btinstallmgr.cpp

	#Search dialog
	src/frontend/searchdialog/csearchdialog.cpp
	src/frontend/searchdialog/csearchresultview.cpp
	src/frontend/searchdialog/csearchdialogareas.cpp
	src/frontend/searchdialog/cmoduleresultview.cpp
	src/frontend/searchdialog/csearchmodulechooserdialog.cpp
	src/frontend/searchdialog/crangechooserdialog.cpp
	src/frontend/searchdialog/analysis/csearchanalysisitem.cpp
	src/frontend/searchdialog/analysis/csearchanalysislegenditem.cpp
	src/frontend/searchdialog/analysis/csearchanalysisscene.cpp
	src/frontend/searchdialog/analysis/csearchanalysisview.cpp
	src/frontend/searchdialog/analysis/csearchanalysisdialog.cpp

	#Profile manager frontend
	src/frontend/profile/cprofilemgr.cpp
	src/frontend/profile/cprofilewindow.cpp
	src/frontend/profile/cprofile.cpp

	#frontend keychooser widgets
	src/frontend/keychooser/ckeychooser.cpp
	src/frontend/keychooser/cbiblekeychooser.cpp
	src/frontend/keychooser/cbookkeychooser.cpp
	src/frontend/keychooser/cbooktreechooser.cpp
	src/frontend/keychooser/clexiconkeychooser.cpp
	src/frontend/keychooser/ckeychooserwidget.cpp
	src/frontend/keychooser/ckeyreferencewidget.cpp
	src/frontend/keychooser/cscrollerwidgetset.cpp
	src/frontend/keychooser/cscrollbutton.cpp

	src/frontend/keychooser/bthistory.cpp
	
	#behaviour for display areas
	src/frontend/display/cdisplay.cpp
	src/frontend/display/creaddisplay.cpp
	src/frontend/display/chtmlreaddisplay.cpp
	src/frontend/display/cwritedisplay.cpp
	src/frontend/display/cplainwritedisplay.cpp
	src/frontend/display/chtmlwritedisplay.cpp
	
	#display windows and their widgets
	src/frontend/displaywindow/cmodulechooserbutton.cpp
	src/frontend/displaywindow/cmodulechooserbar.cpp
	src/frontend/displaywindow/cbuttons.cpp
	src/frontend/displaywindow/cdisplaywindow.cpp
	src/frontend/displaywindow/creadwindow.cpp
	src/frontend/displaywindow/cwritewindow.cpp
	src/frontend/displaywindow/cplainwritewindow.cpp
	src/frontend/displaywindow/chtmlwritewindow.cpp
	src/frontend/displaywindow/clexiconreadwindow.cpp
	src/frontend/displaywindow/cbiblereadwindow.cpp
	src/frontend/displaywindow/ccommentaryreadwindow.cpp
	src/frontend/displaywindow/cbookreadwindow.cpp
	src/frontend/displaywindow/cdisplaywindowfactory.cpp
	
	#toplevel files
	src/bibletime_init.cpp
	src/bibletime_slots.cpp
	src/bibletime_dcop.cpp
	src/bibletime.cpp
	src/bibletimeapp.cpp
	src/main.cpp
)

KDE4_ADD_UI_FILES(bibletime_SOURCES
	src/frontend/bookshelfmanager/manageindicesform.ui
)