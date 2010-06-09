IF(APPLE)
    # Qt Plugins
    INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats" DESTINATION "${BT_DESTINATION}/../../plugins")
    # A file to execute that might get rid of the above mess
    SET(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/bt_start.sh")
    SET(CPACK_BUNDLE_NAME "BibleTime")
    SET(CPACK_PACKAGE_FILE_NAME "BibleTime_Mac_Beta")
    SET(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/Info.plist")
    SET(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
    SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/BibleTime.icns")
    SET(CPACK_GENERATOR "Bundle")

    SET(CPACK_PACKAGE_EXECUTABLES "bibletime" "BibleTime")

    INSTALL(CODE "
        INCLUDE(BundleUtilities)
        FIXUP_BUNDLE(\"\${CMAKE_INSTALL_PREFIX}/bin/BibleTime\" \"\" \"\")
        " COMPONENT Runtime)

    INCLUDE(CPack)
ENDIF(APPLE)

