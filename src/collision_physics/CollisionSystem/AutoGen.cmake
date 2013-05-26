IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /CollisionSystem)

set(SRCS
	${SRCS}	
	${DEV_ROOT_PATH}/CollisionSystem/CollObjBuilder/CollObjBuilder.cpp
	${DEV_ROOT_PATH}/CollisionSystem/CollisionManager.cpp
	${DEV_ROOT_PATH}/CollisionSystem/CollisionObject.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes/b2PolygonShape.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes/b2EdgeShape.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes/b2ChainShape.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes/b2Shape.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes/b2CircleShape.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/b2Collision.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/b2DynamicTree.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/b2TimeOfImpact.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/b2BroadPhase.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/b2Distance.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2Math.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2Timer.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2Draw.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2BlockAllocator.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2StackAllocator.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2GrowableStack.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common/b2Settings.h
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Box2D.h
	${DEV_ROOT_PATH}/CollisionSystem/CollObjBuilder/CollObjBuilder.h
	${DEV_ROOT_PATH}/CollisionSystem/CollisionManager.h
	${DEV_ROOT_PATH}/CollisionSystem/CollisionObject.h
	${DEV_ROOT_PATH}/CollisionSystem/CollisionCell.h
	${DEV_ROOT_PATH}/CollisionSystem/CollisionTypedefs.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/CollisionSystem
	${DEV_ROOT_PATH}/CollisionSystem/CollObjBuilder
	${DEV_ROOT_PATH}/CollisionSystem/Box2D
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Common
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision
	${DEV_ROOT_PATH}/CollisionSystem/Box2D/Collision/Shapes
)

include_directories(${ACTUAL_DIRS})


# TODO: tenemos que tener creada la libreria de box2D
# add imported target
add_library(box2D STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET box2D PROPERTY
IMPORTED_LOCATION ${DEV_ROOT_PATH}/CollisionSystem/Box2D/box2D.a)
