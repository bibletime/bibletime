PROJECT(bibletime CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)


######################################################
# Set CMake policies:
#
IF(MSVC) # Automatically link Qt executables to qtmain target on Windows
    CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()
CMAKE_POLICY(SET CMP0028 NEW)

######################################################
# Misc. settings:
#
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)


######################################################
# Load user configuration files:
#
# If BIBLETIME_BUILDCONFIG set, load the file and fail on error. Otherwise, try
# to include either "config.cmake" under either the build directory or the
# source directory, whichever file first exists, if at all.
IF(DEFINED BIBLETIME_BUILDCONFIG)
  IF(BIBLETIME_BUILDCONFIG_IS_RELATIVE)
      SET(BIBLETIME_BUILDCONFIG
          "${CMAKE_CURRENT_BINARY_DIR}/${BIBLETIME_BUILDCONFIG}")
  ENDIF()
  INCLUDE("${BIBLETIME_BUILDCONFIG}" OPTIONAL RESULT_VARIABLE r)
  IF(r)
    MESSAGE(STATUS "Included \"${BIBLETIME_BUILDCONFIG}\"")
    UNSET(r)
  ELSE()
    MESSAGE(FATAL_ERROR
     "Failed to include build configuration from \"${BIBLETIME_BUILDCONFIG}\"!")
  ENDIF()
ELSE()
  INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.cmake" OPTIONAL RESULT_VARIABLE r)
  IF(r)
    MESSAGE(STATUS "Included \"${r}\"")
  ELSE()
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake"
            OPTIONAL RESULT_VARIABLE r)
    IF(r)
      MESSAGE(STATUS "Included \"${r}\"")
    ENDIF()
  ENDIF()
  UNSET(r)
ENDIF()


######################################################
# Set CMake module path:
#
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")



######################################################
# BibleTime version:
#
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
SET(BT_VERSION_MINOR "11")
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


######################################################
# Find packages:
#
FIND_PACKAGE(CLucene REQUIRED)
SET(REQUIRED_QT_VERSION 5.9)
FIND_PACKAGE(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5LinguistTools ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Svg ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5PrintSupport ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Qml ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Quick ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5QuickWidgets ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Sword 1.8.1 REQUIRED)


######################################################
# Build options, definitions, linker flags etc for all targets:
#
SET(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS("-DBT_VERSION=\"${BT_VERSION_FULL}\"")
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(CMAKE_AUTOMOC_MOC_OPTIONS "-DNDEBUG")
ENDIF()
IF(WIN32)
    ADD_COMPILE_OPTIONS("/Zi")
ELSE()
    ADD_COMPILE_OPTIONS(
        "-std=c++11" "-ggdb" "-Wall" "-Wextra"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-O2>"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DQT_NO_DEBUG>"
        "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
    )
ENDIF()
UNSET(BibleTime_CXXFLAGS)
UNSET(BibleTime_LDFLAGS)
IF(MSVC)
  SET(CMAKE_CXX_FLAGS_RELEASE "-DSWUSINGDLL /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t")
  SET(CMAKE_CXX_FLAGS_DEBUG   "-DSWUSINGDLL /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t /Od")
ELSE()
  ADD_COMPILE_OPTIONS("-fPIE" "-fexceptions")
  SET(CMAKE_CXX_FLAGS_RELEASE "")
  SET(CMAKE_CXX_FLAGS_DEBUG "")
  IF(APPLE)
    # enable support MacOS 10.5+
    SET(T "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk/")
    LIST(APPEND BibleTime_CXXFLAGS
        "-mmacosx-version-min=10.7"
        "-stdlib=libc++"
        "-isysroot" "${T}")
    LIST(APPEND BibleTime_LDFLAGS
        "-mmacosx-version-min=10.7"
        "-stdlib=libc++"
        "-isysroot" "${T}"
        "-F" "/Library/Frameworks/")
    UNSET(T)
  ENDIF()
ENDIF()


######################################################
# Paths for installation:
#
IF(NOT DEFINED BT_BINDIR)
    IF(APPLE)
        SET(BT_BINDIR "../MacOS")
    ELSE()
        SET(BT_BINDIR "./bin")
    ENDIF()
ENDIF()
IF(NOT DEFINED BT_DATAROOTDIR)
    IF(APPLE)
        SET(BT_DATAROOTDIR "../share")
    ELSE()
        SET(BT_DATAROOTDIR "./share")
    ENDIF()
ENDIF()
IF(NOT DEFINED BT_DATADIR)
    SET(BT_DATADIR "${BT_DATAROOTDIR}")
ENDIF()
IF(NOT DEFINED SWORD_DATADIR)
    SET(SWORD_DATADIR "${BT_DATAROOTDIR}")
ENDIF()
IF(NOT DEFINED BT_DOCDIR)
    SET(BT_DOCDIR "${BT_DATAROOTDIR}/doc/bibletime")
ENDIF()
IF(NOT DEFINED BT_LOCALEDIR)
    # The default for the BT_LOCALEDIR variable differs from the default of
    # localedir in the GNU Coding Standards.
    SET(BT_LOCALEDIR "${BT_DATADIR}/bibletime/locale")
ENDIF()


######################################################
# bibletime_backend static library. Shared between the bibletime app and all
# tests:
#
FILE(GLOB_RECURSE bibletime_BACKEND_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.h"
)
ADD_LIBRARY(bibletime_backend STATIC ${bibletime_BACKEND_SOURCES})
TARGET_COMPILE_DEFINITIONS(bibletime_backend PRIVATE ${CLucene_DEFINITIONS})
TARGET_COMPILE_OPTIONS(bibletime_backend
    PUBLIC
        ${BibleTime_CXXFLAGS}
        ${Sword_CFLAGS_OTHER}
)
TARGET_INCLUDE_DIRECTORIES(bibletime_backend
    PRIVATE
        ${CLucene_INCLUDE_DIR}            #CLucene headers
        ${CLucene_LIBRARY_DIR}            #CLucene/clucene-config.h
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}        #for .h files generated from .ui
        ${Sword_INCLUDE_DIRS}
)
TARGET_LINK_LIBRARIES(bibletime_backend
    PUBLIC
        Qt5::Widgets
        Qt5::Xml
        -L${CLucene_LIBRARY_DIR} ${CLucene_LIBRARY}
        ${Sword_LDFLAGS}
        ${BibleTime_LDFLAGS}
)


