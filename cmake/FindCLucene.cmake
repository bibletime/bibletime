#
# SOURCE: http://websvn.kde.org/trunk/kdesupport/strigi/cmake/FindCLucene.cmake?view=log
#
# ONE FIX: /usr/lib64 added to work on Fedora
#


#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene
# CLUCENE_VERSION      = The CLucene version string
# CLucene_FOUND        = set to 1 if clucene is found
#

INCLUDE(FindLibraryWithDebug)

if(NOT CLUCENE_MIN_VERSION)
    set(CLUCENE_MIN_VERSION "0.9.19")
endif(NOT CLUCENE_MIN_VERSION)

IF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)
    INCLUDE(${PROJECT_CMAKE}/CLuceneConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)

FIND_LIBRARY_WITH_DEBUG(CLUCENE_LIBRARY
    NAMES clucene clucene-core clucene-cored)
IF (CLUCENE_LIBRARY)
    MESSAGE(STATUS "Found CLucene library: ${CLUCENE_LIBRARY}")
ENDIF (CLUCENE_LIBRARY)
FIND_PATH(CLUCENE_INCLUDE_DIR
    NAMES CLucene.h)

IF (CLUCENE_INCLUDE_DIR)
    MESSAGE(STATUS "Found CLucene include dir: ${CLUCENE_INCLUDE_DIR}")
ENDIF (CLUCENE_INCLUDE_DIR)

SET(CLUCENE_GOOD_VERSION TRUE)

FIND_PATH(CLUCENE_LIBRARY_DIR NAMES "CLucene/clucene-config.h")
IF (CLUCENE_LIBRARY_DIR)
    MESSAGE(STATUS "Found CLucene library dir: ${CLUCENE_LIBRARY_DIR}")
    FILE(READ ${CLUCENE_LIBRARY_DIR}/CLucene/clucene-config.h CLCONTENT)
    STRING(REGEX MATCH "_CL_VERSION +\".*\"" CLMATCH ${CLCONTENT})
    IF (CLMATCH)
        STRING(REGEX REPLACE "_CL_VERSION +\"(.*)\"" "\\1" CLUCENE_VERSION ${CLMATCH})
        IF (CLUCENE_VERSION VERSION_LESS "${CLUCENE_MIN_VERSION}")
            MESSAGE(ERROR " CLucene version ${CLUCENE_VERSION} is less than the required minimum ${CLUCENE_MIN_VERSION}")
            SET(CLUCENE_GOOD_VERSION FALSE)
        ENDIF (CLUCENE_VERSION VERSION_LESS "${CLUCENE_MIN_VERSION}")
        IF (CLUCENE_VERSION STREQUAL "0.9.17")
            MESSAGE(ERROR "CLucene version 0.9.17 is not supported.")
            SET(CLUCENE_GOOD_VERSION FALSE)
        ENDIF (CLUCENE_VERSION STREQUAL "0.9.17")
        IF (CLUCENE_VERSION VERSION_GREATER "2.0.0")
        ADD_DEFINITIONS(-DCLUCENE2)
            FIND_LIBRARY_WITH_DEBUG(CLUCENE_SHARED_LIB
                NAMES clucene-shared clucene-sharedd)

            IF (CLUCENE_SHARED_LIB)
                MESSAGE(STATUS "Found CLucene shared library: ${CLUCENE_SHARED_LIB}")
		SET(CLUCENE_LIBRARY ${CLUCENE_LIBRARY} ${CLUCENE_SHARED_LIB})
            ELSE (CLUCENE_SHARED_LIB)
                MESSAGE(FATAL_ERROR "Unable to find CLucene shared library.")
                SET(CLUCENE_GOOD_VERSION FALSE)
            ENDIF(CLUCENE_SHARED_LIB)
        ENDIF (CLUCENE_VERSION VERSION_GREATER "2.0.0")
    ENDIF (CLMATCH)
ELSE (CLUCENE_LIBRARY_DIR)
    MESSAGE(STATUS "CLucene library dir not found.")
ENDIF (CLUCENE_LIBRARY_DIR)

IF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR AND CLUCENE_GOOD_VERSION)
    SET(CLucene_FOUND TRUE)
ENDIF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR AND CLUCENE_GOOD_VERSION)

IF(CLucene_FOUND)
    IF(NOT CLucene_FIND_QUIETLY)
        MESSAGE(STATUS "Found CLucene: ${CLUCENE_LIBRARY}")
    ENDIF(NOT CLucene_FIND_QUIETLY)
ELSE(CLucene_FOUND)
    IF(CLucene_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find CLucene.")
    ENDIF(CLucene_FIND_REQUIRED)
ENDIF(CLucene_FOUND)

MARK_AS_ADVANCED(
    CLUCENE_INCLUDE_DIR
    CLUCENE_LIBRARY_DIR
    CLUCENE_LIBRARY
    )
