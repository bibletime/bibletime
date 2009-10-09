
#
# There was an issue using cmake's default mechanisms (maybe a limit to the parameters
# that may be passed to a command), so I used a shellsript instead.
#

#FILE(GLOB_RECURSE ORIG_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src" "*.cpp")
#FOREACH(FILE ${ORIG_SOURCE_FILES})
#	SET(REFORMAT_FILES "${REFORMAT_FILES} ${FILE}")
#ENDFOREACH(FILE ${ORIG_SOURCE_FILES})
#
#FILE(GLOB_RECURSE ORIG_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src" "*.h")
#FOREACH(FILE ${ORIG_SOURCE_FILES})
#	SET(REFORMAT_FILES "${REFORMAT_FILES} ${FILE}")
#ENDFOREACH(FILE ${ORIG_SOURCE_FILES})

#SET(ASTYLE_OPTIONS "--indent=spaces=4 --brackets=attach --indent-classes --indent-switches --brackets=break-closing --pad=oper --suffix=none --options=none")

ADD_CUSTOM_TARGET("reformat_codebase"
#	COMMAND astyle ${ASTYLE_OPTIONS}  ${REFORMAT_FILES}
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/reformat_codebase.sh"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)