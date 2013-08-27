include (${DEV_ROOT_PATH}/gps/pathfinding/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}

)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/gps/pathfinding/unittest/test.cpp
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)
