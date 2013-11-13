include(${DEV_ROOT_PATH}/core/asset/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)

# Define global headers here
set(HDRS ${HDRS}
)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/asset/unittest/test.cpp
)


set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)