FIND_PACKAGE(Qt6 6.7 REQUIRED COMPONENTS Core Network Test)
SET(CMAKE_AUTOMOC ON)

SET(PHOS_SYNC_DIR "${PROJECT_SOURCE_DIR}/src/backend/phossync")

FUNCTION(BT_ADD_PHOS_TEST NAME)
    ADD_EXECUTABLE("${NAME}" "${NAME}.cpp"
        "${PHOS_SYNC_DIR}/btphosbookmarkcodec.cpp"
        "${PHOS_SYNC_DIR}/btphoshttpclient.cpp"
        "${PHOS_SYNC_DIR}/btphossyncengine.cpp")
    TARGET_COMPILE_FEATURES("${NAME}" PRIVATE cxx_std_23)
    TARGET_COMPILE_DEFINITIONS("${NAME}" PRIVATE
        "QT_NO_KEYWORDS" "QT_DISABLE_DEPRECATED_UP_TO=0x060500")
    TARGET_INCLUDE_DIRECTORIES("${NAME}" PRIVATE "${PHOS_SYNC_DIR}")
    TARGET_LINK_LIBRARIES("${NAME}" PRIVATE
        Qt::Core Qt::Network Qt::Test PkgConfig::Sword)
    ADD_TEST(NAME "${NAME}" COMMAND "${NAME}")
ENDFUNCTION()

BT_ADD_PHOS_TEST(test_phossyncengine)
BT_ADD_PHOS_TEST(test_phoscodec)
BT_ADD_PHOS_TEST(test_phoslive)
