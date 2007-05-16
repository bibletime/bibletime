PROJECT(bibletime CXX)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)  #find and setup Qt4 for this project
find_package(KDE4 REQUIRED) #KDE4

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

SET(DEBUG true)

SET(bibletime_SOURCES
#These files already compile cleanly
	bibletime/util/directoryutil.cpp
	bibletime/backend/btstringmgr.cpp
	#backend filters
	bibletime/backend/bt_gbfhtml.cpp
	bibletime/backend/bt_osishtml.cpp
	bibletime/backend/bt_plainhtml.cpp
	bibletime/backend/bt_thmlhtml.cpp
	bibletime/backend/bt_thmlplain.cpp
	bibletime/backend/osismorphsegmentation.cpp
	bibletime/util/cresmgr.cpp
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
	/usr/include/sword
)

#This must come before the target definition!
LINK_DIRECTORIES(
	${KDE4_LIB_DIR}
	${QT_LIBRARY_DIR}
)

ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})

INSTALL(TARGETS bibletime
	DESTINATION bin
)

TARGET_LINK_LIBRARIES(bibletime
	kdeui
	kdecore
	QtCore
	sword
)