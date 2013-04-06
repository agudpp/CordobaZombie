IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Helpers/MetaRscManager)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Helpers/MetaRscManager/MetaRscManager.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Helpers/MetaRscManager/MetaRscManager.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Helpers/MetaRscManager
)

include_directories(${ACTUAL_DIRS})
