
CONFIG += qtquickcompiler

include(../../common/mobile/mobile.pro)

RESOURCES += qtbug55259.qrc

QMAKE_INFO_PLIST = Info.plist

ios_icon.files = $$files($$PWD/icons/*.png)
QMAKE_BUNDLE_DATA += ios_icon

