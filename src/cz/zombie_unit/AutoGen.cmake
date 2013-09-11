# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnit.h
	${DEV_ROOT_PATH}/cz/zombie_unit/AnimTable.h
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZombieFSMDefs.h
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSRunning.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/cz/zombie_unit/ZombieUnit.cpp
	${DEV_ROOT_PATH}/cz/zombie_unit/states/ZSRunning.cpp
)


