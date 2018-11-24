STRING(TIMESTAMP CURRENT_YEAR "%Y")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/FILE_HEADER.cpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" @ONLY)
ADD_CUSTOM_TARGET(fix_cpp_headers
    find "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        -type f "\\(" -name "'*.h'" -o -name "'*.cpp'" "\\)"
        -exec "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fix_cpp_header.sh" "{}"
              "${CMAKE_CURRENT_BINARY_DIR}/FILE_HEADER.cpp" "\\;")

FILE(GLOB_RECURSE bibletime_BACKEND_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.h"
)

FILE(GLOB_RECURSE bibletime_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.h"
)

SET(bibletime_DESKTOP_TRANSLATION_FILE_PREFIX "bibletime_ui_")

SET(bibletime_LINK_DESKTOP_TRANSLATION_FILES FALSE)

SET(bibletime_DESKTOP_TRANSLATION_RESOURCE_FILE "")

SET(bibletime_RESOURCE_FILES
    ${bibletime_DESKTOP_RESOURCE_FILES})
SET(bibletime_QML_FILES
    ${bibletime_DESKTOP_QML_FILES})

SET(bibletime_TRANSLATION_FILE_PREFIX "${bibletime_DESKTOP_TRANSLATION_FILE_PREFIX}")
SET(bibletime_LINK_TRANSLATION_FILES "${bibletime_LINK_DESKTOP_TRANSLATION_FILES}")
SET(bibletime_TRANSLATION_RESOURCE_FILE "${bibletime_DESKTOP_TRANSLATION_RESOURCE_FILE}")

QT5_ADD_RESOURCES(bibletime_RESOURCE_SOURCES ${bibletime_RESOURCE_FILES})

SET(bibletime_SOURCES
    ${bibletime_SOURCES}
    ${bibletime_RESOURCE_SOURCES}
    ${bibletime_QML_FILES}
    ${bibletime_UI_translations}
)

# For the Windows Application Icon
IF(MSVC)
  LIST(APPEND bibletime_SOURCES "cmake/BTWinIcon.rc")
ENDIF()
