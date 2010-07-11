SET(bibletime_UI_translations)

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

FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
    ADD_CUSTOM_COMMAND(OUTPUT "bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
        PRE_BUILD
        COMMAND ${QT_LRELEASE_EXECUTABLE} "bibletime_ui_${MESSAGE_LOCALE_LANG}.ts" -qm "${CMAKE_CURRENT_BINARY_DIR}/bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/"
        COMMENT "Updating UI translation for ${MESSAGE_LOCALE_LANG}"
        VERBATIM
    )
    SET(bibletime_UI_translations
        ${bibletime_UI_translations}
        "bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
    )
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
        DESTINATION "${BT_SHARE_PATH}share/bibletime/locale/"
    )
ENDFOREACH(MESSAGE_LOCALE_LANG)


# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
ADD_CUSTOM_TARGET("messages")
FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
    ADD_CUSTOM_TARGET("messages_${MESSAGE_LOCALE_LANG}"
        COMMAND ${QT_LUPDATE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui_${MESSAGE_LOCALE_LANG}.ts")
    ADD_DEPENDENCIES("messages" "messages_${MESSAGE_LOCALE_LANG}")
ENDFOREACH(MESSAGE_LOCALE_LANG)
# Template file for translators
ADD_CUSTOM_TARGET("messages_default"
    COMMAND ${QT_LUPDATE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui.ts")
ADD_DEPENDENCIES(messages "messages_default")
