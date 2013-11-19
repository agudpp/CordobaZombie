

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/coll_physics/physics/helpers/BulletExporter.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/coll_physics/physics/helpers/BulletExporter.cpp
)

################################################################################
##			Bullet specific stuff				     ###
################################################################################

#check if the variable was already set
list(FIND COMMON_LIBRARIES "BulletDynamics" bulletDefined)
if(bulletDefined EQUAL -1)
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
endif()