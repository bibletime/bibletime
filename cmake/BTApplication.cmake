######################################################
# Find packages:
#
FIND_PACKAGE(Qt6 6.7 REQUIRED COMPONENTS
    Core
    Gui
    LinguistTools
    Widgets
    Xml
    Test
    Svg
    PrintSupport
    Qml
    Quick
    QuickWidgets
)
FIND_PACKAGE(PkgConfig REQUIRED)
pkg_search_module(Sword REQUIRED IMPORTED_TARGET sword>=1.8.1)
MESSAGE(STATUS "Found Sword: ${Sword_VERSION}")
pkg_search_module(CLucene REQUIRED IMPORTED_TARGET libclucene-core>=2.3.3.4)
MESSAGE(STATUS "Found CLucene: ${CLucene_VERSION}")

######################################################
# Build options, definitions, linker flags etc for all targets:
#
IF(NOT (DEFINED BT_RUNTIME_DOCDIR))
    IF(MSVC)
        SET(BT_RUNTIME_DOCDIR "${BT_DOCDIR}")
    ELSE()
        SET(BT_RUNTIME_DOCDIR "${BT_DOCDIR_ABSOLUTE}")
    ENDIF()
ENDIF()

SET(CMAKE_AUTOMOC ON)
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(CMAKE_AUTOMOC_MOC_OPTIONS "-DNDEBUG")
ENDIF()

FILE(GLOB_RECURSE bibletime_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
FILE(GLOB_RECURSE bibletime_QML_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.qml"
)
IF(MSVC)
    ADD_EXECUTABLE("bibletime" WIN32 ${bibletime_SOURCES} "cmake/BTWinIcon.rc")
ELSE()
    ADD_EXECUTABLE("bibletime" ${bibletime_SOURCES})
ENDIF()
TARGET_COMPILE_FEATURES("bibletime" PRIVATE cxx_std_17)
TARGET_COMPILE_DEFINITIONS("bibletime" PRIVATE
    "BT_RUNTIME_DOCDIR=\"${BT_RUNTIME_DOCDIR}\""
    "BT_VERSION=\"${PROJECT_VERSION}\""
    "QT_NO_KEYWORDS"
    "QT_DISABLE_DEPRECATED_UP_TO=0x060500"
    "$<$<CXX_COMPILER_ID:MSVC>:SWUSINGDLL>"
    "$<$<CXX_COMPILER_ID:MSVC>:_UNICODE>"
    "$<$<CXX_COMPILER_ID:MSVC>:UNICODE>"
    "$<$<CONFIG:Release>:NDEBUG>"
    "$<$<CONFIG:Release>:QT_NO_DEBUG>"
)
TARGET_COMPILE_OPTIONS("bibletime" PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/W1>"
    "$<$<CXX_COMPILER_ID:MSVC>:/Zi>"
    "$<$<CXX_COMPILER_ID:MSVC>:/Zc:wchar_t>"
    "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/MD>"
    "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/MDd>"
    "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Od>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fPIE>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"
)
TARGET_INCLUDE_DIRECTORIES("bibletime" PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}

    # work around QTBUG-87221/QTBUG-93443:
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/display/modelview/"
)
TARGET_LINK_LIBRARIES("bibletime" PRIVATE
    PkgConfig::CLucene
    PkgConfig::Sword
    Qt::Network
    Qt::PrintSupport
    Qt::Quick
    Qt::QuickWidgets
    Qt::Svg
    Qt::Widgets
    Qt::Xml
)
SET_TARGET_PROPERTIES("bibletime" PROPERTIES CXX_EXTENSIONS NO)

SET(CMAKE_REQUIRED_QUIET TRUE)
INCLUDE(CheckIPOSupported)
CHECK_IPO_SUPPORTED(RESULT HAVE_IPO)
MESSAGE(STATUS "Using interprocedural optimization: ${HAVE_IPO}")
IF(HAVE_IPO)
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION TRUE)
ENDIF()

