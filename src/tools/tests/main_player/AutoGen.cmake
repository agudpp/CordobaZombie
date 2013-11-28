include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/effect/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/main_player/MainPlayerTester.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/main_player/MainPlayerTester.cpp
	${DEV_ROOT_PATH}/tools/tests/main_player/main.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
    ${DEV_ROOT_PATH}/tools/utils/SelectionHelper.cpp
)


