include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)

# Define global headers here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.h
)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.cpp
)
