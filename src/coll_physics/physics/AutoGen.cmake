

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/coll_physics/physics/BulletObject.h
	${DEV_ROOT_PATH}/coll_physics/physics/BulletUtils.h
	${DEV_ROOT_PATH}/coll_physics/physics/DynamicWorld.h
	${DEV_ROOT_PATH}/coll_physics/physics/RagDoll.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/coll_physics/physics/BulletUtils.cpp
	${DEV_ROOT_PATH}/coll_physics/physics/DynamicWorld.cpp
	${DEV_ROOT_PATH}/coll_physics/physics/RagDoll.cpp
)

################################################################################
##			Bullet specific stuff				     ###
################################################################################

include_directories(${THIRD_PARTY_LIBS}/include/bullet)
if(UNIX)
add_library(BulletCollision STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET BulletCollision PROPERTY
                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libBulletCollision.a)

add_library(BulletDynamics STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET BulletDynamics PROPERTY
                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libBulletDynamics.a)

## We will not used SoftBodies now
#add_library(BulletSoftBody STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET BulletSoftBody PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libBulletSoftBody.a)

add_library(LinearMath STATIC IMPORTED)
# point the imported target at the real file
set_property(TARGET LinearMath PROPERTY
                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libLinearMath.a)
endif(UNIX)

# TODO here we need to put the windows specific stuff

## for all OS is the same
set(COMMON_LIBRARIES ${COMMON_LIBRARIES} BulletDynamics BulletCollision LinearMath)
