# This is the CPack section
# Copied and modified from http://www.vtk.org/Wiki/CMake:Packaging_With_CPack

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BibleTime for Windows Beta")
SET(CPACK_PACKAGE_VENDOR "http://www.bibletime.info")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "BibleTime")

######################################################
# Windows segment
######################################################
IF(WIN32 AND NOT UNIX)
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.

  # We need the libraries, and they're not pulled in automatically
  INCLUDE(${QT_USE_FILE})
  SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
  INSTALL(FILES
			"${QT_LIBRARY_DIR}/QtWebKitd4.dll"
			"${QT_LIBRARY_DIR}/QtGuid4.dll"
			"${QT_LIBRARY_DIR}/QtXmld4.dll"
			"${QT_LIBRARY_DIR}/QtTestd4.dll"
			"${QT_LIBRARY_DIR}/QtNetworkd4.dll"
			"${QT_LIBRARY_DIR}/QtCored4.dll"
			"${QT_LIBRARY_DIR}/phonond4.dll"
			"${QT_LIBRARY_DIR}/QtSvgd4.dll"
			DESTINATION "${BT_DESTINATION}"
			CONFIGURATIONS "Debug"
  )
  INSTALL(FILES
			"${QT_LIBRARY_DIR}/QtWebKit4.dll"
			"${QT_LIBRARY_DIR}/QtGui4.dll"
			"${QT_LIBRARY_DIR}/QtXml4.dll"
			"${QT_LIBRARY_DIR}/QtTest4.dll"
			"${QT_LIBRARY_DIR}/QtNetwork4.dll"
			"${QT_LIBRARY_DIR}/QtCore4.dll"
			"${QT_LIBRARY_DIR}/phonon4.dll"
			"${QT_LIBRARY_DIR}/QtSvg4.dll"
			DESTINATION "${BT_DESTINATION}"
			CONFIGURATIONS "Release"
  )

  # Qt Plugins
  INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/iconengines" "${QT_PLUGINS_DIR}/imageformats" DESTINATION "${BT_DESTINATION}/plugins")

  # This adds in the required Windows system libraries and libsword.dll
  INCLUDE(InstallRequiredSystemLibraries)
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
####################################
#   Apple Segment
####################################
ELSEIF(APPLE)
  INCLUDE(${QT_USE_FILE})
  SET(QT_FRAMEWORKS_USED "QtWebKit"
  			 "QtGui"
			 "QtXml"
			 "QtTest"
			 "QtNetwork"
			 "QtCore"
			 "QtSvg")
  FOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})
    INSTALL(DIRECTORY
            "${QT_LIBRARY_DIR}/${QT_FRAME}.framework"
            DESTINATION "${BT_DESTINATION}/../Library/Frameworks/")
  ENDFOREACH(QT_FRAME ${QT_FRAMEWORKS_USED})
  # Other libraries I need
  INSTALL(FILES "${CLUCENE_LIBRARY}"
	        "${CLUCENE_LIBRARY_DIR}/libclucene.0.0.0.dylib"
		"${CLUCENE_LIBRARY_DIR}/libclucene.0.dylib"
		"/opt/local/lib/libcurl.4.dylib"
   	        "/opt/local/lib/libssl.0.9.8.dylib"
	       	"/opt/local/lib/libcrypto.0.9.8.dylib"
	       	"/opt/local/lib/libicui18n.40.dylib"
	       	"/opt/local/lib/libicui18n.40.0.dylib"
		"/opt/local/lib/libicuuc.40.dylib"
	       	"/opt/local/lib/libicuuc.40.0.dylib"
	       	"/opt/local/lib/libicuio.40.dylib"
	       	"/opt/local/lib/libicuio.40.0.dylib"
		"/opt/local/lib/libicudata.40.dylib"
	       	"/opt/local/lib/libicudata.40.0.dylib"
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
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_PACKAGE_EXECUTABLES "bibletime" "BibleTime")

INCLUDE(CPack)