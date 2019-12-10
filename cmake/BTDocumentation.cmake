######################################################
# Install license file:
#
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/license.html"
        DESTINATION "${BT_DOCDIR}/bibletime/license/")


######################################################
# Scan for handbook and howto translations:
#
FUNCTION(GetDocLangs parentDir out)
    SET(outList tmp)
    LIST(REMOVE_ITEM outList tmp)
    SET(l "[abcdefghijklmnopqrstuvwxyz]")
    SET(u "[ABCDEFGHIJKLMNOPQRSTUVWXYZ]")
    SET(regex "^${l}${l}(_${u}${u}${u}?)?$")
    FILE(GLOB files "${parentDir}/*")
    FOREACH(file IN LISTS files)
        STRING(REGEX REPLACE "^.*/" "" basename "${file}")
        IF(("${basename}" MATCHES "${regex}") AND (IS_DIRECTORY "${file}"))
            LIST(APPEND outList "${basename}")
        ENDIF()
    ENDFOREACH()
    SET("${out}" "${outList}" PARENT_SCOPE)
ENDFUNCTION()
GetDocLangs("${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook" HANDBOOK_LOCALE_LANGS)
GetDocLangs("${CMAKE_CURRENT_SOURCE_DIR}/docs/howto" HOWTO_LOCALE_LANGS)


######################################################
# Generate po4a configurations for handbook and howto:
#
SET(HANDBOOK_PO4A_LANGS "${HANDBOOK_LOCALE_LANGS}")
SET(HOWTO_PO4A_LANGS "${HOWTO_LOCALE_LANGS}")
LIST(REMOVE_ITEM HANDBOOK_PO4A_LANGS "en")
LIST(REMOVE_ITEM HOWTO_PO4A_LANGS "en")
STRING(REPLACE ";" " " HANDBOOK_PO4A_LANGS "${HANDBOOK_PO4A_LANGS}")
STRING(REPLACE ";" " " HOWTO_PO4A_LANGS "${HOWTO_PO4A_LANGS}")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/handbook_po4a.conf.in"
               "${CMAKE_CURRENT_BINARY_DIR}/handbook_po4a.conf" @ONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/howto_po4a.conf.in"
              "${CMAKE_CURRENT_BINARY_DIR}/howto_po4a.conf" @ONLY)


