

SET(bibletime_UIS
     src/frontend/display/bthtmlfindtext.ui
)

SET(bibletime_SOURCES
	#backend bookshelf model:
	src/backend/bookshelfmodel/btbookshelfmodel.cpp
	src/backend/bookshelfmodel/btbookshelftreemodel.cpp
#	src/backend/bookshelfmodel/btcheckstatefilterproxymodel.cpp
#	src/backend/bookshelfmodel/btmodulecategoryfilterproxymodel.cpp
	src/backend/bookshelfmodel/btmodulehiddenfilterproxymodel.cpp
	src/backend/bookshelfmodel/btmodulenamefilterproxymodel.cpp
	src/backend/bookshelfmodel/categoryitem.cpp
	src/backend/bookshelfmodel/distributionitem.cpp
	src/backend/bookshelfmodel/item.cpp
	src/backend/bookshelfmodel/languageitem.cpp
	src/backend/bookshelfmodel/moduleitem.cpp
	#backend filters
	src/backend/filters/bt_gbfhtml.cpp
	src/backend/filters/bt_osishtml.cpp
	src/backend/filters/bt_teihtml.cpp
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
	src/backend/managers/referencemanager.cpp
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
	#backend config
	src/backend/config/cbtconfig.cpp
	#backend
	src/backend/cswordmodulesearch.cpp
	src/backend/btmoduletreeitem.cpp

	#utilities
	src/util/cresmgr.cpp
	src/util/cpointers.cpp
	src/util/tool.cpp
	src/util/dialogutil.cpp
	src/util/directory.cpp
	src/util/migrationutil.cpp

	# frontend top level
	src/frontend/btbookshelfdockwidget.cpp
	src/frontend/cdragdrop.cpp
	src/frontend/crossrefrendering.cpp
	src/frontend/cprinter.cpp
	src/frontend/cmoduleindexdialog.cpp
	src/frontend/cmdiarea.cpp
	src/frontend/cinfodisplay.cpp
	src/frontend/cinputdialog.cpp
	src/frontend/cexportmanager.cpp
	src/frontend/btaboutmoduledialog.cpp
	src/frontend/cmodulechooserdialog.cpp

	# Bookshelf/Bookmarks widget in main window
	src/frontend/mainindex/btbookshelfview.cpp

	src/frontend/mainindex/bookmarks/cbookmarkindex.cpp
	src/frontend/mainindex/bookmarks/btbookmarkloader.cpp
	src/frontend/mainindex/bookmarks/btbookmarkitembase.cpp
	src/frontend/mainindex/bookmarks/btbookmarkitem.cpp
	src/frontend/mainindex/bookmarks/btbookmarkfolder.cpp

	#Settings dialog (configuration)
	src/frontend/settingsdialogs/btshortcutsdialog.cpp
	src/frontend/settingsdialogs/btshortcutseditor.cpp
	src/frontend/settingsdialogs/cacceleratorsettings.cpp
	src/frontend/settingsdialogs/cdisplaysettings.cpp
	src/frontend/settingsdialogs/cswordsettings.cpp
	src/frontend/settingsdialogs/clanguagesettings.cpp
	src/frontend/settingsdialogs/cconfigurationdialog.cpp
	src/frontend/settingsdialogs/cfontchooser.cpp
	src/frontend/settingsdialogs/clistwidget.cpp

	#Bookshelf manager frontend
	src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
	src/frontend/bookshelfmanager/btconfigdialog.cpp
	src/frontend/bookshelfmanager/btmodulemanagerdialog.cpp
	src/frontend/bookshelfmanager/instbackend.cpp
	src/frontend/bookshelfmanager/btinstallmgr.cpp

	src/frontend/bookshelfmanager/installpage/btinstallpage.cpp
	src/frontend/bookshelfmanager/installpage/btinstallthread.cpp
	src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.cpp
	src/frontend/bookshelfmanager/installpage/btsourcewidget.cpp
	src/frontend/bookshelfmanager/installpage/btsourcearea.cpp
	src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.cpp
	src/frontend/bookshelfmanager/installpage/btinstallpathdialog.cpp

	src/frontend/bookshelfmanager/removepage/btremovepage.cpp
	src/frontend/bookshelfmanager/indexpage/btindexpage.cpp

	#Search dialog
	src/frontend/searchdialog/csearchdialog.cpp
	src/frontend/searchdialog/csearchresultview.cpp
	src/frontend/searchdialog/btsearchresultarea.cpp
	src/frontend/searchdialog/btsearchoptionsarea.cpp
	src/frontend/searchdialog/chistorycombobox.cpp
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
	src/frontend/keychooser/cbookkeychooser.cpp
	src/frontend/keychooser/cbooktreechooser.cpp
	src/frontend/keychooser/clexiconkeychooser.cpp
	src/frontend/keychooser/ckeychooserwidget.cpp
	src/frontend/keychooser/cscrollerwidgetset.cpp
	src/frontend/keychooser/cscrollbutton.cpp

	src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.cpp
	src/frontend/keychooser/versekeychooser/cbiblekeychooser.cpp
	src/frontend/keychooser/versekeychooser/ckeyreferencewidget.cpp
	src/frontend/keychooser/versekeychooser/btversekeymenu.cpp

	src/frontend/keychooser/bthistory.cpp

	#behaviour for display areas
    src/frontend/display/bthtmlfindtext.cpp
	src/frontend/display/bthtmljsobject.cpp
	src/frontend/display/bthtmlreaddisplay.cpp
	src/frontend/display/cdisplay.cpp
	src/frontend/display/creaddisplay.cpp
	src/frontend/display/cwritedisplay.cpp
	src/frontend/display/cplainwritedisplay.cpp
	src/frontend/display/chtmlwritedisplay.cpp
	src/frontend/display/btfontsizewidget.cpp
	src/frontend/display/btcolorwidget.cpp

	#display windows and their widgets
	src/frontend/displaywindow/bttoolbarpopupaction.cpp
	src/frontend/displaywindow/btactioncollection.cpp
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

	# web based tab dialogs
	src/frontend/htmldialogs/btaboutdialog.cpp
	src/frontend/htmldialogs/bttabhtmldialog.cpp

	#toplevel files
	src/bibletime_init.cpp
	src/bibletime_slots.cpp
	src/bibletime_dbus.cpp
	src/bibletime.cpp
	src/bibletimeapp.cpp
	src/bibletime_dbus_adaptor.cpp
	src/main.cpp
)

