IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
return()
endif()

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/PathfindingSystem
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh
	${DEV_ROOT_PATH}/PathfindingSystem/lib
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph
)

include_directories(${ACTUAL_DIRS})

add_custom_target(
	PathfindingSystemDependency
	COMMAND cmake . && make -j 4
	WORKING_DIRECTORY ${DEV_ROOT_PATH}/PathfindingSystem/lib
)

add_library(PathfindingSystem STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET PathfindingSystem PROPERTY
	IMPORTED_LOCATION ${DEV_ROOT_PATH}/PathfindingSystem/lib/libPathfindingSystem.a)



set(GLOBAL_LIBRARIES ${GLOBAL_LIBRARIES} PathfindingSystem)
set(GLOBAL_DEPENDENCIES ${GLOBAL_DEPENDENCIES} PathfindingSystemDependency)

