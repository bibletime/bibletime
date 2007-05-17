# This file is taken from KDE4 (strigi)


#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene
# CLucene_FOUND        = set to 1 if clucene is found
#
 INCLUDE(CheckSymbolExists)

IF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/CLuceneConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/CLuceneConfig.cmake)

  SET(TRIAL_LIBRARY_PATHS
    $ENV{CLUCENE_HOME}/lib${LIB_SUFFIX}
    ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
    /usr/lib${LIB_SUFFIX}
    /usr/local/lib${LIB_SUFFIX}
    /sw/lib${LIB_SUFFIX}
  ) 
  SET(TRIAL_INCLUDE_PATHS
    $ENV{CLUCENE_HOME}/include
    ${CMAKE_INSTALL_PREFIX}/include
    /usr/include
    /usr/local/include
    /sw/include
  ) 

  FIND_LIBRARY(CLUCENE_LIBRARY clucene
      NAMES clucene-core
      PATHS ${TRIAL_LIBRARY_PATHS})
  IF (CLUCENE_LIBRARY)
    MESSAGE(STATUS "Found CLucene library: ${CLUCENE_LIBRARY}")
  ENDIF (CLUCENE_LIBRARY)
  FIND_PATH(CLUCENE_INCLUDE_DIR
      NAMES CLucene.h
      PATHS ${TRIAL_INCLUDE_PATHS})

  IF (CLUCENE_INCLUDE_DIR)
    MESSAGE(STATUS "Found CLucene include dir: ${CLUCENE_INCLUDE_DIR}")
  ENDIF (CLUCENE_INCLUDE_DIR)
  
  IF(NOT MSVC)
    FIND_PATH(CLUCENE_LIBRARY_DIR
        NAMES CLucene/clucene-config.h PATHS ${TRIAL_LIBRARY_PATHS})
    IF (CLUCENE_LIBRARY_DIR)
      MESSAGE(STATUS "Found CLucene library dir: ${CLUCENE_LIBRARY_DIR}")
      FILE(READ ${CLUCENE_LIBRARY_DIR}/CLucene/clucene-config.h CLCONTENT)
      STRING(REGEX MATCH "_CL_VERSION +\".*\"" CLMATCH ${CLCONTENT})
      IF (CLMATCH)
        STRING(REGEX REPLACE "_CL_VERSION +\"(.*)\"" "\\1" CLVERSION ${CLMATCH})
      ENDIF (CLMATCH)
      IF (CLVERSION STRLESS "0.9.16")
        MESSAGE(FATAL_ERROR "CLucene version is less than 0.9.16")
      ENDIF (CLVERSION STRLESS "0.9.16")
      IF (CLVERSION STREQUAL "0.9.17")
        MESSAGE(FATAL_ERROR "CLucene version 0.9.17 is not supported.")
      ENDIF (CLVERSION STREQUAL "0.9.17")
    ENDIF (CLUCENE_LIBRARY_DIR)
  ELSE(NOT MSVC)
    #msvc doesnt use a config file
    SET(CLUCENE_LIBRARY_DIR ${CLUCENE_INCLUDE_DIR})
  ENDIF(NOT MSVC)


IF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)
  SET(CLucene_FOUND 1)
  IF(NOT CLucene_FIND_QUIETLY)
    MESSAGE(STATUS "Found CLucene: ${CLUCENE_LIBRARY}")
  ENDIF(NOT CLucene_FIND_QUIETLY)
ELSE(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)
  SET(CLucene_FOUND 0)
  IF(CLucene_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CLucene. Please install CLucene >= 0.9.16 (http://clucene.sf.net)")
  ENDIF(CLucene_FIND_REQUIRED)
ENDIF(CLUCENE_INCLUDE_DIR AND CLUCENE_LIBRARY AND CLUCENE_LIBRARY_DIR)

MARK_AS_ADVANCED(
  CLUCENE_INCLUDE_DIR 
  CLUCENE_LIBRARY_DIR 
  CLUCENE_LIBRARY 
)
