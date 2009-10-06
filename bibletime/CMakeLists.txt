PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

#Version
ADD_DEFINITIONS(-DBT_VERSION="2.4-svn")

#Non-English locales to be processed and installed for UI, handbook and howto
LIST(APPEND MESSAGE_LOCALE_LANGS af bg cs da de en_GB es et fi fr hu it ko nl nn_NO no pl pt_br ro ru sk sv ua vi zh_CN zh_TW)
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
FIND_PACKAGE(Qt4 REQUIRED)
IF (QT_QTDBUS_FOUND AND NOT APPLE)
	SET(QT_USE_QTDBUS 1)
	MESSAGE("Using DBUS")
ELSE (QT_QTDBUS_FOUND AND NOT APPLE)
	ADD_DEFINITIONS(-DNO_DBUS)
	MESSAGE("No DBUS found")
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

# optional: Try to detect CURL and add it to the linker flags if found, maybe Sword needs it
FIND_PACKAGE(CURL)
# optional: Try to detect ICU and add it to the linker flags if found, maybe Sword needs it
FIND_PACKAGE(ICU)
# optional: Try to detect ZLib and add it to the linker flags if found, maybe Sword needs it
FIND_PACKAGE(ZLIB)

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
	${CMAKE_CURRENT_BINARY_DIR}		#for .h files generated from .ui
	${CMAKE_CURRENT_SOURCE_DIR}/src	# so that include paths like "frontend/..." work
	${CLUCENE_INCLUDE_DIR}			#CLucene headers
	${CLUCENE_LIBRARY_DIR}			#CLucene/clucene-config.h
	${Boost_INCLUDE_DIRS}
	${SWORD_INCLUDE_DIR}
)


# this will run uic on .ui files:
QT4_WRAP_UI(bibletime_UIS_H ${bibletime_UIS})

#
#This must come before the target definition!
LINK_DIRECTORIES(
	${CLUCENE_LIBRARY_DIR}
	${SWORD_LIBRARY_DIR}
)

IF(APPLE)
	SET(EXECUTABLE_NAME "BibleTime")
ELSE(APPLE)
	SET(EXECUTABLE_NAME "bibletime")
ENDIF(APPLE)

#Link to QtTest only in debug builds
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
	# this will run moc on source files
	QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})
	ADD_EXECUTABLE(${EXECUTABLE_NAME} ${bibletime_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES})
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
	ADD_EXECUTABLE(${EXECUTABLE_NAME} ${bibletime_SOURCES}
                             ${bibletime_test_SOURCES} ${bibletime_UIS_H} ${bibletime_MOC_SOURCES} ${bibletime_test_MOC_SOURCES})
	SET_TARGET_PROPERTIES(${EXECUTABLE_NAME}
		PROPERTIES
			COMPILE_FLAGS "-DBT_ENABLE_TESTING" #Enable exception handling and Testing
	)
	SET(QT_USE_QTTEST 1)
	INCLUDE(${QT_USE_FILE})
	TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
		${QT_LIBRARIES}
		${QT_LIBRARIES_DEBUG}
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


######################################################

IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX "install/") #install to ./build/install, allow override
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

IF (MSVC)
	SET(CMAKE_CXX_FLAGS_RELEASE "-DNO_DBUS -DSWUSINGDLL -DBT_VERSION=\\\"${BT_VERSION}\\\" /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE ")
	SET(CMAKE_CXX_FLAGS_DEBUG   "-DNO_DBUS -DSWUSINGDLL -DBT_VERSION=\\\"${BT_VERSION}\\\" /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE -DBT_ENABLE_TESTING /ZI")
ELSE (MSVC)
	SET(CMAKE_CXX_FLAGS_RELEASE "-W -O2 -fexceptions")
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
		COMMAND xsltproc --stringparam l10n.gentext.default.language ${HANDBOOK_LOCALE_LANG} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl ../docbook/index.docbook
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


######################################################
# This is the CPack section
# Copied and modified from http://www.vtk.org/Wiki/CMake:Packaging_With_CPack

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BibleTime for Windows Beta")
SET(CPACK_PACKAGE_VENDOR "http://www.bibletime.info")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "BibleTime")

