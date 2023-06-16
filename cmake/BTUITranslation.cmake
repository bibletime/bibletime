SET(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages")
SET(TS_PREFIX "bibletime_ui_")
FILE(GLOB TS_FILES CONFIGURE_DEPENDS "${TS_DIR}/${TS_PREFIX}*.ts")

IF(LinguistTools_FOUND)
    GET_TARGET_PROPERTY(QT_LUPDATE_EXECUTABLE Qt::lupdate IMPORTED_LOCATION)
    GET_TARGET_PROPERTY(QT_LRELEASE_EXECUTABLE Qt::lrelease IMPORTED_LOCATION)
ELSE()
    FIND_PROGRAM(QT_LUPDATE_EXECUTABLE lupdate
        PATHS
            /usr/bin/
            /usr/lib/qt5/bin/
            /usr/lib64/qt5/bin/
            /usr/lib/x86_64-linux-gnu/qt5/bin/
    )
    FIND_PROGRAM(QT_LRELEASE_EXECUTABLE lrelease
        PATHS
            /usr/bin/
            /usr/lib/qt5/bin/
            /usr/lib64/qt5/bin/
            /usr/lib/x86_64-linux-gnu/qt5/bin/
    )
ENDIF()

# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
ADD_CUSTOM_TARGET("messages")
SET_TARGET_PROPERTIES("messages" PROPERTIES FOLDER "Messages")

# Template file for translators:
ADD_CUSTOM_TARGET("messages_default" COMMAND
    "${QT_LUPDATE_EXECUTABLE}" "-no-obsolete" "-extensions" "cpp,h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src" "-ts" "${TS_DIR}/bibletime_ui.ts")
SET_TARGET_PROPERTIES("messages_default" PROPERTIES FOLDER "Messages")
ADD_DEPENDENCIES("messages" "messages_default")

MESSAGE(STATUS "Creating empty translation files for default en_US language...")
SET(TS_FILE "${CMAKE_CURRENT_BINARY_DIR}/bibletime_ui_en_US.ts")
SET(TS_QM_FILE "${CMAKE_CURRENT_BINARY_DIR}/bibletime_ui_en_US.qm")
FILE(WRITE "${TS_FILE}" [=[
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US"></TS>
]=])
EXECUTE_PROCESS(
    COMMAND "${QT_LRELEASE_EXECUTABLE}" "${TS_FILE}" "-qm" "${TS_QM_FILE}"
    RESULT_VARIABLE r
)
IF(NOT ("${r}" EQUAL 0))
    MESSAGE(FATAL_ERROR "Failed to run ${QT_LRELEASE_EXECUTABLE}!")
ENDIF()
INSTALL(FILES "${TS_QM_FILE}" DESTINATION "${BT_LOCALEDIR}/")

FOREACH(TS_FILE IN LISTS TS_FILES)
    STRING(REGEX REPLACE "${TS_DIR}/${TS_PREFIX}(...?(_..)?).ts" "\\1"
           TS_LANG "${TS_FILE}")
    SET(TS_QM_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TS_PREFIX}${TS_LANG}.qm")
    ADD_CUSTOM_COMMAND(TARGET "bibletime" POST_BUILD
        COMMAND
            "${QT_LRELEASE_EXECUTABLE}" "${TS_FILE}" "-qm" "${TS_QM_FILE}"
        COMMENT "Updating translation for ${TS_LANG}"
        VERBATIM
    )
    INSTALL(FILES "${TS_QM_FILE}" DESTINATION "${BT_LOCALEDIR}/")

    ADD_CUSTOM_TARGET("messages_${TS_LANG}" COMMAND
        "${QT_LUPDATE_EXECUTABLE}" "-no-obsolete" "-extensions" "cpp,h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${TS_FILE}")
    SET_TARGET_PROPERTIES("messages_${TS_LANG}" PROPERTIES
        FOLDER "Messages")
    ADD_DEPENDENCIES("messages" "messages_${TS_LANG}")
ENDFOREACH()
