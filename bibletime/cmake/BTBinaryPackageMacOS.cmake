IF(APPLE)
    INCLUDE(${QT_USE_FILE})
    SET(QT_FRAMEWORKS_USED
        "QtWebKit"
        "QtGui"
        "QtXml"
        "QtTest"
        "QtNetwork"
        "QtCore"
        "QtSvg"
    )
    FOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})
        INSTALL(DIRECTORY
            "${QT_LIBRARY_DIR}/${QT_FRAME}.framework"
            DESTINATION "${BT_DESTINATION}/../Library/Frameworks/"
        )
    ENDFOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})

    # Other libraries I need
    INSTALL(FILES "${CLUCENE_LIBRARY}"
        "${CLUCENE_LIBRARY_DIR}/libclucene.0.0.0.dylib"
		"${CLUCENE_LIBRARY_DIR}/libclucene.0.dylib"
		"/opt/local/lib/libcurl.4.dylib"
   	    "/opt/local/lib/libssl.0.9.8.dylib"
       	"/opt/local/lib/libcrypto.0.9.8.dylib"
#        "/opt/local/lib/libicui18n.40.dylib"
#	       	"/opt/local/lib/libicui18n.40.0.dylib"
#		"/opt/local/lib/libicuuc.40.dylib"
#	       	"/opt/local/lib/libicuuc.40.0.dylib"
#	       	"/opt/local/lib/libicuio.40.dylib"
#	       	"/opt/local/lib/libicuio.40.0.dylib"
#		"/opt/local/lib/libicudata.40.dylib"
#	       	"/opt/local/lib/libicudata.40.0.dylib"
		"/opt/local/lib/libpng12.0.dylib"
          DESTINATION "${BT_DESTINATION}/../Library")

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

    INCLUDE(CPack)
ENDIF(APPLE)

