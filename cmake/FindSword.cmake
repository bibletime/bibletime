#
# In the name of the Father, and of the Son, and of the Holy Spirit.
#
# This file is part of BibleTime's source code, https://bibletime.info/.
#
# Copyright 1999-2021 by the BibleTime developers.
# The BibleTime source code is licensed under the GNU General Public License version 2.0.
#

# This module looks for the Sword library by the Crosswire Bible Society
#
# It might define the following values similarly to pkg_search_module()
# Sword_FOUND
# Sword_LIBRARIES
# Sword_LIBRARY_DIRS
# Sword_LDFLAGS
# Sword_LDFLAGS_OTHER
# Sword_INCLUDE_DIRS
# Sword_CFLAGS
# Sword_CFLAGS_OTHER
# Sword_VERSION

MACRO(FindSword_unset_all)
  UNSET(Sword_FOUND)
  UNSET(Sword_LIBRARIES)
  UNSET(Sword_LIBRARY_DIRS)
  UNSET(Sword_LDFLAGS)
  UNSET(Sword_LDFLAGS_OTHER)
  UNSET(Sword_INCLUDE_DIRS)
  UNSET(Sword_CFLAGS)
  UNSET(Sword_CFLAGS_OTHER)
  UNSET(Sword_VERSION)
ENDMACRO()
FindSword_unset_all()

FIND_PACKAGE(PkgConfig QUIET)

IF(PKG_CONFIG_FOUND)
  # Workaround PKG_CONFIG_PATH set to include CMAKE_PREFIX_PATH paths:
  IF(WIN32)
    SET(FindSword_s ";")
  ELSE()
    SET(FindSword_s ":")
  ENDIF()
  FOREACH(d IN LISTS CMAKE_PREFIX_PATH)
    SET(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}${FindSword_s}${d}/lib/pkgconfig${FindSword_s}${d}/share/pkgconfig${FindSword_s}${d}/lib32/pkgconfig${FindSword_s}${d}/lib64/pkgconfig")
  ENDFOREACH()
  UNSET(FindSword_s)

  IF(Sword_FIND_VERSION_EXACT)
    SET(FindSword_Version_Comp "=")
  ELSE()
    SET(FindSword_Version_Comp ">=")
  ENDIF()
  pkg_search_module(Sword QUIET
      "sword${FindSword_Version_Comp}${Sword_FIND_VERSION}"
      "sword-${Sword_FIND_VERSION_MAJOR}${FindSword_Version_Comp}${Sword_FIND_VERSION}"
      "sword-${Sword_FIND_VERSION_MAJOR}.${Sword_FIND_VERSION_MINOR}${FindSword_Version_Comp}${Sword_FIND_VERSION}"
      "sword${Sword_FIND_VERSION_MAJOR}${FindSword_Version_Comp}${Sword_FIND_VERSION}"
      "sword${Sword_FIND_VERSION_MAJOR}.${Sword_FIND_VERSION_MINOR}${FindSword_Version_Comp}${Sword_FIND_VERSION}")
  UNSET(FindSword_Version_Comp)
ENDIF()

# If pkg-config fails we try this:
IF(NOT Sword_FOUND)
  IF(NOT Sword_FIND_QUIETLY)
    MESSAGE(WARNING "Falling back to manual Sword detection...")
  ENDIF()
  FIND_LIBRARY(FindSword_LIBS NAMES sword libsword)
  IF(FindSword_LIBS)
    FIND_PATH(FindSword_INCS NAMES "sword/swmgr.h")
    IF(FindSword_INCS)
      SET(FindSword_INCS "${FindSword_INCS}/sword")
      TRY_RUN(RUN_RESULT COMPILE_RESULT
              "${CMAKE_CURRENT_BINARY_DIR}"
              "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sword_version_print.cpp"
              CMAKE_FLAGS
                "-DINCLUDE_DIRECTORIES:STRING=${FindSword_INCS}"
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
        SET(Sword_FOUND TRUE)
        SET(Sword_VERSION "${RUN_OUTPUT}")
        GET_FILENAME_COMPONENT(Sword_LIBRARY_DIRS "${FindSword_LIBS}" PATH)
        SET(Sword_LIBRARIES "sword")
        IF(WIN32)
            SET(Sword_LDFLAGS "${FindSword_LIBS}")
        ELSE()
            SET(Sword_LDFLAGS "-L${Sword_LIBRARY_DIRS};-l${Sword_LIBRARIES}")
        ENDIF()
        SET(Sword_INCLUDE_DIRS "${FindSword_INCS}")
        SET(Sword_CFLAGS "-I${Sword_INCLUDE_DIRS}")
      ENDIF()
    ENDIF()
    UNSET(FindSword_INCS)
  ENDIF()
  UNSET(FindSword_LIBS)
ENDIF()

IF(Sword_FOUND)
  SET(FindSword_Sword_FOUND "YES")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sword
                                  FOUND_VAR Sword_FOUND
                                  REQUIRED_VARS FindSword_Sword_FOUND
                                  VERSION_VAR Sword_VERSION)
UNSET(FindSword_Sword_FOUND)

IF(Sword_FOUND AND NOT Sword_FIND_QUIETLY)
  MESSAGE(STATUS "Sword ${Sword_VERSION} details:")
  MESSAGE(STATUS "    CFLAGS: ${Sword_CFLAGS}")
  MESSAGE(STATUS "      INCLUDE_DIRS: ${Sword_INCLUDE_DIRS}")
  MESSAGE(STATUS "      CFLAGS_OTHER: ${Sword_CFLAGS_OTHER}")
  MESSAGE(STATUS "    LDFLAGS: ${Sword_LDFLAGS}")
  MESSAGE(STATUS "      LIBRARIES: ${Sword_LIBRARIES}")
  MESSAGE(STATUS "      LIBRARY_DIRS: ${Sword_LIBRARY_DIRS}")
  MESSAGE(STATUS "      LDFLAGS_OTHER: ${Sword_LDFLAGS_OTHER}")
ENDIF()
