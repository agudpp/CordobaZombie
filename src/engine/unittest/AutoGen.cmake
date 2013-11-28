include (${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)
include (${DEV_ROOT_PATH}/multimedia/sound/AutoGen.cmake)
include (${DEV_ROOT_PATH}/multimedia/video/AutoGen.cmake)
include (${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)
include (${DEV_ROOT_PATH}/engine/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}

)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/engine/unittest/test.cpp
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)
