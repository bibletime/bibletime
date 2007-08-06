PROJECT(bibletime CXX C)

cmake_minimum_required(VERSION 2.4.0)
SET(DEBUG true)

######################################################
# Find required packages
#
find_package(Qt4 REQUIRED)  		#Qt4
#Workaround for KDE4 detection. Set ${CMAKE_INSTALL_PREFIX} to your KDE4 install dir.
set(OLD_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}) #backup
set(CMAKE_INSTALL_PREFIX /usr/lib/kde4) #kde4 dir
find_package(KDE4 REQUIRED) 		#KDE4
set(CMAKE_INSTALL_PREFIX ${OLD_CMAKE_INSTALL_PREFIX}) #restore

#custom includes
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(CLucene REQUIRED)	#CLucene
find_package(Sword REQUIRED)		#Sword
######################################################

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/install/bin)
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/install/lib)


#######################################################
# run create-code.pl to create a .cpp file containing
# the builtin templates
ADD_CUSTOM_TARGET(TEMPLATES ALL perl ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/create_code.pl ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates
                     VERBATIM)
#######################################################


SET(bibletime_SOURCES
#
#These files already compile cleanly
#
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
	
	src/backend/cswordmodulesearch.cpp

	#utilities
	src/util/cresmgr.cpp
	src/util/cpointers.cpp
	src/util/ctoolclass.cpp
	src/util/directoryutil.cpp
	
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

	#"Bookshelf" widget in main window
	src/frontend/mainindex/cmainindex.cpp
	src/frontend/mainindex/cindexitembase.cpp
	src/frontend/mainindex/cindexmoduleitem.cpp
	src/frontend/mainindex/cindexbookmarkitem.cpp
	src/frontend/mainindex/cindexfolderbase.cpp
	src/frontend/mainindex/cindextreefolder.cpp
	src/frontend/mainindex/cindexglossaryfolder.cpp
	src/frontend/mainindex/cindexbookmarkfolder.cpp
	src/frontend/mainindex/cindexsubfolder.cpp
	src/frontend/mainindex/cindexoldbookmarksfolder.cpp

	#Settings dialog (configuration)
	src/frontend/settingsdialogs/cacceleratorsettings.cpp
	src/frontend/settingsdialogs/cdisplaysettings.cpp
	src/frontend/settingsdialogs/cswordsettings.cpp
	src/frontend/settingsdialogs/clanguagesettings.cpp
	src/frontend/settingsdialogs/cconfigurationdialog.cpp

	#Bookshelf manager frontend
	#src/frontend/bookshelfmanager/cmanageindiceswidget.cpp
	#src/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
	#src/frontend/bookshelfmanager/kio_ftptransport.cpp
	#src/frontend/bookshelfmanager/btinstallmgr.cpp

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
	
	#toplevel files
	src/bibletimeapp.cpp
	src/main.cpp
	src/bibletime.cpp
	src/bibletime_init.cpp
	src/bibletime_slots.cpp
	src/bibletime_dcop.cpp

#Add files to be ported here
)

kde4_add_ui_files(bibletime_SOURCES
	src/frontend/searchdialog/searchresultsform.ui
	src/frontend/searchdialog/searchoptionsform.ui

	src/frontend/bookshelfmanager/manageindicesform.ui

)

#Add all source files to the binary
#ADD_LIBRARY(Backend ${libbackend_SOURCES} )

#
# Install locations
#
IF(DEBUG)
	SET(CMAKE_INSTALL_PREFIX install)
ELSE(DEBUG)
	SET(CMAKE_INSTALL_PREFIX /usr/local)
ENDIF(DEBUG)

INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QT3SUPPORT_INCLUDE_DIR}
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

#KDE4_AUTOMOC( ${bibletime_SOURCES} ) #not needed anymore after KDE4beta1, uncomment if you use older version

KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})

SET_TARGET_PROPERTIES(bibletime
	PROPERTIES 
		COMPILE_FLAGS "-w -fexceptions " #Enable exception handling
)

INSTALL(TARGETS bibletime
	DESTINATION bin
)

TARGET_LINK_LIBRARIES(bibletime
	${KDE4_KDEUI_LIBS}
	${KDE4_KDECORE_LIBS}
	${KDE4_KHTML_LIBS}
	QtCore
	QtGui
	QtXml
	${CLUCENE_LIBRARY}
	${SWORD_LIBRARY}
)
