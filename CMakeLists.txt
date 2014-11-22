PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

IF("${CMAKE_CURRENT_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  MESSAGE(FATAL_ERROR "You probably didn't mean to run CMake from this directory. Now you have all messed up! You'd better delete CMakeFiles/ and CMakeCache.txt or things will break!")
ENDIF()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


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
SET(BT_VERSION_PATCH "0_rc1")
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
IF(NOT (CMAKE_BUILD_TYPE STREQUAL "Release"))
  SET(CMAKE_BUILD_TYPE "Debug")
  ADD_DEFINITIONS("-DBT_DEBUG")
ELSE()
  ADD_DEFINITIONS("-DBT_NO_DEBUG" "-DNDEBUG" "-DQT_NO_DEBUG")
ENDIF()

#Non-English locales to be processed and installed for handbook and howto
#WARNING: If you modify HANDBOOK_LOCALE_LANGS or HOWTO_LOCALE_LANGS,
#also modify the po4a conf files in cmake/docs/ to make sure all languages are processed.
LIST(APPEND HANDBOOK_LOCALE_LANGS ar cs de es fi fr hu it nl pt_BR)
LIST(APPEND HOWTO_LOCALE_LANGS ar bg cs da de es fi fr it hu ko nl pt_BR ru)

# Automatically link Qt executables to qtmain target on Windows
IF(MSVC)
  cmake_policy(SET CMP0020 NEW)
ENDIF()

# SET INSTALLATION PATHS
IF(APPLE)
  SET(BT_DESTINATION "../MacOS")
  SET(BT_SHARE_PATH "../share")
ELSE()
  SET(BT_DESTINATION "bin")
  SET(BT_SHARE_PATH "share")
ENDIF()

IF(NOT BT_FORCE_USE_QT4)
  # Automatically use Qt5 if available
  FIND_PACKAGE(Qt5Core QUIET)
ENDIF()

# Find required packages
IF(Qt5Core_FOUND)
  FIND_PACKAGE(Qt5LinguistTools REQUIRED)
  FIND_PACKAGE(Qt5Gui REQUIRED)
  FIND_PACKAGE(Qt5Widgets REQUIRED)
  FIND_PACKAGE(Qt5DBus QUIET)
  IF(Qt5DBus_FOUND AND NOT APPLE)
    set(BT_Use_DBus TRUE)
  ELSE()
    set(BT_Use_DBus FALSE)
  ENDIF()
  MESSAGE( "-- Found Qt " ${Qt5Core_VERSION})
ELSE()
  SET(QT_MIN_VERSION 4.8.0)
  SET(QT_USE_QTXML 1)
  SET(QT_USE_QTWEBKIT 1)
  SET(QT_USE_QTMAIN 1)
  FIND_PACKAGE(Qt4 REQUIRED)
  IF(QT_QTDBUS_FOUND AND NOT APPLE)
    set(BT_Use_DBus TRUE)
  ELSE()
    set(BT_Use_DBus FALSE)
  ENDIF()
  MESSAGE( "-- Found Qt " ${QTVERSION})
ENDIF()

FIND_PACKAGE(CLucene REQUIRED)
FIND_PACKAGE(Sword 1.7.0 REQUIRED)

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
# Macros used by BTUITranslationLink.cmake amd BTUITranslation.cmake
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTTranslationMacros.cmake")

# Define rules to generate translation files and link them to
# the executable using a qrc file.
IF (${bibletime_LINK_TRANSLATION_FILES})
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslationLink.cmake")
ENDIF()

#This must come before the target definition!
LINK_DIRECTORIES(${CLucene_LIBRARY_DIR})

IF(MSVC)
  ADD_EXECUTABLE("bibletime" WIN32 ${common_bibletime_SOURCES})
ELSE()
  ADD_EXECUTABLE("bibletime" ${common_bibletime_SOURCES})
ENDIF()

IF(${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
  SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "btm")
ELSEIF(APPLE)
  SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "BibleTime")
ELSE()
  SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "bibletime")
ENDIF()

# Define rules to generate and install translation files
IF (NOT ${bibletime_LINK_TRANSLATION_FILES})
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")
ENDIF()

