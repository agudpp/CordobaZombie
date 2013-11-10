include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)
include(${DEV_ROOT_PATH}/gps/pathfinding/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/helpers/AutoGen.cmake)

include(${DEV_ROOT_PATH}/effect/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/fx/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/firing_system/AutoGen.cmake)

include(${DEV_ROOT_PATH}/cz/static_data_loader/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/ia/path_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/ia/steering_behavior/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/world_object/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/physic_game_object/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/zombie_unit/AutoGen.cmake)


# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/zombie_bodyparts/ZombieBodypartTest.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/zombie_bodyparts/ZombieBodypartTest.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
	${DEV_ROOT_PATH}/tools/tests/zombie_bodyparts/main.cpp
)

