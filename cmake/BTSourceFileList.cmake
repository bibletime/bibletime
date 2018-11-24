STRING(TIMESTAMP CURRENT_YEAR "%Y")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/FILE_HEADER.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" @ONLY)
ADD_CUSTOM_TARGET(fix_cpp_headers
    find "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        -type f "\\(" -name "'*.h'" -o -name "'*.cpp'" "\\)"
        -exec "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fix_cpp_header.sh" "{}"
              "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" "\\;")

FILE(GLOB_RECURSE bibletime_COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.h"
)

FILE(GLOB_RECURSE bibletime_FRONTEND_DESKTOP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.h"
)

SET(bibletime_DESKTOP_TRANSLATION_FILE_PREFIX "bibletime_ui_")

SET(bibletime_LINK_DESKTOP_TRANSLATION_FILES FALSE)

SET(bibletime_DESKTOP_TRANSLATION_RESOURCE_FILE "")

# Default to building a desktop frontend:
IF(NOT (DEFINED BIBLETIME_FRONTEND))
  SET(BIBLETIME_FRONTEND "DESKTOP")
ENDIF(NOT (DEFINED BIBLETIME_FRONTEND))

# Check for valid frontend:
SET(bibletime_AVAILABLE_FRONTENDS DESKTOP)
LIST(FIND bibletime_AVAILABLE_FRONTENDS ${BIBLETIME_FRONTEND} bibletime_FRONTEND_INDEX)
IF(${bibletime_FRONTEND_INDEX} EQUAL -1)
  MESSAGE(STATUS "Invalid frontend specified: ${BIBLETIME_FRONTEND}.")
  MESSAGE(STATUS "Available frontends are:")
  FOREACH(frontend ${bibletime_AVAILABLE_FRONTENDS})
    MESSAGE(STATUS "    ${frontend}")
  ENDFOREACH(frontend ${bibletime_AVAILABLE_FRONTENDS})
  MESSAGE(FATAL_ERROR "Aborting processing because of invalid BIBLETIME_FRONTEND")
ENDIF(${bibletime_FRONTEND_INDEX} EQUAL -1)
MESSAGE(STATUS "Selected frontend: ${BIBLETIME_FRONTEND}")

SET(bibletime_SOURCES
    ${bibletime_FRONTEND_${BIBLETIME_FRONTEND}_SOURCES})
SET(bibletime_RESOURCE_FILES
    ${bibletime_${BIBLETIME_FRONTEND}_RESOURCE_FILES})
SET(bibletime_QML_FILES
    ${bibletime_${BIBLETIME_FRONTEND}_QML_FILES})

SET(bibletime_TRANSLATION_FILE_PREFIX "${bibletime_${BIBLETIME_FRONTEND}_TRANSLATION_FILE_PREFIX}")
SET(bibletime_LINK_TRANSLATION_FILES "${bibletime_LINK_${BIBLETIME_FRONTEND}_TRANSLATION_FILES}")
SET(bibletime_TRANSLATION_RESOURCE_FILE "${bibletime_${BIBLETIME_FRONTEND}_TRANSLATION_RESOURCE_FILE}")

QT5_ADD_RESOURCES(bibletime_RESOURCE_SOURCES ${bibletime_RESOURCE_FILES})

SET(common_bibletime_SOURCES
    ${bibletime_SOURCES}
    ${bibletime_RESOURCE_SOURCES}
    ${bibletime_QML_FILES}
    ${bibletime_UI_translations}
)

# For the Windows Application Icon
IF(MSVC)
  LIST(APPEND common_bibletime_SOURCES "cmake/BTWinIcon.rc")
ENDIF()
