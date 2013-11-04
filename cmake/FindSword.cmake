#
# In the name of the Father, and of the Son, and of the Holy Spirit.
#
# This file is part of BibleTime's source code, http://www.bibletime.info/.
#
# Copyright 1999-2013 by the BibleTime developers.
# The BibleTime source code is licensed under the GNU General Public License version 2.0.
#

# This module looks for the Sword library by the Crosswire Bible Society
#
# It will define the following values
# Sword_FOUND
# Sword_VERSION
# Sword_INCLUDE_DIRS
# Sword_LIBRARIES

FIND_LIBRARY(Sword_LIBRARIES NAMES sword)
FIND_PATH(Sword_INCLUDE_DIRS NAMES "sword/swmgr.h")
IF(Sword_INCLUDE_DIRS)
    SET(Sword_INCLUDE_DIRS "${Sword_INCLUDE_DIRS}/sword")
ENDIF()

# Get Sword version:
FUNCTION(FINDSWORD_GET_SWORD_VERSION version)
  TRY_RUN(RUN_RESULT COMPILE_RESULT
      "${CMAKE_CURRENT_BINARY_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sword_version_print.cpp"
      CMAKE_FLAGS
          "-DINCLUDE_DIRECTORIES:STRING=${Sword_INCLUDE_DIRS}"
      COMPILE_OUTPUT_VARIABLE COMPILE_OUTPUT
      RUN_OUTPUT_VARIABLE RUN_OUTPUT)
  IF(NOT COMPILE_RESULT)
    IF(NOT Sword_FIND_QUIETLY)
      MESSAGE(WARNING "Failed to compile Sword version check: ${COMPILE_OUTPUT}")
    ENDIF()
  ELSEIF(RUN_RESULT)
    IF(NOT Sword_FIND_QUIETLY)
      MESSAGE(WARNING "Failed to run Sword version check: ${COMPILE_OUTPUT}")
    ENDIF()
  ELSE()
    SET("${version}" "${RUN_OUTPUT}" PARENT_SCOPE)
  ENDIF()
ENDFUNCTION()
FINDSWORD_GET_SWORD_VERSION(Sword_VERSION)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sword
                                  FOUND_VAR Sword_FOUND
                                  REQUIRED_VARS Sword_LIBRARIES
                                                Sword_INCLUDE_DIRS
                                                Sword_VERSION
                                  VERSION_VAR Sword_VERSION)
