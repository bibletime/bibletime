#taken from http://websvn.kde.org/trunk/KDE/kdelibs/cmake/modules/KDE4Macros.cmake:1196999


# - MACRO_ADDITIONAL_CLEAN_FILES(files...)
# MACRO_OPTIONAL_FIND_PACKAGE( <name> [QUIT] )

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# MACRO (MACRO_ADDITIONAL_CLEAN_FILES)
#    GET_DIRECTORY_PROPERTY(_tmp_DIR_PROPS ADDITIONAL_MAKE_CLEAN_FILES )
#
#    if (_tmp_DIR_PROPS)
#       set(_tmp_DIR_PROPS ${_tmp_DIR_PROPS} ${ARGN})
#    else (_tmp_DIR_PROPS)
#       set(_tmp_DIR_PROPS ${ARGN})
#    endif (_tmp_DIR_PROPS)
#
#    SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${_tmp_DIR_PROPS}")
# ENDMACRO (MACRO_ADDITIONAL_CLEAN_FILES)


# This macro doesn't set up the RPATH related options for executables anymore,
# since now (wioth cmake 2.6) just the full RPATH is used always for everything.
# It does create wrapper shell scripts for the executables.
# It overrides the defaults set in FindKDE4Internal.cmake.
# For every executable a wrapper script is created, which sets the appropriate
# environment variable for the platform (LD_LIBRARY_PATH on most UNIX systems,
# DYLD_LIBRARY_PATH on OS X and PATH in Windows) so  that it points to the built
# but not yet installed versions of the libraries. So if RPATH is disabled, the executables
# can be run via these scripts from the build tree and will find the correct libraries.
# If RPATH is not disabled, these scripts are also used but only for consistency, because
# they don't really influence anything then, because the compiled-in RPATH overrides
# the LD_LIBRARY_PATH env. variable.
# macro (HANDLE_RPATH_FOR_EXECUTABLE _target_NAME)
#    if (UNIX)
#       if (APPLE)
#          set(_library_path_variable "DYLD_LIBRARY_PATH")
#       elseif (CYGWIN)
#          set(_library_path_variable "PATH")
#       else (APPLE)
#          set(_library_path_variable "LD_LIBRARY_PATH")
#       endif (APPLE)
#
#       if (APPLE)
#          # DYLD_LIBRARY_PATH does not work like LD_LIBRARY_PATH
#          # OSX already has the RPATH in libraries and executables, putting runtime directories in
#          # DYLD_LIBRARY_PATH actually breaks things
#          set(_ld_library_path "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}")
#       else (APPLE)
#          set(_ld_library_path "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/:${LIB_INSTALL_DIR}:${QT_LIBRARY_DIR}")
#       endif (APPLE)
#       get_target_property(_executable ${_target_NAME} LOCATION )
#
#       # use add_custom_target() to have the sh-wrapper generated during build time instead of cmake time
#       add_custom_command(TARGET ${_target_NAME} POST_BUILD
#          COMMAND ${CMAKE_COMMAND}
#          -D_filename=${_executable}.shell -D_library_path_variable=${_library_path_variable}
#          -D_ld_library_path="${_ld_library_path}" -D_executable=${_executable}
#          -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/exec_via_sh.cmake
#          )
#
#       macro_additional_clean_files(${_executable}.shell)
#
#       # under UNIX, set the property WRAPPER_SCRIPT to the name of the generated shell script
#       # so it can be queried and used later on easily
#       set_target_properties(${_target_NAME} PROPERTIES WRAPPER_SCRIPT ${_executable}.shell)
#
#    else (UNIX)
#       # under windows, set the property WRAPPER_SCRIPT just to the name of the executable
#       # maybe later this will change to a generated batch file (for setting the PATH so that the Qt libs are found)
#       get_target_property(_executable ${_target_NAME} LOCATION )
#       set_target_properties(${_target_NAME} PROPERTIES WRAPPER_SCRIPT ${_executable})
#
#       set(_ld_library_path "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}\;${LIB_INSTALL_DIR}\;${QT_LIBRARY_DIR}")
#       get_target_property(_executable ${_target_NAME} LOCATION )
#
#       # use add_custom_target() to have the batch-file-wrapper generated during build time instead of cmake time
#       add_custom_command(TARGET ${_target_NAME} POST_BUILD
#          COMMAND ${CMAKE_COMMAND}
#          -D_filename="${_executable}.bat"
#          -D_ld_library_path="${_ld_library_path}" -D_executable="${_executable}"
#          -P ${KDE4_MODULE_DIR}/exec_via_sh.cmake
#          )
#
#    endif (UNIX)
# endmacro (HANDLE_RPATH_FOR_EXECUTABLE)

