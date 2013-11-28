include(${DEV_ROOT_PATH}/core/command_line/AutoGen.cmake)
# Define global sources here
set(HDRS ${HDRS}

)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/command_line/unittest/test.cpp
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)
