PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

#Version
SET(BT_VERSION "1.7.1.cvs")

#Non-English locales to be processed and installed for UI, handbook and howto
LIST(APPEND MESSAGE_LOCALE_LANGS af bg cs da de en_GB es et fi fr hu it ko nl nn_NO no pl pt_br ro ru sk sv ua vi zh_CN zh_TW)
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS,
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed.
LIST(APPEND HANDBOOK_LOCALE_LANGS de fi fr nl)
LIST(APPEND HOWTO_LOCALE_LANGS bg cs de fi fr it ko nl pt-br ru)

######################################################
# Find required packages
#
SET(QT_MIN_VERSION 4.4.0)
SET(QT_USE_DBUS 1)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTWEBKIT 1)
FIND_PACKAGE(Qt4 REQUIRED)
#
# If cmake says that qdbusxml2cpp is missing, uncomment the following line
#
#SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?

#
# If you have problems with detecting KDE4, try uncommenting the following line
#
#SET(CMAKE_PREFIX_PATH /usr/lib/kde4)
FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
#
#custom includes
#
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
FIND_PACKAGE(CLucene REQUIRED)
FIND_PACKAGE(Sword REQUIRED)
######################################################


######################################################
# Define $bibletime_SOURCES
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/bibletime_source_list.cmake")
#
# Generate config.h
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
######################################################


######################################################
# The actual build options
#
INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	${QT_INCLUDE_DIR}/QtDBus
	${KDE4_INCLUDE_DIR}
	${CLUCENE_INCLUDE_DIR}			#CLucene headers
	${CLUCENE_LIBRARY_DIR}			#CLucene/clucene-config.h
	${Boost_INCLUDE_DIRS}
	${SWORD_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}		#for .h files generated from .ui
	${CMAKE_CURRENT_SOURCE_DIR}/src	# so that include paths like "frontend/..." work
)
#
#This must come before the target definition!
LINK_DIRECTORIES(
	${KDE4_LIB_DIR}
	${QT_LIBRARY_DIR}
	${CLUCENE_LIBRARY_DIR}
)

# this will run uic on .ui files:
QT4_WRAP_UI(bibletime_UIS_H ${bibletime_UIS})

#Link to QtTest only in debug builds
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
	KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES} ${bibletime_UIS_H})
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES
			COMPILE_FLAGS "-fexceptions ${SWORD_CFLAGS}" #Enable exception handling
	)
	INCLUDE(${QT_USE_FILE})
	TARGET_LINK_LIBRARIES(bibletime
		${KDE4_KDEUI_LIBS}
		${KDE4_KDECORE_LIBS}
		${QT_LIBRARIES}
		${CLUCENE_LIBRARY}
		${SWORD_LIBS}
	)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Release")
	SET(CMAKE_BUILD_TYPE "Debug")
	KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES} ${bibletime_test_SOURCES} ${bibletime_UIS_H})
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES
			COMPILE_FLAGS "-fexceptions -DBT_ENABLE_TESTING ${SWORD_CFLAGS}" #Enable exception handling and Testing
	)
	SET(QT_USE_QTTEST 1)
	INCLUDE(${QT_USE_FILE})
	TARGET_LINK_LIBRARIES(bibletime
		${KDE4_KDEUI_LIBS}
		${KDE4_KDECORE_LIBS}
		${QT_LIBRARIES}
		${QT_LIBRARIES_DEBUG}
		${CLUCENE_LIBRARY}
		${SWORD_LIBS}
	)
	# Unit Tests
	ADD_CUSTOM_TARGET(test
		COMMAND "./bibletime --run-tests"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running BibleTime Unit Tests"
	)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

IF(USE_QT_WEB_KIT)
	SET_SOURCE_FILES_PROPERTIES( ${bibletime_SOURCES} COMPILE_FLAGS -DUSE_QTWEBKIT )
ENDIF(USE_QT_WEB_KIT)

######################################################

IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX "install/") #install to ./build/install, allow override
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

SET(CMAKE_CXX_FLAGS_RELEASE "-W -O2")
IF (NOT APPLE)
	SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -pedantic -g") #-pedantic-errors
#	SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wno-return-type -Werror -O0 -g") #-pedantic-errors
ELSE (NOT APPLE)
	SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -pedantic -g")
ENDIF (NOT APPLE)


######################################################
# Install files
#
# main binary - Mac specifics handled by the KDE4_ADD_EXECUTABLE
IF (APPLE)
	SET (BT_DESTINATION ".")
	SET (BT_SHARE_PATH bibletime.app/Contents/)
ELSE (APPLE)
	SET (BT_DESTINATION "bin/")
	SET (BT_SHARE_PATH "")
ENDIF (APPLE)
INSTALL(TARGETS "bibletime"
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
# display templates
FILE(GLOB INSTALL_DISPLAY_TEMPLATES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.tmpl)
INSTALL(FILES ${INSTALL_DISPLAY_TEMPLATES_LIST}
	DESTINATION "${BT_SHARE_PATH}share/bibletime/display-templates/"
)
# javascript
INSTALL(FILES "src/frontend/display/bthtml.js"
	DESTINATION "${BT_SHARE_PATH}share/bibletime/javascript/"
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
######################################################


######################################################
# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
#
ADD_CUSTOM_TARGET("messages")
FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
	ADD_CUSTOM_TARGET("messages_${MESSAGE_LOCALE_LANG}"
		COMMAND lupdate "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui_${MESSAGE_LOCALE_LANG}.ts")
	ADD_CUSTOM_TARGET("compile_messages_${MESSAGE_LOCALE_LANG}"
		COMMAND lrelease "bibletime_ui_${MESSAGE_LOCALE_LANG}.ts" -qm "bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/")
	ADD_DEPENDENCIES("compile_messages_${MESSAGE_LOCALE_LANG}" "messages_${MESSAGE_LOCALE_LANG}")
	ADD_DEPENDENCIES("messages" "compile_messages_${MESSAGE_LOCALE_LANG}")
ENDFOREACH(MESSAGE_LOCALE_LANG)
# Template file for translators
ADD_CUSTOM_TARGET("messages_default"
	COMMAND lupdate "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui.ts")
ADD_DEPENDENCIES(messages "messages_default")
######################################################

######################################################
# Update handbook
ADD_CUSTOM_TARGET("handbook")

ADD_CUSTOM_TARGET("handbook_translations"
	COMMAND po4a -v --no-backups -k 0 cmake/docs/handbook_po4a.conf
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
	ADD_CUSTOM_TARGET("handbook_${HANDBOOK_LOCALE_LANG}"
		COMMAND xsltproc "${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl" "../docbook/index.docbook"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/")
	ADD_DEPENDENCIES("handbook_${HANDBOOK_LOCALE_LANG}" "handbook_translations")
	ADD_DEPENDENCIES("handbook" "handbook_${HANDBOOK_LOCALE_LANG}")
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS})
######################################################

######################################################
# Update howto
ADD_CUSTOM_TARGET("howto")
ADD_CUSTOM_TARGET("howto_translations"
	COMMAND po4a -v --no-backups -k 0 cmake/docs/howto_po4a.conf
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
ADD_DEPENDENCIES("howto" "howto_translations")

FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
	ADD_CUSTOM_TARGET("howto_${HOWTO_LOCALE_LANG}"
		COMMAND xsltproc "${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl" "../docbook/index.docbook"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/")
	ADD_DEPENDENCIES("howto_${HOWTO_LOCALE_LANG}" "howto_translations")
	ADD_DEPENDENCIES("howto" "howto_${HOWTO_LOCALE_LANG}")
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS})

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
