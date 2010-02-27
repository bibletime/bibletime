# Original source:
#   PISM: a Parallel Ice Sheet Model,
#   http://www.pism-docs.org/
#   http://svn.gna.org/viewcvs/*checkout*/pism/trunk/CMake/FindGit.cmake?rev=569
#
# Licence:
#   GPL-2,
#   http://svn.gna.org/viewcvs/*checkout*/pism/trunk/COPYING?rev=569

SET(Git_FOUND FALSE)

FIND_PROGRAM(Git_EXECUTABLE git
  DOC "git command line client")
MARK_AS_ADVANCED(Git_EXECUTABLE)

IF(Git_EXECUTABLE)
  SET(Git_FOUND TRUE)
  MACRO(Git_WC_INFO dir prefix)
    EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} rev-list -n 1 HEAD
       WORKING_DIRECTORY ${dir}
        ERROR_VARIABLE Git_error
       OUTPUT_VARIABLE ${prefix}_WC_REVISION_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${Git_error} EQUAL 0)
      MESSAGE(SEND_ERROR "Command \"${Git_EXECUTABLE} rev-list -n 1 HEAD\" in directory ${dir} failed with output:\n${Git_error}")
    ELSE(NOT ${Git_error} EQUAL 0)
      EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} name-rev ${${prefix}_WC_REVISION_HASH}
         WORKING_DIRECTORY ${dir}
         OUTPUT_VARIABLE ${prefix}_WC_REVISION_NAME
          OUTPUT_STRIP_TRAILING_WHITESPACE)
      EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} rev-list --grep=git-svn-id: -n 1 HEAD
         WORKING_DIRECTORY ${dir}
          ERROR_VARIABLE Git_svn_error
         OUTPUT_VARIABLE ${prefix}_WC_SVN_REVISION_HASH
          OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(Git_svn_error STREQUAL "")
        EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} svn find-rev ${${prefix}_WC_SVN_REVISION_HASH}
           WORKING_DIRECTORY ${dir}
            ERROR_VARIABLE Git_svn_error
           OUTPUT_VARIABLE ${prefix}_WC_SVN_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        IF(NOT ${Git_svn_error} EQUAL 0)
          MESSAGE(SEND_ERROR "Command \"${Git_EXECUTABLE} svn find-rev ${${prefix}_WC_SVN_REVISION_HASH}\" in directory ${dir} failed with output:\n${Git_error}")
        ENDIF(NOT ${Git_svn_error} EQUAL 0)
      ENDIF(Git_svn_error STREQUAL "")
    ENDIF(NOT ${Git_error} EQUAL 0)
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

