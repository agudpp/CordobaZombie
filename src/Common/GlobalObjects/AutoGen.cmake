IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Common/GlobalObjects)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Common/GlobalObjects
)

include_directories(${ACTUAL_DIRS})
