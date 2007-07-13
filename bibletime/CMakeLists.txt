PROJECT(bibletime CXX C)

cmake_minimum_required(VERSION 2.4.0)

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

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

SET(DEBUG true)

SET(bibletime_SOURCES
#
#These files already compile cleanly
#
	#backend filters
	bibletime/backend/bt_gbfhtml.cpp
	bibletime/backend/bt_osishtml.cpp
	bibletime/backend/bt_plainhtml.cpp
	bibletime/backend/bt_thmlhtml.cpp
	bibletime/backend/bt_thmlplain.cpp
	bibletime/backend/osismorphsegmentation.cpp
	#backend rendering
	bibletime/backend/cbookdisplay.cpp
	bibletime/backend/cchapterdisplay.cpp
	bibletime/backend/cdisplayrendering.cpp
	bibletime/backend/centrydisplay.cpp
	bibletime/backend/chtmlexportrendering.cpp
	bibletime/backend/cplaintextexportrendering.cpp
	bibletime/backend/ctextrendering.cpp
	#backend managers
	bibletime/backend/btstringmgr.cpp
	bibletime/backend/cdisplaytemplatemgr.cpp
	bibletime/backend/clanguagemgr.cpp
	bibletime/backend/creferencemanager.cpp
	bibletime/backend/cswordbackend.cpp
	#backend module drivers
	bibletime/backend/cswordmoduleinfo.cpp
	bibletime/backend/cswordbiblemoduleinfo.cpp
	bibletime/backend/cswordbookmoduleinfo.cpp
	bibletime/backend/cswordcommentarymoduleinfo.cpp
	bibletime/backend/cswordlexiconmoduleinfo.cpp
	#backend keys
	bibletime/backend/cswordkey.cpp
	bibletime/backend/cswordldkey.cpp
	bibletime/backend/cswordtreekey.cpp
	bibletime/backend/cswordversekey.cpp
	
	#utilities
	bibletime/util/cresmgr.cpp
	bibletime/util/cpointers.cpp
	bibletime/util/ctoolclass.cpp
	bibletime/util/directoryutil.cpp
	
	#frontend
	bibletime/frontend/cdragdrop.cpp
	bibletime/frontend/kstartuplogo.cpp
	bibletime/frontend/crossrefrendering.cpp
	bibletime/frontend/cprinter.cpp
	bibletime/frontend/cbtconfig.cpp
	bibletime/frontend/cprofilemgr.cpp

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

#Add files to be ported here
)

kde4_add_ui_files(bibletime_SOURCES
	bibletime/frontend/searchdialog/searchresultsform.ui
	bibletime/frontend/searchdialog/searchoptionsform.ui
)

#SET(bibletime_MOC_HEADERS 
#)
#bibletime_MOC_SOURCES will contain a list of source files created by Qt's moc
#QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOC_HEADERS})

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
	${QT_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QT3SUPPORT_INCLUDE_DIR}
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

ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})

SET_TARGET_PROPERTIES(bibletime
	PROPERTIES 
		COMPILE_FLAGS "-w -fexceptions " #Enable exception handling and support for QT3 compatibility classes
)

INSTALL(TARGETS bibletime
	DESTINATION bin
)

TARGET_LINK_LIBRARIES(bibletime
	kdeui
	kdecore
	QtCore
	${CLUCENE_LIBRARY}
	${SWORD_LIBRARY}
)