

QMAKE_FULL_VERSION = 300
QMAKE_SHORT_VERSION = 3.00

CONFIG += qtquickcompiler

include(../../common/mobile/mobile.pro)

RESOURCES += qtbug55259.qrc

ios {
    QMAKE_INFO_PLIST = Info.plist
    ios_icon.files = $$files($$PWD/icons/*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    launch_xib.files = $$files($$PWD/ios/BibleTimeLaunchScreen.xib)
    QMAKE_BUNDLE_DATA += launch_xib
}

