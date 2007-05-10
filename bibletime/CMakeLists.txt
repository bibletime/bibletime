PROJECT(bibletime CXX C)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)  #find and setup Qt4 for this project
find_package(KDE4 REQUIRED) #KDE4

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

SET(DEBUG true)

SET(bibletime_SOURCES
	bibletime/util/directoryutil.cpp
	bibletime/backend/btstringmgr.cpp
	bibletime/backend/bt_plainhtml.cpp
	bibletime/backend/bt_thmlplain.cpp
	bibletime/backend/osismorphsegmentation.cpp
)


#SET(bibletime_MOC_HEADERS 
#)
#bibletime_MOC_SOURCES will contain a list of source files created by Qt's moc
#QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOC_HEADERS})

#Add all source files to the binary
#ADD_LIBRARY(Backend ${libbackend_SOURCES} )
ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})

#
# Install locations
#
IF(DEBUG)
	SET(CMAKE_INSTALL_PREFIX install)
ELSE(DEBUG)
	SET(CMAKE_INSTALL_PREFIX /usr/local)
ENDIF(DEBUG)

INSTALL(TARGETS bibletime
	DESTINATION bin
)

#
# Autoconfiguration section
#

IF (NOT QT4_FOUND)
	MESSAGE(FATAL_ERROR "Qt 4 was not found on your system.")
ENDIF (NOT QT4_FOUND)

INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${KDE4_INCLUDE_DIR}
	/usr/include/sword
)
TARGET_LINK_LIBRARIES(bibletime
#	Backend
	QtCore
	sword
)

