IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Common/DebugUtil)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Common/DebugUtil/PrimitiveDrawer.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Common/DebugUtil/PrimitiveDrawer.h
	${DEV_ROOT_PATH}/Common/DebugUtil/DebugUtil.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Common/DebugUtil
)

include_directories(${ACTUAL_DIRS})
