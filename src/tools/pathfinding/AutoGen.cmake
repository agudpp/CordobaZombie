include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/gps/pathfinding/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/pathfinding/PathfindingTool.h
	${DEV_ROOT_PATH}/tools/utils/SatelliteCamera.h
	${DEV_ROOT_PATH}/tools/utils/SelectionHelper.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/pathfinding/PathfindingTool.cpp
	${DEV_ROOT_PATH}/tools/utils/SatelliteCamera.cpp
	${DEV_ROOT_PATH}/tools/utils/SelectionHelper.cpp
	${DEV_ROOT_PATH}/tools/pathfinding/main.cpp
)