######################################################
# Install handbook images and HTML files:
#
# Images for all translations come from the en translation.
FILE(GLOB HANDBOOK_IMAGES
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/*.png")
FOREACH(l IN LISTS HANDBOOK_LOCALE_LANGS)
    FILE(GLOB HANDBOOK_HTML_FILES_${l}
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${l}/html/*.html")
    INSTALL(FILES ${HANDBOOK_HTML_FILES_${l}}
            DESTINATION "${BT_DOCDIR}/handbook/${l}/")
    INSTALL(FILES ${HANDBOOK_IMAGES} DESTINATION "${BT_DOCDIR}/handbook/${l}/")
ENDFOREACH()

######################################################
# Install howto HTML files:
#
FOREACH(l IN LISTS HOWTO_LOCALE_LANGS)
    FILE(GLOB HOWTO_HTML_FILES_${l}
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${l}/html/*.html")
    INSTALL(FILES ${HOWTO_HTML_FILES_${l}}
            DESTINATION "${BT_DOCDIR}/howto/${l}/")
ENDFOREACH()


IF(NOT DEFINED BT_DOCBOOK_XSL_HTML_CHUNK_XSL)
    FIND_FILE(BT_DOCBOOK_XSL_HTML_CHUNK_XSL
        NAMES "chunk.xsl"
        HINTS
            "/usr/share/xml/docbook/stylesheet/docbook-xsl/html/"
            "/usr/share/sgml/docbook/xsl-stylesheets/html/"
            "/usr/share/xsl/docbook/html/"
            "/usr/share/xml/docbook/stylesheet/nwalsh/html/"
        NO_DEFAULT_PATH)
    IF(NOT BT_DOCBOOK_XSL_HTML_CHUNK_XSL)
        MESSAGE(WARNING "The required file html/chunk.xsl from docbook-xsl was \
not found on the system! Please specify its full path with \
-DBT_DOCBOOK_XSL_HTML_CHUNK_XSL=path/to/html/chunk.xsl")
    ENDIF()
ENDIF()
SET(BT_DOCBOOK_XSL "${CMAKE_CURRENT_BINARY_DIR}/docs.xsl")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl.in"
               "${BT_DOCBOOK_XSL}" @ONLY)

# Update handbook
ADD_CUSTOM_TARGET("handbook")
SET_TARGET_PROPERTIES("handbook" PROPERTIES FOLDER "Documentation")

ADD_CUSTOM_TARGET("handbook_translations"
    COMMAND po4a -v --no-backups -k 0 "${CMAKE_CURRENT_BINARY_DIR}/handbook_po4a.conf"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
SET_TARGET_PROPERTIES("handbook_translations" PROPERTIES FOLDER "Documentation")

FOREACH(HANDBOOK_LOCALE_LANG IN LISTS HANDBOOK_LOCALE_LANGS)
    ADD_CUSTOM_TARGET("handbook_${HANDBOOK_LOCALE_LANG}"
        COMMAND xsltproc --stringparam l10n.gentext.default.language ${HANDBOOK_LOCALE_LANG} ${BT_DOCBOOK_XSL} ../docbook/index.docbook
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/")
    ADD_DEPENDENCIES("handbook_${HANDBOOK_LOCALE_LANG}" "handbook_translations")
    ADD_DEPENDENCIES("handbook" "handbook_${HANDBOOK_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("handbook_${HANDBOOK_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")

ENDFOREACH()

IF(NOT DEFINED BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
    FIND_FILE(BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL
        NAMES "docbook.xsl"
        HINTS
            "/usr/share/xml/docbook/stylesheet/docbook-xsl/fo/"
            "/usr/share/sgml/docbook/xsl-stylesheets/fo/"
            "/usr/local/share/xsl/docbook/fo/"
        NO_DEFAULT_PATH)
    IF(NOT BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
        MESSAGE(WARNING "The required file fo/docbook.xsl from docbook-xsl was \
not found on the system! Please specify its full path with \
-DBT_DOCBOOK_XSL_PDF_DOCBOOK_XSL=path/to/html/chunk.xsl")
    ENDIF()
ENDIF()
SET(BT_DOCBOOK_PDF_XSL "${CMAKE_CURRENT_BINARY_DIR}/pdf.xsl")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/pdf.xsl.in"
               "${BT_DOCBOOK_PDF_XSL}" @ONLY)

# Update handbook pdf
ADD_CUSTOM_TARGET("handbook_pdf")
SET_TARGET_PROPERTIES("handbook_pdf" PROPERTIES FOLDER "Documentation")

FOREACH(HANDBOOK_LOCALE_LANG IN LISTS HANDBOOK_LOCALE_LANGS)
    ADD_CUSTOM_TARGET("handbook_pdf_${HANDBOOK_LOCALE_LANG}"
       COMMENT "Generating PDF handbook for ${HANDBOOK_LOCALE_LANG}"

       COMMAND "${CMAKE_COMMAND}" -E make_directory "../../${HANDBOOK_LOCALE_LANG}/pdf"
       COMMAND xsltproc -o tmp.fo  ${BT_DOCBOOK_PDF_XSL}   ../../${HANDBOOK_LOCALE_LANG}/docbook/index.docbook
       COMMAND fop -pdf ../../${HANDBOOK_LOCALE_LANG}/pdf/handbook.pdf -fo tmp.fo
       COMMAND rm tmp.fo

       WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/")
    ADD_DEPENDENCIES("handbook_pdf" "handbook_pdf_${HANDBOOK_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("handbook_pdf_${HANDBOOK_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")
ENDFOREACH()

# Update howto
ADD_CUSTOM_TARGET("howto")
SET_TARGET_PROPERTIES("howto" PROPERTIES FOLDER "Documentation")
ADD_CUSTOM_TARGET("howto_translations"
    COMMAND po4a -v --no-backups -k 0 "${CMAKE_CURRENT_BINARY_DIR}/howto_po4a.conf"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
SET_TARGET_PROPERTIES("howto_translations" PROPERTIES FOLDER "Documentation")
ADD_DEPENDENCIES("howto" "howto_translations")


FOREACH(HOWTO_LOCALE_LANG IN LISTS HOWTO_LOCALE_LANGS)
    ADD_CUSTOM_TARGET("howto_${HOWTO_LOCALE_LANG}"
        COMMAND xsltproc --stringparam l10n.gentext.default.language ${HOWTO_LOCALE_LANG} ${BT_DOCBOOK_XSL} "../docbook/index.docbook"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/")
    ADD_DEPENDENCIES("howto_${HOWTO_LOCALE_LANG}" "howto_translations")
    ADD_DEPENDENCIES("howto" "howto_${HOWTO_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("howto_${HOWTO_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")
ENDFOREACH()
