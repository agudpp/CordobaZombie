include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/helpers/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/helpers/AutoGen.cmake)
include(${DEV_ROOT_PATH}/gps/pathfinding/AutoGen.cmake)

include(${DEV_ROOT_PATH}/effect/AutoGen.cmake)

include(${DEV_ROOT_PATH}/cz/ia/path_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/static_data_loader/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/world_object/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/physic_game_object/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/static_world_objects/AutoGen.cmake)

include(${DEV_ROOT_PATH}/cz/scene_asset_loader/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/asset/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)


# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/static_world_object_test/StaticWorldObjectTest.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/static_world_object_test/StaticWorldObjectTest.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
	${DEV_ROOT_PATH}/tools/tests/static_world_object_test/main.cpp
)
