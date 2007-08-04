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
ADD_CUSTOM_TARGET(TEMPLATES ALL perl ${CMAKE_CURRENT_SOURCE_DIR}/bibletime/display-templates/create_code.pl ${CMAKE_CURRENT_SOURCE_DIR}/bibletime/display-templates/
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     VERBATIM)
#######################################################


SET(bibletime_SOURCES
#
#These files already compile cleanly
#
	#backend filters
	bibletime/backend/filters/bt_gbfhtml.cpp
	bibletime/backend/filters/bt_osishtml.cpp
	bibletime/backend/filters/bt_plainhtml.cpp
	bibletime/backend/filters/bt_thmlhtml.cpp
	bibletime/backend/filters/bt_thmlplain.cpp
	bibletime/backend/filters/osismorphsegmentation.cpp
	#backend rendering
	bibletime/backend/rendering/cbookdisplay.cpp
	bibletime/backend/rendering/cchapterdisplay.cpp
	bibletime/backend/rendering/cdisplayrendering.cpp
	bibletime/backend/rendering/centrydisplay.cpp
	bibletime/backend/rendering/chtmlexportrendering.cpp
	bibletime/backend/rendering/cplaintextexportrendering.cpp
	bibletime/backend/rendering/ctextrendering.cpp
	#backend managers
	bibletime/backend/managers/btstringmgr.cpp
	bibletime/backend/managers/cdisplaytemplatemgr.cpp
	bibletime/backend/managers/clanguagemgr.cpp
	bibletime/backend/managers/creferencemanager.cpp
	bibletime/backend/managers/cswordbackend.cpp
	#backend module drivers
	bibletime/backend/drivers/cswordmoduleinfo.cpp
	bibletime/backend/drivers/cswordbiblemoduleinfo.cpp
	bibletime/backend/drivers/cswordbookmoduleinfo.cpp
	bibletime/backend/drivers/cswordcommentarymoduleinfo.cpp
	bibletime/backend/drivers/cswordlexiconmoduleinfo.cpp
	#backend keys
	bibletime/backend/keys/cswordkey.cpp
	bibletime/backend/keys/cswordldkey.cpp
	bibletime/backend/keys/cswordtreekey.cpp
	bibletime/backend/keys/cswordversekey.cpp
	
	bibletime/backend/cswordmodulesearch.cpp

	#utilities
	bibletime/util/cresmgr.cpp
	bibletime/util/cpointers.cpp
	bibletime/util/ctoolclass.cpp
	bibletime/util/directoryutil.cpp
	
	# frontend top level
	bibletime/frontend/cdragdrop.cpp
	bibletime/frontend/kstartuplogo.cpp
	bibletime/frontend/crossrefrendering.cpp
	bibletime/frontend/cprinter.cpp
	bibletime/frontend/cbtconfig.cpp
	bibletime/frontend/cmoduleindexdialog.cpp
	bibletime/frontend/cmdiarea.cpp
	bibletime/frontend/cinfodisplay.cpp
	bibletime/frontend/cinputdialog.cpp
	bibletime/frontend/cexportmanager.cpp

	#"Bookshelf" widget in main window
	#bibletime/frontend/mainindex/cmainindex.cpp
	#bibletime/frontend/mainindex/cindexitembase.cpp
	
	#Settings dialog (configuration)
	bibletime/frontend/settingsdialogs/cacceleratorsettings.cpp
	bibletime/frontend/settingsdialogs/cdisplaysettings.cpp
	bibletime/frontend/settingsdialogs/cswordsettings.cpp
	bibletime/frontend/settingsdialogs/clanguagesettings.cpp
	bibletime/frontend/settingsdialogs/cconfigurationdialog.cpp

	#Bookshelf manager frontend
	#bibletime/frontend/bookshelfmanager/cmanageindiceswidget.cpp
	#bibletime/frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.cpp
	#bibletime/frontend/bookshelfmanager/kio_ftptransport.cpp
	#bibletime/frontend/bookshelfmanager/btinstallmgr.cpp

	#Profile manager frontend
	bibletime/frontend/profile/cprofilemgr.cpp
	bibletime/frontend/profile/cprofilewindow.cpp
	bibletime/frontend/profile/cprofile.cpp

	#frontend keychooser widgets
	bibletime/frontend/keychooser/ckeychooser.cpp
	bibletime/frontend/keychooser/cbiblekeychooser.cpp
	bibletime/frontend/keychooser/cbookkeychooser.cpp
	bibletime/frontend/keychooser/cbooktreechooser.cpp
	bibletime/frontend/keychooser/clexiconkeychooser.cpp
	bibletime/frontend/keychooser/ckeychooserwidget.cpp
	bibletime/frontend/keychooser/ckeyreferencewidget.cpp
	bibletime/frontend/keychooser/cscrollerwidgetset.cpp
	bibletime/frontend/keychooser/cscrollbutton.cpp
	
	#behaviour for display areas
	bibletime/frontend/display/cdisplay.cpp
	bibletime/frontend/display/creaddisplay.cpp
	bibletime/frontend/display/chtmlreaddisplay.cpp
	bibletime/frontend/display/cwritedisplay.cpp
	bibletime/frontend/display/cplainwritedisplay.cpp
	bibletime/frontend/display/chtmlwritedisplay.cpp
	
	#display windows and their widgets
	bibletime/frontend/displaywindow/cmodulechooserbutton.cpp
	bibletime/frontend/displaywindow/cmodulechooserbar.cpp
	bibletime/frontend/displaywindow/cbuttons.cpp
	bibletime/frontend/displaywindow/cdisplaywindow.cpp
	bibletime/frontend/displaywindow/creadwindow.cpp
	bibletime/frontend/displaywindow/cwritewindow.cpp
	bibletime/frontend/displaywindow/cplainwritewindow.cpp
	bibletime/frontend/displaywindow/chtmlwritewindow.cpp
	bibletime/frontend/displaywindow/clexiconreadwindow.cpp
	bibletime/frontend/displaywindow/cbiblereadwindow.cpp
	bibletime/frontend/displaywindow/ccommentaryreadwindow.cpp
	bibletime/frontend/displaywindow/cbookreadwindow.cpp
	
	#toplevel files
	bibletime/bibletimeapp.cpp
	bibletime/main.cpp
	bibletime/bibletime.cpp
	bibletime/bibletime_init.cpp
	bibletime/bibletime_slots.cpp
	bibletime/bibletime_dcop.cpp

#Add files to be ported here
)

kde4_add_ui_files(bibletime_SOURCES
	bibletime/frontend/searchdialog/searchresultsform.ui
	bibletime/frontend/searchdialog/searchoptionsform.ui

	bibletime/frontend/bookshelfmanager/manageindicesform.ui

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
	${CMAKE_CURRENT_SOURCE_DIR}/bibletime # so that include paths like "frontend/..." work
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