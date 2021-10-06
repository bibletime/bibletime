IF(APPLE)
    # Workaround: macdeployqt does not seem to set a proper rpath for the package,
    #   so we do this manually.
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks")
    SET_TARGET_PROPERTIES("bibletime" PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/Info.plist )

    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
        # Note: building based on the Qt shipped by homebrew does not work currently (2018-06),
        #   as it seems to ship a broken macdeployqt tool. So use the official Qt packages to build.

        SET(QT_MACDEPLOYQT_EXECUTABLE "${_qt5Core_install_prefix}/bin/macdeployqt")
        SET(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/Info.plist")
        SET(BT_MAC_APP  "${CMAKE_INSTALL_PREFIX}/BibleTime.app")

        SET(INFO_FILE ${BT_MAC_APP}/Contents/Info.plist)
        INSTALL(CODE "
            EXECUTE_PROCESS(COMMAND ${QT_MACDEPLOYQT_EXECUTABLE} ${BT_MAC_APP}   -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/display/modelview)
            CONFIGURE_FILE( ${INFO_FILE} ${INFO_FILE})
        ")
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

    SET(CPACK_BUNDLE_NAME "BibleTime")
    SET(CPACK_PACKAGE_FILE_NAME "BibleTime ${BT_VERSION_FULL}")
    SET(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/Info.plist")
    SET(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
    SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
    SET(CPACK_GENERATOR "Bundle")

    SET(CPACK_PACKAGE_EXECUTABLES "bibletime" "BibleTime")

    INCLUDE(CPack)
ENDIF(APPLE)
