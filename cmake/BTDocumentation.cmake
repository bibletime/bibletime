SET(GENERATE_HANDBOOK_HTML "ON" CACHE BOOL
    "Whether to generate and install the handbook in HTML format")
SET(GENERATE_HANDBOOK_HTML_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to generate and \
install the handbook in HTML format if GENERATE_HANDBOOK_HTML is enabled. \
Leave empty use all supported languages.")

SET(GENERATE_HANDBOOK_PDF "ON" CACHE BOOL
    "Whether to generate and install the handbook in PDF")
SET(GENERATE_HANDBOOK_PDF_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to generate and \
install the handbook in PDF format if GENERATE_HANDBOOK_PDF is enabled. \
Leave empty use all supported languages.")

SET(GENERATE_HOWTO_HTML "ON" CACHE BOOL
    "Whether to generate and install the howto in HTML format")
SET(GENERATE_HOWTO_HTML_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to generate and \
install the howto in HTML format if GENERATE_HOWTO_HTML is enabled. \
Leave empty use all supported languages.")

SET(GENERATE_HOWTO_PDF "ON" CACHE BOOL
    "Whether to generate and install the howto in PDF format")
SET(GENERATE_HOWTO_PDF_LANGUAGES "" CACHE STRING
    "A semicolon-separated list of language codes for which to generate and \
install the howto in PDF format if GENERATE_HOWTO_PDF is enabled. \
Leave empty use all supported languages.")


######################################################
# Some helper variables:
#
MACRO(ToggleOnOff out)
    IF(${ARGN})
        SET("${out}" ON)
    ELSE()
        SET("${out}" OFF)
    ENDIF()
ENDMACRO()
ToggleOnOff("GENERATE_HTML" GENERATE_HANDBOOK_HTML OR GENERATE_HOWTO_HTML)
ToggleOnOff("GENERATE_PDF" GENERATE_HANDBOOK_PDF OR GENERATE_HOWTO_PDF)
ToggleOnOff("GENERATE_HANDBOOK" GENERATE_HANDBOOK_HTML OR GENERATE_HANDBOOK_PDF)
ToggleOnOff("GENERATE_HOWTO" GENERATE_HOWTO_HTML OR GENERATE_HOWTO_PDF)
ToggleOnOff("GENERATE_DOCS" GENERATE_HTML OR GENERATE_PDF)
SET(DOCS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")


######################################################
# Always install the license file:
#
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/license.html"
        DESTINATION "${BT_DOCDIR}/license/")


######################################################
# Return if nothing to do:
#
IF(NOT GENERATE_DOCS)
    RETURN()
ENDIF()


######################################################
# Find dependencies
#
MACRO(FIND_REQUIRED_PROGRAM var name)
    FIND_PROGRAM("${var}" "${name}" ${ARGN})
    IF(NOT ${var})
        MESSAGE(FATAL_ERROR "Required program \"${name}\" not found!")
    ELSE()
        MESSAGE(STATUS "Found ${name}: ${${var}}")
    ENDIF()
ENDMACRO()
FIND_REQUIRED_PROGRAM(PO4A_COMMAND po4a)
FIND_REQUIRED_PROGRAM(XSLTPROC_COMMAND xsltproc)
IF(GENERATE_PDF)
    FIND_REQUIRED_PROGRAM(FOP_COMMAND fop)
ENDIF()


######################################################
# Scan for handbook and howto translations:
#
FUNCTION(NewUniqueSortedList out)
    SET(r tmp)
    LIST(REMOVE_ITEM r tmp)
    LIST(APPEND r ${ARGN})
    LIST(SORT r)
    LIST(REMOVE_DUPLICATES r)
    SET("${out}" "${r}" PARENT_SCOPE)
ENDFUNCTION()
FUNCTION(GetDocLangs type out)
    NewUniqueSortedList(outList)
    SET(l "[abcdefghijklmnopqrstuvwxyz]")
    SET(u "[ABCDEFGHIJKLMNOPQRSTUVWXYZ]")
    SET(regex "^${l}${l}(_${u}${u}${u}?)?$")
    FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/i18n/${type}/${type}-*.po")
    FOREACH(file IN LISTS files)
        GET_FILENAME_COMPONENT(basename "${file}" NAME_WE)
        STRING(REGEX REPLACE "^${type}-" "" basename "${basename}")
        IF(("${basename}" MATCHES "${regex}")
            AND NOT (IS_DIRECTORY "${file}"))
            LIST(APPEND outList "${basename}")
        ENDIF()
    ENDFOREACH()
    NewUniqueSortedList(outList "en" ${outList})
    SET("${out}" "${outList}" PARENT_SCOPE)
ENDFUNCTION()
FUNCTION(CheckMissingTranslations doc type)
    STRING(TOUPPER "${doc}" udoc)
    IF(GENERATE_${udoc}_${type})
        SET(in "GENERATE_${udoc}_${type}_LANGUAGES")
        SET(available ${AVAILABLE_${udoc}_LANGUAGES})
        SET(out "DO_${in}")
        SET(in "${${in}}")
        IF("${in}" STREQUAL "")
            SET("${out}" "${available}" PARENT_SCOPE)
            STRING(REPLACE ";" " " available "${available}")
            MESSAGE(STATUS
               "Languages for ${doc} (${type}): ${available}")
        ELSE()
            LIST(SORT in)
            FOREACH(lang IN LISTS in)
                IF(NOT ("${lang}" IN_LIST available))
                    MESSAGE(FATAL_ERROR
                            "Language for ${doc} (${type}) not found: ${lang}")
                ENDIF()
                LIST(REMOVE_ITEM available "${lang}")
            ENDFOREACH()
            SET("${out}" "${in}" PARENT_SCOPE)
            STRING(REPLACE ";" " " in "${in}")
            IF(NOT ("${available}" STREQUAL ""))
                STRING(REPLACE ";" " -" available ";${available}")
            ENDIF()
            MESSAGE(STATUS
               "Languages for ${doc} (${type}): ${in}${available}")
        ENDIF()
    ENDIF()
ENDFUNCTION()
IF(GENERATE_HANDBOOK)
    GetDocLangs("handbook" AVAILABLE_HANDBOOK_LANGUAGES)
    CheckMissingTranslations("handbook" "HTML")
    CheckMissingTranslations("handbook" "PDF")
    NewUniqueSortedList(GENERATE_HANDBOOK_LANGUAGES
        ${DO_GENERATE_HANDBOOK_HTML_LANGUAGES}
        ${DO_GENERATE_HANDBOOK_PDF_LANGUAGES})
ENDIF()
IF(GENERATE_HOWTO)
    GetDocLangs("howto" AVAILABLE_HOWTO_LANGUAGES)
    CheckMissingTranslations("howto" "HTML")
    CheckMissingTranslations("howto" "PDF")
    NewUniqueSortedList(GENERATE_HOWTO_LANGUAGES
        ${DO_GENERATE_HOWTO_HTML_LANGUAGES}
        ${DO_GENERATE_HOWTO_PDF_LANGUAGES})
ENDIF()
NewUniqueSortedList(ALL_DOC_LANGUAGES
    ${GENERATE_HANDBOOK_LANGUAGES}
    ${GENERATE_HOWTO_LANGUAGES})


######################################################
# Generate po4a configurations and targets for handbook and howto:
#
FUNCTION(GENERATE_PO4A_CONF_AND_TARGET doc)
    STRING(TOUPPER "${doc}" udoc)
    IF(GENERATE_${udoc})
        SET("${udoc}_PO4A_LANGUAGES" "${GENERATE_${udoc}_LANGUAGES}")
        LIST(REMOVE_ITEM "${udoc}_PO4A_LANGUAGES" "en")
        FOREACH(l IN LISTS ${udoc}_PO4A_LANGUAGES)
            FILE(MAKE_DIRECTORY "${DOCS_BINARY_DIR}/${doc}/${l}")
        ENDFOREACH()
        STRING(REPLACE ";" " " "${udoc}_PO4A_LANGUAGES"
            "${${udoc}_PO4A_LANGUAGES}")
        FILE(GLOB srcFiles
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/docbook/*.docbook")
        FILE(WRITE "${DOCS_BINARY_DIR}/${doc}/po4a.conf"
            "[po4a_langs] ${${udoc}_PO4A_LANGUAGES}
[po4a_paths] ${CMAKE_CURRENT_SOURCE_DIR}/i18n/${doc}/${doc}.pot \
$lang:${CMAKE_CURRENT_SOURCE_DIR}/i18n/${doc}/${doc}-$lang.po\n")
        FOREACH(srcFile IN LISTS srcFiles)
            GET_FILENAME_COMPONENT(srcFileName "${srcFile}" NAME)
            FILE(APPEND "${DOCS_BINARY_DIR}/${doc}/po4a.conf"
                "[type: docbook] ${srcFile} \
\$lang:${DOCS_BINARY_DIR}/${doc}/\$lang/docbook/${srcFileName}\n")
        ENDFOREACH()
        ADD_CUSTOM_TARGET("${doc}_translations"
            COMMAND "${PO4A_COMMAND}" --verbose --no-backups --keep 0
                "${DOCS_BINARY_DIR}/${doc}/po4a.conf")
        SET_TARGET_PROPERTIES("${doc}_translations"
            PROPERTIES FOLDER "Documentation")
    ENDIF()
ENDFUNCTION()
GENERATE_PO4A_CONF_AND_TARGET("handbook")
GENERATE_PO4A_CONF_AND_TARGET("howto")


######################################################
# General documentation targets generation:
#
ADD_CUSTOM_TARGET("docs" ALL)
SET_TARGET_PROPERTIES("docs" PROPERTIES FOLDER "Documentation")
FUNCTION(AddDocTypeTargets doc)
    IF(NOT TARGET "${doc}")
        ADD_CUSTOM_TARGET("${doc}")
        SET_TARGET_PROPERTIES("${doc}" PROPERTIES FOLDER "Documentation")
        ADD_DEPENDENCIES("docs" "${doc}")
    ENDIF()
    ADD_DEPENDENCIES("${doc}" ${ARGN})
ENDFUNCTION()


######################################################
# Rules for HTML:
#
IF(GENERATE_HTML)
    # Generate XSL files for HTML:
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
            MESSAGE(WARNING "The required file html/chunk.xsl from docbook-xsl \
    was not found on the system! Please specify its full path with \
    -DBT_DOCBOOK_XSL_HTML_CHUNK_XSL=path/to/html/chunk.xsl")
        ELSE()
            MESSAGE(STATUS "Found XSL for HTML generation: \
${BT_DOCBOOK_XSL_HTML_CHUNK_XSL}")
        ENDIF()
    ENDIF()
    SET(BT_DOCBOOK_XSL "${DOCS_BINARY_DIR}/docs.xsl")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/docs.xsl.in"
                   "${BT_DOCBOOK_XSL}" @ONLY)

    FUNCTION(GenerateHtmlDoc doc)
        STRING(TOUPPER "${doc}" udoc)
        IF(${GENERATE_${udoc}_HTML})
            ADD_CUSTOM_TARGET("${doc}_html")
            SET_TARGET_PROPERTIES("${doc}_html"
                PROPERTIES FOLDER "Documentation")
            AddDocTypeTargets("${doc}" "${doc}_html")
            FILE(GLOB IMAGE_FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/images/*.png")
            FOREACH(l IN LISTS "DO_GENERATE_${udoc}_HTML_LANGUAGES")
                SET(d "${DOCS_BINARY_DIR}/${doc}/${l}")
                IF("${l}" STREQUAL "en")
                    SET(i "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}")
                ELSE()
                    SET(i "${d}")
                ENDIF()
                FILE(MAKE_DIRECTORY "${d}/html")
                ADD_CUSTOM_TARGET("${doc}_html_${l}"
                    COMMAND "${XSLTPROC_COMMAND}" --nonet --output "${d}/html/"
                        --stringparam l10n.gentext.default.language "${l}"
                        "${BT_DOCBOOK_XSL}" "${i}/docbook/index.docbook"
                    DEPENDS "${doc}_translations")
                SET_TARGET_PROPERTIES("${doc}_html_${l}"
                    PROPERTIES FOLDER "Documentation")
                ADD_DEPENDENCIES("${doc}_html" "${doc}_html_${l}")
                INSTALL(DIRECTORY "${d}/html"
                        DESTINATION "${BT_DOCDIR}/${doc}/${l}/")
                INSTALL(FILES ${HTML_FILES} ${IMAGE_FILES}
                        DESTINATION "${BT_DOCDIR}/${doc}/${l}/html/")
            ENDFOREACH()
        ENDIF()
    ENDFUNCTION()
    GenerateHtmlDoc("handbook")
    GenerateHtmlDoc("howto")
ENDIF()

######################################################
# Rules for PDF:
#
IF(GENERATE_PDF)
    # Generate XSL files for PDF:
    IF(NOT DEFINED BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
        FIND_FILE(BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL
            NAMES "docbook.xsl"
            HINTS
                "/usr/share/xml/docbook/stylesheet/docbook-xsl/fo/"
                "/usr/share/sgml/docbook/xsl-stylesheets/fo/"
                "/usr/local/share/xsl/docbook/fo/"
            NO_DEFAULT_PATH)
        IF(NOT BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
            MESSAGE(WARNING "The required file fo/docbook.xsl from docbook-xsl \
was not found on the system! Please specify its full path with \
-DBT_DOCBOOK_XSL_PDF_DOCBOOK_XSL=path/to/html/chunk.xsl")
        ELSE()
            MESSAGE(STATUS "Found XSL for PDF generation: \
${BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL}")
        ENDIF()
    ENDIF()
    SET(BT_DOCBOOK_PDF_XSL "${DOCS_BINARY_DIR}/pdf.xsl")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/docs/pdf.xsl.in"
                   "${BT_DOCBOOK_PDF_XSL}" @ONLY)

    FUNCTION(GeneratePdfDoc doc)
        STRING(TOUPPER "${doc}" udoc)
        IF(${GENERATE_${udoc}_PDF})
            ADD_CUSTOM_TARGET("${doc}_pdf")
            SET_TARGET_PROPERTIES("${doc}_pdf"
                PROPERTIES FOLDER "Documentation")
            AddDocTypeTargets("${doc}" "${doc}_pdf")
            NewUniqueSortedList(images)
            SET(imageDir "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/images")
            IF(IS_DIRECTORY "${imageDir}")
                FILE(GLOB images "${imageDir}/*")
            ENDIF()
            FOREACH(l IN LISTS DO_GENERATE_${udoc}_PDF_LANGUAGES)
                SET(d "${DOCS_BINARY_DIR}/${doc}/${l}")
                FOREACH(image IN LISTS images)
                    GET_FILENAME_COMPONENT(imageName "${image}" NAME)
                    IF(CMAKE_VERSION VERSION_LESS 3.14)
                        FILE(COPY "${image}" "${d}/pdf/")
                    ELSE()
                        FILE(CREATE_LINK "${image}" "${d}/pdf/${imageName}"
                            COPY_ON_ERROR)
                    ENDIF()
                ENDFOREACH()
                IF("${l}" STREQUAL "en")
                    SET(i "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}")
                ELSE()
                    SET(i "${d}")
                ENDIF()
                FILE(MAKE_DIRECTORY "${d}/pdf")
                ADD_CUSTOM_COMMAND(OUTPUT "${d}/pdf/tmp.fo"
                    COMMENT "Generating xsl:fo file for ${doc} (PDF) generation"
                    COMMAND "${XSLTPROC_COMMAND}" --nonet
                        --output "${d}/pdf/tmp.fo"
                        "${BT_DOCBOOK_PDF_XSL}" "${i}/docbook/index.docbook"
                    DEPENDS "${doc}_translations")
                ADD_CUSTOM_COMMAND(OUTPUT "${d}/pdf/${doc}.pdf"
                    COMMENT "Generating ${doc} (PDF) for ${l}"
                    COMMAND "${FOP_COMMAND}" -pdf "${d}/pdf/${doc}.pdf"
                        -fo "${d}/pdf/tmp.fo"
                    DEPENDS "${d}/pdf/tmp.fo")
                ADD_CUSTOM_TARGET("${doc}_pdf_${l}"
                    DEPENDS "${d}/pdf/${doc}.pdf")
                INSTALL(FILES "${d}/pdf/${doc}.pdf"
                        DESTINATION "${BT_DOCDIR}/${doc}/${l}/pdf/")
                SET_TARGET_PROPERTIES("${doc}_pdf_${l}"
                    PROPERTIES FOLDER "Documentation")
                ADD_DEPENDENCIES("${doc}_pdf" "${doc}_pdf_${l}")
            ENDFOREACH()
        ENDIF()
    ENDFUNCTION()
    GeneratePdfDoc("handbook")
    GeneratePdfDoc("howto")
ENDIF()
