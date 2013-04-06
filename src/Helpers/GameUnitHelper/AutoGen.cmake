IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Helpers/GameUnitHelper)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Helpers/GameUnitHelper/GameUnitHelper.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Helpers/GameUnitHelper/GameUnitHelper.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Helpers/GameUnitHelper
)

include_directories(${ACTUAL_DIRS})
