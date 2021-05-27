######################################################
# Find packages:
#
FIND_PACKAGE(CLucene REQUIRED)
SET(REQUIRED_QT_VERSION 5.9)
FIND_PACKAGE(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5LinguistTools ${REQUIRED_QT_VERSION})
FIND_PACKAGE(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Svg ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5PrintSupport ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Qml ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Quick ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Qt5QuickWidgets ${REQUIRED_QT_VERSION} REQUIRED)
FIND_PACKAGE(Sword 1.8.1 REQUIRED)


######################################################
# Build options, definitions, linker flags etc for all targets:
#
INCLUDE(BTUseCcache)
INCLUDE(CheckIPOSupported)
CHECK_IPO_SUPPORTED(RESULT HAVE_IPO)
MESSAGE(STATUS "Interprocedural optimization support: ${HAVE_IPO}")
FUNCTION(PREPARE_CXX_TARGET target)
    TARGET_COMPILE_FEATURES("${target}" PUBLIC cxx_std_11)
    SET_TARGET_PROPERTIES("${target}" PROPERTIES CXX_EXTENSIONS NO)
    IF(HAVE_IPO)
        SET_TARGET_PROPERTIES("${target}" PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
    ENDIF()
ENDFUNCTION()
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
ADD_DEFINITIONS("-DBT_VERSION=\"${BT_VERSION_FULL}\"")
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(CMAKE_AUTOMOC_MOC_OPTIONS "-DNDEBUG")
ENDIF()
IF(WIN32)
    ADD_COMPILE_OPTIONS("/Zi")
ELSE()
    ADD_COMPILE_OPTIONS(
        "-ggdb" "-Wall" "-Wextra"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-O2>"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
        "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DQT_NO_DEBUG>"
        "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
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
    SET(T "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/")
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
# bibletime_backend static library. Shared between the bibletime app and all
# tests:
#
FILE(GLOB_RECURSE bibletime_BACKEND_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/backend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.h"
)
ADD_LIBRARY(bibletime_backend STATIC ${bibletime_BACKEND_SOURCES})
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
        Qt5::Widgets
        Qt5::Xml
        -L${CLucene_LIBRARY_DIR} ${CLucene_LIBRARY}
        ${Sword_LDFLAGS}
        ${BibleTime_LDFLAGS}
)


######################################################
# The bibletime application:
#
FILE(GLOB_RECURSE bibletime_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/*.qrc"
)
IF(APPLE)
    ADD_EXECUTABLE("bibletime" MACOSX_BUNDLE ${bibletime_SOURCES})
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES OUTPUT_NAME "BibleTime")
ELSEIF(MSVC)
    ADD_EXECUTABLE("bibletime" WIN32 ${bibletime_SOURCES} "cmake/BTWinIcon.rc")
ELSE()
    ADD_EXECUTABLE("bibletime" ${bibletime_SOURCES})
ENDIF()
PREPARE_CXX_TARGET(bibletime)
TARGET_LINK_LIBRARIES("bibletime"
    PRIVATE
        bibletime_backend
        Qt5::Network
        Qt5::PrintSupport
        Qt5::Quick
        Qt5::QuickWidgets
        Qt5::Svg
        Qt5::Widgets
        Qt5::Xml
)


######################################################
# Define rules to generate and install translation files:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BTUITranslation.cmake")


######################################################
# Installation:
#
INSTALL(TARGETS "bibletime" DESTINATION "${BT_BINDIR}")
FILE(GLOB INSTALL_ICONS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/*.svg")
INSTALL(FILES ${INSTALL_ICONS_LIST}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")
FILE(GLOB INSTALL_ICONS_LIST_PNG
    "${CMAKE_CURRENT_SOURCE_DIR}/pics/icons/bibletime.png")
INSTALL(FILES ${INSTALL_ICONS_LIST_PNG}
        DESTINATION "${BT_DATADIR}/bibletime/icons/")

INSTALL(FILES "docs/license.html"
        DESTINATION "${BT_DATADIR}/bibletime/license/")

FILE(GLOB INSTALL_TMPL_LIST
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

INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/linux/info.bibletime.BibleTime.metainfo.xml"
        DESTINATION "${BT_DATAROOTDIR}/metainfo/")


IF(MSVC) # Windows:
  # sword locale information
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
  INSTALL(FILES ${INSTALL_SWORD_LOCALE_LIST}
          DESTINATION "${SWORD_DATADIR}/sword/locales.d/")

  # install pdb file for debugging purposes
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(bibletime_PDB "${bibletime_BINARY_DIR}/Debug/bibletime.pdb")
    INSTALL(FILES "${bibletime_PDB}" DESTINATION "${BT_BINDIR}")
  ENDIF()
ELSEIF(APPLE) # OS X:
  # sword locale information, needed for DMG image
  FILE(GLOB INSTALL_SWORD_LOCALE_LIST "${Sword_INCLUDE_DIRS}/../../share/sword/locales.d/*")
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