######################################################
# Windows segment
######################################################
IF(WIN32 AND NOT UNIX)
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.

  # We need the libraries, and they're not pulled in automatically
  INCLUDE(${QT_USE_FILE})
  SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
  INSTALL(FILES
			"${QT_LIBRARY_DIR}/QtWebKitd4.dll"
			"${QT_LIBRARY_DIR}/QtGuid4.dll"
			"${QT_LIBRARY_DIR}/QtXmld4.dll"
			"${QT_LIBRARY_DIR}/QtTestd4.dll"
			"${QT_LIBRARY_DIR}/QtNetworkd4.dll"
			"${QT_LIBRARY_DIR}/QtCored4.dll"
			"${QT_LIBRARY_DIR}/phonond4.dll"
			"${QT_LIBRARY_DIR}/QtSvgd4.dll"
			DESTINATION "${BT_DESTINATION}"
			CONFIGURATIONS "Debug"
  )
  INSTALL(FILES
			"${QT_LIBRARY_DIR}/QtWebKit4.dll"
			"${QT_LIBRARY_DIR}/QtGui4.dll"
			"${QT_LIBRARY_DIR}/QtXml4.dll"
			"${QT_LIBRARY_DIR}/QtTest4.dll"
			"${QT_LIBRARY_DIR}/QtNetwork4.dll"
			"${QT_LIBRARY_DIR}/QtCore4.dll"
			"${QT_LIBRARY_DIR}/phonon4.dll"
			"${QT_LIBRARY_DIR}/QtSvg4.dll"
			DESTINATION "${BT_DESTINATION}"
			CONFIGURATIONS "Release"
  )

  # Qt Plugins
  INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats" DESTINATION "${BT_DESTINATION}/plugins")

  # This adds in the required Windows system libraries and libsword.dll
  INCLUDE(InstallRequiredSystemLibraries)
  STRING(REPLACE ".lib" ".dll" SWORD_DLL "${SWORD_LIBRARY}")
  INSTALL(FILES ${SWORD_DLL} DESTINATION ${BT_DESTINATION}) # This will also take effect in the regular install

  # Some options for the CPack system.  These should be pretty self-evident
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\pics\\\\icons\\\\bibletime.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\bibletime.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} for Windows")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.bibletime.info")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.bibletime.info")
  SET(CPACK_NSIS_CONTACT "bt-devel@crosswire.org")
  SET(CPACK_NSIS_MODIFY_PATH OFF)
  SET(CPACK_GENERATOR "NSIS")
####################################
#   Apple Segment
####################################
ELSEIF(APPLE)
  INCLUDE(${QT_USE_FILE})
  SET(QT_FRAMEWORKS_USED "QtWebKit"
  			 "QtGui"
			 "QtXml"
			 "QtTest"
			 "QtNetwork"
			 "QtCore"
			 "QtSvg")
  FOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})
    INSTALL(DIRECTORY
            "${QT_LIBRARY_DIR}/${QT_FRAME}.framework"
            DESTINATION "${BT_DESTINATION}/../Library/Frameworks/")
  ENDFOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})
  # Other libraries I need
  INSTALL(FILES "${CLUCENE_LIBRARY}"
	        "${CLUCENE_LIBRARY_DIR}/libclucene.0.0.0.dylib"
		"${CLUCENE_LIBRARY_DIR}/libclucene.0.dylib"
		"/opt/local/lib/libcurl.4.dylib"
   	        "/opt/local/lib/libssl.0.9.8.dylib"
	       	"/opt/local/lib/libcrypto.0.9.8.dylib"
	       	"/opt/local/lib/libicui18n.40.dylib"
	       	"/opt/local/lib/libicui18n.40.0.dylib"
		"/opt/local/lib/libicuuc.40.dylib"
	       	"/opt/local/lib/libicuuc.40.0.dylib"
	       	"/opt/local/lib/libicuio.40.dylib"
	       	"/opt/local/lib/libicuio.40.0.dylib"
		"/opt/local/lib/libicudata.40.dylib"
	       	"/opt/local/lib/libicudata.40.0.dylib"
		"/opt/local/lib/libpng12.0.dylib"
          DESTINATION "${BT_DESTINATION}/../Library")

  # Qt Plugins
  INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats" DESTINATION "${BT_DESTINATION}/../../plugins")
  # A file to execute that might get rid of the above mess
  SET(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/bt_start.sh")
  SET(CPACK_BUNDLE_NAME "BibleTime")
  SET(CPACK_PACKAGE_FILE_NAME "BibleTime_Mac_Beta")
  SET(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/Info.plist")
  SET(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
  SET(CPACK_GENERATOR "Bundle")
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_PACKAGE_EXECUTABLES "bibletime" "BibleTime")

INCLUDE(CPack)
