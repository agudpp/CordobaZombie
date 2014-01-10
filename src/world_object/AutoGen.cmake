
# check if it is already defined
if (NOT WORLD_OBJECT)
set (WORLD_OBJECT 1)

# include global modules
include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/world_object/Coll2DRepresentation.h
    ${DEV_ROOT_PATH}/world_object/PhysicsRepresentation.h
    ${DEV_ROOT_PATH}/world_object/MaskData.h
    ${DEV_ROOT_PATH}/world_object/GraphicRepresentation.h
    ${DEV_ROOT_PATH}/world_object/WorldObject.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/world_object/Coll2DRepresentation.cpp
    ${DEV_ROOT_PATH}/world_object/PhysicsRepresentation.cpp
    ${DEV_ROOT_PATH}/world_object/WorldObject.cpp
)

endif()