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
ToggleOnOff("BUILD_HTML" BUILD_HANDBOOK_HTML OR BUILD_HOWTO_HTML)
ToggleOnOff("BUILD_PDF" BUILD_HANDBOOK_PDF OR BUILD_HOWTO_PDF)
ToggleOnOff("BUILD_HANDBOOK" BUILD_HANDBOOK_HTML OR BUILD_HANDBOOK_PDF)
ToggleOnOff("BUILD_HOWTO" BUILD_HOWTO_HTML OR BUILD_HOWTO_PDF)
ToggleOnOff("BUILD_DOCS" BUILD_HTML OR BUILD_PDF)
SET(DOCS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")

######################################################

IF(INSTALL_GENERATED_DOCS)
    INSTALL(DIRECTORY ${GENERATED_DOCS_DIR}/handbook DESTINATION ${BT_DOCDIR})
    INSTALL(DIRECTORY ${GENERATED_DOCS_DIR}/howto DESTINATION ${BT_DOCDIR})
ENDIF()


######################################################
# Return if nothing to do:
#
IF(NOT BUILD_DOCS)
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
IF(BUILD_PDF)
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
    FILE(GLOB files CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/i18n/${type}/${type}-*.po")
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
FUNCTION(GetDocArtifacts doc)
    STRING(TOUPPER "${doc}" udoc)
    NewUniqueSortedList(images)
    SET(imageDir "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/images")
    IF(IS_DIRECTORY "${imageDir}")
        FILE(GLOB images CONFIGURE_DEPENDS "${imageDir}/*")
    ENDIF()
    SET("${udoc}_IMAGES" "${images}" PARENT_SCOPE)
ENDFUNCTION()
FUNCTION(CheckMissingTranslations doc type)
    STRING(TOUPPER "${doc}" udoc)
    IF(BUILD_${udoc}_${type})
        SET(in "BUILD_${udoc}_${type}_LANGUAGES")
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
IF(BUILD_HANDBOOK)
    GetDocLangs("handbook" AVAILABLE_HANDBOOK_LANGUAGES)
    GetDocArtifacts("handbook")
    CheckMissingTranslations("handbook" "HTML")
    CheckMissingTranslations("handbook" "PDF")
    NewUniqueSortedList(BUILD_HANDBOOK_LANGUAGES
        ${DO_BUILD_HANDBOOK_HTML_LANGUAGES}
        ${DO_BUILD_HANDBOOK_PDF_LANGUAGES})
ENDIF()
IF(BUILD_HOWTO)
    GetDocLangs("howto" AVAILABLE_HOWTO_LANGUAGES)
    GetDocArtifacts("howto")
    CheckMissingTranslations("howto" "HTML")
    CheckMissingTranslations("howto" "PDF")
    NewUniqueSortedList(BUILD_HOWTO_LANGUAGES
        ${DO_BUILD_HOWTO_HTML_LANGUAGES}
        ${DO_BUILD_HOWTO_PDF_LANGUAGES})
ENDIF()
NewUniqueSortedList(ALL_DOC_LANGUAGES
    ${BUILD_HANDBOOK_LANGUAGES}
    ${BUILD_HOWTO_LANGUAGES})


######################################################
# General documentation targets generation:
#
FUNCTION(AddDocTarget name)
    ADD_CUSTOM_TARGET("${name}" DEPENDS ${ARGN})
    SET_TARGET_PROPERTIES("${name}" PROPERTIES FOLDER "Documentation")
ENDFUNCTION()
AddDocTarget("docs")
SET_TARGET_PROPERTIES("docs" PROPERTIES EXCLUDE_FROM_ALL FALSE)
FUNCTION(AddDocSubTarget parent name)
    IF(NOT TARGET "${name}")
        AddDocTarget("${name}" ${ARGN})
        ADD_DEPENDENCIES("${parent}" "${name}")
    ELSEIF(NOT ("${ARGN}" STREQUAL ""))
        ADD_DEPENDENCIES("${name}" ${ARGN})
    ENDIF()
ENDFUNCTION()


######################################################
# Generate po4a configurations and targets for handbook and howto:
#
FUNCTION(BUILD_PO4A_CONF_AND_TARGET doc)
    STRING(TOUPPER "${doc}" udoc)
    IF(BUILD_${udoc})
        AddDocTarget("${doc}_translations")
        SET(docbookInputDir "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/docbook")
        FILE(GLOB_RECURSE inputs CONFIGURE_DEPENDS "${docbookInputDir}/*")
        AddDocSubTarget("${doc}_translations" "${doc}_translation_en"
            "${inputs}")

        SET(generatedLanguages "${BUILD_${udoc}_LANGUAGES}")
        LIST(REMOVE_ITEM generatedLanguages "en")
        SET(po4aDirBase "${DOCS_BINARY_DIR}/${doc}/po4a")
        SET(docbookDirBase "${DOCS_BINARY_DIR}/${doc}/docbook")
        FOREACH(l IN LISTS generatedLanguages)
            FILE(MAKE_DIRECTORY "${po4aDirBase}/${l}")
            SET(docbookDir "${docbookDirBase}/${l}")
            FILE(MAKE_DIRECTORY "${docbookDir}")
            SET(confFile "${po4aDirBase}/${l}/po4a.conf")
            FILE(WRITE "${confFile}"
                "[po4a_langs] ${l}
[po4a_paths] ${CMAKE_CURRENT_SOURCE_DIR}/i18n/${doc}/${doc}.pot \
$lang:${CMAKE_CURRENT_SOURCE_DIR}/i18n/${doc}/${doc}-$lang.po\n")
            FOREACH(input IN LISTS inputs)
                FILE(RELATIVE_PATH relativeInput "${docbookInputDir}"
                    "${input}")
                FILE(APPEND "${confFile}"
                    "[type: docbook] ${docbookInputDir}/${relativeInput} \
\$lang:${docbookDirBase}/\$lang/${relativeInput}\n")
            ENDFOREACH()
            SET(stampFile "${docbookDir}/stamp")
            ADD_CUSTOM_COMMAND(OUTPUT "${stampFile}"
                COMMAND "${PO4A_COMMAND}" --verbose --no-backups --keep 0
                    --porefs none "${confFile}"
                COMMAND "${CMAKE_COMMAND}" -E touch "${stampFile}"
                DEPENDS "${confFile}" ${inputs})
            AddDocSubTarget("${doc}_translations" "${doc}_translation_${l}"
                "${stampFile}")
        ENDFOREACH()
    ENDIF()
ENDFUNCTION()
BUILD_PO4A_CONF_AND_TARGET("handbook")
BUILD_PO4A_CONF_AND_TARGET("howto")


######################################################
# Rules for HTML:
#
IF(BUILD_HTML)
    # Generate XSL files for HTML:
    IF(NOT DEFINED BT_DOCBOOK_XSL_HTML_CHUNK_XSL)
        FIND_FILE(BT_DOCBOOK_XSL_HTML_CHUNK_XSL
            NAMES "chunk.xsl"
            HINTS
                "/usr/share/xml/docbook/stylesheet/docbook-xsl/html/"
                "/usr/share/sgml/docbook/xsl-stylesheets/html/"
                "/usr/share/xsl/docbook/html/"
                "/usr/share/xml/docbook/stylesheet/nwalsh/html/"
                "/usr/local/share/xml/docbook/stylesheet/docbook-xsl/html/"
                "/usr/local/share/sgml/docbook/xsl-stylesheets/html/"
                "/usr/local/share/xsl/docbook/html/"
                "/usr/local/share/xml/docbook/stylesheet/nwalsh/html/"
            NO_DEFAULT_PATH)
        IF(NOT BT_DOCBOOK_XSL_HTML_CHUNK_XSL)
            MESSAGE(FATAL_ERROR "The required file html/chunk.xsl from \
docbook-xsl was not found on the system! Please specify its full path with \
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
        IF(${BUILD_${udoc}_HTML})
            AddDocSubTarget("docs" "${doc}")
            AddDocSubTarget("${doc}" "${doc}_html")
            FOREACH(l IN LISTS "DO_BUILD_${udoc}_HTML_LANGUAGES")
                SET(d "${DOCS_BINARY_DIR}/${doc}/html/${l}")
                FILE(MAKE_DIRECTORY "${d}")
                IF("${l}" STREQUAL "en")
                    SET(i "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/docbook")
                ELSE()
                    SET(i "${DOCS_BINARY_DIR}/${doc}/docbook/${l}")
                ENDIF()
                SET(stampFile "${d}/.stamp")
                ADD_CUSTOM_COMMAND(OUTPUT "${stampFile}"
                    COMMAND "${XSLTPROC_COMMAND}" --nonet --output "${d}/"
                        --stringparam l10n.gentext.default.language "${l}"
                        "${BT_DOCBOOK_XSL}" "${i}/index.docbook"
                    COMMAND "${CMAKE_COMMAND}" -E touch "${stampFile}"
                    DEPENDS "${doc}_translation_${l}")
                AddDocSubTarget("${doc}_html" "${doc}_html_${l}" "${stampFile}")
                INSTALL(DIRECTORY "${d}/"
                        DESTINATION "${BT_DOCDIR}/${doc}/html/${l}/"
                        PATTERN ".*" EXCLUDE)
                INSTALL(FILES ${${udoc}_IMAGES}
                        DESTINATION "${BT_DOCDIR}/${doc}/html/${l}/")
            ENDFOREACH()
        ENDIF()
    ENDFUNCTION()
    GenerateHtmlDoc("handbook")
    GenerateHtmlDoc("howto")
ENDIF()

######################################################
# Rules for PDF:
#
IF(BUILD_PDF)
    # Generate XSL files for PDF:
    IF(NOT DEFINED BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
        FIND_FILE(BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL
            NAMES "docbook.xsl"
            HINTS
                "/usr/share/xml/docbook/stylesheet/docbook-xsl/fo/"
                "/usr/share/sgml/docbook/xsl-stylesheets/fo/"
                "/usr/share/xsl/docbook/fo/"
                "/usr/local/share/xml/docbook/stylesheet/docbook-xsl/fo/"
                "/usr/local/share/sgml/docbook/xsl-stylesheets/fo/"
                "/usr/local/share/xsl/docbook/fo/"
            NO_DEFAULT_PATH)
        IF(NOT BT_DOCBOOK_XSL_PDF_DOCBOOK_XSL)
            MESSAGE(FATAL_ERROR "The required file fo/docbook.xsl from \
docbook-xsl was not found on the system! Please specify its full path with \
-DBT_DOCBOOK_XSL_PDF_DOCBOOK_XSL=path/to/fo/docbook.xsl")
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
        IF(${BUILD_${udoc}_PDF})
            AddDocSubTarget("docs" "${doc}")
            AddDocSubTarget("${doc}" "${doc}_xslfo")
            AddDocSubTarget("${doc}" "${doc}_pdf")
            FOREACH(l IN LISTS DO_BUILD_${udoc}_PDF_LANGUAGES)
                SET(dx "${DOCS_BINARY_DIR}/${doc}/xslfo/${l}")
                FILE(MAKE_DIRECTORY "${dx}")
                IF("${l}" STREQUAL "en")
                    SET(i "${CMAKE_CURRENT_SOURCE_DIR}/docs/${doc}/docbook")
                ELSE()
                    SET(i "${DOCS_BINARY_DIR}/${doc}/docbook/${l}")
                ENDIF()
                ADD_CUSTOM_COMMAND(OUTPUT "${dx}/${doc}.fo"
                    COMMENT "Generating ${dx}/${doc}.fo"
                    COMMAND "${XSLTPROC_COMMAND}" --nonet
                        --output "${dx}/${doc}.fo"
                        "${BT_DOCBOOK_PDF_XSL}" "${i}/index.docbook"
                    DEPENDS "${doc}_translation_${l}")
                AddDocSubTarget("${doc}_xslfo" "${doc}_xslfo_${l}"
                    "${dx}/${doc}.fo")

                FOREACH(image IN LISTS "${udoc}_IMAGES")
                    IF(CMAKE_VERSION VERSION_LESS 3.14)
                        FILE(COPY "${image}" DESTINATION "${dx}/")
                    ELSE()
                        GET_FILENAME_COMPONENT(imageName "${image}" NAME)
                        FILE(CREATE_LINK "${image}" "${dx}/${imageName}"
                            COPY_ON_ERROR)
                    ENDIF()
                ENDFOREACH()

                SET(dp "${DOCS_BINARY_DIR}/${doc}/pdf/${l}")
                FILE(MAKE_DIRECTORY "${dp}")
                ADD_CUSTOM_COMMAND(OUTPUT "${dp}/${doc}.pdf"
                    COMMENT "Generating ${dp}/${doc}.pdf"
                    COMMAND "${FOP_COMMAND}" -pdf "${dp}/${doc}.pdf"
                        -fo "${dx}/${doc}.fo"
                    DEPENDS "${doc}_xslfo_${l}")
                AddDocSubTarget("${doc}_pdf" "${doc}_pdf_${l}"
                    "${dp}/${doc}.pdf" ${${udoc}_IMAGES})
                INSTALL(FILES "${dp}/${doc}.pdf"
                        DESTINATION "${BT_DOCDIR}/${doc}/pdf/${l}/")
            ENDFOREACH()
        ENDIF()
    ENDFUNCTION()
    GeneratePdfDoc("handbook")
    GeneratePdfDoc("howto")
ENDIF()
