CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
MESSAGE(STATUS "Using CMake ${CMAKE_VERSION}: ${CMAKE_COMMAND}")
PROJECT(BibleTime
  VERSION 3.1.0
  DESCRIPTION "A Bible study application"
  HOMEPAGE_URL "https://bibletime.info/"
  LANGUAGES NONE # Defer compiler detection (logs) to AFTER versioning logic
)
SET(PROJECT_VERSION_TWEAK "") # e.g. _alpha1, _beta2, _rc3
SET(PROJECT_VERSION "${PROJECT_VERSION}${PROJECT_VERSION_TWEAK}")
MESSAGE(STATUS "BibleTime version in CMakeLists.txt: ${PROJECT_VERSION}")
BLOCK(PROPAGATE PROJECT_VERSION)
  IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    FIND_PACKAGE(Git REQUIRED)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} describe --match "v*.*.*" --dirty --abbrev=12
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_DESCRIBE
      ENCODING "UTF-8"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      COMMAND_ERROR_IS_FATAL ANY
    )
    IF(NOT ("${GIT_DESCRIBE}" MATCHES "^v${PROJECT_VERSION}"))
      MESSAGE(FATAL_ERROR "Project version doesn't match `git describe`!")
    ENDIF()
    STRING(SUBSTRING "${GIT_DESCRIBE}" 1 -1 PROJECT_VERSION)
  ENDIF()
ENDBLOCK()
IF(PROJECT_IS_TOP_LEVEL)
  SET(CMAKE_PROJECT_VERSION "${PROJECT_VERSION}")
ENDIF()
MESSAGE(STATUS "Configuring BibleTime version ${PROJECT_VERSION}")
ENABLE_LANGUAGE(CXX) # This logs compiler detection messages AFTER the above


######################################################
# Retrieve and log more information about build environment:
#
BLOCK()
  FOREACH(KEY IN ITEMS OS_NAME OS_RELEASE OS_VERSION OS_PLATFORM)
    CMAKE_HOST_SYSTEM_INFORMATION(RESULT VALUE QUERY "${KEY}")
    MESSAGE(STATUS "${KEY}: ${VALUE}")
  ENDFOREACH()
  CMAKE_HOST_SYSTEM_INFORMATION(RESULT OS_RELEASE_FILE QUERY DISTRIB_INFO)
  IF("(${OS_RELEASE_FILE})" STREQUAL "()")
    MESSAGE(STATUS "No /etc/os-release information found.")
  ELSE()
    MESSAGE(STATUS "Information from /etc/os-release:")
    FOREACH(OS_RELEASE_KEY IN LISTS OS_RELEASE_FILE)
      MESSAGE(STATUS "  ${OS_RELEASE_KEY}: ${${OS_RELEASE_KEY}}")
    ENDFOREACH()
  ENDIF()
ENDBLOCK()


######################################################
# Build-time user options:
#
SET(BUILD_BIBLETIME "ON" CACHE BOOL
    "Whether to build and install the BibleTime application")

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
# Paths for installation:
#
INCLUDE(GNUInstallDirs)
IF(NOT DEFINED BT_BINDIR)
    SET(BT_BINDIR "${CMAKE_INSTALL_BINDIR}")
ENDIF()
IF(NOT DEFINED BT_DATAROOTDIR)
    SET(BT_DATAROOTDIR "${CMAKE_INSTALL_DATAROOTDIR}")
ENDIF()
IF(NOT DEFINED BT_DATADIR)
    SET(BT_DATADIR "${CMAKE_INSTALL_DATADIR}")
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
