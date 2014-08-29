IF(APPLE)
    # Qt Plugins
    IF(Qt5Core_FOUND)

        MACRO(InstallPlugin _QtTarget)
            GET_TARGET_PROPERTY(_QtLocation ${_QtTarget} LOCATION)
            # Get the Qt plugin directory for our SVG hack below...
            STRING(REGEX MATCH ".*/plugins/"    _QtPluginBase ${_QtLocation})
            STRING(REGEX MATCH "plugins/[^/]+/" _QtPluginPath ${_QtLocation})
            #MESSAGE("INSTALL ${_QtLocation} DESTINATION ${BT_DESTINATION}/${_QtPluginPath}" )
            INSTALL(FILES ${_QtLocation} DESTINATION "${BT_DESTINATION}/${_QtPluginPath}")
        ENDMACRO(InstallPlugin)

        FOREACH(Plugin ${Qt5Gui_PLUGINS})
            InstallPlugin(${Plugin})
        ENDFOREACH()
        FOREACH(Plugin ${Qt5Widget_PLUGINS})
            InstallPlugin(${Plugin})
        ENDFOREACH()
        # This does not work in Qt <= 5.3.1, see
        # https://bugreports.qt-project.org/browse/QTBUG-39171?page=com.atlassian.jira.plugin.system.issuetabpanels:changehistory-tabpanel
        #FOREACH(Plugin ${Qt5Svg_PLUGINS})
        #    InstallPlugin(${Plugin})
        #ENDFOREACH()

        # So we'll use a hack here
        INSTALL(FILES "${_QtPluginBase}/imageformats/libqsvg.dylib" DESTINATION "${BT_DESTINATION}/plugins/imageformats")
        INSTALL(FILES "${_QtPluginBase}/iconengines/libqsvgicon.dylib" DESTINATION "${BT_DESTINATION}/plugins/iconengines")

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