# mocable headers
SET(bibletime_MOCABLE_HEADERS
	src/backend/bookshelfmodel/btbookshelfmodel.h
	src/backend/bookshelfmodel/btbookshelftreemodel.h
#	src/backend/bookshelfmodel/btcheckstatefilterproxymodel.h
#	src/backend/bookshelfmodel/btmodulecategoryfilterproxymodel.h
	src/backend/bookshelfmodel/btmodulehiddenfilterproxymodel.h
	src/backend/bookshelfmodel/btmodulenamefilterproxymodel.h
	src/backend/drivers/cswordmoduleinfo.h
	src/backend/managers/cswordbackend.h
	src/backend/cswordmodulesearch.h
	src/bibletime.h
	src/bibletime_dbus_adaptor.h
	src/frontend/btbookshelfdockwidget.h
	src/frontend/searchdialog/csearchmodulechooserdialog.h
	src/frontend/searchdialog/crangechooserdialog.h
	src/frontend/searchdialog/btsearchresultarea.h
	src/frontend/searchdialog/btsearchoptionsarea.h
	src/frontend/searchdialog/cmoduleresultview.h
	src/frontend/searchdialog/csearchdialog.h
	src/frontend/searchdialog/analysis/csearchanalysisdialog.h
	src/frontend/searchdialog/analysis/csearchanalysisscene.h
	src/frontend/searchdialog/csearchresultview.h
	src/frontend/searchdialog/chistorycombobox.h
	src/frontend/cdragdrop.h
	src/frontend/mainindex/btbookshelfview.h
	src/frontend/mainindex/bookmarks/cbookmarkindex.h
	src/frontend/cmdiarea.h
	src/frontend/cinfodisplay.h
	src/frontend/cinputdialog.h
	src/frontend/cmodulechooserdialog.h
	src/frontend/keychooser/cscrollerwidgetset.h
	src/frontend/keychooser/cbooktreechooser.h
	src/frontend/keychooser/versekeychooser/btversekeymenu.h
	src/frontend/keychooser/versekeychooser/cbiblekeychooser.h
	src/frontend/keychooser/versekeychooser/ckeyreferencewidget.h
	src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.h
	src/frontend/keychooser/cscrollbutton.h
	src/frontend/keychooser/bthistory.h
	src/frontend/keychooser/ckeychooserwidget.h
	src/frontend/keychooser/cbookkeychooser.h
	src/frontend/keychooser/ckeychooser.h
	src/frontend/keychooser/clexiconkeychooser.h
	src/frontend/cprinter.h
	src/frontend/btaboutmoduledialog.h
	src/frontend/htmldialogs/bttabhtmldialog.h
	src/frontend/htmldialogs/btaboutdialog.h
	src/frontend/display/cdisplay.h
	src/frontend/display/bthtmlreaddisplay.h
	src/frontend/display/btfontsizewidget.h
	src/frontend/display/btcolorwidget.h
	src/frontend/display/bthtmljsobject.h
	src/frontend/display/chtmlwritedisplay.h
	src/frontend/display/bthtmlfindtext.h
	src/frontend/cmoduleindexdialog.h
	src/frontend/settingsdialogs/cdisplaysettings.h
	src/frontend/settingsdialogs/cswordsettings.h
	src/frontend/settingsdialogs/clistwidget.h
	src/frontend/settingsdialogs/cconfigurationdialog.h
	src/frontend/settingsdialogs/btshortcutsdialog.h
	src/frontend/settingsdialogs/btshortcutseditor.h
	src/frontend/settingsdialogs/cacceleratorsettings.h
	src/frontend/settingsdialogs/cfontchooser.h
	src/frontend/settingsdialogs/clanguagesettings.h
	src/frontend/displaywindow/cbookreadwindow.h
	src/frontend/displaywindow/ccommentaryreadwindow.h
	src/frontend/displaywindow/cmodulechooserbar.h
	src/frontend/displaywindow/clexiconreadwindow.h
	src/frontend/displaywindow/chtmlwritewindow.h
	src/frontend/displaywindow/bttoolbarpopupaction.h
	src/frontend/displaywindow/btactioncollection.h
	src/frontend/displaywindow/creadwindow.h
	src/frontend/displaywindow/cmodulechooserbutton.h
	src/frontend/displaywindow/cbuttons.h
	src/frontend/displaywindow/cbiblereadwindow.h
	src/frontend/displaywindow/cplainwritewindow.h
	src/frontend/displaywindow/cdisplaywindow.h
	src/frontend/displaywindow/cwritewindow.h
	src/frontend/bookshelfmanager/btconfigdialog.h
	src/frontend/bookshelfmanager/removepage/btremovepage.h
	src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h
	src/frontend/bookshelfmanager/installpage/btsourcewidget.h
	src/frontend/bookshelfmanager/installpage/btinstallpage.h
	src/frontend/bookshelfmanager/installpage/btsourcearea.h
	src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallpathdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallthread.h
	src/frontend/bookshelfmanager/btmodulemanagerdialog.h
	src/frontend/bookshelfmanager/indexpage/btindexpage.h
	src/frontend/bookshelfmanager/btinstallmgr.h
)

#unit tests
SET(bibletime_test_SOURCES
	#main file
	src/tests/bibletime_test.cpp
	#frontend
	src/tests/backend/config/cbtconfig_test.cpp
)

SET(bibletime_test_MOCABLE_HEADERS
	src/tests/bibletime_test.h
)
