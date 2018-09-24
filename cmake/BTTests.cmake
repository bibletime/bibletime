
ENABLE_TESTING(true)

SET(test_OTHER_SOURCES
    ${bibletime_SOURCE_DIR}/src/frontend/messagedialog.cpp
)

FUNCTION(test_a_class testDir testClass )
    PROJECT(test_${testClass})
    ADD_EXECUTABLE(test_${testClass}
        ${testDir}/test_${testClass}.cpp
        ${test_OTHER_SOURCES}
        ${test_${testClass}_MOC_SRC}
    )
    SET_TARGET_PROPERTIES("test_${testClass}" PROPERTIES COMPILE_FLAGS -std=c++14 )
    TARGET_LINK_LIBRARIES(test_${testClass} Qt5::Widgets Qt5::Xml Qt5::Network Qt5::Test)
    TARGET_LINK_LIBRARIES(test_${testClass}
        bibletime_common
        ${Swordxx_LIBRARIES}
        ${CLucene_LIBRARY}
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
