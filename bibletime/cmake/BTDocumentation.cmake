######################################################
# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
#

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

ADD_CUSTOM_TARGET("messages")
FOREACH(MESSAGE_LOCALE_LANG ${MESSAGE_LOCALE_LANGS})
	ADD_CUSTOM_TARGET("messages_${MESSAGE_LOCALE_LANG}"
		COMMAND ${QT_LUPDATE_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui_${MESSAGE_LOCALE_LANG}.ts")
	ADD_CUSTOM_TARGET("compile_messages_${MESSAGE_LOCALE_LANG}"
		COMMAND ${QT_LRELEASE_EXECUTABLE} "bibletime_ui_${MESSAGE_LOCALE_LANG}.ts" -qm "bibletime_ui_${MESSAGE_LOCALE_LANG}.qm"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/")
	ADD_DEPENDENCIES("compile_messages_${MESSAGE_LOCALE_LANG}" "messages_${MESSAGE_LOCALE_LANG}")
	ADD_DEPENDENCIES("messages" "compile_messages_${MESSAGE_LOCALE_LANG}")
ENDFOREACH(MESSAGE_LOCALE_LANG)
# Template file for translators
ADD_CUSTOM_TARGET("messages_default"
	COMMAND lupdate "${CMAKE_CURRENT_SOURCE_DIR}/src" -ts "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui.ts")
ADD_DEPENDENCIES(messages "messages_default")
######################################################

IF(CMAKE_SYSTEM MATCHES "BSD")
	SET(BT_DOCBOOK_XSL "${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs_freebsd.xsl")
ELSE(CMAKE_SYSTEM MATCHES "BSD")
	SET(BT_DOCBOOK_XSL "${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl")
ENDIF(CMAKE_SYSTEM MATCHES "BSD")

######################################################
# Update handbook
ADD_CUSTOM_TARGET("handbook")

ADD_CUSTOM_TARGET("handbook_translations"
	COMMAND po4a -v --no-backups -k 0 cmake/docs/handbook_po4a.conf
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
	ADD_CUSTOM_TARGET("handbook_${HANDBOOK_LOCALE_LANG}"
		COMMAND xsltproc --stringparam l10n.gentext.default.language ${HANDBOOK_LOCALE_LANG} ${BT_DOCBOOK_XSL} ../docbook/index.docbook
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/")
	ADD_DEPENDENCIES("handbook_${HANDBOOK_LOCALE_LANG}" "handbook_translations")
	ADD_DEPENDENCIES("handbook" "handbook_${HANDBOOK_LOCALE_LANG}")
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS})
######################################################

######################################################
# Update howto
ADD_CUSTOM_TARGET("howto")
ADD_CUSTOM_TARGET("howto_translations"
	COMMAND po4a -v --no-backups -k 0 cmake/docs/howto_po4a.conf
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
ADD_DEPENDENCIES("howto" "howto_translations")


FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
	ADD_CUSTOM_TARGET("howto_${HOWTO_LOCALE_LANG}"
		COMMAND xsltproc --stringparam l10n.gentext.default.language ${HOWTO_LOCALE_LANG} ${BT_DOCBOOK_XSL} "../docbook/index.docbook"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/")
	ADD_DEPENDENCIES("howto_${HOWTO_LOCALE_LANG}" "howto_translations")
	ADD_DEPENDENCIES("howto" "howto_${HOWTO_LOCALE_LANG}")
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS})

######################################################