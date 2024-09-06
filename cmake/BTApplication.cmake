######################################################
# Find packages:
#
FIND_PACKAGE(CLucene REQUIRED)

IF (USE_QT6)
    FIND_PACKAGE(QT NAMES Qt6 VERSION 6.4 COMPONENTS Core)
ELSE()
    FIND_PACKAGE(QT NAMES Qt5 VERSION 5.15 REQUIRED COMPONENTS Core)
ENDIF()
MESSAGE(STATUS "Found Qt ${QT_VERSION}")
FIND_PACKAGE(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
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
IF(BUILD_TEXT_TO_SPEECH)
    FIND_PACKAGE(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        TextToSpeech
    )
    ADD_DEFINITIONS(-DBUILD_TEXT_TO_SPEECH)
ENDIF()
IF(NOT USE_QT6)
    FIND_PACKAGE(Qt5 REQUIRED COMPONENTS QuickCompiler)
ENDIF()

FIND_PACKAGE(Sword 1.8.1 REQUIRED)

######################################################
# Build options, definitions, linker flags etc for all targets:
#
INCLUDE(BTCompileFlags)
INCLUDE(BTUseCcache)
INCLUDE(CheckIPOSupported)
CHECK_IPO_SUPPORTED(RESULT HAVE_IPO)
MESSAGE(STATUS "Interprocedural optimization support: ${HAVE_IPO}")
FUNCTION(PREPARE_CXX_TARGET target)
    TARGET_COMPILE_FEATURES("${target}" PUBLIC cxx_std_17)
    SET_TARGET_PROPERTIES("${target}" PROPERTIES CXX_EXTENSIONS NO)
    IF(HAVE_IPO)
        SET_TARGET_PROPERTIES("${target}" PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
    ENDIF()
ENDFUNCTION()
SET(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS(
    "-DBT_VERSION=\"${BT_VERSION_FULL}\""
    "-DQT_NO_KEYWORDS"
    "-DQT_DISABLE_DEPRECATED_UP_TO=0x060500"
)
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(CMAKE_AUTOMOC_MOC_OPTIONS "-DNDEBUG")
ENDIF()
IF(WIN32)
    ADD_COMPILE_OPTIONS("/Zi")
ELSE()
    ADD_COMPILE_OPTIONS(
        "-O2" "-ggdb" "-Wall" "-Wextra"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DQT_NO_DEBUG>"
    )
ENDIF()
UNSET(BibleTime_CXXFLAGS)
UNSET(BibleTime_LDFLAGS)
IF(MSVC)
  SET(CMAKE_CXX_FLAGS_RELEASE "-DSWUSINGDLL /MD  /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t")
  SET(CMAKE_CXX_FLAGS_DEBUG   "-DSWUSINGDLL /MDd /Zc:wchar_t- /W1 /D_UNICODE /DUNICODE /Zc:wchar_t /Od")
ELSE()
  ADD_COMPILE_OPTIONS("-fPIE" "-fexceptions")
  SET(CMAKE_CXX_FLAGS_RELEASE "")
  SET(CMAKE_CXX_FLAGS_DEBUG "")
  IF(APPLE)
    SET(T "/Applications/Xcode_12.4.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/")
    LIST(APPEND BibleTime_CXXFLAGS
        "-mmacosx-version-min=10.12"
        "-stdlib=libc++"
        "-isysroot" "${T}")
    LIST(APPEND BibleTime_LDFLAGS
        "-mmacosx-version-min=10.12"
        "-stdlib=libc++"
    )
    UNSET(T)
  ENDIF()
ENDIF()


######################################################
# bibletime_backend object library. This mostly exists for the purpose of
# keeping the backend and frontend as separate modules with no GUI dependencies
# or frontend code in the backend.
#
FILE(GLOB_RECURSE bibletime_BACKEND_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.h"
)
ADD_LIBRARY(bibletime_backend OBJECT ${bibletime_BACKEND_SOURCES})
GET_SOURCE_FILE_PROPERTY(d
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/directory.cpp" COMPILE_DEFINITIONS)
IF(DEFINED BT_RUNTIME_DOCDIR)
    LIST(APPEND d "BT_RUNTIME_DOCDIR=\"${BT_RUNTIME_DOCDIR}\"")
ELSEIF(APPLE OR MSVC)
    LIST(APPEND d "BT_RUNTIME_DOCDIR=\"${BT_DOCDIR}\"")
ELSE()
    LIST(APPEND d "BT_RUNTIME_DOCDIR=\"${BT_DOCDIR_ABSOLUTE}\"")
ENDIF()
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_SOURCE_DIR}/src/util/directory.cpp"
    PROPERTIES COMPILE_DEFINITIONS "${d}")
PREPARE_CXX_TARGET(bibletime_backend)
TARGET_COMPILE_DEFINITIONS(bibletime_backend PRIVATE ${CLucene_DEFINITIONS})
TARGET_COMPILE_OPTIONS(bibletime_backend
    PUBLIC
        ${BibleTime_CXXFLAGS}
        ${Sword_CFLAGS_OTHER}
)
BtAddCxxCompilerFlags(bibletime_backend PUBLIC
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
TARGET_INCLUDE_DIRECTORIES(bibletime_backend
    PRIVATE
        ${CLucene_INCLUDE_DIR}            #CLucene headers
        ${CLucene_LIBRARY_DIR}            #CLucene/clucene-config.h
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}        #for .h files generated from .ui
        ${Sword_INCLUDE_DIRS}
)
TARGET_LINK_LIBRARIES(bibletime_backend
    PUBLIC
        Qt::Widgets
        Qt::Xml
        -L${CLucene_LIBRARY_DIR} ${CLucene_LIBRARY}
        ${Sword_LDFLAGS}
        ${BibleTime_LDFLAGS}
)


######################################################
# The bibletime application:
#
FILE(GLOB_RECURSE bibletime_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.h"
)
if (NOT USE_QT6)
    qtquick_compiler_add_resources(bibletime_RESOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/display/modelview/modelviewqml.qrc")
    LIST(APPEND bibletime_SOURCES ${bibletime_RESOURCES})
ENDIF()
IF(APPLE)
    ADD_EXECUTABLE("bibletime" MACOSX_BUNDLE ${bibletime_SOURCES})
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "BibleTime")
ELSEIF(MSVC)
    ADD_EXECUTABLE("bibletime" WIN32 ${bibletime_SOURCES} "cmake/BTWinIcon.rc")
ELSE()
    ADD_EXECUTABLE("bibletime" ${bibletime_SOURCES})
ENDIF()
PREPARE_CXX_TARGET(bibletime)

if (USE_QT6)
    SET(bibletime_QML_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/display/modelview")
    FILE(GLOB bibletime_QML_FILES CONFIGURE_DEPENDS
        "${bibletime_QML_PATH}/*.qml")
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
    INCLUDE_DIRECTORIES( # work around QTBUG-87221/QTBUG-93443
        "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/display/modelview/")
ENDIF()

TARGET_LINK_LIBRARIES("bibletime"
    PRIVATE
        bibletime_backend
        Qt::Network
        Qt::PrintSupport
        Qt::Quick
        Qt::QuickWidgets
        Qt::Svg
        Qt::Widgets
        Qt::Xml
)
IF(BUILD_TEXT_TO_SPEECH)
    TARGET_LINK_LIBRARIES("bibletime" PRIVATE Qt::TextToSpeech)
ENDIF()


######################################################
# Define rules to generate and install translation files:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")


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
ELSEIF(APPLE) # OS X:
  # sword locale information, needed for DMG image
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST CONFIGURE_DEPENDS
          "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "./BibleTime.app/Contents/share/sword/locales.d/")
  INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns"
        DESTINATION "${BT_RESOURCEDIR}/")
ENDIF()


######################################################
# Binary packages:
#
IF(WIN32 AND NOT UNIX)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageWindows.cmake")
ELSEIF(APPLE)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTBinaryPackageMacOS.cmake")
ENDIF()


######################################################
# "make uninstall" target; see http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
#
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
