#
# SOURCE: http://websvn.kde.org/trunk/kdesupport/strigi/cmake/FindCLucene.cmake?view=log
#
# ONE FIX: /usr/lib64 added to work on Fedora
#

#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLucene_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLucene_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLucene_LIBRARY      = the library to link against CLucene
# CLucene_VERSION      = The CLucene version string
# CLucene_FOUND        = set to 1 if clucene is found
#

INCLUDE(KDE4FindLibraryWithDebug)

IF(NOT CLucene_MIN_VERSION)
  SET(CLucene_MIN_VERSION "2.0.0")
ENDIF()

FIND_LIBRARY_WITH_DEBUG(CLucene_LIBRARY
                        NAMES clucene clucene-core clucene-cored)
IF(CLucene_LIBRARY)
  MESSAGE(STATUS "Found CLucene library: ${CLucene_LIBRARY}")
ENDIF()
FIND_PATH(CLucene_INCLUDE_DIR NAMES CLucene.h)

IF(CLucene_INCLUDE_DIR)
  MESSAGE(STATUS "Found CLucene include dir: ${CLucene_INCLUDE_DIR}")
ENDIF()

SET(CLucene_GOOD_VERSION TRUE)

# Work around clucene-config.h being installed in weird places on some distros:
GET_FILENAME_COMPONENT(CLucene_LIBRARY_DIR_HINTS "${CLucene_LIBRARY}" PATH)

FIND_PATH(CLucene_LIBRARY_DIR
          NAMES "CLucene/clucene-config.h"
          HINTS "${CLucene_LIBRARY_DIR_HINTS}")
IF(CLucene_LIBRARY_DIR)
  MESSAGE(STATUS "Found CLucene library dir: ${CLucene_LIBRARY_DIR}")
  FILE(READ ${CLucene_LIBRARY_DIR}/CLucene/clucene-config.h CLCONTENT)
  STRING(REGEX MATCH "_CL_VERSION +\".*\"" CLMATCH ${CLCONTENT})
  IF(CLMATCH)
    STRING(REGEX REPLACE "_CL_VERSION +\"(.*)\"" "\\1" CLucene_VERSION ${CLMATCH})
    IF(CLucene_VERSION VERSION_LESS "${CLucene_MIN_VERSION}")
      MESSAGE(ERROR " CLucene version ${CLucene_VERSION} is less than the required minimum ${CLucene_MIN_VERSION}")
      SET(CLucene_GOOD_VERSION FALSE)
    ENDIF()
    FIND_LIBRARY_WITH_DEBUG(CLucene_SHARED_LIB
                            NAMES clucene-shared clucene-sharedd)
    IF(CLucene_SHARED_LIB)
      MESSAGE(STATUS "Found CLucene shared library: ${CLucene_SHARED_LIB}")
      SET(CLucene_LIBRARY ${CLucene_LIBRARY} ${CLucene_SHARED_LIB})
    ELSE()
      MESSAGE(FATAL_ERROR "Unable to find CLucene shared library.")
      SET(CLucene_GOOD_VERSION FALSE)
    ENDIF()
  ENDIF()
ELSE()
  MESSAGE(STATUS "CLucene library dir not found.")
ENDIF()

IF(CLucene_INCLUDE_DIR AND CLucene_LIBRARY AND CLucene_LIBRARY_DIR AND CLucene_GOOD_VERSION)
  SET(CLucene_FOUND TRUE)
ENDIF()

IF(CLucene_FOUND)
  IF(NOT CLucene_FIND_QUIETLY)
    MESSAGE(STATUS "Found CLucene: ${CLucene_LIBRARY}")
  ENDIF()
ELSE()
  IF(CLucene_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CLucene.")
  ENDIF()
ENDIF()

MARK_AS_ADVANCED(
    CLucene_DEFINITIONS
    CLucene_INCLUDE_DIR
    CLucene_LIBRARY_DIR
    CLucene_LIBRARY
)
