
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTTranslationMacros.cmake")

SET(TS_PREFIX "bibletime_ui_")
SET(TS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages)

BT_GET_TRANSLATION_FILELISTS(${TS_DIR} ${TS_PREFIX} TS_FILES QM_FILES)

BT_CREATE_QM_FILES("${TS_FILES}" "${QM_FILES}")

BT_INSTALL_QM_FILES("${QM_FILES}")

SET(bibletime_UI_translations "${QM_FILES}")


# Update source catalog files (this is the basis for the translator's work)
# Invoke this with "make messages"
BT_UPDATE_TS_FILES()
