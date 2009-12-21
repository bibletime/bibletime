SET(bibletime_UIS
	src/frontend/display/bthtmlfindtext.ui
)

SET(bibletime_SOURCES
	# Toplevel files:
	src/bibletime.cpp
	src/bibletime_dbus.cpp
	src/bibletime_dbus_adaptor.cpp
	src/bibletime_init.cpp
	src/bibletime_slots.cpp
	src/bibletimeapp.cpp
	src/main.cpp

	# Backend top level:
	src/backend/btmoduletreeitem.cpp
	src/backend/cswordmodulesearch.cpp

	# Bookshelf model:
	src/backend/bookshelfmodel/btbookshelffiltermodel.cpp
	src/backend/bookshelfmodel/btbookshelfmodel.cpp
	src/backend/bookshelfmodel/btbookshelftreemodel.cpp
	src/backend/bookshelfmodel/categoryitem.cpp
	src/backend/bookshelfmodel/indexingitem.cpp
	src/backend/bookshelfmodel/item.cpp
	src/backend/bookshelfmodel/languageitem.cpp
	src/backend/bookshelfmodel/moduleitem.cpp

	# Backend config:
	src/backend/config/cbtconfig.cpp

	# Backend drivers:
	src/backend/drivers/cswordbiblemoduleinfo.cpp
	src/backend/drivers/cswordbookmoduleinfo.cpp
	src/backend/drivers/cswordcommentarymoduleinfo.cpp
	src/backend/drivers/cswordlexiconmoduleinfo.cpp
	src/backend/drivers/cswordmoduleinfo.cpp

	# Backend filters:
	src/backend/filters/bt_gbfhtml.cpp
	src/backend/filters/bt_osishtml.cpp
	src/backend/filters/bt_plainhtml.cpp
	src/backend/filters/bt_teihtml.cpp
	src/backend/filters/bt_thmlhtml.cpp
	src/backend/filters/bt_thmlplain.cpp
	src/backend/filters/osismorphsegmentation.cpp

	# Backend keys:
	src/backend/keys/cswordkey.cpp
	src/backend/keys/cswordldkey.cpp
	src/backend/keys/cswordtreekey.cpp
	src/backend/keys/cswordversekey.cpp

	# Backend rendering:
	src/backend/rendering/cbookdisplay.cpp
	src/backend/rendering/cchapterdisplay.cpp
	src/backend/rendering/cdisplayrendering.cpp
	src/backend/rendering/centrydisplay.cpp
	src/backend/rendering/chtmlexportrendering.cpp
	src/backend/rendering/cplaintextexportrendering.cpp
	src/backend/rendering/ctextrendering.cpp

	# Backend managers:
	src/backend/managers/btstringmgr.cpp
	src/backend/managers/cdisplaytemplatemgr.cpp
	src/backend/managers/clanguagemgr.cpp
	src/backend/managers/cswordbackend.cpp
	src/backend/managers/referencemanager.cpp

	# Utilities:
	src/util/cpointers.cpp
	src/util/cresmgr.cpp
	src/util/dialogutil.cpp
	src/util/directory.cpp
	src/util/migrationutil.cpp
	src/util/tool.cpp

	# Frontend top level:
	src/frontend/btaboutdialog.cpp
	src/frontend/btaboutmoduledialog.cpp
	src/frontend/btbookshelfdockwidget.cpp
	src/frontend/btbookshelfview.cpp
	src/frontend/btmenuview.cpp
	src/frontend/btmodulechooserdialog.cpp
	src/frontend/cdragdrop.cpp
	src/frontend/cexportmanager.cpp
	src/frontend/cinfodisplay.cpp
	src/frontend/cinputdialog.cpp
	src/frontend/cmdiarea.cpp
	src/frontend/cmodulechooserdialog.cpp
	src/frontend/cmoduleindexdialog.cpp
	src/frontend/cprinter.cpp
	src/frontend/crossrefrendering.cpp

	# Bookshelf manager frontend:
	src/frontend/bookshelfmanager/btconfigdialog.cpp
	src/frontend/bookshelfmanager/btinstallmgr.cpp
	src/frontend/bookshelfmanager/btmodulemanagerdialog.cpp
	src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
	src/frontend/bookshelfmanager/instbackend.cpp

	# Bookshelf manager frontend index page:
	src/frontend/bookshelfmanager/indexpage/btindexpage.cpp

	# Bookshelf manager frontend install page:
	src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.cpp
	src/frontend/bookshelfmanager/installpage/btinstallpage.cpp
	src/frontend/bookshelfmanager/installpage/btinstallpathdialog.cpp
	src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.cpp
	src/frontend/bookshelfmanager/installpage/btinstallthread.cpp
	src/frontend/bookshelfmanager/installpage/btsourcearea.cpp
	src/frontend/bookshelfmanager/installpage/btsourcewidget.cpp

	# Bookshelf manager frontend remove page:
	src/frontend/bookshelfmanager/removepage/btremovepage.cpp
	src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.cpp

	# Bookshelf/Bookmarks widget in main window:
	src/frontend/mainindex/bookmarks/btbookmarkfolder.cpp
	src/frontend/mainindex/bookmarks/btbookmarkitem.cpp
	src/frontend/mainindex/bookmarks/btbookmarkitembase.cpp
	src/frontend/mainindex/bookmarks/btbookmarkloader.cpp
	src/frontend/mainindex/bookmarks/cbookmarkindex.cpp

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

	# Search analysis:
	src/frontend/searchdialog/analysis/csearchanalysisdialog.cpp
	src/frontend/searchdialog/analysis/csearchanalysisitem.cpp
	src/frontend/searchdialog/analysis/csearchanalysislegenditem.cpp
	src/frontend/searchdialog/analysis/csearchanalysisscene.cpp
	src/frontend/searchdialog/analysis/csearchanalysisview.cpp

	# Settings dialog (configuration):
	src/frontend/settingsdialogs/btshortcutsdialog.cpp
	src/frontend/settingsdialogs/btshortcutseditor.cpp
	src/frontend/settingsdialogs/cacceleratorsettings.cpp
	src/frontend/settingsdialogs/cconfigurationdialog.cpp
	src/frontend/settingsdialogs/cdisplaysettings.cpp
	src/frontend/settingsdialogs/cfontchooser.cpp
	src/frontend/settingsdialogs/clanguagesettings.cpp
	src/frontend/settingsdialogs/clistwidget.cpp
	src/frontend/settingsdialogs/cswordsettings.cpp

	# Profile manager frontend:
	src/frontend/profile/cprofile.cpp
	src/frontend/profile/cprofilemgr.cpp
	src/frontend/profile/cprofilewindow.cpp

	# Frontend keychooser widgets:
	src/frontend/keychooser/bthistory.cpp
	src/frontend/keychooser/cbookkeychooser.cpp
	src/frontend/keychooser/cbooktreechooser.cpp
	src/frontend/keychooser/ckeychooser.cpp
	src/frontend/keychooser/ckeychooserwidget.cpp
	src/frontend/keychooser/clexiconkeychooser.cpp
	src/frontend/keychooser/cscrollbutton.cpp
	src/frontend/keychooser/cscrollerwidgetset.cpp

	# Frontent verse key chooser widgets:
	src/frontend/keychooser/versekeychooser/btdropdownchooserbutton.cpp
	src/frontend/keychooser/versekeychooser/btversekeymenu.cpp
	src/frontend/keychooser/versekeychooser/cbiblekeychooser.cpp
	src/frontend/keychooser/versekeychooser/ckeyreferencewidget.cpp

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

# Mocable headers:
SET(bibletime_MOCABLE_HEADERS
	src/bibletime.h
	src/bibletime_dbus_adaptor.h
	src/backend/bookshelfmodel/btbookshelffiltermodel.h
	src/backend/bookshelfmodel/btbookshelfmodel.h
	src/backend/bookshelfmodel/btbookshelftreemodel.h
	src/backend/cswordmodulesearch.h
	src/backend/drivers/cswordmoduleinfo.h
	src/backend/managers/cswordbackend.h
	src/frontend/bookshelfmanager/btconfigdialog.h
	src/frontend/bookshelfmanager/btinstallmgr.h
	src/frontend/bookshelfmanager/btmodulemanagerdialog.h
	src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h
	src/frontend/bookshelfmanager/indexpage/btindexpage.h
	src/frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallpage.h
	src/frontend/bookshelfmanager/installpage/btinstallpathdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallprogressdialog.h
	src/frontend/bookshelfmanager/installpage/btinstallthread.h
	src/frontend/bookshelfmanager/installpage/btsourcearea.h
	src/frontend/bookshelfmanager/installpage/btsourcewidget.h
	src/frontend/bookshelfmanager/removepage/btremovepage.h
	src/frontend/bookshelfmanager/removepage/btremovepagetreemodel.h
	src/frontend/btaboutdialog.h
	src/frontend/btaboutmoduledialog.h
	src/frontend/btbookshelfdockwidget.h
	src/frontend/btbookshelfview.h
	src/frontend/btmenuview.h
	src/frontend/btmodulechooserdialog.h
	src/frontend/cdragdrop.h
	src/frontend/cinfodisplay.h
	src/frontend/cinputdialog.h
	src/frontend/cmdiarea.h
	src/frontend/cmodulechooserdialog.h
	src/frontend/cmoduleindexdialog.h
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
	src/frontend/keychooser/versekeychooser/ckeyreferencewidget.h
	src/frontend/mainindex/bookmarks/cbookmarkindex.h
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
	src/frontend/settingsdialogs/cfontchooser.h
	src/frontend/settingsdialogs/clanguagesettings.h
	src/frontend/settingsdialogs/clistwidget.h
	src/frontend/settingsdialogs/cswordsettings.h
)

# Unit tests:
SET(bibletime_test_SOURCES
	#main file
	src/tests/bibletime_test.cpp
	#frontend
	src/tests/backend/config/cbtconfig_test.cpp
)

SET(bibletime_test_MOCABLE_HEADERS
	src/tests/bibletime_test.h
)
