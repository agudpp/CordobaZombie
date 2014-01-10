include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)
include(${DEV_ROOT_PATH}/world_object/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/world_object_tests/representations/WorldObjectRepTest.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/world_object_tests/representations/WorldObjectRepTest.cpp
	${DEV_ROOT_PATH}/tools/tests/world_object_tests/representations/main.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
)