# Add a unit test, which is executed when running make test .
# The targets are always created, but only built for the "all"
# target if the option BUILD_TESTS is enabled. Otherwise the rules for the target
# are created but not built by default. You can build them by manually building the target.
# The name of the target can be specified using TESTNAME <testname>, if it is not given
# the macro will default to the <name>
macro (ADD_UNIT_TEST _test_NAME)
    set(_srcList ${ARGN})
    set(_targetName ${_test_NAME})
    if( ${ARGV1} STREQUAL "TESTNAME" )
        set(_targetName ${ARGV2})
        list(REMOVE_AT _srcList 0 1)
    endif( ${ARGV1} STREQUAL "TESTNAME" )

    #qt4_wrap_cpp(ignore_me ${_srcList})

    #SET(_test_SOURCES ${_srcList} ${${_test_NAME}_MOCFILES})
    #http://www.cmake.org/pipermail/cmake/2009-December/033804.html
    #not working with cmake version 2.8.2
    QT4_AUTOMOC(${_srcList})
    #QT4_GENERATE_MOC(${_srcList} ${_srcList})

    if(WIN32)
        add_executable(${_test_NAME} WIN32 ${_srcList})
    else(WIN32)
        add_executable(${_test_NAME} ${_srcList})
    endif(WIN32)

    target_link_libraries(${_test_NAME} ${EXECUTABLE_NAME}_test)
    # so we get QTest
    target_link_libraries(${_test_NAME} ${QT_LIBRARIES})

# not needed, we don't have libraries, only .o files
#    handle_rpath_for_executable(${_test_NAME})

    if (WIN32)
        target_link_libraries(${_test_NAME} ${QT_QTMAIN_LIBRARY})
    endif (WIN32)

    if(NOT TEST_OUTPUT)
        set(TEST_OUTPUT plaintext)
    endif(NOT TEST_OUTPUT)
    set(TEST_OUTPUT ${TEST_OUTPUT} CACHE STRING "The output to generate when running the QTest unit tests")

    get_target_property( loc ${_test_NAME} LOCATION )
    if(WIN32)
      if(MSVC_IDE)
        STRING(REGEX REPLACE "\\$\\(.*\\)" "\${CTEST_CONFIGURATION_TYPE}" loc "${loc}")
      endif(MSVC_IDE)
      set(_executable ${loc})
    else(WIN32)
      if (Q_WS_MAC AND NOT _nogui)
        set(_executable ${EXECUTABLE_OUTPUT_PATH}/${_test_NAME}.app/Contents/MacOS/${_test_NAME})
      else (Q_WS_MAC AND NOT _nogui)
        set(_executable ${loc})
      endif (Q_WS_MAC AND NOT _nogui)
    endif(WIN32)

    if (TEST_OUTPUT STREQUAL "xml")
        #MESSAGE(STATUS "${_targetName} : Using QTestLib, can produce XML report.")
        add_test( ${_targetName} ${_executable} -xml -o ${_targetName}.tml)
    else (TEST_OUTPUT STREQUAL "xml")
        #MESSAGE(STATUS "${_targetName} : NOT using QTestLib, can't produce XML report, please use QTestLib to write your unit tests.")
        add_test( ${_targetName} ${_executable} )
    endif (TEST_OUTPUT STREQUAL "xml")

    if (NOT MSVC_IDE)   #not needed for the ide
        get_directory_property(_buildtestsAdded BUILDTESTS_ADDED)
        if(NOT _buildtestsAdded)
            add_custom_target(buildtests)
            set_directory_properties(PROPERTIES BUILDTESTS_ADDED TRUE)
        endif(NOT _buildtestsAdded)
        add_dependencies(buildtests ${_test_NAME})
    endif (NOT MSVC_IDE)
endmacro (ADD_UNIT_TEST)