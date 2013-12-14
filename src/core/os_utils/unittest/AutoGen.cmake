include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}

)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/os_utils/unittest/test.cpp
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)
