

include_directories(${THIRD_PARTY_LIBS}/include/bullet)

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/coll_physics/physics/CollisionObject.h
    ${DEV_ROOT_PATH}/coll_physics/physics/Shape.h
    ${DEV_ROOT_PATH}/coll_physics/physics/RigidBody.h
    ${DEV_ROOT_PATH}/coll_physics/physics/MotionState.h
    ${DEV_ROOT_PATH}/coll_physics/physics/PhysicsBuilder.h
    ${DEV_ROOT_PATH}/coll_physics/physics/DynamicWorld.h
    ${DEV_ROOT_PATH}/coll_physics/physics/RagDoll.h
    ${DEV_ROOT_PATH}/coll_physics/physics/BulletUtils.h
    ${DEV_ROOT_PATH}/coll_physics/physics/BulletDebugDrawer.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/coll_physics/physics/PhysicsBuilder.cpp
    ${DEV_ROOT_PATH}/coll_physics/physics/DynamicWorld.cpp
    ${DEV_ROOT_PATH}/coll_physics/physics/RagDoll.cpp
    ${DEV_ROOT_PATH}/coll_physics/physics/BulletUtils.cpp
    ${DEV_ROOT_PATH}/coll_physics/physics/BulletDebugDrawer.cpp
)

################################################################################
##			Bullet specific stuff				     ###
################################################################################
if (NOT BULLET_LIBS_DEFINED)
    set (BULLET_LIBS_DEFINED 1)
    if(UNIX OR WIN32)
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
    endif(UNIX OR WIN32)
    
    # TODO here we need to put the windows specific stuff
    
    ## for all OS is the same
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES} BulletDynamics BulletCollision LinearMath)
endif()
