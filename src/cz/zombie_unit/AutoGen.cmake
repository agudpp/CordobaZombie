# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnit.h
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZombieFSMDefs.h

	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSRunning.h
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSBeingHit.h
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSDying.h

	${DEV_ROOT_PATH}/cz/zombie_unit/ResourceQueue.h
	${DEV_ROOT_PATH}/cz/zombie_unit/BodyPartQueue.h
	${DEV_ROOT_PATH}/cz/zombie_unit/RagDollQueue.h
    ${DEV_ROOT_PATH}/cz/zombie_unit/ZombieBody.h
    ${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnitBuilder.h
    ${DEV_ROOT_PATH}/cz/zombie_unit/BodyPartElement.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnit.cpp
	
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSRunning.cpp
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSBeingHit.cpp
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSDying.cpp
	
    ${DEV_ROOT_PATH}/cz/zombie_unit/ZombieBody.cpp
    ${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnitBuilder.cpp
    ${DEV_ROOT_PATH}/cz/zombie_unit/BodyPartElement.cpp
)


