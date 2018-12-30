
ENABLE_TESTING(true)

QT5_ADD_RESOURCES(test_RESOURCE_SOURCES
    ${bibletime_SOURCE_DIR}/tests/tests.qrc
)

SET(test_OTHER_SOURCES
    ${bibletime_SOURCE_DIR}/src/frontend/messagedialog.cpp
)

FUNCTION(test_a_class testDir testClass )
    ADD_EXECUTABLE("test_${testClass}"
        "${testDir}/test_${testClass}.cpp"
        ${test_OTHER_SOURCES}
        ${test_RESOURCE_SOURCES}
    )
    TARGET_INCLUDE_DIRECTORIES("test_${testClass}"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    TARGET_LINK_LIBRARIES("test_${testClass}"
        PRIVATE
            bibletime_backend
            Qt5::Network
            Qt5::Test
            Qt5::Widgets
            Qt5::Xml
    )
    ADD_TEST(NAME ${testClass} COMMAND test_${testClass})
ENDFUNCTION(test_a_class)

# The first 2 tests install modules that the other tests need
# They should be ran in this order
test_a_class(tests/backend/btsourcesthread btsourcesthread)
test_a_class(tests/backend/btinstallthread btinstallthread)

test_a_class(tests/backend/managers/cswordbackend cswordbackend)
test_a_class(tests/backend/keys/cswordversekey cswordversekey)
test_a_class(tests/backend/models/btlistmodel btlistmodel)
