IF(APPLE)
    # Qt Plugins
    INSTALL(
        DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats"
        DESTINATION "${BT_DESTINATION}/plugins"
    )

    INSTALL(
        DIRECTORY "${QT_LIBRARY_DIR}/QtGui.framework/Resources/qt_menu.nib"
        DESTINATION "${BT_DESTINATION}/../Frameworks/QtGui.framework/Resources"
    )

    INSTALL(
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms/macos/qt.conf"
        DESTINATION "${BT_SHARE_PATH}/../Resources"
    )

    IF (CMAKE_BUILD_TYPE STREQUAL "Release")

        INSTALL(CODE "
            FILE(GLOB_RECURSE QTPLUGINS
              \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#            FILE(GLOB_RECURSE QTPLUGINS
#              \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/plugins/*.bundle\")
            INCLUDE(BundleUtilities)


            function(gp_item_default_embedded_path_override item default_embedded_path_var)
                set(path \"\@executable_path/../MacOS\")
                set(overridden 0)

                if(item MATCHES \"plugins/iconengines/.*\\\\.dylib$\")
                    set(path \"\@executable_path/plugins/iconengines\")
                    set(overridden 1)
                endif(item MATCHES \"plugins/iconengines/.*\\\\.dylib$\")

                if(item MATCHES \"plugins/imageformats/.*\\\\.dylib$\")
                    set(path \"\@executable_path/plugins/imageformats\")
                    set(overridden 1)
                endif(item MATCHES \"plugins/imageformats/.*\\\\.dylib$\")

                if (overridden)
                    set(\${default_embedded_path_var} \"\${path}\" PARENT_SCOPE)
                endif (overridden)
            endfunction(gp_item_default_embedded_path_override)

            FIXUP_BUNDLE(
                \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/BibleTime\"
                \"\${QTPLUGINS}\"
                \"\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/plugins/imageformats;\${CMAKE_INSTALL_PREFIX}/${BT_DESTINATION}/plugins/iconengines\"
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

