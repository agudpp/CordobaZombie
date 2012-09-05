IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
return()
endif()

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/CollisionSystem
	${DEV_ROOT_PATH}/CollisionSystem/Box2D
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes
	${DEV_ROOT_PATH}/CollisionSystem/lib
	${DEV_ROOT_PATH}/CollisionSystem/lib/dist
	${DEV_ROOT_PATH}/CollisionSystem/lib/dist/bin
	${DEV_ROOT_PATH}/CollisionSystem/lib/dist/media
	${DEV_ROOT_PATH}/CollisionSystem/CollObjBuilder
)

include_directories(${ACTUAL_DIRS})

add_custom_target(
	CollisionSystemDependency
	COMMAND cmake . && make -j 4
	WORKING_DIRECTORY ${DEV_ROOT_PATH}/CollisionSystem/lib
)

add_library(CollisionSystem STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET CollisionSystem PROPERTY
	IMPORTED_LOCATION ${DEV_ROOT_PATH}/CollisionSystem/lib/libCollisionSystem.a)


# TODO: tenemos que tener creada la libreria de box2D
# add imported target
add_library(box2D STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET box2D PROPERTY
IMPORTED_LOCATION ${DEV_ROOT_PATH}/CollisionSystem/Box2D/box2D.a)


set(GLOBAL_LIBRARIES ${GLOBAL_LIBRARIES} CollisionSystem box2D)
set(GLOBAL_DEPENDENCIES ${GLOBAL_DEPENDENCIES} CollisionSystemDependency)

