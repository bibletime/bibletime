PROJECT(bibletime CXX C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

SET(BT_VERSION "1.7.alpha")

#Specify which languages are to be installed for UI messages
SET(INSTALL_LOCALE_LANGS "de")

######################################################
# Find required packages
#
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK
FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(Gettext REQUIRED)

#custom includes
#
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(CLucene REQUIRED)
find_package(Sword REQUIRED)
######################################################

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
	#src/frontend/searchdialog/csearchanalysis.cpp
	src/frontend/searchdialog/cmodulechooserdialog.cpp
	src/frontend/searchdialog/crangechooserdialog.cpp

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

kde4_add_ui_files(bibletime_SOURCES
	src/frontend/bookshelfmanager/manageindicesform.ui
)

#
# Generate config.h
#
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.h "
//This file is autogenerated by CMake, do not edit.
\#ifndef CONFIG_H_
\#define CONFIG_H_
\#define BT_VERSION \"${BT_VERSION}\"
\#endif
")

#
# Install locations
#
IF(DEBUG)
	SET(CMAKE_INSTALL_PREFIX install)
ELSE(DEBUG)
	SET(CMAKE_INSTALL_PREFIX /usr/local)
ENDIF(DEBUG)

INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} 
	${KDE4_INCLUDE_DIR}
	${CLUCENE_INCLUDE_DIR} #CLucene headers
	${CLUCENE_LIBRARY_DIR} #CLucene/clucene-config.h
	${SWORD_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # for .h files generated from .ui
	${CMAKE_CURRENT_SOURCE_DIR}/src # so that include paths like "frontend/..." work
)

#This must come before the target definition!
LINK_DIRECTORIES(
	${KDE4_LIB_DIR}
	${QT_LIBRARY_DIR}
	${CLUCENE_LIBRARY_DIR}
)

KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})

IF(MODE STREQUAL "RELEASE")
	MESSAGE(STATUS "Building in release mode")
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES 
			COMPILE_FLAGS "-w -fexceptions -O2"
	)
ELSE(MODE STREQUAL "RELEASE")
	MESSAGE(STATUS "Building in debug mode")
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES 
			COMPILE_FLAGS "-w -fexceptions -O0 -g" #Enable exception handling, debug code and no optimization
	)
ENDIF(MODE STREQUAL "RELEASE")

TARGET_LINK_LIBRARIES(bibletime
	${KDE4_KDEUI_LIBS}
	${KDE4_KDECORE_LIBS}
	${KDE4_KHTML_LIBS}
	QtCore
	QtGui
	QtXml
	${CLUCENE_LIBRARY}
	${SWORD_LIBRARY}
#	curl #??? not sure why I need this, since update to Ubuntu Gutsy / Sword 1.5.10, who knows if there is a workaround?
)

###############################
# Install files
#
# locales
FOREACH(_INSTALL_LOCALE_LANG ${INSTALL_LOCALE_LANGS})
	GETTEXT_PROCESS_PO_FILES(${_INSTALL_LOCALE_LANG} ALL "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/${INSTALL_LOCALE_LANGS}.po")
	INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_LOCALE_LANGS}.gmo" 
		DESTINATION "share/bibletime/locale/${INSTALL_LOCALE_LANGS}/LC_MESSAGES"
		RENAME "bibletime.mo")
ENDFOREACH(_INSTALL_LOCALE_LANG)
# main binary
INSTALL(TARGETS "bibletime" DESTINATION "bin/")
# icons
FILE(GLOB BT_INSTALL_ICONS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg)
INSTALL(FILES ${BT_INSTALL_ICONS_LIST} DESTINATION "share/bibletime/icons/")
# display templates
FILE(GLOB BT_INSTALL_DISPLAY_TEMPLATES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.tmpl)
INSTALL(FILES ${BT_INSTALL_DISPLAY_TEMPLATES_LIST} DESTINATION "share/bibletime/display-templates/")
# splash screen
INSTALL(FILES "pics/startuplogo.png" DESTINATION "share/bibletime/pics/")
# xml menu and toolbar definition
INSTALL(FILES "src/xml/bibletimeui.rc" DESTINATION "share/bibletime/xml/")
# tip of day database
INSTALL(FILES "docs/tips.xml" DESTINATION "share/bibletime/docs/")
###############################


################################
# "make uninstall" target; see http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
#
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
 )
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
################################
