# Original source:
#   PISM: a Parallel Ice Sheet Model,
#   http://www.pism-docs.org/
#   http://svn.gna.org/viewcvs/*checkout*/pism/trunk/CMake/FindGit.cmake?rev=569
#
# Licence:
#   GPL-2,
#   http://svn.gna.org/viewcvs/*checkout*/pism/trunk/COPYING?rev=569

SET(Git_FOUND FALSE)

FIND_PROGRAM(Git_EXECUTABLE git DOC "git command line client")
MARK_AS_ADVANCED(Git_EXECUTABLE)

IF(Git_EXECUTABLE)
  SET(Git_FOUND TRUE)
  MACRO(Git_WC_INFO dir prefix)
    EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} rev-list -n 1 HEAD
       WORKING_DIRECTORY ${dir}
       RESULT_VARIABLE Git_result
       OUTPUT_VARIABLE ${prefix}_WC_REVISION_HASH
       OUTPUT_STRIP_TRAILING_WHITESPACE
       ERROR_QUIET)
    IF(NOT ((${Git_result} EQUAL 0) AND (NOT ${${prefix}_WC_REVISION_HASH} MATCHES "^$")))
      # MESSAGE(STATUS "Unable to find a git setup.")
      UNSET(${prefix}_WC_REVISION_HASH)
    ELSE(NOT ((${Git_result} EQUAL 0) AND (NOT ${${prefix}_WC_REVISION_HASH} MATCHES "^$")))
      # MESSAGE(STATUS "Found git HEAD: ${${prefix}_WC_REVISION_HASH}")
      EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} rev-list --grep=git-svn-id: -n 1 HEAD
         WORKING_DIRECTORY ${dir}
         RESULT_VARIABLE Git_result2
         OUTPUT_VARIABLE ${prefix}_WC_SVN_REVISION_HASH
         OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(NOT ((${Git_result2} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION_HASH} MATCHES "^$")))
        # MESSAGE(STATUS "Unable to find a git-svn setup.")
        UNSET(${prefix}_WC_SVN_REVISION_HASH)
      ELSE(NOT ((${Git_result2} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION_HASH} MATCHES "^$")))
        # MESSAGE(STATUS "Latest commit also in SVN is ${${prefix}_WC_REVISION_HASH}")
        EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} svn find-rev ${${prefix}_WC_SVN_REVISION_HASH}
           WORKING_DIRECTORY ${dir}
           RESULT_VARIABLE Git_result3
           OUTPUT_VARIABLE ${prefix}_WC_SVN_REVISION
           OUTPUT_STRIP_TRAILING_WHITESPACE)
        IF((${Git_result3} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION} MATCHES "^$"))
          # MESSAGE(STATUS " which is in SVN as revision ${${prefix}_WC_SVN_REVISION}")
        ELSE((${Git_result3} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION} MATCHES "^$"))
          # MESSAGE(WARNING "Unable to determine the SVN revision number!")
          UNSET(${prefix}_WC_SVN_REVISION)
        ENDIF((${Git_result3} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION} MATCHES "^$"))
      ENDIF(NOT ((${Git_result2} EQUAL 0) AND (NOT ${${prefix}_WC_SVN_REVISION_HASH} MATCHES "^$")))
    ENDIF(NOT ((${Git_result} EQUAL 0) AND (NOT ${${prefix}_WC_REVISION_HASH} MATCHES "^$")))
  ENDMACRO(Git_WC_INFO)
ENDIF(Git_EXECUTABLE)

IF(NOT Git_FOUND)
  IF(NOT Git_FIND_QUIETLY)
    MESSAGE(STATUS "Git was not found")
  ELSE(NOT Git_FIND_QUIETLY)
    if(Git_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Git was not found")
    ENDIF(Git_FIND_REQUIRED)
  ENDIF(NOT Git_FIND_QUIETLY)
ENDIF(NOT Git_FOUND)

