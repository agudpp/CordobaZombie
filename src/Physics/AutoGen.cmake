IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Physics)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Physics/PhysicsHelper.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Physics/PhysicObject.h
	${DEV_ROOT_PATH}/Physics/PhysicsHelper.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Physics
)

include_directories(${ACTUAL_DIRS})
