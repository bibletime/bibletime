PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

#Version
SET(BT_VERSION "1.7.alpha2")

#Non-English locales to be processed and installed for UI, handbook and howto
LIST(APPEND MESSAGE_LOCALE_LANGS af bg cs da de en_GB es et fi fr hu it ko nl nn_NO no pl pt_br ro ru sk sv ua zh_CN zh_TW)
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS, 
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed. 
LIST(APPEND HANDBOOK_LOCALE_LANGS de fi fr nl)
LIST(APPEND HOWTO_LOCALE_LANGS bg cs de fi fr it ko nl pt-br ru)

######################################################
# Find required packages
#
SET(QT_MIN_VERSION 4.4.0)
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?
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

#Link to QtTest only in debug builds
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
	KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES})
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES 
			COMPILE_FLAGS "-fexceptions" #Enable exception handling
	)
	TARGET_LINK_LIBRARIES(bibletime
		${KDE4_KDEUI_LIBS}
		${KDE4_KDECORE_LIBS}
		${KDE4_KHTML_LIBS}
		QtCore
		QtGui
		QtXml
		QtDBus
		${CLUCENE_LIBRARY}
		${SWORD_LIBRARY}
	)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Release")
	KDE4_ADD_EXECUTABLE(bibletime ${bibletime_SOURCES} ${bibletime_test_SOURCES})
	SET_TARGET_PROPERTIES(bibletime
		PROPERTIES 
			COMPILE_FLAGS "-fexceptions -DBT_ENABLE_TESTING" #Enable exception handling and Testing
	)
	TARGET_LINK_LIBRARIES(bibletime
		${KDE4_KDEUI_LIBS}
		${KDE4_KDECORE_LIBS}
		${KDE4_KHTML_LIBS}
		QtCore
		QtGui
		QtXml
		QtDBus
		QtTest
		${CLUCENE_LIBRARY}
		${SWORD_LIBRARY}
	)
	# Unit Tests
	ADD_CUSTOM_TARGET(test
		COMMAND "./bibletime --run-tests"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Running BibleTime Unit Tests"
	)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")


######################################################

IF (NOT CMAKE_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX "install/") #install to ./build/install, allow override
ENDIF (NOT CMAKE_INSTALL_PREFIX)

SET(CMAKE_CXX_FLAGS_RELEASE "-W -O2")
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -pedantic-errors -O0 -g")

SET(CMAKE_CXX_FLAGS_DEBUG_BINONLY ${CMAKE_CXX_FLAGS_DEBUG})
SET(CMAKE_C_FLAGS_DEBUG_BINONLY ${CMAKE_C_FLAGS_DEBUG})
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG_BINONLY ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG_BINONLY ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
MARK_AS_ADVANCED(
	CMAKE_CXX_FLAGS_DEBUG_BINONLY
	CMAKE_C_FLAGS_DEBUG_BINONLY
	CMAKE_EXE_LINKER_FLAGS_DEBUG_BINONLY
	CMAKE_SHARED_LINKER_FLAGS_DEBUG_BINONLY
)


######################################################
# Install files
#
# main binary
INSTALL(TARGETS "bibletime" 
	DESTINATION "bin/"
	CONFIGURATIONS Debug Release Debug_Binonly
)
# compiled catalogs
FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
	INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/${MESSAGE_LOCALE_LANG}.qm" 
		DESTINATION "share/bibletime/locale/"
		CONFIGURATIONS Debug Release
	)
ENDFOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
# icons
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST} 
	DESTINATION "share/bibletime/icons/"
	CONFIGURATIONS Debug Release
)
# display templates
FILE(GLOB INSTALL_DISPLAY_TEMPLATES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.tmpl)
INSTALL(FILES ${INSTALL_DISPLAY_TEMPLATES_LIST} 
	DESTINATION "share/bibletime/display-templates/"
	CONFIGURATIONS Debug Release
)
# splash screen
INSTALL(FILES "pics/startuplogo.png" 
	DESTINATION "share/bibletime/pics/"
	CONFIGURATIONS Debug Release
)
#handbook (install images from en/ to all languages)
FILE(GLOB INSTALL_HANDBOOK_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/*.png")
FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
	FILE(GLOB INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/*.html")
	INSTALL(FILES ${INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG}} 
		DESTINATION "share/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
		CONFIGURATIONS Debug Release
	)
	INSTALL(FILES ${INSTALL_HANDBOOK_IMAGES} 
		DESTINATION "share/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
		CONFIGURATIONS Debug Release
	)
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
#howto (does not have images)
FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
	FILE(GLOB INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/*.html")
	INSTALL(FILES ${INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG}} 
		DESTINATION "share/bibletime/docs/howto/${HOWTO_LOCALE_LANG}/"
		CONFIGURATIONS Debug Release
	)
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
#
# Platform specific installation
#
# LINUX: application icon
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg" 
	DESTINATION "share/icons/"
	CONFIGURATIONS Debug Release
)
# LINUX: desktop file
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake" "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop")
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop" 
	DESTINATION "share/applications/"
	CONFIGURATIONS Debug Release
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
