IF(APPLE)
    # Qt Plugins
    IF(Qt5Core_FOUND)
        GET_TARGET_PROPERTY(ICON_PLUGIN Qt5::QICOPlugin  LOCATION)
        GET_TARGET_PROPERTY(GIF_PLUGIN  Qt5::QGifPlugin  LOCATION)
        GET_TARGET_PROPERTY(JPG_PLUGIN  Qt5::QJpegPlugin LOCATION)
        GET_TARGET_PROPERTY(SVG_PLUGIN  Qt5::QSvgPlugin  LOCATION)
        GET_TARGET_PROPERTY(TIFF_PLUGIN Qt5::QTiffPlugin LOCATION)
        GET_TARGET_PROPERTY(BMP_PLUGIN  Qt5::QWbmpPlugin LOCATION)
        GET_TARGET_PROPERTY(MNG_PLUGIN  Qt5::QMngPlugin  LOCATION)
        INSTALL(
            FILES
                "${ICON_PLUGIN}"
                "${GIF_PLUGIN}"
                "${JPG_PLUGIN}"
                "${SVG_PLUGIN}"
                "${TIFF_PLUGIN}"
                "${BMP_PLUGIN}"
                "${MNG_PLUGIN}"
            DESTINATION "${BT_DESTINATION}/plugins"
        )
    ELSE(Qt5Core_FOUND)
        INSTALL(
            DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats"
            DESTINATION "${BT_DESTINATION}/plugins"
        )

        INSTALL(
            DIRECTORY "${QT_LIBRARY_DIR}/QtGui.framework/Resources/qt_menu.nib"
            DESTINATION "${BT_DESTINATION}/../Frameworks/QtGui.framework/Resources"
        )
    ENDIF(Qt5Core_FOUND)

    INSTALL(
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/qt.conf"
        DESTINATION "${BT_SHARE_PATH}/../Resources"
    )

    # Install the Sword library
    INSTALL(
        FILES "${Sword_LIBRARY_DIRS}/lib${Sword_LIBRARIES}-${Sword_VERSION}.dylib"
        DESTINATION "${BT_DESTINATION}/"
    )

    IF (CMAKE_BUILD_TYPE STREQUAL "Release")

        INSTALL(CODE "
            FILE(GLOB_RECURSE QTPLUGINS
              \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

            SET(BU_CHMOD_BUNDLE_ITEMS ON)   # Add write permissions for libs that need it like libssl
            INCLUDE(BundleUtilities)

            FIXUP_BUNDLE(
                \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/BibleTime\"
                \"\${QTPLUGINS}\"
                \"\"
            )
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

