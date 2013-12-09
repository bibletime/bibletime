PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


################################################################################
# BibleTime version
################################################################################
# NOTICE! The version number must only be changed during the release procedures
#  A N D   N O T  during development or bug-fixing. This guarantees that all
# versions of BibleTime between OLDVERSION and NEXTVERSION have version strings
# in the form of OLDVERSION+githash where githash is the git commit hash ID.
#
# Note: for pre-#.#.0 versions, use the following suffixes:
#   _dev      if pre-beta1
#   _beta1    if post-beta1
#   _beta2    if post-beta2
#   _rc1    if post-rc1
#   _rc2    if post-rc2
# For post-full-release versions, no suffix is used.

SET(BT_VERSION_MAJOR "2")
SET(BT_VERSION_MINOR "10")
SET(BT_VERSION_PATCH "0_dev")
#SET(BT_VERSION_BUILD "") # Temporarily uncomment this line for release procedures


# Determine build, if needed:
IF(NOT (DEFINED BT_VERSION_BUILD))
  FIND_PACKAGE(Git)
  IF(NOT GIT_FOUND)
    FIND_PROGRAM(GIT_EXECUTABLE NAMES git)
    IF(GIT_EXECUTABLE)
      SET(GIT_FOUND TRUE)
    ENDIF()
  ENDIF()
  IF(GIT_FOUND)
    FUNCTION(BtGitRevision out)
      EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      ERROR_QUIET
                      RESULT_VARIABLE resVar
                      OUTPUT_VARIABLE outVar
                      OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(${resVar} EQUAL 0)
        STRING(TOLOWER "${outVar}" hashCandidate)
        STRING(REGEX MATCH "^[0123456789abcdef]+$" hasHash "${hashCandidate}")
        IF(hasHash)
          STRING(LENGTH "${hashCandidate}" hashLength)
          IF(hashLength EQUAL 40)
            SET(${out} "${hashCandidate}" PARENT_SCOPE)
          ENDIF()
        ENDIF()
      ENDIF()
    ENDFUNCTION()
    BtGitRevision(BibleTimeGitRevision)
    IF(DEFINED BibleTimeGitRevision)
      SET(BT_VERSION_BUILD "+${BibleTimeGitRevision}")
    ENDIF()
  ENDIF()
ENDIF()

SET(BT_VERSION_FULL "${BT_VERSION_MAJOR}.${BT_VERSION_MINOR}.${BT_VERSION_PATCH}${BT_VERSION_BUILD}")
MESSAGE(STATUS "Setting up build environment for BibleTime version ${BT_VERSION_FULL}")

ADD_DEFINITIONS(-DBT_VERSION="${BT_VERSION_FULL}")

# Define debug flags
IF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_BUILD_TYPE "Debug")
    ADD_DEFINITIONS(-DBT_DEBUG)
ELSE (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS(-DBT_NO_DEBUG -DNDEBUG -DQT_NO_DEBUG)
ENDIF (NOT CMAKE_BUILD_TYPE STREQUAL "Release")

#Non-English locales to be processed and installed for handbook and howto
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS,
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed.
LIST(APPEND HANDBOOK_LOCALE_LANGS ar cs de es fi fr hu it nl pt_BR)
LIST(APPEND HOWTO_LOCALE_LANGS ar bg cs da de es fi fr it hu ko nl pt_BR ru)

IF(NOT BT_FORCE_USE_QT4)
# Automatically use Qt5 if available
FIND_PACKAGE(Qt5Core QUIET)
ENDIF()

# Find required packages
IF (Qt5Core_FOUND)
    FIND_PACKAGE(Qt5LinguistTools REQUIRED)
    FIND_PACKAGE(Qt5Widgets REQUIRED)
    MESSAGE( "-- Found Qt " ${Qt5Core_VERSION})
ELSE (Qt5Core_FOUND)
    SET(QT_MIN_VERSION 4.5.0)
    SET(QT_USE_QTXML 1)
    SET(QT_USE_QTWEBKIT 1)
    SET(QT_USE_QTMAIN 1)
    FIND_PACKAGE(Qt4 REQUIRED)
    MESSAGE( "-- Found Qt " ${QTVERSION})
ENDIF (Qt5Core_FOUND)

IF (QT_QTDBUS_FOUND AND NOT APPLE)
    SET(QT_USE_QTDBUS 1)
    MESSAGE(STATUS "Using DBUS")
ELSE (QT_QTDBUS_FOUND AND NOT APPLE)
    ADD_DEFINITIONS(-DNO_DBUS)
    MESSAGE(STATUS "No DBUS found")
ENDIF (QT_QTDBUS_FOUND AND NOT APPLE)
# If cmake says that qdbusxml2cpp is missing, uncomment the following line
#SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?

# Libs needed for packaging
IF (WIN32 AND NOT UNIX)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
ENDIF (WIN32 AND NOT UNIX)

FIND_PACKAGE(CLucene REQUIRED)
FIND_PACKAGE(Sword 1.7.0 REQUIRED)

# directory layout settings up front, needed in various places
SET(BT_DESTINATION "bin/")
SET(BT_SHARE_PATH "share/")
IF (APPLE)
    SET(BT_DESTINATION "../MacOS/")
    SET(BT_SHARE_PATH "../share/")
    SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
ENDIF (APPLE)

# Define $bibletime_SOURCES
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTSourceFileList.cmake")

#
# The actual build options
#
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}        #for .h files generated from .ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src    # so that include paths like "frontend/..." work
    ${CLucene_INCLUDE_DIR}            #CLucene headers
    ${CLucene_LIBRARY_DIR}            #CLucene/clucene-config.h
    ${Sword_INCLUDE_DIRS}
)