IF(Qt5Core_FOUND)
  TARGET_LINK_LIBRARIES("bibletime"
      ${CLucene_LIBRARY}
      ${Sword_LDFLAGS}
  )

  IF(${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    qt5_use_modules("bibletime" Widgets Qml Quick Xml Network PrintSupport SVG)
  ELSE()
    IF(BT_Use_DBus)
      qt5_use_modules("bibletime" DBus Widgets WebKit WebKitWidgets Xml Network PrintSupport SVG)
      MESSAGE(STATUS "Using DBUS")
    ELSE()
      qt5_use_modules("bibletime" Widgets WebKit WebKitWidgets Xml Network PrintSupport SVG)
      MESSAGE(STATUS "No DBUS found")
      ADD_DEFINITIONS("-DNO_DBUS")
    ENDIF()
  ENDIF()
ELSE()
  IF(BT_Use_DBus)
    SET(QT_USE_QTDBUS 1) # Required by INCLUDE(${QT_USE_FILE}) below
    MESSAGE(STATUS "Using DBUS")
  ELSE()
    ADD_DEFINITIONS("-DNO_DBUS")
    MESSAGE(STATUS "No DBUS found")
  ENDIF()
  # If cmake says that qdbusxml2cpp is missing, uncomment the following line
  #SET(QT_DBUSXML2CPP_EXECUTABLE "/usr/bin/qdbusxml2cpp") #TEMP HACK?
  INCLUDE(${QT_USE_FILE})
  TARGET_LINK_LIBRARIES("bibletime"
      ${QT_LIBRARIES}
      ${CLucene_LIBRARY}
      ${Sword_LDFLAGS}
  )
ENDIF()

IF(${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    ADD_DEFINITIONS("-DBT_MOBILE")
ENDIF()

SET(BibleTime_CFLAGS "-Wall")
SET(BibleTime_LDFLAGS "")
IF(MSVC)
  SET(CMAKE_CXX_FLAGS_RELEASE "-DNO_DBUS -DSWUSINGDLL /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t")
  SET(CMAKE_CXX_FLAGS_DEBUG   "-DNO_DBUS -DSWUSINGDLL /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t /Od")
  SET(BibleTime_LDFLAGS "${BibleTime_LDFLAGS} /SUBSYSTEM:WINDOWS")
ELSE()
  SET(BibleTime_CFLAGS "${BibleTime_CFLAGS} -Wextra -fPIE -fexceptions")
  IF(${BIBLETIME_FRONTEND} STREQUAL "MOBILE")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -std=c++11")
    SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb -std=c++11")
  ELSE()
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
    SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
  ENDIF()
  IF(APPLE)
    # enable support MacOS 10.5+
    SET(T "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/")
    SET(BibleTime_CFLAGS "${BibleTime_CFLAGS} -mmacosx-version-min=10.7 -stdlib=libc++ -isysroot ${T}")
    SET(BibleTime_LDFLAGS "${BibleTime_LDFLAGS} -mmacosx-version-min=10.7 -stdlib=libc++ -isysroot ${T} -F /Library/Frameworks/")
    UNSET(T)
  ENDIF()
ENDIF()

SET_TARGET_PROPERTIES("bibletime" PROPERTIES
                      COMPILE_FLAGS "${Sword_CFLAGS_OTHER} ${BibleTime_CFLAGS}"
                      LINK_FLAGS "${BibleTime_LDFLAGS}")

# Install files
#
INSTALL(TARGETS "bibletime" DESTINATION "${BT_DESTINATION}")
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
        DESTINATION "${BT_SHARE_PATH}/bibletime/icons/")
FILE(GLOB INSTALL_ICONS_LIST_PNG "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
        DESTINATION "${BT_SHARE_PATH}/bibletime/icons/")
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/"
        DESTINATION "${BT_SHARE_PATH}/bibletime/display-templates/")
INSTALL(FILES "src/frontend/display/bthtml.js"
        DESTINATION "${BT_SHARE_PATH}/bibletime/javascript/")
INSTALL(FILES "docs/license.html"
        DESTINATION "${BT_SHARE_PATH}/bibletime/license/")
INSTALL(FILES ${bibletime_QML_FILES} DESTINATION "${BT_SHARE_PATH}/qml/")
INSTALL(FILES "pics/startuplogo.png" "pics/startuplogo_christmas.png"
              "pics/startuplogo_easter.jpg"
        DESTINATION "${BT_SHARE_PATH}/bibletime/pics/")

#
# Platform specific installation
#

# Linux: application icon and desktop file:
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg"
        DESTINATION "${BT_SHARE_PATH}/icons/")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop"
        DESTINATION "${BT_SHARE_PATH}/applications/")


IF(MSVC)
  # sword locale information
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${BT_SHARE_PATH}/sword/locales.d/")

  # install pdb file for debugging purposes
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(bibletime_PDB "${bibletime_BINARY_DIR}/Debug/bibletime.pdb")
    INSTALL(FILES "${bibletime_PDB}" DESTINATION "${BT_DESTINATION}")
  ENDIF()
ENDIF()

IF(APPLE)
  # sword locale information, needed for DMG image
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${BT_SHARE_PATH}/sword/locales.d/")
ENDIF()

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")

IF(WIN32 AND NOT UNIX)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageWindows.cmake")
ENDIF()
IF(APPLE)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageMacOS.cmake")
ENDIF()

######################################################
# "make uninstall" target; see http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
#
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
######################################################