INCLUDE(CheckCXXCompilerFlag)
FOREACH(flag IN ITEMS
    "-Walloca"
    "-Wextra-semi"
    "-Wformat=2"
    "-Wformat-signedness"
    "-Wfloat-equal"
    "-Wformat"
    "-Wlogical-op"
    "-Wno-packed"
    "-Wno-padded"
    "-Wno-switch-enum"
    "-Wpointer-arith"
    "-Wsuggest-destructor-override"
    "-Wsuggest-override"
    "-Wunused-parameter"
    "-Wzero-as-null-pointer-constant"
    "-fasynchronous-unwind-tables"
    "-fcf-protection=full"
    "-fstack-clash-protection"
    "-fstack-protector-strong"
    "-pipe"
)
    STRING(SHA512 flag_id "${flag}")
    CHECK_CXX_COMPILER_FLAG("${flag}" "cxx_compiler_has_flag_${flag_id}")
    IF("${cxx_compiler_has_flag_${flag_id}}")
        TARGET_COMPILE_OPTIONS("bibletime" PRIVATE "${flag}")
        MESSAGE(STATUS "Using C++ compiler flag: ${flag}")
    ELSE()
        MESSAGE(STATUS "Flag not supported by C++ compiler: ${flag}")
    ENDIF()
ENDFOREACH()

FOREACH(file IN LISTS bibletime_QML_FILES)
    STRING(REGEX REPLACE "^.*/([^/]+)$" "\\1" filename "${file}")
    SET_SOURCE_FILES_PROPERTIES("${file}" PROPERTIES
        QT_RESOURCE_ALIAS "${filename}")
ENDFOREACH()
QT_ADD_QML_MODULE("bibletime"
    URI "BibleTime"
    RESOURCE_PREFIX "/qt/qml"
    NO_RESOURCE_TARGET_PATH
    VERSION 1.0
    QML_FILES ${bibletime_QML_FILES}
)


######################################################
# Define rules to generate and install translation files:
#
FILE(GLOB BT_TS_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages/bibletime_ui_*.ts"
)
qt_add_translations("bibletime"
  SOURCE_TARGETS "bibletime"
  TS_FILES ${BT_TS_FILES}
  QM_FILES_OUTPUT_VARIABLE BT_QM_FILES
  LUPDATE_TARGET "lupdate"
  LRELEASE_TARGET "lrelease"
)
INSTALL(FILES ${BT_QM_FILES} DESTINATION "${BT_LOCALEDIR}/")


######################################################
# Installation:
#
INSTALL(TARGETS "bibletime" DESTINATION "${BT_BINDIR}")
FILE(GLOB INSTALL_ICONS_LIST CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")
FILE(GLOB INSTALL_ICONS_LIST_PNG CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")

INSTALL(FILES "LICENSE"
        DESTINATION "${BT_DATADIR}/bibletime/license/")

FILE(GLOB INSTALL_TMPL_LIST CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.css"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/*.cmap"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/Basic.tmpl"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/display-templates/basic_template.txt")
INSTALL(FILES ${INSTALL_TMPL_LIST}
        DESTINATION "${BT_DATADIR}/bibletime/display-templates/")
INSTALL(FILES "pics/startuplogo.png" "pics/startuplogo_christmas.png"
              "pics/startuplogo_easter.jpg"
        DESTINATION "${BT_DATADIR}/bibletime/pics/")

# Platform specific installation

# Linux: application icon and desktop file:
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.svg"
        DESTINATION "${BT_DATAROOTDIR}/icons/hicolor/scalable/apps" RENAME "info.bibletime.BibleTime.svg")


CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/bibletime.desktop.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/bibletime.desktop"
        DESTINATION "${BT_DATAROOTDIR}/applications/" RENAME "info.bibletime.BibleTime.desktop")


CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/info.bibletime.BibleTime.metainfo.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/info.bibletime.BibleTime.metainfo.xml" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/info.bibletime.BibleTime.metainfo.xml"
        DESTINATION "${BT_DATAROOTDIR}/metainfo/")



IF(MSVC) # Windows:
  # sword locale information
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST CONFIGURE_DEPENDS
          "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${SWORD_DATADIR}/sword/locales.d/")

  # install pdb file for debugging purposes
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(bibletime_PDB "${bibletime_BINARY_DIR}/Debug/bibletime.pdb")
    INSTALL(FILES "${bibletime_PDB}" DESTINATION "${BT_BINDIR}")
  ENDIF()
ENDIF()


######################################################
# Binary packages:
#
IF(WIN32 AND NOT UNIX)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageWindows.cmake")
ENDIF()
