
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/load_resources/ResourcesLoadingTest.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/load_resources/ResourcesLoadingTest.cpp
	${DEV_ROOT_PATH}/tools/tests/load_resources/main.cpp
)

