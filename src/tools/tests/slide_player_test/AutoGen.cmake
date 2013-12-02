
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/multimedia/video/AutoGen.cmake)
include(${DEV_ROOT_PATH}/ui/frontend/AutoGen.cmake)
include(${DEV_ROOT_PATH}/effect/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/helpers/slide_player/AutoGen.cmake)


# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/slide_player_test/SlidePlayerTest.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/slide_player_test/SlidePlayerTest.cpp
	${DEV_ROOT_PATH}/tools/tests/slide_player_test/main.cpp
)

