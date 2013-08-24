include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/anim_player/AnimPlayer.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/anim_player/AnimPlayer.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
	${DEV_ROOT_PATH}/tools/anim_player/main.cpp
)


