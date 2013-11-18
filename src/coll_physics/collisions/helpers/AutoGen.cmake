include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}	
	${DEV_ROOT_PATH}/coll_physics/collisions/helpers/CollStaticWorldLoader.h
	${DEV_ROOT_PATH}/coll_physics/collisions/helpers/CollObjectExporter.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/coll_physics/collisions/helpers/CollStaticWorldLoader.cpp
    ${DEV_ROOT_PATH}/coll_physics/collisions/helpers/CollObjectExporter.cpp
)