######################################################
# The bibletime application:
#
FILE(GLOB_RECURSE bibletime_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.h"
)
IF(MSVC) # For the Windows Application Icon
    LIST(APPEND bibletime_SOURCES "cmake/BTWinIcon.rc")
ENDIF()
IF(MSVC)
    ADD_EXECUTABLE("bibletime" WIN32 ${bibletime_SOURCES})
ELSE()
    ADD_EXECUTABLE("bibletime" ${bibletime_SOURCES})
ENDIF()
IF(APPLE)
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "BibleTime")
ELSE()
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "bibletime")
ENDIF()
TARGET_LINK_LIBRARIES("bibletime"
    PRIVATE
        bibletime_backend
        Qt5::Network
        Qt5::PrintSupport
        Qt5::Quick
        Qt5::QuickWidgets
        Qt5::Svg
        Qt5::Widgets
        Qt5::Xml
)


######################################################
# Define rules to generate and install translation files:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")


######################################################
# Tests:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTTests.cmake")


######################################################
# Documentation:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")


######################################################
# Installation:
#
INSTALL(TARGETS "bibletime" DESTINATION "${BT_BINDIR}")
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")
FILE(GLOB INSTALL_ICONS_LIST_PNG
    "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")

INSTALL(FILES "docs/license.html"
        DESTINATION "${BT_DATADIR}/bibletime/license/")

# See QTBUG-55259, Affects windeployqt and macdeployqt
INSTALL(FILES "src/frontend/display/modelview/DisplayView.qml"
        DESTINATION "${BT_BINDIR}/")

FILE(GLOB INSTALL_TMPL_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.css"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/Basic.tmpl"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/basic_template.txt")
INSTALL(FILES ${INSTALL_TMPL_LIST}
        DESTINATION "${BT_DATADIR}/bibletime/display-templates/")
INSTALL(FILES "pics/startuplogo.png" "pics/startuplogo_christmas.png"
              "pics/startuplogo_easter.jpg"
        DESTINATION "${BT_DATADIR}/bibletime/pics/")

# Platform specific installation

# Linux: application icon and desktop file:
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg"
        DESTINATION "${BT_DATAROOTDIR}/icons/hicolor/scalable/apps" RENAME "info.bibletime.BibleTime.svg")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop"
        DESTINATION "${BT_DATAROOTDIR}/applications/" RENAME "info.bibletime.BibleTime.desktop")

INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/info.bibletime.BibleTime.appdata.xml"
        DESTINATION "${BT_DATAROOTDIR}/metainfo/")

# Windows:
IF(MSVC)
  # sword locale information
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../../locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${SWORD_DATADIR}/sword/locales.d/")

  # install pdb file for debugging purposes
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(bibletime_PDB "${bibletime_BINARY_DIR}/Debug/bibletime.pdb")
    INSTALL(FILES "${bibletime_PDB}" DESTINATION "${BT_BINDIR}")
  ENDIF()
ENDIF()

# OS X:
IF(APPLE)
  # sword locale information, needed for DMG image
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${SWORD_DATADIR}/sword/locales.d/")
ENDIF()


######################################################
# Binary packages:
#
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
# "fix_cpp_headers" target to fix those copyright headers of files.
#
STRING(TIMESTAMP CURRENT_YEAR "%Y")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/FILE_HEADER.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" @ONLY)
ADD_CUSTOM_TARGET(fix_cpp_headers
    find "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        -type f "\\(" -name "'*.h'" -o -name "'*.cpp'" "\\)"
        -exec "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fix_cpp_header.sh" "{}"
              "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" "\\;")
