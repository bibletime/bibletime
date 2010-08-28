FILE(GLOB_RECURSE ORIG_SOURCE_FILES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
FOREACH(FILE ${ORIG_SOURCE_FILES})
    SET(REFORMAT_FILES "${REFORMAT_FILES} ${FILE}")
ENDFOREACH(FILE ${ORIG_SOURCE_FILES})

SET(ASTYLE_OPTIONS "--indent=spaces=4 --brackets=attach --indent-classes --indent-switches --brackets=break-closing --pad=oper --suffix=none --options=none")

ADD_CUSTOM_TARGET("reformat_codebase"
    COMMAND "sh" "-c" "astyle ${ASTYLE_OPTIONS}  ${REFORMAT_FILES}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    VERBATIM
)