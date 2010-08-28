IF(WIN32 AND NOT UNIX)

    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BibleTime for Windows svn")
    SET(CPACK_PACKAGE_VENDOR "http://www.bibletime.info")
    SET(CPACK_PACKAGE_VERSION_MAJOR ${BT_VERSION_MAJOR})
    SET(CPACK_PACKAGE_VERSION_MINOR ${BT_VERSION_MINOR})
    SET(CPACK_PACKAGE_VERSION_PATCH ${BT_REVISION_PATCH})
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "BibleTime")

    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.

    # We need the libraries, and they're not pulled in automatically
    INCLUDE(${QT_USE_FILE})
    SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
    SET(QT_BINARY_DIR "${QT_LIBRARY_DIR}/../bin")
    INSTALL(FILES
        "${QT_BINARY_DIR}/QtWebKitd4.dll"
        "${QT_BINARY_DIR}/QtGuid4.dll"
        "${QT_BINARY_DIR}/QtXmld4.dll"
        "${QT_BINARY_DIR}/QtTestd4.dll"
        "${QT_BINARY_DIR}/QtNetworkd4.dll"
        "${QT_BINARY_DIR}/QtCored4.dll"
        "${QT_BINARY_DIR}/phonond4.dll"
        "${QT_BINARY_DIR}/QtSvgd4.dll"
        "${QT_BINARY_DIR}/QtXmlPatternsd4.dll"
        DESTINATION "${BT_DESTINATION}"
		CONFIGURATIONS "Debug"
    )
    INSTALL(FILES
        "${QT_BINARY_DIR}/QtWebKit4.dll"
        "${QT_BINARY_DIR}/QtGui4.dll"
        "${QT_BINARY_DIR}/QtXml4.dll"
        "${QT_BINARY_DIR}/QtTest4.dll"
        "${QT_BINARY_DIR}/QtNetwork4.dll"
        "${QT_BINARY_DIR}/QtCore4.dll"
        "${QT_BINARY_DIR}/phonon4.dll"
        "${QT_BINARY_DIR}/QtSvg4.dll"
        "${QT_BINARY_DIR}/QtXmlPatterns4.dll"
        DESTINATION "${BT_DESTINATION}"
        CONFIGURATIONS "Release"
    )

    # Qt Plugins
    INSTALL(FILES
        "${QT_PLUGINS_DIR}/iconengines/qsvgicon4.dll"
        DESTINATION "${BT_DESTINATION}/plugins/iconengines"
        CONFIGURATIONS "Release"
    )
    INSTALL(FILES
		"${QT_PLUGINS_DIR}/imageformats/qgif4.dll"
		"${QT_PLUGINS_DIR}/imageformats/qico4.dll"
		"${QT_PLUGINS_DIR}/imageformats/qjpeg4.dll"
		"${QT_PLUGINS_DIR}/imageformats/qmng4.dll"
		"${QT_PLUGINS_DIR}/imageformats/qsvg4.dll"
		"${QT_PLUGINS_DIR}/imageformats/qtiff4.dll"
		DESTINATION "${BT_DESTINATION}/plugins/imageformats"
		CONFIGURATIONS "Release"
    )

    # This adds in the required Windows system libraries
    INSTALL(PROGRAMS ${MSVC_REDIST} DESTINATION bin)
    SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        ExecWait \\\"$INSTDIR\\\\bin\\\\vcredist_x86.exe  /q:a\\\"
        Delete   \\\"$INSTDIR\\\\bin\\\\vcredist_x86.exe\\\"
    ")

    # add the libsword.dll
    STRING(REPLACE ".lib" ".dll" SWORD_DLL "${SWORD_LIBRARY}")
    INSTALL(FILES ${SWORD_DLL} DESTINATION ${BT_DESTINATION}) # This will also take effect in the regular install

    # Some options for the CPack system.  These should be pretty self-evident
    SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\pics\\\\icons\\\\bibletime.png")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\bibletime.exe")
    SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} for Windows")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.bibletime.info")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.bibletime.info")
    SET(CPACK_NSIS_CONTACT "bt-devel@crosswire.org")
    SET(CPACK_NSIS_MODIFY_PATH OFF)
    SET(CPACK_GENERATOR "NSIS")

    SET(CPACK_PACKAGE_EXECUTABLES "bibletime" "BibleTime")

    INCLUDE(CPack)

ENDIF(WIN32 AND NOT UNIX)

