IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /BillboardManager)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/BillboardManager/BillboardBatery.cpp
	${DEV_ROOT_PATH}/BillboardManager/BillboardManager.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/BillboardManager/BillboardManager.h
	${DEV_ROOT_PATH}/BillboardManager/BillboardBatery.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/BillboardManager
)

include_directories(${ACTUAL_DIRS})