IF (Qt5Core_FOUND)
    QT5_WRAP_UI(bibletime_UIS_H ${bibletime_UIS})
    QT5_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})
ELSE (Qt5Core_FOUND)
    QT4_WRAP_UI(bibletime_UIS_H ${bibletime_UIS})
    QT4_WRAP_CPP(bibletime_MOC_SOURCES ${bibletime_MOCABLE_HEADERS})
ENDIF (Qt5Core_FOUND)

SOURCE_GROUP("QM Files" REGULAR_EXPRESSION ".*\\.qm")

#This must come before the target definition!
LINK_DIRECTORIES(${CLucene_LIBRARY_DIR})

IF (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    SET(EXECUTABLE_NAME "btm")
ELSE (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    SET(EXECUTABLE_NAME "bibletime")
    IF (APPLE)
        SET(EXECUTABLE_NAME "BibleTime")
    ENDIF (APPLE)
ENDIF (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")


# Define rules to generate ${bibletime_UI_translations} files
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")

SET(common_bibletime_SOURCES
    ${bibletime_SOURCES}
    ${bibletime_UIS_H}
    ${bibletime_MOC_SOURCES}
    ${bibletime_QML_FILES}
    ${bibletime_RC}
    ${bibletime_UI_translations}
)

# For the Windows Application Icon
IF (MSVC)
    SET(common_bibletime_SOURCES
        ${common_bibletime_SOURCES}
        "cmake/BTWinIcon.rc"
    )

    SOURCE_GROUP( "Icon Files"
        FILES "cmake/BTWinIcon.rc" }
    )
    ADD_EXECUTABLE(${EXECUTABLE_NAME} WIN32 ${common_bibletime_SOURCES})
ELSE (MSVC)
    ADD_EXECUTABLE(${EXECUTABLE_NAME} ${common_bibletime_SOURCES})
ENDIF (MSVC)

#IF (CMAKE_BUILD_TYPE STREQUAL "Release")
#    SET_TARGET_PROPERTIES(${EXECUTABLE_NAME}
#        PROPERTIES
#            COMPILE_FLAGS "${SWORD_CFLAGS}" #Enable exception handling
#    )
#ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

IF (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    SET (Qt5RequiredModules Widgets Qml Quick Network)
ELSE (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    SET (Qt5RequiredModules Widgets WebKit WebKitWidgets Xml Network)
ENDIF (${BIBLETIME_FRONTEND} STREQUAL "MOBILE")

IF (Qt5Core_FOUND)
    TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
        ${CLucene_LIBRARY}
        ${Sword_LDFLAGS}
    )
    qt5_use_modules(${EXECUTABLE_NAME} ${Qt5RequiredModules})
ELSE (Qt5Core_FOUND)
    INCLUDE(${QT_USE_FILE})
    TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME}
        ${QT_LIBRARIES}
        ${CLucene_LIBRARY}
        ${Sword_LDFLAGS}
    )
ENDIF (Qt5Core_FOUND)

SET(BibleTime_CFLAGS "-Wall -Wextra ${Sword_CFLAGS_OTHER}")
SET(BibleTime_LDFLAGS "")

IF(APPLE)
  # enable support MacOS 10.5+
  SET(T "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/")
  SET(BibleTime_CFLAGS "${BibleTime_CFLAGS} -mmacosx-version-min=10.5 -isysroot ${T}")
  SET(BibleTime_LDFLAGS "${BibleTime_LDFLAGS} -mmacosx-version-min=10.5 -isysroot ${T} -F /Library/Frameworks/")
  UNSET(T)
ENDIF()

SET_TARGET_PROPERTIES(${EXECUTABLE_NAME} PROPERTIES
                                         COMPILE_FLAGS "${BibleTime_CFLAGS}"
                                         LINK_FLAGS "${BibleTime_LDFLAGS}")

IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "install/") #install to ./build/install, allow override
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

