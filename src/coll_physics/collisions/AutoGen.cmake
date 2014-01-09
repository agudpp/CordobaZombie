
# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/coll_physics/collisions/CollCell.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollDefines.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollisionHandler.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollObject.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollPreciseInfo.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollPreciseInfoBuilder.h
	${DEV_ROOT_PATH}/coll_physics/collisions/CollDebugDrawer.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/coll_physics/collisions/CollisionHandler.cpp
	${DEV_ROOT_PATH}/coll_physics/collisions/CollPreciseInfo.cpp
	${DEV_ROOT_PATH}/coll_physics/collisions/CollPreciseInfoBuilder.cpp
	${DEV_ROOT_PATH}/coll_physics/collisions/CollDebugDrawer.cpp
)


# Box2D static linkage
if (NOT BOX2D_LIBS_DEFINED)
    set(BOX2D_LIBS_DEFINED 1) 
    add_library(Box2D STATIC IMPORTED)
    # point the imported target at the real file
    set_property(TARGET Box2D PROPERTY
                    IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/Box2D.a)
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES} Box2D)
endif()

