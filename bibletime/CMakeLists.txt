PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(BT_WC_REVISION "0")
IF (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    # Probably a SVN workspace, determine revision level
    FIND_PACKAGE(Subversion)
    IF(Subversion_FOUND)
        Subversion_WC_INFO(${PROJECT_SOURCE_DIR} BT)
        MESSAGE(STATUS "Current revision is ${BT_WC_REVISION}")
    ENDIF(Subversion_FOUND)
ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/.svn)

#Version
SET (BT_VERSION_MAJOR "2")
SET (BT_VERSION_MINOR "4")
#SET (BT_REVISION_PATCH "+svnr${BT_WC_REVISION}")
SET (BT_REVISION_PATCH ".rc1")
ADD_DEFINITIONS(-DBT_SVN_REVISION="${SVN_REVISION}")
ADD_DEFINITIONS(-DBT_VERSION="${BT_VERSION_MAJOR}.${BT_VERSION_MINOR}${BT_REVISION_PATCH}")

MESSAGE(STATUS "Setting up build environment for BibleTime version ${BT_VERSION_MAJOR}.${BT_VERSION_MINOR}${BT_REVISION_PATCH}")

#Non-English locales to be processed and installed for UI, handbook and howto
LIST(APPEND MESSAGE_LOCALE_LANGS af bg cs da de en_GB es et fi fr hu it ko nl nn_NO no pl pt_br ro ru sk sv uk vi zh_CN zh_TW)
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS,
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed.
LIST(APPEND HANDBOOK_LOCALE_LANGS cs de fi fr hu nl pt-br)
LIST(APPEND HOWTO_LOCALE_LANGS bg cs da de es fi fr it hu ko nl pt-br ru)

######################################################
# Find required packages
#
SET(QT_MIN_VERSION 4.4.0)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTWEBKIT 1)
SET(QT_USE_QTMAIN 1)
FIND_PACKAGE(Qt4 REQUIRED)
IF (QT_QTDBUS_FOUND AND NOT APPLE)
	SET(QT_USE_QTDBUS 1)
	MESSAGE(STATUS "Using DBUS")
ELSE (QT_QTDBUS_FOUND AND NOT APPLE)
	ADD_DEFINITIONS(-DNO_DBUS)
	MESSAGE(STATUS "No DBUS found")
ENDIF(QT_QTDBUS_FOUND AND NOT APPLE)
#
# If cmake says that qdbusxml2cpp is missing, uncomment the following line
#
#SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?

FIND_PACKAGE(Boost REQUIRED)
#
#custom includes
#
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
FIND_PACKAGE(CLucene REQUIRED)

# optional: Try to detect CURL, ICU and ZLib and add it to the linker flags if found, maybe Sword needs it
FIND_PACKAGE(CURL)
FIND_PACKAGE(ICU)
FIND_PACKAGE(ZLIB)

FIND_PACKAGE(Sword REQUIRED)
######################################################


######################################################
# Define $bibletime_SOURCES
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTSourceFileList.cmake")
######################################################


######################################################
# The actual build options
#
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}		#for .h files generated from .ui
	${CMAKE_CURRENT_SOURCE_DIR}/src	# so that include paths like "frontend/..." work
	${CLUCENE_INCLUDE_DIR}			#CLucene headers
	${CLUCENE_LIBRARY_DIR}			#CLucene/clucene-config.h
	${Boost_INCLUDE_DIRS}
	${SWORD_INCLUDE_DIR}
)

# this will run uic on .ui files:
QT4_WRAP_UI(bibletime_UIS_H ${bibletime_UIS})

#This must come before the target definition!
LINK_DIRECTORIES(
	${CLUCENE_LIBRARY_DIR}
	${SWORD_LIBRARY_DIR}
)

SET(EXECUTABLE_NAME "bibletime")
IF(APPLE)
	SET(EXECUTABLE_NAME "BibleTime")
ENDIF(APPLE)

# For the Windows Application Icon
SET (bibletime_RC  cmake/BTWinIcon.rc)

#Link to QtTest only in debug builds
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
	# this will run moc on source files
	QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})
	IF (MSVC)
		ADD_EXECUTABLE(${EXECUTABLE_NAME} WIN32 ${bibletime_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES} ${bibletime_RC})
	ELSE (MSVC)
		ADD_EXECUTABLE(${EXECUTABLE_NAME}       ${bibletime_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES})
	ENDIF (MSVC)
#	SET_TARGET_PROPERTIES(${EXECUTABLE_NAME}
#		PROPERTIES
#			COMPILE_FLAGS "${SWORD_CFLAGS}" #Enable exception handling
#	)
	INCLUDE(${QT_USE_FILE})
	TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
		${QT_LIBRARIES}
		${CLUCENE_LIBRARY}
		${CURL_LIBRARIES} # optional, empty if not found
		${ICU_LIBRARIES} # optional, empty if not found
		${ICU_I18N_LIBRARIES} # optional, empty if not found
		${ZLIB_LIBRARIES} # optional, empty if not found
		${SWORD_LIBRARY}
	)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Release")
	# this will run moc on source files
	QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})
	QT4_WRAP_CPP(bibletime_test_MOC_SOURCES ${bibletime_test_MOCABLE_HEADERS})
	SET(CMAKE_BUILD_TYPE "Debug")
	IF (MSVC)
		ADD_EXECUTABLE(${EXECUTABLE_NAME} WIN32 ${bibletime_SOURCES}
			${bibletime_test_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES} ${bibletime_test_MOC_SOURCES} ${bibletime_RC})
	ELSE (MSVC)
		ADD_EXECUTABLE(${EXECUTABLE_NAME}       ${bibletime_SOURCES}
			${bibletime_test_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES} ${bibletime_test_MOC_SOURCES})
	ENDIF (MSVC)
	SET_TARGET_PROPERTIES(${EXECUTABLE_NAME}
		PROPERTIES
			COMPILE_FLAGS "-DBT_ENABLE_TESTING" #Enable exception handling and Testing
	)
	SET(QT_USE_QTTEST 1)
	INCLUDE(${QT_USE_FILE})
	TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
		${QT_LIBRARIES}
		${CLUCENE_LIBRARY}
		${CURL_LIBRARIES} # optional, empty if not found
		${ICU_LIBRARIES} # optional, empty if not found
		${ICU_I18N_LIBRARIES} # optional, empty if not found
		${SWORD_LIBRARY}
	)
	# Unit Tests
	ADD_CUSTOM_TARGET(test
		COMMAND ./${EXECUTABLE_NAME} --run-tests
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running BibleTime Unit Tests"
	)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