IF (APPLE)
    SET(CMAKE_INSTALL_PREFIX "install/BibleTime.app/Contents/Resources")
ENDIF (APPLE)

IF (MSVC)
    SET(CMAKE_CXX_FLAGS_RELEASE "-DNO_DBUS -DSWUSINGDLL /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t")
    SET(CMAKE_CXX_FLAGS_DEBUG   "-DNO_DBUS -DSWUSINGDLL /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t /Od")
    SET_TARGET_PROPERTIES(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    SET_TARGET_PROPERTIES(${EXECUTABLE_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
ELSE (MSVC)
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fpie -fexceptions")
    SET(CMAKE_CXX_FLAGS_DEBUG   "-ggdb -fpie -fexceptions")
ENDIF (MSVC)


# Install files
#

INSTALL(TARGETS "${EXECUTABLE_NAME}"
    DESTINATION "${BT_DESTINATION}"
)

# icons
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
    DESTINATION "${BT_SHARE_PATH}bibletime/icons/"
)
# icons - png, needed for About Dialog
FILE(GLOB INSTALL_ICONS_LIST_PNG "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
    DESTINATION "${BT_SHARE_PATH}bibletime/icons/"
)
# display templates
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/
        DESTINATION "${BT_SHARE_PATH}bibletime/display-templates/")
# javascript
INSTALL(FILES "src/frontend/display/bthtml.js"
    DESTINATION "${BT_SHARE_PATH}bibletime/javascript/"
)
# license.html
INSTALL(FILES "docs/license.html"
    DESTINATION "${BT_SHARE_PATH}bibletime/license/"
)
# qml files
INSTALL(FILES ${bibletime_QML_FILES}
    DESTINATION "${BT_SHARE_PATH}qml/"
)
# splash screen
INSTALL(FILES "pics/startuplogo.png" "pics/startuplogo_christmas.png" "pics/startuplogo_easter.jpg"
    DESTINATION "${BT_SHARE_PATH}bibletime/pics/"
)
#
# Platform specific installation
#
# LINUX: application icon
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg"
    DESTINATION "${BT_SHARE_PATH}icons/"
)
# LINUX: desktop file
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake" "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop")
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop"
    DESTINATION "${BT_SHARE_PATH}applications/"
)
IF (MSVC)
    # sword locale information
    FILE(GLOB INSTALL_SWORD_LOCALE_LIST ${Sword_INCLUDE_DIRS}/../locales.d/*)
    INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
        DESTINATION "${BT_SHARE_PATH}sword/locales.d/"
    )

    # install pdb file for debugging purposes
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET( bibletime_PDB ${bibletime_BINARY_DIR}/Debug/bibletime.pdb)
        INSTALL (FILES "${bibletime_PDB}"
            DESTINATION "${BT_DESTINATION}"
        )
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF (MSVC)

IF (APPLE)
    # sword locale information, needed for DMG image
    FILE(GLOB INSTALL_SWORD_LOCALE_LIST ${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*)
    INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
        DESTINATION "${BT_SHARE_PATH}sword/locales.d/"
    )
ENDIF (APPLE)


INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDeveloper.cmake")

IF (WIN32 AND NOT UNIX)
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageWindows.cmake")
ENDIF (WIN32 AND NOT UNIX)
IF (APPLE)
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageMacOS.cmake")
ENDIF (APPLE)

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

