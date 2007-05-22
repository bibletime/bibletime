PROJECT(bibletime CXX)

cmake_minimum_required(VERSION 2.4.0)

######################################################
# Find required packages
#
#System includes
find_package(Qt4 REQUIRED)  		#Qt4
find_package(KDE4 REQUIRED) 		#KDE4
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
	#frontend/keychooser
	bibletime/frontend/keychooser/ckeychooserwidget.cpp
	bibletime/frontend/keychooser/cscrollerwidgetset.cpp
	bibletime/frontend/keychooser/cscrollbutton.cpp
	bibletime/frontend/keychooser/ckeychooser.cpp
	bibletime/frontend/keychooser/cbiblekeychooser.cpp
	bibletime/frontend/keychooser/cbookkeychooser.cpp
	bibletime/frontend/keychooser/clexiconkeychooser.cpp
#Add files to be ported here
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
		COMPILE_FLAGS "-fexceptions" #Enable exception handling
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