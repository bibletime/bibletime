IF(APPLE)
    # Install the Sword library
    INSTALL(
        FILES "${Sword_LIBRARY_DIRS}/lib${Sword_LIBRARIES}-${Sword_VERSION}.dylib"
        DESTINATION "${BT_DESTINATION}/"
    )

    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
        # Note: building based on the Qt shipped by homebrew does not work currently (2018-06),
        #   as it seems to ship a b0rken macdeployqt tool. So use the official Qt packages to build.

        # Workaround: macdeployqt does not seem to set a proper rpath for the package,
        #   so we do this manually.
        SET_TARGET_PROPERTIES("bibletime" PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks")

        SET(QT_MACDEPLOYQT_EXECUTABLE "${_qt5Core_install_prefix}/bin/macdeployqt")

        INSTALL(CODE "
            EXECUTE_PROCESS(COMMAND ${QT_MACDEPLOYQT_EXECUTABLE} \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/../..\"   -qmldir=${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION} )
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
