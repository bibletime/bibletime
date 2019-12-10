#handbook (install images from en/ to all languages)

# For grouping handbooks in Visual Studio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

FILE(GLOB INSTALL_HANDBOOK_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/*.png")
FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
    FILE(GLOB INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/*.html")
    INSTALL(FILES ${INSTALL_HANDBOOK_HTML_FILES_${HANDBOOK_LOCALE_LANG}}
        DESTINATION "${BT_SHARE_PATH}/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
    )
    INSTALL(FILES ${INSTALL_HANDBOOK_IMAGES}
        DESTINATION "${BT_SHARE_PATH}/bibletime/docs/handbook/${HANDBOOK_LOCALE_LANG}/"
    )
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")

#howto (does not have images)
FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
    FILE(GLOB INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG} "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/*.html")
    INSTALL(FILES ${INSTALL_HOWTO_HTML_FILES_${HOWTO_LOCALE_LANG}}
        DESTINATION "${BT_SHARE_PATH}/bibletime/docs/howto/${HOWTO_LOCALE_LANG}/"
    )
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")


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
    COMMAND po4a -v --no-backups -k 0 cmake/docs/handbook_po4a.conf
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
SET_TARGET_PROPERTIES("handbook_translations" PROPERTIES FOLDER "Documentation")

FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
    ADD_CUSTOM_TARGET("handbook_${HANDBOOK_LOCALE_LANG}"
        COMMAND xsltproc --stringparam l10n.gentext.default.language ${HANDBOOK_LOCALE_LANG} ${BT_DOCBOOK_XSL} ../docbook/index.docbook
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/${HANDBOOK_LOCALE_LANG}/html/")
    ADD_DEPENDENCIES("handbook_${HANDBOOK_LOCALE_LANG}" "handbook_translations")
    ADD_DEPENDENCIES("handbook" "handbook_${HANDBOOK_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("handbook_${HANDBOOK_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")

ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS})

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

FOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS} "en")
    ADD_CUSTOM_TARGET("handbook_pdf_${HANDBOOK_LOCALE_LANG}"
       COMMENT "Generating PDF handbook for ${HANDBOOK_LOCALE_LANG}"

       COMMAND xsltproc -o tmp.fo  ${BT_DOCBOOK_PDF_XSL}   ../../${HANDBOOK_LOCALE_LANG}/docbook/index.docbook
       COMMAND fop -pdf ../../${HANDBOOK_LOCALE_LANG}/pdf/handbook.pdf -fo tmp.fo 
       COMMAND rm tmp.fo

       WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/handbook/en/html/")
    ADD_DEPENDENCIES("handbook_pdf" "handbook_pdf_${HANDBOOK_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("handbook_pdf_${HANDBOOK_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")
ENDFOREACH(HANDBOOK_LOCALE_LANG ${HANDBOOK_LOCALE_LANGS})

# Update howto
ADD_CUSTOM_TARGET("howto")
SET_TARGET_PROPERTIES("howto" PROPERTIES FOLDER "Documentation")
ADD_CUSTOM_TARGET("howto_translations"
    COMMAND po4a -v --no-backups -k 0 cmake/docs/howto_po4a.conf
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
SET_TARGET_PROPERTIES("howto_translations" PROPERTIES FOLDER "Documentation")
ADD_DEPENDENCIES("howto" "howto_translations")


FOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS} "en")
    ADD_CUSTOM_TARGET("howto_${HOWTO_LOCALE_LANG}"
        COMMAND xsltproc --stringparam l10n.gentext.default.language ${HOWTO_LOCALE_LANG} ${BT_DOCBOOK_XSL} "../docbook/index.docbook"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/howto/${HOWTO_LOCALE_LANG}/html/")
    ADD_DEPENDENCIES("howto_${HOWTO_LOCALE_LANG}" "howto_translations")
    ADD_DEPENDENCIES("howto" "howto_${HOWTO_LOCALE_LANG}")
    SET_TARGET_PROPERTIES("howto_${HOWTO_LOCALE_LANG}" PROPERTIES FOLDER "Documentation")
ENDFOREACH(HOWTO_LOCALE_LANG ${HOWTO_LOCALE_LANGS})
