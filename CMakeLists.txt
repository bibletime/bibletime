PROJECT(bibletime CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)


######################################################
# Set CMake policies:
#
MESSAGE(STATUS "Using CMake ${CMAKE_VERSION}: ${CMAKE_COMMAND}")
IF(MSVC) # Automatically link Qt executables to qtmain target on Windows
    CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()
CMAKE_POLICY(SET CMP0028 NEW)


######################################################
# Build-time user options:
#
SET(USE_QT6 "OFF" CACHE BOOL
    "Whether to build with Qt6 instead of Qt5.")
#NOTE: BibleTime will not compile with Qt6 yet.

SET(BUILD_BIBLETIME "ON" CACHE BOOL
    "Whether to build and install the BibleTime application")

SET(BUILD_TEXT_TO_SPEECH "ON" CACHE BOOL
    "Whether to build BibleTime with text-to-speech (TTS) support. \
This requires Qt's TextToSpeech module.")

SET(BUILD_HANDBOOK_HTML "ON" CACHE BOOL
    "Whether to build and install the handbook in HTML format")
SET(BUILD_HANDBOOK_HTML_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to build and
install the handbook in HTML format if BUILD_HANDBOOK_HTML is enabled. \
Leave empty use all supported languages.")

SET(BUILD_HANDBOOK_PDF "ON" CACHE BOOL
    "Whether to build and install the handbook in PDF")
SET(BUILD_HANDBOOK_PDF_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to build and \
install the handbook in PDF format if BUILD_HANDBOOK_PDF is enabled. \
Leave empty use all supported languages.")

SET(BUILD_HOWTO_HTML "ON" CACHE BOOL
    "Whether to build and install the howto in HTML format")
SET(BUILD_HOWTO_HTML_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to build and \
install the howto in HTML format if BUILD_HOWTO_HTML is enabled. \
Leave empty use all supported languages.")

SET(BUILD_HOWTO_PDF "ON" CACHE BOOL
    "Whether to build and install the howto in PDF format")
SET(BUILD_HOWTO_PDF_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to build and \
install the howto in PDF format if BUILD_HOWTO_PDF is enabled. \
Leave empty use all supported languages.")

SET(INSTALL_GENERATED_DOCS "OFF" CACHE BOOL
    "Install generated_docs from a specified path.")
SET(GENERATED_DOCS_DIR "" CACHE PATH
    "Path to the checked out generated_docs for this revision of BibleTime")

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

SET(BT_VERSION_MAJOR "3")
SET(BT_VERSION_MINOR "1")
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


######################################################
# Paths for installation:
#
INCLUDE(GNUInstallDirs)
IF(NOT DEFINED BT_BINDIR)
    IF(APPLE)
        SET(BT_BINDIR ".")
    ELSE()
        SET(BT_BINDIR "${CMAKE_INSTALL_BINDIR}")
    ENDIF()
ENDIF()
IF(NOT DEFINED BT_DATAROOTDIR)
    IF(APPLE)
        SET(BT_DATAROOTDIR "./BibleTime.app/Contents/share")
    ELSE()
        SET(BT_DATAROOTDIR "${CMAKE_INSTALL_DATAROOTDIR}")
    ENDIF()
ENDIF()
IF(NOT DEFINED BT_DATADIR)
    IF(APPLE)
        SET(BT_DATADIR "./BibleTime.app/Contents/share")
    ELSE()
        SET(BT_DATADIR "${CMAKE_INSTALL_DATADIR}")
    ENDIF()
ENDIF()
IF(APPLE AND NOT DEFINED BT_RESOURCEDIR)
    SET(BT_RESOURCEDIR "./BibleTime.app/Contents/Resources")
ENDIF()
IF(NOT DEFINED SWORD_DATADIR)
    SET(SWORD_DATADIR "${BT_DATAROOTDIR}")
ENDIF()
IF(NOT DEFINED BT_DOCDIR)
    SET(BT_DOCDIR "${CMAKE_INSTALL_DOCDIR}")
ENDIF()
IF(IS_ABSOLUTE "${BT_DOCDIR}")
    SET(BT_DOCDIR_ABSOLUTE "${BT_DOCDIR}")
ELSE()
    SET(BT_DOCDIR_ABSOLUTE "${CMAKE_INSTALL_PREFIX}/${BT_DOCDIR}")
ENDIF()
IF(NOT DEFINED BT_LOCALEDIR)
    # The default for the BT_LOCALEDIR variable differs from the default of
    # localedir in the GNU Coding Standards.
    SET(BT_LOCALEDIR "${BT_DATADIR}/bibletime/locale")
ENDIF()


######################################################
# The BibleTime application:
#
IF(BUILD_BIBLETIME)
    INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTApplication.cmake")
ENDIF()

######################################################
# Documentation:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTDocumentation.cmake")


######################################################
# "fix_cpp_headers" target to fix those copyright headers of files.
#
STRING(TIMESTAMP CURRENT_YEAR "%Y")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/FILE_HEADER.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" @ONLY)
ADD_CUSTOM_TARGET(fix_cpp_headers
    find "${CMAKE_CURRENT_SOURCE_DIR}"
        -path "${CMAKE_CURRENT_SOURCE_DIR}/.git" -prune -o
        -path "${CMAKE_CURRENT_BINARY_DIR}" -prune -o
        -type f "\\(" -name "'*.h'" -o -name "'*.cpp'" -o -name "'*.qml'" "\\)"
        -exec "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fix_cpp_header.sh" "{}"
              "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" "\\;")
