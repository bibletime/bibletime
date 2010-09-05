PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# BibleTime version
SET(BT_VERSION_MAJOR "2")
SET(BT_VERSION_MINOR "7")
SET(BT_REVISION_PATCH "3")

# Determine SVN patch version, if needed:
IF (NOT (DEFINED BT_REVISION_PATCH))
  IF (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
      # Probably a SVN workspace, determine revision level
      FIND_PACKAGE(Subversion)
      IF (Subversion_FOUND)
          Subversion_WC_INFO(${PROJECT_SOURCE_DIR} BT)
          IF (DEFINED BT_WC_REVISION)
            SET(BT_SVN_REVISION ${BT_WC_REVISION})
          ENDIF (DEFINED BT_WC_REVISION)
      ENDIF (Subversion_FOUND)
  ELSE (EXISTS ${PROJECT_SOURCE_DIR}/.svn)
      # Check for git-svn workspace, determine revision level
      SET(Git_FIND_QUIETLY TRUE)
      FIND_PACKAGE(Git)
      IF (Git_FOUND)
          Git_WC_INFO(${PROJECT_SOURCE_DIR} BT)
          IF (DEFINED BT_WC_SVN_REVISION)
            SET(BT_SVN_REVISION ${BT_WC_SVN_REVISION})
          ENDIF (DEFINED BT_WC_SVN_REVISION)
      ENDIF (Git_FOUND)
  ENDIF (EXISTS ${PROJECT_SOURCE_DIR}/.svn)

  IF (DEFINED BT_SVN_REVISION)
    MESSAGE(STATUS "Found SVN revision ${BT_SVN_REVISION}")
    SET(BT_REVISION_PATCH "+svn${BT_SVN_REVISION}")
  ENDIF (DEFINED BT_SVN_REVISION)
ENDIF (NOT (DEFINED BT_REVISION_PATCH))

SET(BT_VERSION_FULL "${BT_VERSION_MAJOR}.${BT_VERSION_MINOR}.${BT_REVISION_PATCH}")
MESSAGE(STATUS "Setting up build environment for BibleTime version ${BT_VERSION_FULL}")

ADD_DEFINITIONS(-DBT_VERSION="${BT_VERSION_FULL}")
IF (DEFINED BT_SVN_REVISION)
    ADD_DEFINITIONS(-DBT_SVN_REVISION="${BT_SVN_REVISION}")
ENDIF (DEFINED BT_SVN_REVISION)

# Define debug flags
IF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS(-DBT_DEBUG)
ENDIF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")

#Non-English locales to be processed and installed for UI, handbook and howto
LIST(APPEND MESSAGE_LOCALE_LANGS cs da de en_GB es et fi fr hu it nl nn_NO pl pt_BR ro ru sk)
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS,
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed.
LIST(APPEND HANDBOOK_LOCALE_LANGS cs de fi fr hu it nl pt_BR)
LIST(APPEND HOWTO_LOCALE_LANGS bg cs da de es fi fr it hu ko nl pt_BR ru)

# Find required packages
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
ENDIF (QT_QTDBUS_FOUND AND NOT APPLE)
# If cmake says that qdbusxml2cpp is missing, uncomment the following line
#SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?

FIND_PACKAGE(Boost REQUIRED)
#
#custom includes
#
FIND_PACKAGE(CLucene REQUIRED)

# optional: Try to detect CURL, ICU and ZLib and add it to the linker flags if found, maybe Sword needs it
FIND_PACKAGE(CURL)
FIND_PACKAGE(ICU)
FIND_PACKAGE(ZLIB)

FIND_PACKAGE(Sword REQUIRED)


# Define $bibletime_SOURCES
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTSourceFileList.cmake")

# Define rules to generate ${bibletime_UI_translations} files
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")

#
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

# this will run moc on source files
QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})

SOURCE_GROUP("QM Files" REGULAR_EXPRESSION ".*\\.qm")

#This must come before the target definition!
LINK_DIRECTORIES(
    ${CLUCENE_LIBRARY_DIR}
    ${SWORD_LIBRARY_DIR}
)

SET(EXECUTABLE_NAME "bibletime")
IF (APPLE)
    SET(EXECUTABLE_NAME "BibleTime")
ENDIF (APPLE)

SET(common_bibletime_SOURCES
    ${bibletime_SOURCES}
    ${bibletime_UIS_H}
    ${bibletime_MOC_SOURCES}
    ${bibletime_RC}
    ${bibletime_UI_translations}
)

# include test files in debug mode
IF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	# this will run moc on test source files
	QT4_WRAP_CPP(bibletime_test_MOC_SOURCES ${bibletime_test_MOCABLE_HEADERS})

SOURCE_GROUP( "Generated Files"
    FILES ${bibletime_UIS} ${bibletime_MOC_SOURCES} ${bibletime_test_MOC_SOURCES}
)

	SET(common_bibletime_SOURCES
        ${common_bibletime_SOURCES}
        ${bibletime_test_SOURCES}
        ${bibletime_test_MOC_SOURCES}
    )
ENDIF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")

# For the Windows Application Icon
IF (MSVC)
	SET(common_bibletime_SOURCES
	    ${common_bibletime_SOURCES}
	    "cmake/BTWinIcon.rc"
	)

SOURCE_GROUP( "Icon Files"
    FILES "cmake/BTWinIcon.rc" }
)

ENDIF (MSVC)

#Link to QtTest only in debug builds
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
    IF (MSVC)
        ADD_EXECUTABLE(${EXECUTABLE_NAME} WIN32 ${common_bibletime_SOURCES})
    ELSE (MSVC)
        ADD_EXECUTABLE(${EXECUTABLE_NAME} ${common_bibletime_SOURCES})
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
    SET(CMAKE_BUILD_TYPE "Debug")
    IF (MSVC)
        ADD_EXECUTABLE(${EXECUTABLE_NAME} WIN32 ${common_bibletime_SOURCES})
    ELSE (MSVC)
        ADD_EXECUTABLE(${EXECUTABLE_NAME} ${common_bibletime_SOURCES})
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


# Install files
#
SET(BT_DESTINATION "bin/")
SET(BT_SHARE_PATH "")

INSTALL(TARGETS "${EXECUTABLE_NAME}"
    DESTINATION "${BT_DESTINATION}"
)

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
INSTALL(FILES "pics/startuplogo.png" "pics/startuplogo_christmas.png"
    DESTINATION "${BT_SHARE_PATH}share/bibletime/pics/"
)
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


INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDeveloper.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTCpack.cmake")


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

