
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/game_states/AutoGen.cmake)
include(${DEV_ROOT_PATH}/multimedia/video/AutoGen.cmake)
include(${DEV_ROOT_PATH}/ui/frontend/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/main_menu_state/MainMenuStateTest.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/main_menu_state/MainMenuStateTest.cpp
	${DEV_ROOT_PATH}/tools/tests/main_menu_state/main.cpp
)

