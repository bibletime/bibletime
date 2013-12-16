SET(bibletime_UI_translations)

IF (Qt5Core_FOUND)
    SET(QT_LUPDATE_EXECUTABLE  ${Qt5_LUPDATE_EXECUTABLE})
    SET(QT_LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
ELSE (Qt5Core_FOUND)
    FIND_PROGRAM(QT_LUPDATE_EXECUTABLE
        NAMES lupdate-qt4 lupdate
        PATHS ${QT_BINARY_DIR}
        NO_DEFAULT_PATH
    )

    FIND_PROGRAM(QT_LRELEASE_EXECUTABLE
        NAMES lrelease-qt4 lrelease
        PATHS ${QT_BINARY_DIR}
        NO_DEFAULT_PATH
    )
ENDIF (Qt5Core_FOUND)

SET(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages")
FILE(GLOB TS_FILES "${TS_DIR}/bibletime_ui_*.ts")
FOREACH(TSFILE_FULLPATH ${TS_FILES})
    STRING(REGEX REPLACE "${TS_DIR}/bibletime_ui_(..(_..)?).ts" "\\1"
           TS_LANG "${TSFILE_FULLPATH}")
    SET(TS_LANGS ${TS_LANGS} ${TS_LANG})
    SET(QMFILE "bibletime_ui_${TS_LANG}.qm")
    ADD_CUSTOM_COMMAND(
        TARGET "bibletime"
        POST_BUILD
        COMMAND ${QT_LRELEASE_EXECUTABLE} "${TSFILE_FULLPATH}" -qm "${QMFILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Updating UI translation for ${TS_LANG}"
        VERBATIM
    )
    SET(bibletime_UI_translations
        ${bibletime_UI_translations}
        "${QMFILE}"
    )
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${QMFILE}"
        DESTINATION "${BT_SHARE_PATH}/bibletime/locale/"
    )
ENDFOREACH(TSFILE_FULLPATH)


# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
ADD_CUSTOM_TARGET("messages")
FOREACH(TS_LANG ${TS_LANGS})
    ADD_CUSTOM_TARGET("messages_${TS_LANG}"
        COMMAND ${QT_LUPDATE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${TS_DIR}/bibletime_ui_${TS_LANG}.ts")
    ADD_DEPENDENCIES("messages" "messages_${TS_LANG}")
ENDFOREACH(TS_LANG)


# Template file for translators
ADD_CUSTOM_TARGET("messages_default"
    COMMAND ${QT_LUPDATE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${TS_DIR}/bibletime_ui.ts")
ADD_DEPENDENCIES(messages "messages_default")
