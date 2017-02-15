
# Build the qtbug-35164-patcher tool
MESSAGE(STATUS "qtbug-35164-patcher")
PROJECT(qtbug-35164-patcher)
FIND_PACKAGE(Qt5Core REQUIRED)
ADD_EXECUTABLE(qtbug-35164-patcher
    ${bibletime_SOURCE_DIR}/i18n/qtbug-35164-patcher.cpp
)
TARGET_LINK_LIBRARIES(qtbug-35164-patcher Qt5::Core)


# Run    qtbug-35164-patcher <file>.ts <file>.ts2,    then rename <file>.ts2 to <file>.ts
FUNCTION(qtbug_35164_patcher_func)
    MESSAGE(STATUS "Bug patcher")
    SET(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n/messages")

    # Fixup source file to be pushed to transifex.
    ADD_CUSTOM_TARGET("qtbug_35164_patcher_default"
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/qtbug-35164-patcher${CMAKE_EXECUTABLE_SUFFIX}
            ${TS_DIR}/bibletime_ui.ts
            ${TS_DIR}/bibletime_ui.ts2

        # Comment out the next 3 lines to be able to see both the unpatched and patched files.
        COMMAND ${CMAKE_COMMAND} -E rename
            ${TS_DIR}/bibletime_ui.ts2
            ${TS_DIR}/bibletime_ui.ts

        COMMENT "ran qtbug_35164_patcher on bibletime_ui.ts"
    )
    # Ensure tool is built first and that the ts file is created first
    ADD_DEPENDENCIES("qtbug_35164_patcher_default" "messages_default" "qtbug-35164-patcher" )
    ADD_DEPENDENCIES("messages" "qtbug_35164_patcher_default")

    # Fixup translation files as well.
    BT_GET_TS_LANGS(TS_LANGS)
    FOREACH(TS_LANG ${TS_LANGS})
        ADD_CUSTOM_TARGET("qtbug_35164_patcher_${TS_LANG}"
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/qtbug-35164-patcher${CMAKE_EXECUTABLE_SUFFIX}
                ${TS_DIR}/bibletime_ui_${TS_LANG}.ts
                ${TS_DIR}/bibletime_ui_${TS_LANG}.ts2

            # Comment out the next 3 lines to be able to see both the unpatched and patched files.
            COMMAND ${CMAKE_COMMAND} -E rename
                ${TS_DIR}/bibletime_ui_${TS_LANG}.ts2
                ${TS_DIR}/bibletime_ui_${TS_LANG}.ts

            COMMENT "ran qtbug_35164_patcher on bibletime_ui_${TS_LANG}.ts"
        )
        # Ensure tool is built first and that the ts file is created first
        ADD_DEPENDENCIES("qtbug_35164_patcher_${TS_LANG}" "messages_${TS_LANG}" "qtbug-35164-patcher" )
        ADD_DEPENDENCIES("messages" "qtbug_35164_patcher_${TS_LANG}")
    ENDFOREACH()
endfunction()

qtbug_35164_patcher_func()
