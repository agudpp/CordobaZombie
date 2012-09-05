IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /CameraController)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/CameraController/CameraController.cpp
	${DEV_ROOT_PATH}/CameraController/CameraAnimUpdater.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/CameraController/CameraController.h
	${DEV_ROOT_PATH}/CameraController/CameraAnimUpdater.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/CameraController
)

include_directories(${ACTUAL_DIRS})
