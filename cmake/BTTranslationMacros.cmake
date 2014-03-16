# makes a list of language codes from the *.ts files
function(BT_GET_TS_LANGS TS_LANGS)
    SET(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages")
    FILE(GLOB TS_FILES "${TS_DIR}/bibletime_ui_*.ts")
    FOREACH(TSFILE_FULLPATH ${TS_FILES})
        STRING(REGEX REPLACE "${TS_DIR}/bibletime_ui_(..(_..)?).ts" "\\1"
               TS_LANG "${TSFILE_FULLPATH}")
        SET(_TS_LANGS ${_TS_LANGS} ${TS_LANG})
    ENDFOREACH()
    SET(${TS_LANGS} ${_TS_LANGS} PARENT_SCOPE)
endfunction()


#bt_get_translation_executables(QT_LUPDATE_EXECUTABLE QT_LRELEASE_EXECUTABLE)
function(BT_GET_TRANSLATION_EXECUTABLES lupdate lrelease rcc)
    IF (Qt5Core_FOUND)
        SET(QT_LUPDATE_EXECUTABLE  ${Qt5_LUPDATE_EXECUTABLE})
        SET(QT_LRELEASE_EXECUTABLE ${Qt5_LRELEASE_EXECUTABLE})
        SET(QT_RESOURCE_EXECUTABLE ${Qt5Core_RCC_EXECUTABLE})
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
    SET(${lupdate}  ${QT_LUPDATE_EXECUTABLE}  PARENT_SCOPE)
    SET(${lrelease} ${QT_LRELEASE_EXECUTABLE} PARENT_SCOPE)
    SET(${rcc} ${QT_RESOURCE_EXECUTABLE} PARENT_SCOPE)
endfunction()

# get a list of the ts files with full path and a list
# of the qm files with no path.
function(BT_GET_TRANSLATION_FILELISTS
    TS_DIR
    TS_PREFIX
    TS_FILES_FULLPATH
    QM_FILES
)
    FILE(GLOB TS_FILES "${TS_DIR}/${TS_PREFIX}*.ts")
    FOREACH(TS_FILE_FULLPATH ${TS_FILES})
        STRING(REGEX REPLACE "${TS_DIR}/${TS_PREFIX}(.+(_..)?).ts" "\\1"
               TS_LANG "${TS_FILE_FULLPATH}")
        SET(TS_LANGS ${TS_LANGS} ${TS_LANG})
        SET(QM_FILE "${TS_PREFIX}${TS_LANG}.qm")
        SET(_QM_FILES ${_QM_FILES} ${TS_PREFIX}${TS_LANG}.qm)
    ENDFOREACH()
    SET(${TS_FILES_FULLPATH} ${TS_FILES} PARENT_SCOPE)
    SET(${QM_FILES} ${_QM_FILES} PARENT_SCOPE)
endfunction()


# creates the qm files in the build directory from the ts files in the source directory
function(BT_CREATE_QM_FILES TS_FILES QM_FILES bibletime_LINK_TRANSLATION_FILES)
    BT_GET_TRANSLATION_EXECUTABLES( QT_LUPDATE_EXECUTABLE QT_LRELEASE_EXECUTABLE QT_RCC_EXECUTABLE)
    list(LENGTH QM_FILES QM_Length)
    math(EXPR stop "${QM_Length}-1")
    FOREACH(Index RANGE 0 ${stop})
        LIST(GET TS_FILES ${Index} TS_FILE)
        LIST(GET QM_FILES ${Index} QM_FILE)
        IF(${bibletime_LINK_TRANSLATION_FILES})
            ADD_CUSTOM_COMMAND(
                OUTPUT ${QM_FILE}
                POST_BUILD
                COMMAND ${QT_LRELEASE_EXECUTABLE} "${TS_FILE}" -qm "${QM_FILE}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Updating UI translation for ${QM_FILE}"
                VERBATIM
            )
        ELSE()
            ADD_CUSTOM_COMMAND(
                TARGET "bibletime"
                POST_BUILD
                COMMAND ${QT_LRELEASE_EXECUTABLE} "${TS_FILE}" -qm "${QM_FILE}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Updating UI translation for ${QM_FILE}"
                VERBATIM
            )
        ENDIF()
    ENDFOREACH()
endfunction()


# installs the qm files for bibletime.
function(BT_INSTALL_QM_FILES QM_FILES)
    list(LENGTH QM_FILES QM_Length)
    math(EXPR stop "${QM_Length}-1")
    FOREACH(Index RANGE 0 ${stop})
        LIST(GET QM_FILES ${Index} QM_FILE)
        INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${QM_FILE}"
            DESTINATION "${BT_SHARE_PATH}/bibletime/locale/"
        )
    ENDFOREACH()
endfunction()

function(BT_UPDATE_TS_FILES)
    # Update source catalog files (this is the basis for the translator's work)
    # Invoke this with "make messages"
    ADD_CUSTOM_TARGET("messages")
    BT_GET_TS_LANGS(TS_LANGS)
    FOREACH(TS_LANG ${TS_LANGS})
        ADD_CUSTOM_TARGET("messages_${TS_LANG}"
            COMMAND ${QT_LUPDATE_EXECUTABLE} -extensions cpp,h "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${TS_DIR}/${TS_PREFIX}${TS_LANG}.ts")
        ADD_DEPENDENCIES("messages" "messages_${TS_LANG}")
    ENDFOREACH(TS_LANG)

    # Template file for translators
    ADD_CUSTOM_TARGET("messages_default"
        COMMAND ${QT_LUPDATE_EXECUTABLE} -extensions cpp,h "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${TS_DIR}/bibletime_ui.ts")
    ADD_DEPENDENCIES(messages "messages_default")
endfunction()

    # copy translation qrc file to build dir
function(BT_COPY_QRC_TO_BUILD_DIR QRC_FILE QRC_COPIED_FILE)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${QRC_COPIED_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy ${QRC_FILE} ${QRC_COPIED_FILE}
        MAIN_DEPENDENCY ${QRC_FILE}
        COMMENT "Copying translation resource file: ${QRC_FILE}"
        VERBATIM
    )
endfunction()

    # run resource compiler on translation qrc file
function(BT_RUN_RESOURCE_COMPILER QM_FILES QRC_COPIED_FILE QRC_CPP_FILE)
    BT_GET_TRANSLATION_EXECUTABLES( QT_LUPDATE_EXECUTABLE QT_LRELEASE_EXECUTABLE QT_RCC_EXECUTABLE)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${QRC_CPP_FILE}
        COMMAND ${QT_RCC_EXECUTABLE}
        ARGS ${rcc_options} ${QRC_COPIED_FILE} -o ${QRC_CPP_FILE}
        MAIN_DEPENDENCY ${QM_COPIED_FILE}
        DEPENDS ${QM_FILES} ${QRC_COPIED_FILE}
        COMMENT "Compiling resource file: ${QRC_COPIED_FILE}"
    )
endfunction()