IF(USE_QT_WEB_KIT)
	SET_SOURCE_FILES_PROPERTIES( ${bibletime_SOURCES} COMPILE_FLAGS -DUSE_QTWEBKIT )
ENDIF(USE_QT_WEB_KIT)

SOURCE_GROUP( "Generated Files"
	FILES ${bibletime_UIS} ${bibletime_MOC_SOURCES}
)


######################################################

IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX "install/") #install to ./build/install, allow override
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

IF (MSVC)
	SET(CMAKE_CXX_FLAGS_RELEASE "-DNO_DBUS -DSWUSINGDLL -DBT_VERSION=\\\"${BT_VERSION}\\\" /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE ")
	SET(CMAKE_CXX_FLAGS_DEBUG   "-DNO_DBUS -DSWUSINGDLL -DBT_VERSION=\\\"${BT_VERSION}\\\" /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE -DBT_ENABLE_TESTING /ZI")
ELSE (MSVC)
	SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -fexceptions")
	SET(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Werror -O0 -g -fexceptions -DBT_ENABLE_TESTING")
ENDIF (MSVC)


######################################################
# Install files
#
SET (BT_DESTINATION "bin/")
SET (BT_SHARE_PATH "")

INSTALL(TARGETS "${EXECUTABLE_NAME}"
	DESTINATION "${BT_DESTINATION}"
)

# compiled catalogs
FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
	INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
		DESTINATION "${BT_SHARE_PATH}share/bibletime/locale/"
	)
ENDFOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
# icons
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
	DESTINATION "${BT_SHARE_PATH}share/bibletime/icons/"
)
# icons - png, needed for About Dialog
FILE(GLOB INSTALL_ICONS_LIST_PNG "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
	DESTINATION "${BT_SHARE_PATH}share/bibletime/icons/"
)
# display templates
FILE(GLOB INSTALL_DISPLAY_TEMPLATES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.tmpl)
INSTALL(FILES ${INSTALL_DISPLAY_TEMPLATES_LIST}
	DESTINATION "${BT_SHARE_PATH}share/bibletime/display-templates/"
)
# javascript
INSTALL(FILES "src/frontend/display/bthtml.js"
	DESTINATION "${BT_SHARE_PATH}share/bibletime/javascript/"
)
# license.html
INSTALL(FILES "docs/license.html"
	DESTINATION "${BT_SHARE_PATH}share/bibletime/license/"
)
# splash screen
INSTALL(FILES "pics/startuplogo.png"
	DESTINATION "${BT_SHARE_PATH}share/bibletime/pics/"
)
#handbook (install images from en/ to all languages)
FILE(GLOB INSTALL_HANDBOOK_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/*.png")
FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
	FILE(GLOB INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/*.html")
	INSTALL(FILES ${INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG}}
		DESTINATION "${BT_SHARE_PATH}share/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
	)
	INSTALL(FILES ${INSTALL_HANDBOOK_IMAGES}
		DESTINATION "${BT_SHARE_PATH}share/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
	)
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
#howto (does not have images)
FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
	FILE(GLOB INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/*.html")
	INSTALL(FILES ${INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG}}
		DESTINATION "${BT_SHARE_PATH}share/bibletime/docs/howto/${HOWTO_LOCALE_LANG}/"
	)
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
#
# Platform specific installation
#
# LINUX: application icon
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg"
	DESTINATION "${BT_SHARE_PATH}share/icons/"
)
# LINUX: desktop file
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake" "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop")
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop"
	DESTINATION "${BT_SHARE_PATH}share/applications/"
)
IF (MSVC)
	# sword locale information
	FILE(GLOB INSTALL_SWORD_LOCALE_LIST ${SWORD_INCLUDE_DIR}/../locales.d/*)
	INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
		DESTINATION "${BT_SHARE_PATH}share/sword/locales.d/"
	)
		
	# install pdb file for debugging purposes
	IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
		SET( bibletime_PDB ${bibletime_BINARY_DIR}/Debug/bibletime.pdb)
		INSTALL (FILES "${bibletime_PDB}" 
			DESTINATION "${BT_DESTINATION}"
		)
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")


ENDIF (MSVC)
######################################################

######################################################
# define helper targets for documentation management
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")
######################################################

######################################################

######################################################
# "make uninstall" target; see http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
#
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
 )
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
######################################################

######################################################
# Developer section
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDeveloper.cmake")
######################################################

######################################################
# Cpack section
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTCpack.cmake")
######################################################

