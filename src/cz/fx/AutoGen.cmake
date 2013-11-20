# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/cz/fx/effects/EffectQueue.h
    ${DEV_ROOT_PATH}/cz/fx/effects/EffectQueueDefs.h
    ${DEV_ROOT_PATH}/cz/fx/effects/QueuedEffect.h

    ${DEV_ROOT_PATH}/cz/fx/effects/blood/BloodParticles.h
    
    ${DEV_ROOT_PATH}/cz/fx/effects/billboards/BillboardGroupHandler.h
    ${DEV_ROOT_PATH}/cz/fx/effects/billboards/BillboardStack.h
    ${DEV_ROOT_PATH}/cz/fx/effects/billboards/BillboardLoader.h
    ${DEV_ROOT_PATH}/cz/fx/effects/billboards/UVCoordsDefs.h
    
    ${DEV_ROOT_PATH}/cz/fx/effects/body_part/BodyPartEffect.h
    
    ${DEV_ROOT_PATH}/cz/fx/effects/bullet_impact/BulletImpactEffect.h
    ${DEV_ROOT_PATH}/cz/fx/effects/bullet_impact/BulletImpactQueueHandler.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/cz/fx/effects/blood/BloodParticles.cpp
    ${DEV_ROOT_PATH}/cz/fx/effects/billboards/BillboardLoader.cpp
    
    ${DEV_ROOT_PATH}/cz/fx/effects/body_part/BodyPartEffect.cpp
    
        ${DEV_ROOT_PATH}/cz/fx/effects/bullet_impact/BulletImpactEffect.cpp
    ${DEV_ROOT_PATH}/cz/fx/effects/bullet_impact/BulletImpactQueueHandler.cpp
)


